/** @file
  EAS execution services implementations.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ShellLib.h>

#include "ENTS/EasDispatcher/Include/Eas.h"
#include <Protocol/Eftp.h>
#include "ENTS/EasDispatcher/Include/EftpImplement.h"

STATIC EFI_FILE                     *mRealFileHandle      = NULL;
STATIC UINTN                        mRealFileSize         = 0;
STATIC VOID                         *mRealFileBuffer      = NULL;
STATIC UINT64                       mTransferSize         = 0;

STATIC EFI_MANAGED_NETWORK_PROTOCOL *TmpMnp               = NULL;
STATIC EFI_SERVICE_BINDING_PROTOCOL *TmpMnpSb             = NULL;
STATIC EFI_HANDLE                   TmpMnpInstanceHandle  = NULL;
STATIC EFI_EFTP_PROTOCOL            *EftpIo               = NULL;
STATIC EFI_SERVICE_BINDING_PROTOCOL *TmpEftpSb            = NULL;
STATIC EFI_HANDLE                   TmpEftpInstanceHandle = NULL;

STATIC
UINTN
UnicodeToChar (
  OUT CHAR8  *Dst,
  IN  CHAR16 *Src
  )
/*++

Routine Description:

  UnicodeToChar.

Arguments:

  Dst - Dest char8 string.
  Src - Src char16 string.

Returns:

  Length of string that tranversed.

--*/
{
  UINTN Index;

  for (Index = 0; Index < StrLen (Src); Index++) {
    Dst[Index] = (CHAR8) (Src[Index] & 0xff);
  }

  Dst[Index] = 0;
  return Index;
}

STATIC
EFI_STATUS
EftpTimeoutCallback (
  IN EFI_EFTP_PROTOCOL          *This,
  IN EFI_EFTP_TOKEN             *Token
  )
/*++

Routine Description:

  Eftp callback function when timeout happened.

Arguments:

  This  - A pointer to the EFI_EFTP_PROTOCOL instance.
  Token - Pointer to the EFI_EFTP_TOKEN structure.

Returns:

  EFI_SUCCESS - Operation succeeded.

--*/
{
  Print (L"Timeout happen!\n");
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
OpenMnpProtocol (
  VOID
  )
/*++

Routine Description:

  Open MNP protocol.

Arguments:

  None

Returns:

  EFI_SUCCESS           - Operation succeeded.
  Others                - Some failure happened.

--*/
{
  EFI_STATUS                          Status;
  EFI_HANDLE                          ControllerHandle;

  Status = EntsNetworkServiceBindingGetControllerHandle (
             &gEfiManagedNetworkServiceBindingProtocolGuid,
             &ControllerHandle
             );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->HandleProtocol (
                 ControllerHandle,
                 &gEfiManagedNetworkServiceBindingProtocolGuid,
                 &TmpMnpSb
                 );
  if (EFI_ERROR (Status)) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"OpenMnpProtocol: LocateProtocol error ! - %r", Status));
    return Status;
  }

  Status = TmpMnpSb->CreateChild (TmpMnpSb, &TmpMnpInstanceHandle);
  if (EFI_ERROR (Status)) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"OpenMnpProtocol: TmpMnpSb->CreateChild error ! - %r", Status));
    return Status;
  }
  //
  // Retrieve the ManagedNetwork Protocol
  //
#if 1
  Status = gBS->OpenProtocol (
                TmpMnpInstanceHandle,
                &gEfiManagedNetworkProtocolGuid,
                (VOID **) &TmpMnp,
                mImageHandle,
                TmpMnpInstanceHandle,
                EFI_OPEN_PROTOCOL_GET_PROTOCOL
                );
  if (EFI_ERROR (Status)) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"OpenMnpProtocol: OpenProtocol error ! - %r\n", Status));
    return Status;
  }
#else
  Status = gBS->HandleProtocol (
                 TmpMnpInstanceHandle,
                 &gEfiManagedNetworkProtocolGuid,
                 (VOID **) &TmpMnp
                 );
  if (EFI_ERROR(Status)) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"OpenMnpProtocol: HandleProtocol error ! - %r", Status));
    return Status;
  }
#endif
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
CloseMnpProtocol (
  VOID
  )
/*++

Routine Description:

  Close MNP protocol.

Arguments:

  None

Returns:

  EFI_SUCCESS           - Operation succeeded.
  Others                - Some failure happened.

--*/
{
  EFI_STATUS  Status;
#if 0
  Status = gBS->CloseProtocol (
                TmpMnpInstanceHandle,
                &gEfiManagedNetworkProtocolGuid,
                mImageHandle,
                TmpMnpInstanceHandle
                );
  if (EFI_ERROR (Status)) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"CloseMnpProtocol: CloseProtocol error ! - %r\n", Status));
    return Status;
  }
#endif
  Status = TmpMnpSb->DestroyChild (TmpMnpSb, TmpMnpInstanceHandle);
  if (EFI_ERROR (Status)) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"CloseMnpProtocol: DestroyChild error ! - %r", Status));
    return Status;
  }

  TmpMnp                = NULL;
  TmpMnpSb              = NULL;
  TmpMnpInstanceHandle  = NULL;

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
OpenEftpProtocol (
  VOID
  )
{
  EFI_STATUS                         Status;
  EFI_HANDLE                         ControllerHandle;

  Status = EntsNetworkServiceBindingGetControllerHandle (
             &gEfiManagedNetworkServiceBindingProtocolGuid,
             &ControllerHandle
             );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->HandleProtocol (
                 ControllerHandle,
                 &gEfiEftpServiceBindingProtocolGuid,
                 &TmpEftpSb
                 );
  if (EFI_ERROR(Status)) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"No Eftp driver has found!\n"));
    return Status;
  }

  TmpEftpInstanceHandle = NULL;
  Status      = TmpEftpSb->CreateChild (TmpEftpSb, &TmpEftpInstanceHandle);
  if (EFI_ERROR (Status)) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"Create EFTP child failed with %r.\n", Status));
    return Status;
  }
#if 0
  Status = gBS->OpenProtocol (
                TmpEftpInstanceHandle,
                &gEfiEftpProtocolGuid,
                (VOID **) &EftpIo,
                mImageHandle,
                TmpEftpInstanceHandle,
                EFI_OPEN_PROTOCOL_GET_PROTOCOL
                );
  if (EFI_ERROR (Status)) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"OpenEftpProtocol: Get EFI_EFTP_PROTOCOL fail - %r", Status));
    TmpEftpSb->DestroyChild (TmpEftpSb, TmpEftpInstanceHandle);
    return Status;
  }
#else
  Status = gBS->HandleProtocol (
                 TmpEftpInstanceHandle,
                 &gEfiEftpProtocolGuid,
                 (VOID **) &EftpIo
                 );
  if (EFI_ERROR (Status)) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"OpenEftpProtocol: Get EFI_EFTP_PROTOCOL fail - %r", Status));
    TmpEftpSb->DestroyChild (TmpEftpSb, TmpEftpInstanceHandle);
    return Status;
  }
#endif
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
CloseEftpProtocol (
  VOID
  )
{
  EFI_STATUS                           Status;
#if 0
  Status = gBS->CloseProtocol (
                TmpEftpInstanceHandle,
                &gEfiEftpProtocolGuid,
                mImageHandle,
                TmpEftpInstanceHandle
                );
  if (EFI_ERROR (Status)) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"CloseMnpProtocol: CloseProtocol error ! - %r\n", Status));
    return Status;
  }
#endif
  Status = TmpEftpSb->DestroyChild (TmpEftpSb, TmpEftpInstanceHandle);
  if (EFI_ERROR (Status)) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"CloseMnpProtocol: DestroyChild error ! - %r\n", Status));
    return Status;
  }

  EftpIo                 = NULL;
  TmpEftpSb              = NULL;
  TmpEftpInstanceHandle  = NULL;

  return EFI_SUCCESS;

}

//
// Internal Function
//
STATIC
EFI_STATUS
OpenRealFile (
  IN EFI_HANDLE          Handle,
  IN CHAR16              *FileName,
  OUT EFI_FILE           **FileHandle
  )
/*++

Routine Description:

  Open Real file by name.

Arguments:

  Handle      - File handler.
  FileName    - File name string.
  FileHandle  - Pointer to EFI_FILE instance.

Returns:

  EFI_SUCCESS           - Operation succeeded.
  Others                - Some failure happened.

--*/
{
  EFI_LOADED_IMAGE_PROTOCOL       *Image;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath;
  EFI_HANDLE                      DeviceHandle;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem;
  EFI_FILE                        *Root;
  EFI_STATUS                      Status;

  Status = gBS->HandleProtocol (
                mImageHandle,
                &gEfiLoadedImageProtocolGuid,
                &Image
                );
  if (EFI_ERROR (Status)) {
    Print (L"Error: HandleProtocol LoadedImage ! - %r\n", Status);
    return Status;
  }

  Status = gBS->HandleProtocol (
                Image->DeviceHandle,
                &gEfiDevicePathProtocolGuid,
                &DevicePath
                );
  if (EFI_ERROR (Status)) {
    Print (L"Error: HandleProtocol DevicePath ! - %r\n", Status);
    return Status;
  }

  Status = gBS->LocateDevicePath (
                &gEfiSimpleFileSystemProtocolGuid,
                &DevicePath,
                &DeviceHandle
                );
  if (EFI_ERROR (Status)) {
    Print (L"Error: LocateDevicePath SimpleFileSystem ! - %r\n", Status);
    return Status;
  }

  Status = gBS->HandleProtocol (
                DeviceHandle,
                &gEfiSimpleFileSystemProtocolGuid,
                (VOID *) &SimpleFileSystem
                );
  if (EFI_ERROR (Status)) {
    Print (L"Error: HandleProtocol SimpleFileSystem ! - %r\n", Status);
    return Status;
  }

  Status = SimpleFileSystem->OpenVolume (
                              SimpleFileSystem,
                              &Root
                              );
  if (EFI_ERROR (Status)) {
    Print (L"Error: SimpleFileSystem->OpenVolume() ! - %r\n", Status);
    return Status;
  }

  Status = Root->Open (
                  Root,
                  FileHandle,
                  FileName,
                  EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                  0
                  );
  if (EFI_ERROR (Status)) {
    Print (L"Error: Root->Open() ! - %r\n", Status);
    return Status;
  }

  Status = Root->Close (Root);
  if (EFI_ERROR (Status)) {
    Print (L"Error: Root->Close() ! - %r\n", Status);
    return Status;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
CloseRealFile (
  IN EFI_FILE           *FileHandle
  )
/*++

Routine Description:

  Close real file by handler.

Arguments:

  FileHandle  - Pointer to EFI_FILE instance.

Returns:

  EFI_SUCCESS           - Operation succeeded.
  Others                - Some failure happened.

--*/
{
  if (mRealFileBuffer != NULL) {
    FreePool (mRealFileBuffer);
    mRealFileBuffer = NULL;
  }

  if (FileHandle != NULL) {
    FileHandle->Close (FileHandle);
    FileHandle = NULL;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
WriteRealFile (
  IN EFI_FILE        *FileHandle,
  IN UINTN           *FileSize,
  IN VOID            *FileBuffer
  )
/*++

Routine Description:

  Write filebuffer intot realfile.

Arguments:

  FileHandle  - Pointer to EFI_FILE instance.
  FileSize    - Fill buffer size to be written.
  FileBuffer  - Buffer to be written.

Returns:

  EFI_SUCCESS           - Operation succeeded.
  Others                - Some failure happened.

--*/
{
  EFI_STATUS  Status;

  //
  // Write the data to the file
  //
  Status = FileHandle->Write (
                        FileHandle,
                        FileSize,
                        FileBuffer
                        );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ReadRealFile (
  IN EFI_FILE        *FileHandle,
  OUT UINTN          *FileSize,
  OUT VOID           **FileBuffer
  )
/*++

Routine Description:

  Read real file into filebuffer.

Arguments:

  FileHandle  - Pointer to EFI_FILE instance.
  FileSize    - Fill buffer size to be written.
  FileBuffer  - Buffer to be written.

Returns:

  EFI_SUCCESS           - Operation succeeded.
  EFI_OUT_OF_RESOURCES  - FileInfo is NULL.
  Others                - Some failure happened.

--*/
{
  EFI_FILE_INFO *FileInfo;
  UINTN         BufferSize;
  EFI_STATUS    Status;

  //
  // Get file information
  //
  FileInfo    = NULL;
  BufferSize  = sizeof (EFI_FILE_INFO) + 1024;
  FileInfo    = AllocatePool (BufferSize);
  if (FileInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SetMem (FileInfo, BufferSize, 0);

  Status = FileHandle->GetInfo (
                        FileHandle,
                        &gEfiFileInfoGuid,
                        &BufferSize,
                        (VOID *) FileInfo
                        );
  if (EFI_ERROR (Status)) {
    FreePool (FileInfo);
    return Status;
  }
  //
  // Allocate buffer according to file size
  //
  *FileSize   = (UINTN) (FileInfo->FileSize);
  *FileBuffer = AllocatePool (*FileSize);
  if (*FileBuffer == NULL) {
    FreePool (FileInfo);
    return EFI_OUT_OF_RESOURCES;
  }

  FreePool (FileInfo);
  SetMem (*FileBuffer, *FileSize, 0);

  //
  // Read the data to the buffer
  //
  Status = FileHandle->Read (
                        FileHandle,
                        FileSize,
                        *FileBuffer
                        );
  if (EFI_ERROR (Status)) {
    FreePool (*FileBuffer);
    return Status;
  }

  return EFI_SUCCESS;
}
//
// External Function Implementation
//
EFI_STATUS
EftpDispatchFileTransferComd (
  ENTS_CMD_TYPE Operation
  )
/*++

Routine Description:

  Dispatch an get_file command, download file from SCT management side.

Arguments:

  Operation - Operation type, defined in ENTS_CMD_TYPE.

Returns:

  EFI_SUCCESS           - Operation succeeded.
  EFI_UNSUPPORTED       - File was not supported.
  Others                - Some failure happened.

--*/
{
  EFI_STATUS                        Status;
  EFI_EFTP_TOKEN                    Token;
  EFI_EFTP_CONFIG_DATA              EftpCfgData;
  UINT8                             FileName[MAX_FILENAME_LEN];
  UINT8                             ModeStr[MAX_MODE_STR_LEN];
  UINTN                             MacAddrLen;

  //
  // Open file to be transferred.
  //
  Status = OpenRealFile (mImageHandle, (gEasFT->Cmd)->ComdArg, &mRealFileHandle);
  if (EFI_ERROR (Status)) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"File %s is not supported.\n", (gEasFT->Cmd)->ComdArg));
    return EFI_UNSUPPORTED;
  }

  //
  // ReadRealFile for PUT_FILE
  //
  if (Operation == PUT_FILE) {
    Status = ReadRealFile (mRealFileHandle, &mRealFileSize, &mRealFileBuffer);
    if (EFI_ERROR (Status)) {
      EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"Real File Error - %r\n", Status));
      Status = EFI_UNSUPPORTED;
      goto Cleanup1;
    }
  }

  Status = GetMacAddress(EftpCfgData.SrcMac);
  if (EFI_ERROR(Status)) {
    EFI_ENTS_DEBUG((EFI_ENTS_D_ERROR, L"EftpDispatchFileTransferComd: GetMacAddress - %r", Status));
    return Status;
  }

  EFI_ENTS_DEBUG ((EFI_ENTS_D_TRACE, L"EftpCfgData.SrcMac :%x:%x:%x:%x:%x:%x",
    EftpCfgData.SrcMac[0], EftpCfgData.SrcMac[1],
    EftpCfgData.SrcMac[2], EftpCfgData.SrcMac[3],
    EftpCfgData.SrcMac[4], EftpCfgData.SrcMac[5]
    ));

  //
  // GetVariable from ENTS_SERVER_MAC_ADDRESS_NAME to get ServerMac address
  // It was recorded once MnpMonitor receive the first packet from server.
  //
  MacAddrLen = NET_ETHER_ADDR_LEN;
  Status = GetContextRecord(
             gntDevicePath,
             SCT_PASSIVE_MODE_RECORD_FILE,
             ENTS_SERVER_MAC_ADDRESS_NAME,
             &MacAddrLen,
             &EftpCfgData.DstMac
             );
  if(EFI_ERROR(Status)) {
    EFI_ENTS_DEBUG((EFI_ENTS_D_ERROR, L"EftpDispatchFileTransferComd: GetContextRecord fail - %r", Status));
    goto Cleanup1;
  }

  EFI_ENTS_DEBUG ((EFI_ENTS_D_TRACE, L"EftpCfgData.DstMac :%x:%x:%x:%x:%x:%x",
    EftpCfgData.DstMac[0], EftpCfgData.DstMac[1],
    EftpCfgData.DstMac[2], EftpCfgData.DstMac[3],
    EftpCfgData.DstMac[4], EftpCfgData.DstMac[5]
    ));

  Status = OpenEftpProtocol ();
  if (EFI_ERROR(Status)) {
    EFI_ENTS_DEBUG((EFI_ENTS_D_ERROR, L"OpenEftpProtocol fail - %r", Status));
    goto Cleanup1;
  }

  //
  // Set Time event (Optional)
  //
  //
  // Configure EFTP protocol instance.
  //
  EftpCfgData.TimeoutValue  = EFTP_DEFAULT_TIMEOUT_VALUE;
  EftpCfgData.TryCount      = EFTP_DEFAULT_RETRY_VALUE;

  Status                    = EftpIo->Configure (EftpIo, &EftpCfgData);
  if (EFI_ERROR (Status)) {
    Print (L"Eftp->Configure return %r.\n", Status);
    goto Cleanup2;
  }

  CopyMem (ModeStr, "octet", 6);

  if (StrLen ((gEasFT->Cmd)->ComdArg) > MAX_FILENAME_LEN) {
    Print (L"Too long Filename.\n");
    goto Cleanup2;
  }

  UnicodeToChar (FileName, (gEasFT->Cmd)->ComdArg);
  Token.Filename        = FileName;
  Token.OverrideData    = NULL;
  Token.ModeStr         = ModeStr;
  Token.OptionCount     = 0;
  Token.OptionList      = NULL;
  Token.Context         = NULL;
  Token.TimeoutCallback = NULL;
  Token.BufferSize      = 0;
  Token.Buffer          = NULL;
  Token.CheckPacket     = NULL;
  Token.PacketNeeded    = NULL;

Operation_start:
  //
  // User Synchronize mode to download file.
  //
  Token.Event = NULL;

  switch (Operation) {

  case GET_FILE:
    //
    // Download file
    //
    Token.CheckPacket = NULL;
    Token.BufferSize  = MAX_REAL_FILE_SIZE;
    Token.Buffer      = AllocatePool ((UINTN) Token.BufferSize);

    Print (L"Begin download ... ");
    Status = EftpIo->ReadFile (EftpIo, &Token);
    if (EFI_ERROR (Status)) {
      Print (L"EftpIo ReadFile return %r.\n", Status);
    }

    Print (L"End download ! ");

    if (!EFI_ERROR (Status)) {
      //
      // write real file
      //
      mRealFileSize = (UINTN) Token.BufferSize;
      Status        = WriteRealFile (mRealFileHandle, &mRealFileSize, Token.Buffer);
      if (EFI_ERROR (Status)) {
        Print (L"Write File Error - %r\n", Status);
      }

      if (Token.Buffer != NULL) {
        FreePool (Token.Buffer);
      }

    } else if (Status == EFI_BUFFER_TOO_SMALL) {
      mTransferSize = MAX_REAL_FILE_SIZE;
      Print (L"Buffer too small, try to allocate a larger buffer(%ld).\n", mTransferSize);
      Token.Buffer = AllocatePool ((UINTN) mTransferSize);
      if (Token.Buffer == NULL) {
        Print (L"Allocate buffer (%ld bytes) failed.\n", mTransferSize);
        goto Cleanup2;
      }

      Token.Status = EFI_SUCCESS;
      goto Operation_start;
    } else if (EFI_ERROR (Status)) {
      Print (L"Download error - %r\n", Status);
    }

    break;

  case PUT_FILE:
    //
    // Upload file
    //
    Token.PacketNeeded  = NULL;
    mTransferSize       = mRealFileSize;
    Token.Buffer        = AllocatePool ((UINTN) mRealFileSize);
    if (Token.Buffer == NULL) {
      Print (L"Allocate buffer (%ld bytes) failed.\n", mRealFileSize);
      goto Cleanup2;
    }

    Token.BufferSize = mRealFileSize;
    CopyMem (Token.Buffer, mRealFileBuffer, (UINTN) mRealFileSize);

    Print (L"Begin upload ... ");
    Status = EftpIo->WriteFile (EftpIo, &Token);
    if (EFI_ERROR (Status)) {
      Print (L"WriteFile return %r.\n", Status);
    }

    Print (L"End upload ! ");

    if (Token.Buffer != NULL) {
      FreePool (Token.Buffer);
    }

    if (EFI_ERROR (Status)) {
      Print (L"Upload error - %r\n", Status);
    }
    break;

  default:
    break;
  }

Cleanup2:
  CloseEftpProtocol();
Cleanup1:
  CloseRealFile (mRealFileHandle);
  return Status;
}
