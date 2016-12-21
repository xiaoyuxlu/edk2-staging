/** @file
  Test Framework installer.

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
#include <Library/ShellCommandLib.h>
#include <Protocol/ShellParameters.h>
#include <Guid/FileSystemInfo.h>

//
// Global definitions
//
#define INSTALL_SCT_FREE_SPACE          (100 * SIZE_1MB)
#define INSTALL_SCT_MAX_FILE_SYSTEM     0xFF
#define INSTALL_SCT_STARTUP_FILE        L"SctStartup.nsh"

///
///
///
UINTN  Argc;
CHAR16 **Argv;

/**

  This function parse application ARG.

  @return Status
**/
EFI_STATUS
GetArg (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_SHELL_PARAMETERS_PROTOCOL *ShellParameters;

  Status = gBS->OpenProtocol(
                  gImageHandle,
                  &gEfiShellParametersProtocolGuid,
                  (VOID **)&ShellParameters,
                  gImageHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Argc = ShellParameters->Argc;
  Argv = ShellParameters->Argv;

  gBS->CloseProtocol(
         gImageHandle,
         &gEfiShellParametersProtocolGuid,
         gImageHandle,
         NULL
         );
  return EFI_SUCCESS;
}

VOID
PrintUsage (
  VOID
  )
{
  Print (
    L"Install Test Infrastructure Harness, Version 0.9\n"
    L"\n"
    L"Notes: Make sure the shell commands CP, DEL, and MV are enabled.\n"
    L"       They are used in this installation.\n"
    L"\n"
    L"Usage: InstallSct\n"
    L"\n"
    );
}

EFI_STATUS
GetFreeSpace (
  IN CHAR16             *FsName,
  OUT UINT64            *FreeSpace
  )
{
  EFI_STATUS            Status;
  SHELL_FILE_HANDLE     ShellFileHandle;
  EFI_FILE_SYSTEM_INFO  *SysInfo;
  UINTN                 SysInfoSize;
  EFI_FILE_PROTOCOL     *EfiFpHandle;

  //
  // Get the device path of file system
  //
  Status = ShellOpenFileByName (FsName, &ShellFileHandle, EFI_FILE_MODE_READ, 0);
  if (EFI_ERROR(Status) || ShellFileHandle == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Get the Volume Info from ShellFileHandle
  //
  SysInfo     = NULL;
  SysInfoSize = 0;
  EfiFpHandle = ConvertShellHandleToEfiFileProtocol(ShellFileHandle);
  Status = EfiFpHandle->GetInfo(
                          EfiFpHandle,
                          &gEfiFileSystemInfoGuid,
                          &SysInfoSize,
                          SysInfo
                          );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    SysInfo = AllocateZeroPool(SysInfoSize);
    Status = EfiFpHandle->GetInfo(
                            EfiFpHandle,
                            &gEfiFileSystemInfoGuid,
                            &SysInfoSize,
                            SysInfo
                            );
  }

  if (SysInfo == NULL) {
    ShellCloseFile (&ShellFileHandle);
    return EFI_OUT_OF_RESOURCES;
  }

  *FreeSpace = SysInfo->FreeSpace;

  FreePool (SysInfo);

  ShellCloseFile (&ShellFileHandle);

  return EFI_SUCCESS;
}


EFI_STATUS
RemoveDirFile (
  IN CHAR16             *FsName,
  IN CHAR16             *Path
  )
{
  EFI_STATUS  Status;
  EFI_STATUS  ExecuteStatus;
  CHAR16      *CmdLine;

  Print (L"RemoveDirFile() FsName = %s  Path = %s\n", FsName, Path);

  //
  // Create command line to delete this directory or file
  //
  CmdLine = CatSPrint (
              NULL,
              L"rm -q \"%s\\%s\"",
              FsName,
              Path
              );
  if (CmdLine == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Execute this shell command
  //
  Print (L"ShellExecute %s\n", CmdLine);
  Status = ShellExecute (
             &gImageHandle,
             CmdLine,
             FALSE,
             NULL,
             &ExecuteStatus
             );
  if (EFI_ERROR (ExecuteStatus)) {
    Print (L"Error: Could not execute \"%s\"\n", CmdLine);
  }
  FreePool (CmdLine);
  return ExecuteStatus;
}


EFI_STATUS
BackupDirFile (
  IN CHAR16  *FsName,
  IN CHAR16  *Path
  )
{
  EFI_STATUS  Status;
  UINTN       Index;
  CHAR16      *TmpName;

  EFI_STATUS  ExecuteStatus;
  CHAR16      *CmdLine;

  Print (L"BackupDirFile() FsName = %s  Path = %s\n", FsName, Path);

  //
  // Find the latest backup number
  //
  for (Index = 0; Index < MAX_UINT32; Index ++) {
    //
    // Create the backup file name
    //
    TmpName = CatSPrint (
                NULL,
                L"%s\\bak%08X.%s",
                FsName,
                Index,
                Path
                );
    if (TmpName == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    Status = ShellIsDirectory (TmpName);
    if (!EFI_ERROR (Status)) {
      FreePool (TmpName);
      continue;
    }
    Status = ShellIsFile (TmpName);
    if (!EFI_ERROR (Status)) {
      FreePool (TmpName);
      continue;
    }
    //
    // Exit look on first name that is not a directory or a file
    //
    FreePool (TmpName);
    break;
  }

  //
  // Check the latest backup number
  //
  if (Index == MAX_UINT32) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Create command line to backup it
  //
  CmdLine = CatSPrint (
              NULL,
              L"mv \"%s\\%s\" \"%s\\bak%08X.%s\"",
              FsName,
              Path,
              FsName,
              Index,
              Path
              );
  if (CmdLine == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Execute shell command
  //
  Print (L"ShellExecute %s\n", CmdLine);
  Status = ShellExecute (
             &gImageHandle,
             CmdLine,
             FALSE,
             NULL,
             &ExecuteStatus
             );
  if (EFI_ERROR (ExecuteStatus)) {
    Print (L"Error: Could not execute \"%s\"\n", CmdLine);
  }
  FreePool (CmdLine);

  //
  // Done
  //
  return ExecuteStatus;
}


EFI_STATUS
CopyDirFile (
  IN CHAR16   *SrcFsName,
  IN CHAR16   *SrcName,
  IN CHAR16   *DstFsName,
  IN CHAR16   *DstName,
  IN BOOLEAN  Recursive
  )
{
  EFI_STATUS         Status;
  EFI_STATUS         ExecuteStatus;
  CHAR16             *CmdLine;
  CHAR16             *SrcPathName;
  CHAR16             *DstPathName;
  CHAR16             *ParentPath;
  UINTN              Index;
  SHELL_FILE_HANDLE  ShellFileHandle;

  Print (L"CopyDirFile() SrcFsName = %s  SrcName = %s  DstFsName = %s  DstName = %s\n", SrcFsName, SrcName, DstFsName, DstName);

  SrcPathName = CatSPrint (NULL, L"%s\\%s", SrcFsName, SrcName);
  if (SrcPathName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  DstPathName = CatSPrint (NULL, L"%s\\%s", DstFsName, DstName);
  if (DstPathName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Print (L"SrcPathName = %s\n", SrcPathName);
  Print (L"DstPathName = %s\n", DstPathName);

  //
  // Create the parent directory
  //
  ParentPath = CatSPrint (NULL, L"%s", DstPathName);
  if (!Recursive) {
    for (Index = StrLen(ParentPath); Index > 0; Index--) {
      if (ParentPath[Index - 1] == L'\\' || ParentPath[Index - 1] == L'/') {
        continue;
      }
      ParentPath[Index] = 0;
      break;
    }
    for (Index = StrLen(ParentPath); Index > 0; Index--) {
      if (ParentPath[Index - 1] == L'\\' || ParentPath[Index - 1] == L'/') {
        ParentPath[Index - 1] = 0;
        break;
      }
    }
  }
  Status = ShellIsDirectory (ParentPath);
  if (EFI_ERROR (Status)) {
    Print (L"CreateDirectory %s\n", ParentPath);
    Status = ShellCreateDirectory (ParentPath, &ShellFileHandle);
    if (EFI_ERROR (Status)) {
      Print (L"  CreateDirectory %s FAILED\n", ParentPath);
      FreePool (ParentPath);
      FreePool (SrcPathName);
      FreePool (DstPathName);
      return Status;
    }
    ShellCloseFile (&ShellFileHandle);
    Print (L"  CreateDirectory %s SUCCESS\n", ParentPath);
  }
  FreePool (ParentPath);

  //
  // Create command line to copy it
  //
  CmdLine = CatSPrint (
              NULL,
              L"cp %s -q \"%s\" \"%s\"",
              Recursive ? L"-r" : L"",
              SrcPathName,
              DstPathName
              );
  if (CmdLine == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  FreePool (SrcPathName);
  FreePool (DstPathName);

  //
  // Execute shell command
  //
  Print (L"ShellExecute %s\n", CmdLine);
  Status = ShellExecute (
             &gImageHandle,
             CmdLine,
             FALSE,
             NULL,
             &ExecuteStatus
             );
  if (EFI_ERROR (ExecuteStatus)) {
    Print (L"Error: Could not execute \"%s\"\n", CmdLine);
    FreePool (CmdLine);
    return ExecuteStatus;
  }

  FreePool (CmdLine);

  //
  // Done
  //
  return EFI_SUCCESS;
}

EFI_STATUS
GetDestination (
  OUT CHAR16            **FsName
  )
{
  EFI_STATUS  Status;
  UINTN       Index;
  CHAR16      *TmpName;
  UINTN       MaxExistIndex;
  UINTN       ExistFs[INSTALL_SCT_MAX_FILE_SYSTEM];
  UINT64      FreeSpaces[INSTALL_SCT_MAX_FILE_SYSTEM];
  CHAR16      *Response;

  //
  // Search each file system
  //
  MaxExistIndex = 0;

  for (Index = 0; Index < INSTALL_SCT_MAX_FILE_SYSTEM; Index ++) {
    //
    // NOTE: Here the file system name is hard coded. I don't know how to find
    // all file system names via a shell service.
    //
    FreeSpaces[Index] = 0;

    //
    // Create the name of file system
    //
    TmpName = CatSPrint (NULL, L"fs%x:\\", Index);
    if (TmpName == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    //
    // Get free space
    //
    Status = GetFreeSpace (TmpName, &FreeSpaces[Index]);
    if (EFI_ERROR (Status)) {
      FreePool (TmpName);
      continue;
    }

    ExistFs[MaxExistIndex] = Index;
    MaxExistIndex ++;

    //
    // Print system information
    //
    Print (
      L"  %d: %s: (Free Space: %d MB)\n",
      MaxExistIndex,
      TmpName,
      (UINTN) DivU64x32Remainder (FreeSpaces[Index], SIZE_1MB, NULL)
      );

    FreePool (TmpName);
  }

  Print (
    L"  Space Required: %d MB\n",
    INSTALL_SCT_FREE_SPACE / SIZE_1MB
    );

  //
  // User must choice a required file system
  //
  while (TRUE) {
    //
    // Input the index of destination file system
    //
    ShellPromptForResponse (
      ShellPromptResponseTypeFreeform,
      L"Input index of destination FS. 'q' to exit:",
      &Response
      );
    Print (L"\n");

    //
    // Deal with the user input
    //
    if (StrCmp (Response, L"q") == 0 || StrCmp (Response, L"Q") == 0) {
      FreePool (Response);
      Status = EFI_ABORTED;
      break;
    }

    //
    // Convert the input to an index
    //
    Index = ShellStrToUintn(Response) - 1;
    FreePool (Response);
    if (Index >= MaxExistIndex) {
      Print (L"  Only 1 to %d is valid.\n", MaxExistIndex);
      continue;
    }

    //
    // Check the free space
    //
    if (FreeSpaces[ExistFs[Index]] < (UINT64) INSTALL_SCT_FREE_SPACE) {
      Print (L"  Not enough free space.\n");
      continue;
    }

    Status = EFI_SUCCESS;
    break;
  }

  //
  // Create the destination directory
  //
  *FsName = CatSPrint (NULL, L"FS%x:", ExistFs[Index]);
  if (*FsName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Done
  //
  return Status;
}

EFI_STATUS
BackupTests (
  IN CHAR16          *FsName
  )
{
  EFI_STATUS         Status;
  CHAR16             *TmpName;
  CHAR16             *Prompt;
  CHAR16             *Response;

  Print (L"BackupTests()  FsName = %s\n", FsName);

  TmpName = CatSPrint (NULL, L"%s\\SCT", FsName);
  if (TmpName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = ShellIsDirectory (TmpName);
  if (EFI_ERROR (Status)) {
    Print (L"  SCT dir does not exist.  Skip backup\n");
    FreePool (TmpName);
    return EFI_SUCCESS;
  }

  //
  // Create prompt string
  //
  Prompt = CatSPrint (
             NULL,
             L"Found the existing test directory '%s'.\n"
             L"Select (B)ackup or (R)emove. 'q' to exit:",
             TmpName
             );
  if (Prompt == NULL) {
    FreePool (TmpName);
    return EFI_OUT_OF_RESOURCES;
  }

  while (TRUE) {
    //
    // User input his selection
    //
    ShellPromptForResponse (
      ShellPromptResponseTypeFreeform,
      Prompt,
      &Response
      );
    Print (L"\n");

    //
    // Deal with the user input
    //
    if (StrCmp (Response, L"q") == 0 || StrCmp (Response, L"Q") == 0) {
      Status = EFI_ABORTED;
      break;
    }
    if (StrCmp (Response, L"b") == 0 || StrCmp (Response, L"B") == 0) {
      //
      // Backup or Backup All
      //
      Status = BackupDirFile (FsName, L"SCT");
      break;
    }
    if (StrCmp (Response, L"r") == 0 || StrCmp (Response, L"R") == 0) {
      //
      // Remove or Remove All
      //
      Status = RemoveDirFile (FsName, L"SCT");
      break;
    }
    FreePool (Response);
  }

  //
  // Done
  //
  FreePool (TmpName);
  FreePool (Prompt);
  FreePool (Response);
  return Status;
}

EFI_STATUS
BackupStartups (
  IN CHAR16             *FsName
  )
{
  EFI_STATUS  Status;
  CHAR16      *TmpName;
  CHAR16      *Prompt;
  CHAR16      *Response;

  Print (L"BackupStartups()  FsName = %s\n", FsName);

  TmpName = CatSPrint (NULL, L"%s\\Startup.nsh", FsName);
  if (TmpName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = ShellFileExists (TmpName);
  if (EFI_ERROR (Status)) {
    Print (L"  Startup.nsh file does not exist.  Skip backup startups\n");
    FreePool (TmpName);
    return EFI_SUCCESS;
  }

  //
  // Create prompt string
  //
  Prompt = CatSPrint (
             NULL,
             L"Found the existing startup script '%s'.\n"
             L"Select (B)ackup or (R)emove. 'q' to exit:",
             TmpName
             );
  if (Prompt == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  while (TRUE) {
    //
    // User input his selection
    //
    ShellPromptForResponse (
      ShellPromptResponseTypeFreeform,
      Prompt,
      &Response
      );
    Print (L"\n");

    //
    // Deal with the user input
    //
    if (StrCmp (Response, L"q") == 0 || StrCmp (Response, L"Q") == 0) {
      Status = EFI_ABORTED;
      break;
    }
    if (StrCmp (Response, L"b") == 0 || StrCmp (Response, L"B") == 0) {
      //
      // Backup or Backup All
      //
      Status = BackupDirFile (FsName, L"Startup.nsh");
      break;
    }
    if (StrCmp (Response, L"r") == 0 || StrCmp (Response, L"R") == 0) {
      //
      // Remove or Remove All
      //
      Status = RemoveDirFile (FsName, L"Startup.nsh");
      break;
    }
    FreePool (Response);
  }

  //
  // Done
  //
  FreePool (TmpName);
  FreePool (Prompt);
  FreePool (Response);
  return Status;
}

EFI_STATUS
InstallTest (
  IN CHAR16  *FsName
  )
{
  EFI_STATUS    Status;
  const CHAR16  *CurrentWorkingDirectory;
  CHAR16        *SrcFsName;
  CHAR16        *SrcPath;
  CHAR16        *InstallerApp;
  UINTN         Index;

  Print (L"InstallTest()  FsName = %s\n", FsName);

  //
  // Copy the EFI SCT Harness
  //
  CurrentWorkingDirectory = ShellGetCurrentDir (NULL);
  SrcFsName = CatSPrint (NULL, L"%s", CurrentWorkingDirectory);
  if (SrcFsName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  for (Index = 0; SrcFsName[Index] != 0;  Index++) {
    if (SrcFsName[Index] == L':') {
      SrcPath = CatSPrint (NULL, L"%s", &SrcFsName[Index + 1]);
      if (SrcPath == NULL) {
        FreePool (SrcFsName);
        return EFI_OUT_OF_RESOURCES;
      }
      SrcFsName[Index + 1] = 0;
      break;
    }
  }

  InstallerApp = NULL;
  for (Index = StrLen(SrcPath); Index > 0; Index--) {
    if (SrcPath[Index - 1] == L'\\' || SrcPath[Index - 1] == L'/') {
      break;
    }
  }
  if (Index > 0) {
    InstallerApp = CatSPrint (NULL, L"SCT\\%s\\InstallSct.efi", &SrcPath[Index]);
  }

  Print (L"SrcFsName = %s\n", SrcFsName);
  Print (L"SrcPath   = %s\n", SrcPath);

  Status = CopyDirFile (
             SrcFsName,
             SrcPath,
             FsName,
             L"SCT",
             TRUE                           // Recursive
             );

  //
  // Remove installer from install directory
  //
  if (InstallerApp != NULL) {
    Print (L"InstallerApp = %s\n", InstallerApp);

    Status = RemoveDirFile (FsName, InstallerApp);

    FreePool (InstallerApp);
  }

  FreePool (SrcFsName);
  FreePool (SrcPath);
  return Status;
}

EFI_STATUS
InstallStartup (
  IN CHAR16             *FsName
  )
{
  EFI_STATUS    Status;
  const CHAR16  *CurrentWorkingDirectory;
  CHAR16        *SrcFsName;
  CHAR16        *SrcPath;
  UINTN         Index;

  Print (L"InstallStartup()  FsName = %s\n", FsName);

  //
  // Copy the startup script file
  //
  CurrentWorkingDirectory = ShellGetCurrentDir (NULL);
  SrcFsName = CatSPrint (NULL, L"%s", CurrentWorkingDirectory);
  if (SrcFsName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  for (Index = 0; SrcFsName[Index] != 0;  Index++) {
    if (SrcFsName[Index] == L':') {
      SrcPath = CatSPrint (NULL, L"%s\\..\\%s", &SrcFsName[Index + 1], INSTALL_SCT_STARTUP_FILE);
      if (SrcPath == NULL) {
        FreePool (SrcFsName);
        return EFI_OUT_OF_RESOURCES;
      }
      SrcFsName[Index + 1] = 0;
      break;
    }
  }

  Print (L"SrcFsName = %s\n", SrcFsName);
  Print (L"SrcPath   = %s\n", SrcPath);

  Status = CopyDirFile (
             SrcFsName,
             SrcPath,
             FsName,
             L"Startup.nsh",
             FALSE                           // Recursive
             );
  FreePool (SrcFsName);
  FreePool (SrcPath);
  return Status;
}

//
// Entry point
//
EFI_STATUS
EFIAPI
InstallSct (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status;
  CHAR16      *FsName;

  GetArg ();

  //
  // Check parameters
  //
  if (Argc != 1) {
    PrintUsage ();
    return EFI_SUCCESS;
  }

  //
  // Get the destination directory
  //
  Print (L"\nGather system information ...\n");

  Status = GetDestination (&FsName);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Remove or backup the existing tests
  //
  Print (L"\nBackup the existing tests ...\n");

  Status = BackupTests (FsName);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Remove or backup the startup files
  //
  Status = BackupStartups (FsName);
  if (EFI_ERROR (Status)) {
    goto Done;
  }


  //
  // Install the EFI SCT Harness
  //
  Print (L"\nInstalling...\n");

  Status = InstallTest (FsName);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Install the startup file
  //
  Status = InstallStartup (FsName);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Done
  //
Done:
  Print (L"\nDONE!\n");
  FreePool (FsName);
  return EFI_SUCCESS;
}
