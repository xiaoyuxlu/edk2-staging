/** @file
  EFI SCT Framework

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Sct.h"

//
// Global veriables definition
//

EFI_SCT_FRAMEWORK_TABLE *gFT = NULL;

EFI_GUID gEfiSystemHangAssertionGuid = EFI_SYSTEM_HANG_ASSERTION_GUID;

UINTN gTestCaseMaxRunTimeMax = 0x7FFFFFFF;

BOOLEAN gForceExecution = FALSE;

EFI_UNICODE_COLLATION_PROTOCOL    *gtUnicodeCollation           = NULL;
EFI_SHELL_PROTOCOL                *gShell                       = NULL;

//
// Internal functions declaration
//

VOID
PrintUsage (
  VOID
  );

EFI_STATUS
ParseCommandLine (
  IN EFI_HANDLE                   ImageHandle
  );

EFI_STATUS
SctMainFunc (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  );

//
// Check MonitorName if it is Mnp, Ip4 or Serial
//
STATIC
EFI_STATUS
CheckMonitorName (
  CHAR16  *Argv
  );

//
// Entry point
//

EFI_STATUS
EFIAPI
InitializeSct (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
/*++

Routine Description:

  Entry point of SCT.

Arguments:

  ImageHandle   - The image handle.
  SystemTable   - The system table.

Returns:

  EFI_SUCCESS   - Successfully.
  OTHERS        - Something failed.

--*/
{
  EFI_STATUS  Status;
  EFI_TPL     OldTPL;

  Print(L"InitializeSct()\n");

  OldTPL = gBS->RaiseTPL(TPL_HIGH_LEVEL);
  if(OldTPL != TPL_APPLICATION) {
    gBS->RestoreTPL(OldTPL);
    Print(L"ERROR: SCT should run at TPL_APPLICATION level\n");
    return EFI_SUCCESS;
  }
  gBS->RestoreTPL(OldTPL);

  //
  //
  //
  Status = gBS->LocateProtocol(&gEfiUnicodeCollation2ProtocolGuid, NULL, (VOID**)&gtUnicodeCollation);
  Status = gBS->LocateProtocol(&gEfiShellProtocolGuid, NULL, (VOID**)&gShell);

  //
  // Initialize the framework table
  //
  Status = InitializeFrameworkTable (ImageHandle, SystemTable);
  if (EFI_ERROR (Status)) {
    Print (L"ERROR: Cannot initialize the framework table. Status - %r\n", Status);
    return Status;
  }

  //
  // Parse the command line
  //
  Status = ParseCommandLine (ImageHandle);
  if (EFI_ERROR (Status)) {
    Print (L"ERROR: Invalid command line. Status - %r\n", Status);
    FreeFrameworkTable ();
    return Status;
  }

  //
  // Invoke the SCT main function to do the operations
  //
  Status = SctMainFunc (ImageHandle, SystemTable);
  if (EFI_ERROR (Status)) {
    Print (L"ERROR: Cannot do the operations. Status - %r\n", Status);
    FreeFrameworkTable ();
    return Status;
  }

  //
  // Free the framework table
  //
  Status = FreeFrameworkTable ();
  if (EFI_ERROR (Status)) {
    Print (L"ERROR: Cannot free the framework table. Status - %r\n", Status);
    return Status;
  }

  //
  // Done
  //
  Print (L"Done!\n");
  return EFI_SUCCESS;
}


//
// Internal functions implementation
//

VOID
PrintUsage (
  VOID
  )
/*++

Routine Description:

  Print out the usage model of the SCT.

--*/
{
  Print (
    L"%s\n"
    L"\n"
    L"usage:\n"
    L"%s [-a | -c | -s <seq> | -u | -p <MNP | IP4 | SERIAL>] [-r] [-g <report>]\n"
    L"\n"
    L"  -a    Executes all test cases.\n"
    L"  -c    Continues execute the test cases.\n"
    L"  -g    Generates test report.\n"
    L"  -p    Passive Mode with specified communication layer\n"
    L"  -r    Resets all test results.\n"
    L"  -s    Executes the test cases in the test sequence file.\n"
    L"  -u    Turns into user-friendly interface.\n"
    L"  -f    Force the operation execution, no confirmation from user.\n"
    L"\n",
    EFI_SCT_NAME,
    EFI_SCT_SHORT_NAME
    );
}

STATIC UINT32 SCT_VALID_OPERATIONS[] = {
  EFI_SCT_OPERATIONS_NONE,                                      // sct -h | sct -?
  EFI_SCT_OPERATIONS_ALL,                                       // sct -a
  EFI_SCT_OPERATIONS_RESET,                                     // sct -r
  EFI_SCT_OPERATIONS_ALL | EFI_SCT_OPERATIONS_RESET,            // sct -r -a
  EFI_SCT_OPERATIONS_CONTINUE,                                  // sct -c
  EFI_SCT_OPERATIONS_REPORT,                                    // sct -g
  EFI_SCT_OPERATIONS_SEQUENCE,                                  // sct -s
  EFI_SCT_OPERATIONS_UI,                                        // sct -u
  EFI_SCT_OPERATIONS_RESET | EFI_SCT_OPERATIONS_UI,             // sct -r -u
  EFI_SCT_OPERATIONS_PASSIVEMODE,                               // sct -p
  EFI_SCT_OPERATIONS_PASSIVEMODE | EFI_SCT_OPERATIONS_SEQUENCE, // sct -s -p
  EFI_SCT_OPERATIONS_PASSIVEMODE | EFI_SCT_OPERATIONS_CONTINUE, // sct -c -p
  EFI_SCT_OPERATIONS_REPORT | EFI_SCT_OPERATIONS_PASSIVEMODE,   // sct -g -p
  EFI_SCT_OPERATIONS_EXTENDED,                                  // sct -x omit
  EFI_SCT_OPERATIONS_INVALID                                    // invalid end of table
};

STATIC
EFI_STATUS
SctCmdLineGetReportName (
  IN EFI_HANDLE                   ImageHandle,
  IN CHAR16                       *ArgStr
  )
{
  EFI_STATUS  Status;
  CHAR16      *TempStr;

  Status = ExpandFileName (
             EFI_SCT_PATH_REPORT,
             &gFT->RepDevicePath,
             &TempStr
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gFT->RepFileName = CatSPrint (NULL, L"%s\\%s", TempStr, ArgStr);
  FreePool (TempStr);

  return EFI_SUCCESS;
}

STATIC
VOID
SctCmdLineGetMonitorName (
  IN CHAR16                       *ArgStr
  )
{
  EFI_STATUS                      Status;

  Status = CheckMonitorName(ArgStr);
  if (!EFI_ERROR (Status)) {
    gFT->MonitorName = ArgStr;
      } else {
    gFT->MonitorName = NULL;
      }
}

STATIC
EFI_STATUS
SctCmdLineGetSequenceFileName (
  IN EFI_HANDLE                   ImageHandle,
  IN CHAR16                       *ArgStr
  )
{
  EFI_STATUS  Status;
  CHAR16      *TempStr;

  Status = ExpandFileName (
             EFI_SCT_PATH_SEQUENCE,
             &gFT->SeqDevicePath,
             &TempStr
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gFT->SeqFileName = CatSPrint (NULL, L"%s\\%s", TempStr, ArgStr);
  FreePool (TempStr);

  return EFI_SUCCESS;
}

#define SCT_OPERATION_MASK(OP)    (gFT->Operations |= (OP))
#define SCT_OPERATION_EQU(OP)     (gFT->Operations == (OP))

EFI_STATUS
ParseCommandLine (
  IN EFI_HANDLE                   ImageHandle
  )
{
  EFI_STATUS  Status;
  UINTN       Index;
  UINTN       Argc;
  CHAR16      **Argv;
  EFI_SHELL_PARAMETERS_PROTOCOL *ShellParameters;

  //
  // Get the parameters from the shell interface
  //
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

  gFT->Operations = 0;

  for (Index = 1; Index < Argc; Index++) {
    if (Argv[Index][0] == L'-') {
      switch(Argv[Index][1]) {
      case L'a': case L'A':
        SCT_OPERATION_MASK(EFI_SCT_OPERATIONS_ALL);
        break;
      case L'c': case L'C':
        SCT_OPERATION_MASK(EFI_SCT_OPERATIONS_CONTINUE);
    break;
      case L'g': case L'G':
        SCT_OPERATION_MASK(EFI_SCT_OPERATIONS_REPORT);
        Index++;
        if (Index == Argc) {
          EFI_SCT_DEBUG((EFI_SCT_D_ERROR, L"Not set the report file name"));
          return EFI_NOT_FOUND;
        }
    Status = SctCmdLineGetReportName(ImageHandle, Argv[Index]);
    if (EFI_ERROR(Status)) {
          EFI_SCT_DEBUG((EFI_SCT_D_ERROR, L"Get report file name fail - %r", Status));
          return Status;
      }
      break;
      case L'p': case L'P':
        SCT_OPERATION_MASK(EFI_SCT_OPERATIONS_PASSIVEMODE);
        Index++;
        if (Index == Argc) {
          EFI_SCT_DEBUG((EFI_SCT_D_ERROR, L"Not set the monitor name"));
        return EFI_INVALID_PARAMETER;
        } else {
          SctCmdLineGetMonitorName(Argv[Index]);
      }
      break;
      case L'r': case L'R':
        SCT_OPERATION_MASK(EFI_SCT_OPERATIONS_RESET);
        break;
      case L's': case L'S':
        SCT_OPERATION_MASK(EFI_SCT_OPERATIONS_SEQUENCE);
        Index++;
        if (Index == Argc) {
          EFI_SCT_DEBUG((EFI_SCT_D_ERROR, L"Not set the sequence file name"));
          return EFI_NOT_FOUND;
        }
        Status = SctCmdLineGetSequenceFileName(ImageHandle, Argv[Index]);
        if (EFI_ERROR(Status)) {
          EFI_SCT_DEBUG((EFI_SCT_D_ERROR, L"get sequence file name fail - %r", Status));
          return Status;
        }
        break;
      case L'u': case L'U':
      SCT_OPERATION_MASK(EFI_SCT_OPERATIONS_UI);
        break;
      case L'x': case L'X':
        SCT_OPERATION_MASK(EFI_SCT_OPERATIONS_EXTENDED);
        break;
      case L'f': case L'F':
  gForceExecution = TRUE;
  break;
      case L'h': case L'H': case L'?':
        SCT_OPERATION_MASK(EFI_SCT_OPERATIONS_NONE);
        return EFI_SUCCESS;;
    default:
      return EFI_INVALID_PARAMETER;
    }
    } else {
      return EFI_INVALID_PARAMETER;
    }
  }

  Status = EFI_INVALID_PARAMETER;
  for(Index = 0; SCT_VALID_OPERATIONS[Index] != EFI_SCT_OPERATIONS_INVALID; Index++) {
    if (SCT_OPERATION_EQU(SCT_VALID_OPERATIONS[Index])) {
      Status = EFI_SUCCESS;
      break;
    }
}

  return Status;
}

EFI_STATUS
SctMainFunc (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
/*++

Routine Description:

  The main routine of SCT.

Returns:

  EFI_SUCCESS   - Successfully.
  OTHERS        - Something failed.

--*/
{
  EFI_STATUS  Status;

  //
  // Check the operations
  //
  if (gFT->Operations == EFI_SCT_OPERATIONS_NONE) {
    //
    // No operation. Print out the help information
    //
    PrintUsage ();
    return EFI_SUCCESS;
  }

  //
  // If EFI_SCT_OPERATIONS_PASSIVEMODE unmasked then delete the passive test context
  //
  if ((gFT->Operations & EFI_SCT_OPERATIONS_PASSIVEMODE) == 0) {
    PassiveTestContextDelete(gFT->DevicePath, SCT_PASSIVE_MODE_RECORD_FILE);
  }

  //
  // Attach the first stage test data (before the test files are loaded)
  //
  Status = AttachFirstStageTestData ();
  if (EFI_ERROR (Status)) {
    EFI_SCT_DEBUG ((EFI_SCT_D_ERROR, L"Attach first stage test data - %r", Status));
    return Status;
  }

  //
  // Do the first stage operations (before the test files are loaded)
  //
  Status = DoFirstStageOperations ();
  if (EFI_ERROR (Status)) {
    EFI_SCT_DEBUG ((EFI_SCT_D_ERROR, L"First stage operations - %r", Status));
    return Status;
  }

  //
  // Attach the second stage test data (after the test files are loaded)
  //
  Status = AttachSecondStageTestData ();
  if (EFI_ERROR (Status)) {
    EFI_SCT_DEBUG ((EFI_SCT_D_ERROR, L"Attach second stage test data - %r", Status));
    return Status;
  }

  //
  // Do the second stage operations (after the test files are loaded)
  //
  Status = DoSecondStageOperations ();
  if (EFI_ERROR (Status)) {
    EFI_SCT_DEBUG ((EFI_SCT_D_ERROR, L"Second stage operations - %r", Status));
    return Status;
  }

  //
  // Done
  //
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
CheckMonitorName (
  CHAR16  *Argv
  )
{
//  CHAR16 *Temp;
//  Temp = UpperCaseString(Argv);

  if ((StrCmp(Argv, L"MNP") == 0) || (StrCmp(Argv, L"mnp") == 0))
    return EFI_SUCCESS;
  if ((StrCmp(Argv, L"IP4") == 0) || (StrCmp(Argv, L"ip4") == 0))
    return EFI_SUCCESS;
  if ((StrCmp(Argv, L"SERIAL") == 0) || (StrCmp(Argv, L"serial") == 0))
    return EFI_SUCCESS;

  return EFI_INVALID_PARAMETER;
}


EFI_FILE_HANDLE
LibOpenRoot (
  IN EFI_HANDLE                   DeviceHandle
  )
/*++

Routine Description:

  Function opens and returns a file handle to the root directory of a volume.

Arguments:

  DeviceHandle         - A handle for a device

Returns:

  A valid file handle or NULL is returned

--*/
{
  EFI_STATUS                      Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Volume;
  EFI_FILE_HANDLE                 File;

  File = NULL;

  //
  // Handle the file system interface to the device
  //
  Status = gBS->HandleProtocol (
                DeviceHandle,
                &gEfiSimpleFileSystemProtocolGuid,
                (VOID *) &Volume
                );

  //
  // Open the root directory of the volume
  //
  if (!EFI_ERROR (Status)) {
    Status = Volume->OpenVolume (
                      Volume,
                      &File
                      );
  }
  //
  // Done
  //
  return EFI_ERROR (Status) ? NULL : File;
}

BOOLEAN
GrowBuffer (
  IN OUT EFI_STATUS   *Status,
  IN OUT VOID         **Buffer,
  IN UINTN            BufferSize
  )
/*++

Routine Description:

  Helper function called as part of the code needed
  to allocate the proper sized buffer for various
  EFI interfaces.

Arguments:

  Status      - Current status

  Buffer      - Current allocated buffer, or NULL

  BufferSize  - Current buffer size needed

Returns:

  TRUE - if the buffer was reallocated and the caller
  should try the API again.

--*/
{
  BOOLEAN TryAgain;

  ASSERT (Status != NULL);
  ASSERT (Buffer != NULL);

  //
  // If this is an initial request, buffer will be null with a new buffer size
  //
  if (NULL == *Buffer && BufferSize) {
    *Status = EFI_BUFFER_TOO_SMALL;
  }
  //
  // If the status code is "buffer too small", resize the buffer
  //
  TryAgain = FALSE;
  if (*Status == EFI_BUFFER_TOO_SMALL) {

    if (*Buffer) {
      FreePool (*Buffer);
    }

    *Buffer = AllocateZeroPool (BufferSize);

    if (*Buffer) {
      TryAgain = TRUE;
    } else {
      *Status = EFI_OUT_OF_RESOURCES;
    }
  }
  //
  // If there's an error, free the buffer
  //
  if (!TryAgain && EFI_ERROR (*Status) && *Buffer) {
    FreePool (*Buffer);
    *Buffer = NULL;
  }

  return TryAgain;
}
