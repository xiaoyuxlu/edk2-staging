/** @file
  This is the file that implements the file system access API. 

  Copyright (c) 2007 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include <Protocol/PciIo.h>
#include <Library/DevicePathLib.h>
#include <Protocol/LoadedImage.h>

#include <PiDxe.h>
#include <Library/DxeServicesLib.h>
#include <Library/FileHandleLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>

#define EFI_PATH_STRING_LENGTH  260
#define PATH_NAME_SEPARATOR     L'\\'

/** 
  Read file (part of file) from media by file name and EFI patition name

  @param PartitionName   The EFI Partition name.
  @param FileName        The file name.
  @param offset          The offset in file to start reading.
  @param BufferLenth     The lenth of file to read. 
                         input:  0 means to read in full lenth
                         output: exact length to read. 0 means failure
  
  @return Return the pointer of file read in memory.

**/
VOID *
ReadFromDevice(
  CHAR16 *PartitionName,
  CHAR16 *FileName,
  UINTN  Offset,
  UINTN  *BufferLenth
  )
{
  EFI_HANDLE                       *SimpleFileSystemHandles;
  UINTN                            NumberSimpleFileSystemHandles;
  EFI_HANDLE                       Device = NULL;
  EFI_STATUS                       Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *Volume;
  EFI_FILE_HANDLE                  Root = NULL;
  EFI_FILE_HANDLE                  ThisFile = NULL;
  UINTN                            BufferSize;
  UINTN                            FileSize;
  EFI_FILE_INFO                    *Info;
  UINTN                            Index;
  VOID                             *FileBuffer;
  EFI_DEVICE_PATH_PROTOCOL         *DevicePath;
  CHAR16                           *DevicePathText = NULL;
  BOOLEAN                          GetPartition = FALSE;

  if ((PartitionName == NULL) || (FileName == NULL) || (BufferLenth == NULL)) {
    return NULL;
  }  
  
  FileBuffer = NULL;
  FileSize = *BufferLenth;
  *BufferLenth = 0;

  Status = gBS->LocateHandleBuffer(
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NumberSimpleFileSystemHandles,
                  &SimpleFileSystemHandles
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  } 
      
  DEBUG((EFI_D_INFO, "NumberSimpleFileSystemHandles= 0x%x; SimpleFileSystemHandles= 0x%x\n", NumberSimpleFileSystemHandles, SimpleFileSystemHandles));

  for (Index = 0; Index < NumberSimpleFileSystemHandles; Index++) {
    Device = SimpleFileSystemHandles[Index];
    Root = NULL;
    ThisFile = NULL;

    DevicePath = DevicePathFromHandle(Device);
    //
    //Convert EFI path to text Path
    //
    DevicePathText = ConvertDevicePathToText(
      DevicePath,
      FALSE,
      FALSE
      );
    ASSERT(DevicePathText != NULL);

    if (StrnCmp(PartitionName, DevicePathText, StrLen(PartitionName)) == 0){
      GetPartition = TRUE;
      break;
    }
  }

  if (GetPartition){
    //
    // Handle the file system interface to the device
    //
    Status = gBS->HandleProtocol(
      Device,
      &gEfiSimpleFileSystemProtocolGuid,
      (VOID *)&Volume
      );
    if (!EFI_ERROR(Status)) {

      Status = Volume->OpenVolume(
                         Volume,
                         &Root
                         );
      if (!EFI_ERROR(Status)) {

        ASSERT(Root != NULL);
        Status = Root->Open(Root, &ThisFile, FileName, EFI_FILE_MODE_READ, 0);
        if (!EFI_ERROR(Status)) {

          ASSERT(ThisFile != NULL);
          //
          // Get file size
          //
          BufferSize = SIZE_OF_EFI_FILE_INFO + EFI_PATH_STRING_LENGTH;
          do {
            Info = NULL;
            Status = gBS->AllocatePool(EfiBootServicesData, BufferSize, (VOID **)&Info);
            if (EFI_ERROR(Status)) {
              goto Done;
            }
            Status = ThisFile->GetInfo(
                             ThisFile,
                             &gEfiFileInfoGuid,
                             &BufferSize,
                             Info
                             );
            if (!EFI_ERROR(Status)) {
              break;
            }
            if (Status != EFI_BUFFER_TOO_SMALL) {
              FreePool(Info);
              goto Done;
            }
            FreePool(Info);
          } while (TRUE);

          if (FileSize == 0 || FileSize > Info->FileSize){
            FileSize = (UINTN)(Info->FileSize);
          }
          FreePool(Info);

          Status = gBS->AllocatePool(EfiBootServicesData, FileSize, (VOID **)&FileBuffer);
          if (EFI_ERROR(Status)) {
            goto Done;
          }
          if (Offset){
            Status = ThisFile->SetPosition(ThisFile, Offset);
            ASSERT_EFI_ERROR(Status);
          }
          Status = ThisFile->Read(ThisFile, &FileSize, FileBuffer);
          DEBUG((EFI_D_INFO, "FileRead_Status= 0x%x\n", Status));

          if (EFI_ERROR(Status)) {
            Status = EFI_LOAD_ERROR;
            goto Done;
          }
          *BufferLenth = FileSize;
        }
      }
    }
  }

Done:
  if (ThisFile != NULL) {
    ThisFile->Close(ThisFile);
  }
  if (Root != NULL) {
    Root->Close(Root);
  }

  return FileBuffer;
}


/**
  Write file to media by file name and EFI patition name

  @param PartitionName   The EFI Partition name.
  @param FileName        The file name.
  @param FileSize        The file size.
  @param FileBuffer      The file content.
  @param Offset          The offset in file to start writing.

  @return Return  
  EFI_SUCCESS           - Set the file info successfully.
  EFI_WRITE_PROTECTED   - The disk is write protect.
  EFI_ACCESS_DENIED     - The file is read-only.
  EFI_DEVICE_ERROR      - The OFile is not valid.
  EFI_UNSUPPORTED       - The open file is not a file.
                        - The writing file size is larger than 4GB.
  other                 - An error occurred when operation.

**/
EFI_STATUS
WriteToDevice(
  CHAR16 *PartitionName,
  CHAR16 *FileName,
  UINT64 FileSize,
  VOID   *FileBuffer,
  UINTN  Offset
  )
{
  EFI_HANDLE                       *SimpleFileSystemHandles;
  UINTN                            NumberSimpleFileSystemHandles;
  EFI_HANDLE                       Device = NULL;
  EFI_STATUS                       Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *Volume;
  EFI_FILE_HANDLE                  Root = NULL;
  EFI_FILE_HANDLE                  ThisFile = NULL;
  UINTN                            Index;
  EFI_DEVICE_PATH_PROTOCOL         *DevicePath;
  CHAR16                           *DevicePathText = NULL;
  BOOLEAN                          GetPartition = FALSE;
  UINTN                            FileSize1;
  
  if ((PartitionName == NULL) || (FileName == NULL) || (FileBuffer == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  
  Status = gBS->LocateHandleBuffer(
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NumberSimpleFileSystemHandles,
                  &SimpleFileSystemHandles
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }       
  DEBUG((EFI_D_INFO, "NumberSimpleFileSystemHandles= 0x%x; SimpleFileSystemHandles= 0x%x\n", NumberSimpleFileSystemHandles, SimpleFileSystemHandles));

  for (Index = 0; Index < NumberSimpleFileSystemHandles; Index++) {
    Device = SimpleFileSystemHandles[Index];
    Root = NULL;
    ThisFile = NULL;

    DevicePath = DevicePathFromHandle(Device);
    //
    //Convert EFI path to text Path
    //
    DevicePathText = ConvertDevicePathToText(
      DevicePath,
      FALSE,
      FALSE
      );
    ASSERT(DevicePathText != NULL);

    if (StrnCmp(PartitionName, DevicePathText, StrLen(PartitionName)) == 0){
      GetPartition = TRUE;
      break;
    }
  }

  if (GetPartition){
    //
    // Handle the file system interface to the device
    //
    Status = gBS->HandleProtocol(
      Device,
      &gEfiSimpleFileSystemProtocolGuid,
      (VOID *)&Volume
      );
    if (!EFI_ERROR(Status)) {

      Status = Volume->OpenVolume(
                         Volume,
                         &Root
                         );
      if (!EFI_ERROR(Status)) {
        
        ASSERT(Root != NULL);
        Status = Root->Open(Root, &ThisFile, FileName, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, 0);
        DEBUG((EFI_D_INFO, "Root->Open_Status= 0x%x\n", Status));
        if (!EFI_ERROR(Status)) {

          ASSERT(ThisFile != NULL);
          if (Offset){
            Status = ThisFile->SetPosition(ThisFile, Offset);
            ASSERT_EFI_ERROR(Status);
          }
		  FileSize1 = (UINTN)FileSize;
          Status = ThisFile->Write(ThisFile, &FileSize1, FileBuffer);
          DEBUG((EFI_D_INFO, "FileWrite_Status= 0x%x\n", Status));
        }
      }
    }
  }

  if (ThisFile != NULL) {
    ThisFile->Close(ThisFile);
  }
  if (Root != NULL) {
    Root->Close(Root);
  }

  return Status;
}

/**
  Create directory on media by name and EFI patition name

  This function can create the directory with full path and directory name.
  
  @param PartitionName   The EFI Partition name.
  @param FileName        The directory path and name.

  @return Return  
  EFI_INVALID_PARAMETER - The FileName is NULL or the file string is empty.
  The OpenMode is not supported.
  The Attributes is not the valid attributes.
  EFI_OUT_OF_RESOURCES  - Can not allocate the memory for file string.
  EFI_SUCCESS           - Open the file successfully.
  Others                - The status of open file.
**/
EFI_STATUS
CreateDirectory(
  CHAR16 *PartitionName,
  CHAR16 *FilePath
  )
{
  EFI_HANDLE                       *SimpleFileSystemHandles;
  UINTN                            NumberSimpleFileSystemHandles;
  EFI_HANDLE                       Device = NULL;
  EFI_STATUS                       Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *Volume;
  EFI_FILE_HANDLE                  Root = NULL;
  EFI_FILE_HANDLE                  ThisFile = NULL;
  UINTN                            Index;
  EFI_DEVICE_PATH_PROTOCOL         *DevicePath;
  CHAR16                           *DevicePathText = NULL;
  BOOLEAN                          GetPartition = FALSE;
  UINTN                            FileNameLen;
  CHAR16                           ComponentName[EFI_PATH_STRING_LENGTH];
  CHAR16                           *FileName;

  if ((PartitionName == NULL) || (FilePath == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  
  Status = gBS->AllocatePool(EfiBootServicesData, StrSize(FilePath) + 2, (VOID **)&FileName);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  Status = StrnCpyS(FileName, EFI_PATH_STRING_LENGTH, FilePath, StrLen(FilePath));
  ASSERT_EFI_ERROR(Status);
  //
  // if the path end without "\", add it
  //  
  if (*(FilePath + StrLen(FilePath) - 1) != PATH_NAME_SEPARATOR){
    Status = StrCatS(FileName, StrSize(FilePath) + 2, L"\\");
    ASSERT_EFI_ERROR(Status);
  }
  
  Status = gBS->LocateHandleBuffer(
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NumberSimpleFileSystemHandles,
                  &SimpleFileSystemHandles
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }     
  DEBUG((EFI_D_INFO, "NumberSimpleFileSystemHandles= 0x%x; SimpleFileSystemHandles= 0x%x\n", NumberSimpleFileSystemHandles, SimpleFileSystemHandles));

  for (Index = 0; Index < NumberSimpleFileSystemHandles; Index++) {
    Device = SimpleFileSystemHandles[Index];
    Root = NULL;
    ThisFile = NULL;

    DevicePath = DevicePathFromHandle(Device);
    //
    //Convert EFI path to text Path
    //
    DevicePathText = ConvertDevicePathToText(
                       DevicePath,
                       FALSE,
                       FALSE
                       );
    ASSERT(DevicePathText != NULL);

    if (StrnCmp(PartitionName, DevicePathText, StrLen(PartitionName)) == 0){
      GetPartition = TRUE;
      break;
    }
  }

  if (GetPartition){
    //
    // Handle the file system interface to the device
    //
    Status = gBS->HandleProtocol(
      Device,
      &gEfiSimpleFileSystemProtocolGuid,
      (VOID *)&Volume
      );
    if (!EFI_ERROR(Status)) {

      Status = Volume->OpenVolume(
                         Volume,
                         &Root
                         );
      if (!EFI_ERROR(Status)) {

        ASSERT(Root != NULL);
        FileNameLen = 0;
        for (;;) {
          //
          // bypass "\\"
          //
          FileNameLen++;                       
          if (*(FileName + FileNameLen) == 0)
          break;
          
          while (*(FileName + FileNameLen) != PATH_NAME_SEPARATOR) {
          FileNameLen++;
          }
          Status = StrnCpyS(ComponentName, EFI_PATH_STRING_LENGTH, FileName, FileNameLen+1);
          DEBUG((EFI_D_INFO, "StrnCpyS_Status= 0x%x\n", Status));
          ASSERT_EFI_ERROR(Status);
          DEBUG((EFI_D_INFO, "ComponentName= %s\n", ComponentName));

          Status = Root->Open(Root, &ThisFile, ComponentName, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, EFI_FILE_DIRECTORY);
        }

        DEBUG((EFI_D_INFO, "FileWrite_Status= 0x%x\n", Status));
      }
    }
  }

  if (ThisFile != NULL) {
    ThisFile->Close(ThisFile);
  }
  if (Root != NULL) {
    Root->Close(Root);
  }

  return Status;
}

/**
  delete file/directory on media by file/directory name and EFI patition Path

  This function can only delete one file or empty directory. 

  @param PartitionName   The EFI Partition name.
  @param FileName        The file or directory name.

  @return Return  
  EFI_SUCCESS                     - Delete the file successfully.
  EFI_WARN_DELETE_FAILURE         - Fail to delete the file.
  @retval  EFI_INVALID_PARAMETER  Invalid input Paras.

**/
EFI_STATUS
DeleteFileOnDevice(
  CHAR16 *PartitionName,
  CHAR16 *FileName
  )
{
  EFI_HANDLE                       *SimpleFileSystemHandles;
  UINTN                            NumberSimpleFileSystemHandles;
  EFI_HANDLE                       Device = NULL;
  EFI_STATUS                       Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *Volume;
  EFI_FILE_HANDLE                  Root;
  EFI_FILE_HANDLE                  ThisFile;
  UINTN                            Index;
  EFI_DEVICE_PATH_PROTOCOL         *DevicePath;
  CHAR16                           *DevicePathText = NULL;
  BOOLEAN                          GetPartition = FALSE;

  if ((PartitionName == NULL) || (FileName == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // if the path end with "\", remove it
  //
  if(*(FileName + StrLen(FileName) - 1) == PATH_NAME_SEPARATOR){
    *(FileName + StrLen(FileName) - 1) = 0x0;
  }

  Status = gBS->LocateHandleBuffer(
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NumberSimpleFileSystemHandles,
                  &SimpleFileSystemHandles
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }              
  DEBUG((EFI_D_INFO, "NumberSimpleFileSystemHandles= 0x%x; SimpleFileSystemHandles= 0x%x\n", NumberSimpleFileSystemHandles, SimpleFileSystemHandles));

  for (Index = 0; Index < NumberSimpleFileSystemHandles; Index++) {
    Device = SimpleFileSystemHandles[Index];
    Root = NULL;
    ThisFile = NULL;

    DevicePath = DevicePathFromHandle(Device);
    //
    //Convert EFI path to text Path
    //
    DevicePathText = ConvertDevicePathToText(
      DevicePath,
      FALSE,
      FALSE
      );
    ASSERT(DevicePathText != NULL);

    if (StrnCmp(PartitionName, DevicePathText, StrLen(PartitionName)) == 0){
      GetPartition = TRUE;
      break;
    }
  }

  if (GetPartition){
    //
    // Handle the file system interface to the device
    //
    Status = gBS->HandleProtocol(
      Device,
      &gEfiSimpleFileSystemProtocolGuid,
      (VOID *)&Volume
      );
    if (!EFI_ERROR(Status)) {
      Status = Volume->OpenVolume(
                         Volume,
                         &Root
                         );
      if (!EFI_ERROR(Status)) {

        ASSERT(Root != NULL);
        Status = Root->Open(Root, &ThisFile, FileName, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, 0);
        DEBUG((EFI_D_INFO, "FileOpen_Status= 0x%x\n", Status));
        if (!EFI_ERROR(Status)) {

          ASSERT(ThisFile != NULL);
          Status = ThisFile->Delete(ThisFile);
          DEBUG((EFI_D_INFO, "FileDelete_Status= 0x%x\n", Status));
        }
      }
    }
  } else {
    return EFI_NOT_FOUND;
  }

  if (Root != NULL) {
    Root->Close(Root);
  }

  return Status;
}

/**
  Run/Boot EFI PE file from media by file name and EFI patition path

  @param PartitionName     The EFI media Partition path.
  @param FileName          The file name.
  @param OptionalData      The pointer of comdline input Para.
  @param OptionalDataSize  The input Para Length

  @retval  EFI_SUCCESS            Run/Boot EFI PE file successfully.
  @retval  EFI_NOT_FOUND          The specified file could not be found.
  @retval  EFI_OUT_OF_RESOURCES   There are not enough resources available to retrieve 
                                  the matching FFS section.
  @retval  RETURN_LOAD_ERROR      The image fail to load.
  @retval  EFI_INVALID_PARAMETER  Invalid input Paras.

**/
EFI_STATUS
RunFromEfiPeFile(
  IN CHAR16   *PartitionName,
  IN CHAR16   *FileName,
  IN UINT16   *OptionalData,
  IN UINT32   OptionalDataSize
  )
{
  UINT8                         *FileBuffer = NULL;
  UINTN                         FileSize = 0;
  EFI_HANDLE                    ImageHandle = NULL;
  EFI_STATUS                    Status;
  EFI_LOADED_IMAGE_PROTOCOL     *ImageInfo;
  CHAR16                        *FullPathName;
  EFI_DEVICE_PATH_PROTOCOL      *FilePath;
  UINTN                         ExitDataSize;
  CHAR16                        *ExitData;
  UINTN                         Size1;
  UINTN                         Size2;
  UINTN                         MaxLen;

  if ((PartitionName == NULL) || (FileName == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  
  FileBuffer = ReadFromDevice(PartitionName, FileName, 0x0, &FileSize);
  if ((FileBuffer == NULL) || (FileSize == 0)) {
    return EFI_NOT_FOUND;
  }  

  Size1 = StrSize(PartitionName);
  Size2 = StrSize(FileName);
  MaxLen = (Size1 + Size2 + sizeof (CHAR16)) / sizeof (CHAR16);
  FullPathName = AllocateZeroPool(Size1 + Size2 + sizeof (CHAR16));
  if (FullPathName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  
  StrCpyS(FullPathName, MaxLen, PartitionName);
  StrCatS(FullPathName, MaxLen, FileName);
  DEBUG((EFI_D_INFO, " FullPathName = %s\n", FullPathName));
  //
  //Convert text path to EFI Path
  //
  FilePath = ConvertTextToDevicePath(FullPathName);
  ASSERT(FilePath != NULL);

  Status = gBS->LoadImage(
                    TRUE,
                    gImageHandle,
                    FilePath,
                    FileBuffer,
                    FileSize,
                    &ImageHandle
                    );
  if (FileBuffer != NULL) {
    FreePool(FileBuffer);
  }
  if (FilePath != NULL) {
    FreePool(FilePath);
  }
  if (EFI_ERROR(Status)) {
    return Status;
  }
  Status = gBS->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **)&ImageInfo);
  ASSERT_EFI_ERROR(Status);

  if (OptionalDataSize && OptionalData != NULL) {
    ImageInfo->LoadOptionsSize = OptionalDataSize;
    ImageInfo->LoadOptions = OptionalData;
  }
  //
  // Clean to NULL because the image is loaded directly from the firmwares boot manager.
  //
  ImageInfo->ParentHandle = NULL;
  //
  // Before calling the image, enable the Watchdog Timer for 5 minutes period
  //
  gBS->SetWatchdogTimer(5 * 60, 0x0000, 0x00, NULL);
  //
  // Need to down TPL to TPL_APPLICATION for running Image
  //

  Status = gBS->StartImage(ImageHandle, &ExitDataSize, &ExitData);
  DEBUG((DEBUG_INFO | DEBUG_LOAD, "Image Return Status = %r\n", Status));
  //
  // When go back, means to continue running BIOS booting and it is in call back function,
  // Raise to TPL_CALLBACK
  //

  return Status;
}

/**
  This function search all the folders/files under specific directory path and get back their info.
  the info is the table of EFI_FILE_INFO.

  @param PartitionName     The pointer of EFI Partition path.
  @param FileName          The pointer of directory path name.
  @param BufferLenth       The pointer of pointer array size.

  @return Return the address of pointer array which include 
  EFI_FILE_INFO pointers for directory info.

**/
VOID *
ReadDirectoryFromDevice (
  IN  CHAR16 *PartitionName,
  IN  CHAR16 *FileName,
  OUT UINTN  *BufferLenth
  )
{
  EFI_HANDLE                       *SimpleFileSystemHandles;
  UINTN                            NumberSimpleFileSystemHandles;
  EFI_HANDLE                       Device = NULL;
  EFI_STATUS                       Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *Volume;
  EFI_FILE_HANDLE                  Root = NULL;
  EFI_FILE_HANDLE                  ThisFile = NULL;
  EFI_FILE_INFO                    *InfoBuffer;
  EFI_FILE_INFO                    *Info;
  UINTN                            FileSize;
  UINTN                            Index;
  EFI_FILE_INFO                    **DirInfo;
  EFI_DEVICE_PATH_PROTOCOL         *DevicePath;
  CHAR16                           *DevicePathText = NULL;
  BOOLEAN                          GetPartition = FALSE;
  BOOLEAN                          NoFile;

  if ((PartitionName == NULL) || (FileName == NULL) || (BufferLenth == NULL)) {
    return NULL;
  }
  
  Info = NULL;
  *BufferLenth = 0;
  DirInfo = NULL;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NumberSimpleFileSystemHandles,
                  &SimpleFileSystemHandles
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }                  

  for (Index = 0; Index < NumberSimpleFileSystemHandles; Index++) {
    Device = SimpleFileSystemHandles[Index];
    Root = NULL;
    ThisFile = NULL;
    //
    // Get device Path and convert to text
    //
    DevicePath = DevicePathFromHandle(Device);
    DevicePathText = ConvertDevicePathToText (
      DevicePath,
      FALSE,
      FALSE
      );
    ASSERT(DevicePathText != NULL);
    //
    // Check if the device path matches input one
    //
    if (StrnCmp(PartitionName, DevicePathText, StrLen(PartitionName)) == 0) {
      GetPartition = TRUE;
      break;
    }
  }
  
  Status = EFI_ACCESS_DENIED;
  if (GetPartition) {
    //
    // Handle the file system interface to the device
    //
    Status = gBS->HandleProtocol(
      Device,
      &gEfiSimpleFileSystemProtocolGuid,
      (VOID *)&Volume
      );
    if (!EFI_ERROR(Status)) {

      Status = Volume->OpenVolume(
                         Volume,
                         &Root
                         );
      if (!EFI_ERROR(Status)) {

        ASSERT(Root != NULL);
    
        Status = Root->Open(Root, &ThisFile, FileName, EFI_FILE_MODE_READ, 0);
        if (!EFI_ERROR(Status)) {
          ASSERT(ThisFile != NULL);
          //
          // Count the number of entries in this folder
          //
          Index = 0;
          Status = FileHandleFindFirstFile(ThisFile, &InfoBuffer);
          if (!EFI_ERROR(Status)) {
            Index ++;
            while (1) {
              NoFile = FALSE;
              Status = FileHandleFindNextFile(ThisFile, InfoBuffer, &NoFile);
              if (!EFI_ERROR(Status) && (!NoFile)) {
                Index ++;
              } else {
                break;
              }
            }
          }
          Status = gBS->AllocatePool(EfiBootServicesData, Index * sizeof (UINTN), (VOID **)&DirInfo);
          ASSERT_EFI_ERROR(Status);
          if (EFI_ERROR(Status)) {
            return NULL;
          }

          Status = ThisFile->SetPosition(ThisFile, 0);
          ASSERT_EFI_ERROR(Status);

          //
          // Retrieve file info of the files in the folder
          //
          Index = 0;
          Status = FileHandleFindFirstFile(ThisFile, &InfoBuffer);
          if (!EFI_ERROR(Status)) {
            //
            //suppose size is EFI_FILE_INFO + EFI_PATH_STRING_LENGTH to allocate buffer
            //
            FileSize = SIZE_OF_EFI_FILE_INFO + EFI_PATH_STRING_LENGTH;
            Status = gBS->AllocatePool(EfiBootServicesData, FileSize, (VOID **)&Info);
            if (!EFI_ERROR(Status)) {
              CopyMem(Info, InfoBuffer, FileSize);
              DirInfo[Index++] = Info;
            }
            while (1) {
              NoFile = FALSE;
              Status = FileHandleFindNextFile(ThisFile, InfoBuffer, &NoFile);
              if (!EFI_ERROR(Status) && (!NoFile)) {
                FileSize = SIZE_OF_EFI_FILE_INFO + EFI_PATH_STRING_LENGTH;
                Status = gBS->AllocatePool(EfiBootServicesData, FileSize, (VOID **)&Info);
                ASSERT_EFI_ERROR(Status);
                if (!EFI_ERROR(Status)) {
                  CopyMem(Info, InfoBuffer, FileSize);
                  DirInfo[Index++] = Info;
                }
              } else {
                break;
              }
            }
          }
          if (!EFI_ERROR(Status)) {
            Status = ThisFile->SetPosition(ThisFile, 0);
            ASSERT_EFI_ERROR(Status);
          }
        } else {
          Status = EFI_ACCESS_DENIED;
        }
      }
    }  
  }

  if(!EFI_ERROR(Status)){
    if (Index > 0){
      //
      // At least one item
      //
      *BufferLenth = Index * sizeof(UINTN); 
    }
  }
  if (ThisFile != NULL) {
    ThisFile->Close(ThisFile);
  }
  if (Root != NULL) {
    Root->Close(Root);
  }

  return (VOID*) DirInfo;
}

/**
  Searches all the available firmware volumes and returns the first matching FFS section. 

  This function searches all the firmware volumes for FFS files with an FFS filename specified by NameGuid.  
  The order in which the firmware volumes are searched is not deterministic. For each FFS file found, a search 
  is made for FFS sections of type SectionType. If the FFS file contains at least SectionInstance instances 
  of the FFS section specified by SectionType, then the SectionInstance instance is returned in Buffer. 
  Buffer is allocated using AllocatePool(), and the size of the allocated buffer is returned in Size. 
  It is the caller's responsibility to use FreePool() to free the allocated buffer.  
  See EFI_FIRMWARE_VOLUME2_PROTOCOL.ReadSection() for details on how sections 
  are retrieved from an FFS file based on SectionType and SectionInstance.

  If SectionType is EFI_SECTION_TE, and the search with an FFS file fails, 
  the search will be retried with a section type of EFI_SECTION_PE32.
  This function must be called with a TPL <= TPL_NOTIFY.

  If NameGuid is NULL, then ASSERT().
  If Buffer is NULL, then ASSERT().
  If Size is NULL, then ASSERT().


  @param  NameGuid                A pointer to to the FFS filename GUID to search for  
                                  within any of the firmware volumes in the platform. 
  @param  Buffer                  On output, a pointer to a callee-allocated buffer 
                                  containing the FFS file section that was found.  
                                  It is the caller's responsibility to free this 
                                  buffer using FreePool().
  @param  Size                    On output, a pointer to the size, in bytes, of Buffer.

  @retval  EFI_SUCCESS            The specified FFS section was returned.
  @retval  EFI_NOT_FOUND          The specified FFS section could not be found.
  @retval  EFI_OUT_OF_RESOURCES   There are not enough resources available to retrieve 
                                  the matching FFS section.
  @retval  EFI_DEVICE_ERROR       The FFS section could not be retrieves due to a 
                                  device error.
  @retval  EFI_ACCESS_DENIED      The FFS section could not be retrieves because the 
                                  firmware volume that contains the matching FFS 
                                  section does not allow reads.
  @retval  EFI_INVALID_PARAMETER  Invalid input Paras.                               
**/ 
EFI_STATUS
EFIAPI
GetFileFromFlash (
  IN  EFI_GUID             *NameGuid,
  OUT VOID                 **Buffer,
  OUT UINTN                *Size
  )
{
  EFI_STATUS               Status;

  if ((NameGuid == NULL) || (Buffer == NULL) || (Size == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  Status = GetSectionFromAnyFv(NameGuid, EFI_SECTION_RAW, 0, (VOID **)Buffer, Size);
  
  return Status;
}

/**
This function get all the File Partition info on the connected storages and get back.
the info is the EFI device Path String table of Char*.

@param  BufferLenth               On output, a pointer to the size, in bytes,

@return Return the address of pointer arrary which include
String pointers for Partition device Path info.

**/
VOID *
GetAllPartitionPaths(
	OUT UINT64 *BufferLenth
)
{
	EFI_HANDLE                       *SimpleFileSystemHandles;
	UINTN                            NumberSimpleFileSystemHandles;
	EFI_HANDLE                       Device = NULL;
	EFI_STATUS                       Status;
	UINTN                            Index;
	CHAR16                           **ParInfo;
	EFI_DEVICE_PATH_PROTOCOL         *DevicePath;
	CHAR16                           *DevicePathText = NULL;

	*BufferLenth = 0;
	ParInfo = NULL;
	DEBUG((EFI_D_INFO, "Enter GetAllPartitionPaths \n"));

	Status = gBS->LocateHandleBuffer(
		ByProtocol,
		&gEfiSimpleFileSystemProtocolGuid,
		NULL,
		&NumberSimpleFileSystemHandles,
		&SimpleFileSystemHandles
	);
	DEBUG((EFI_D_INFO, "Status= 0x%x \n", Status));
	if (EFI_ERROR(Status)) {
		return NULL;
	}

  if (NumberSimpleFileSystemHandles) {
		Status = gBS->AllocatePool(EfiBootServicesData, NumberSimpleFileSystemHandles * sizeof (UINTN), (VOID **)&ParInfo);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR (Status)) {
      return NULL;
    }
  }
	DEBUG((EFI_D_INFO, "NumberSimpleFileSystemHandles= 0x%x; SimpleFileSystemHandles= 0x%x\n", NumberSimpleFileSystemHandles, SimpleFileSystemHandles));
	for (Index = 0; Index < NumberSimpleFileSystemHandles; Index++) {
		Device = SimpleFileSystemHandles[Index];
		//
		// Get device Path and convert to text
		//
		DevicePath = DevicePathFromHandle(Device);
		DevicePathText = ConvertDevicePathToText(
			DevicePath,
			FALSE,
			FALSE
		);
		ASSERT(DevicePathText != NULL);
		DEBUG((EFI_D_INFO, "DevicePathText = %s;\n", DevicePathText));

		ParInfo[Index] = DevicePathText;

	}

	*BufferLenth = Index * sizeof(UINTN);

	return (VOID*) ParInfo;
}

