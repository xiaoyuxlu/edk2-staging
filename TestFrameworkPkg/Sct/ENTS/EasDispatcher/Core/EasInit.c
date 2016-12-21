/** @file

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
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ShellLib.h>

#include "ENTS/EasDispatcher/Include/Eas.h"

#include <Protocol/EntsMonitorProtocol.h>

EFI_NETWORK_TEST_FRAMEWORK_TABLE  *gEasFT = NULL;

EFI_STATUS
AttachNetworkTestFrameworkTable (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable,
  IN CHAR16             *MonitorName
  );

EFI_STATUS
DetachNetworkTestFrameworkTable (
  VOID
  );

EFI_STATUS
EntsAttachMonitor (
  IN CHAR16                     *MonitorName
  );

EFI_STATUS
EntsDetachMonitor (
  VOID
  );

EFI_STATUS
EntsAttachSupportFiles (
  VOID
  );

EFI_STATUS
EntsDetachSupportFiles (
  VOID
  );

EFI_STATUS
EntsAttachTestFiles (
  VOID
  );

EFI_STATUS
EntsDetachTestFiles (
  VOID
  );

EFI_STATUS
InitResources (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable,
  IN CHAR16             *MonitorName
  )
/*++

Routine Description:

  Initialize the system resources.

Arguments:

  ImageHandle - The image handle.
  SystemTable - The system table.
  MonitorName - Monitor name for Communication layer, current we define the
                following three types: Mnp  Ip4  Serial

Returns:

  EFI_SUCCESS - Operation succeeded.
  Others - Operation failed.

--*/
{
  EFI_STATUS  Status;


  //
  // Attach Framework Table and the items of Framework Table
  //
  Status = AttachNetworkTestFrameworkTable (
             ImageHandle,
             SystemTable,
             MonitorName
             );
  if (EFI_ERROR (Status)) {
    EFI_ENTS_DEBUG (
      (EFI_ENTS_D_ERROR,
      L"%s: Cannot intiailize NetworkTest framework table - %r\n",
      EFI_ENTS_SHORT_NAME,
      Status)
      );
    return Status;
  }

  EFI_ENTS_DEBUG((EFI_ENTS_D_TRACE, L"AttachNetworkTestFrameworkTable success"));

  //
  // Initialize debug file
  //
  Status = EntsInitializeDebugServices (gEasFT->DevicePath, gEasFT->FilePath);
  if (EFI_ERROR (Status)) {
    goto EntsInitDebugServicesError;
  }

  EFI_ENTS_DEBUG((EFI_ENTS_D_TRACE, L"EntsInitializeDebugServices success"));

  //
  // Attach Support Files
  //
  EFI_ENTS_STATUS ((L"Begin to Attach NetworkTest support files"));
  Status = EntsAttachSupportFiles ();
  if (EFI_ERROR (Status)) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"%s: Cannot intiailize NetworkTest support files - %r", EFI_ENTS_SHORT_NAME, Status));
    goto AttachSupportFilesError;
  }

  EFI_ENTS_DEBUG((EFI_ENTS_D_TRACE, L"EntsAttachSupportFiles"));

  //
  // Attach Monitor
  //
  Status = EntsAttachMonitor (MonitorName);
  if (EFI_ERROR (Status)) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"Fail to Attach NetworkTest monitor - %r", Status));
  goto AttachMonitorError;
  }

  EFI_ENTS_DEBUG((EFI_ENTS_D_TRACE, L"EntsAttachMonitor"));

  //
  // Attach Test Files
  //
  EFI_ENTS_STATUS ((L"Begin to Attach NetworkTest test files"));
  Status = EntsAttachTestFiles ();
  if (EFI_ERROR (Status)) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"%s: Cannot intiailize NetworkTest test files - %r", EFI_ENTS_SHORT_NAME, Status));
  goto AttachTestFilesError;
  }

  EFI_ENTS_DEBUG((EFI_ENTS_D_TRACE, L"EntsAttachTestFiles"));

  return EFI_SUCCESS;

AttachTestFilesError:
  EntsDetachMonitor();
AttachMonitorError:
  EntsDetachSupportFiles();
AttachSupportFilesError:
  CloseDebugServices();
EntsInitDebugServicesError:
  DetachNetworkTestFrameworkTable ();
  return Status;
}

EFI_STATUS
FreeResources (
  VOID
  )
/*++

Routine Description:

  Detach all test data.

Arguments:

  None

Returns:

  EFI_SUCCESS - Operation succeeded.
  Others - Operation failed.

--*/
{
  EFI_STATUS                      Status;

  Status = EntsDetachTestFiles ();
  if (EFI_ERROR(Status)) {
    EFI_ENTS_DEBUG((EFI_ENTS_D_ERROR, L"Detach Test Files fail - %r", Status));
  return Status;
  }

  Status = EntsDetachMonitor();
  if (EFI_ERROR(Status)) {
    EFI_ENTS_DEBUG((EFI_ENTS_D_ERROR, L"Detach Monitor fail - %r", Status));
  return Status;
  }

  Status = EntsDetachSupportFiles ();
  if (EFI_ERROR(Status)) {
    EFI_ENTS_DEBUG((EFI_ENTS_D_ERROR, L"Detach Support Files fail - %r", Status));
  return Status;
  }

  Status = DetachNetworkTestFrameworkTable ();
  if (EFI_ERROR(Status)) {
    EFI_ENTS_DEBUG((EFI_ENTS_D_ERROR, L"Detach Network Teset Files fail - %r", Status));
  return Status;
  }

  Status = CloseDebugServices ();
  if (EFI_ERROR(Status)) {
    EFI_ENTS_DEBUG((EFI_ENTS_D_ERROR, L"Close Debug Services fail - %r", Status));
  return Status;
  }


  return EFI_SUCCESS;
}

EFI_STATUS
AttachNetworkTestFrameworkTable (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable,
  IN CHAR16             *MonitorName
  )
/*++

Routine Description:

  Initialize Framework Table and the items of Framework Table.

Arguments:

  ImageHandle           - The image handle.
  SystemTable           - The system table.
  MonitorName           - Monitor name for Communication layer, current we
                          define the following three types: Mnp  Ip4  Serial

Returns:

  EFI_SUCCESS - Operation succeeded.
  Others      - Some failure happened.

--*/
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  CHAR16                    *FilePath;

  //
  // Allocate memory for Framework Table
  //
  Status = gBS->AllocatePool (
                EfiBootServicesData,
                sizeof (EFI_NETWORK_TEST_FRAMEWORK_TABLE),
                &gEasFT
                );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Initialize the items of Framework Table
  //
  ZeroMem (gEasFT, sizeof (EFI_NETWORK_TEST_FRAMEWORK_TABLE));

  Status = gBS->AllocatePool (
                EfiBootServicesData,
                sizeof (EFI_MONITOR_COMMAND),
                &gEasFT->Cmd
                );
  if (EFI_ERROR (Status)) {
    FreePool (gEasFT->Cmd);
    return Status;
  }

  Status = GetImageDevicePath (
            ImageHandle,
            &DevicePath,
            &FilePath
            );
  if (EFI_ERROR (Status)) {
  goto AttachFail;
  }

  gEasFT->Signature                 = EFI_NETWORK_TEST_FRAMEWORK_TABLE_SIGNATURE;
  gEasFT->Version                   = EFI_NETWORK_TEST_FRAMEWORK_TABLE_VERSION;
  gEasFT->ImageHandle               = ImageHandle;
  gEasFT->SystemTable               = SystemTable;
  gEasFT->DevicePath                = DevicePath;
  gEasFT->FilePath                  = FilePath;

  gEasFT->Cmd->Signature            = EFI_MONITOR_COMMAND_SIGNATURE;
  gEasFT->Cmd->Version              = EFI_MONITOR_COMMAND_VERSION;
  gEasFT->Cmd->ComdName             = NULL;
  gEasFT->Cmd->ComdArg              = NULL;
  gEasFT->Cmd->ComdRuntimeInfo      = NULL;
  gEasFT->Cmd->ComdRuntimeInfoSize  = 0;
  gEasFT->Cmd->ComdOutput           = NULL;
  gEasFT->Cmd->ComdOutputSize       = 0;
  gEasFT->Cmd->ComdResult           = PASS;
  gEasFT->Cmd->TestFile             = NULL;
  gEasFT->Cmd->ComdInterface        = NULL;

  return EFI_SUCCESS;

AttachFail:
  FreePool (gEasFT->Cmd);
  FreePool (gEasFT);
  gEasFT = NULL;
  return Status;
}

EFI_STATUS
DetachNetworkTestFrameworkTable (
  VOID
  )
/*++

Routine Description:

  Free Framework Table and the items of Framework Table.

Arguments:

  None

Returns:

  EFI_SUCCESS - Operation succeeded.
  Others      - Some failure happened.

--*/
{
  if (gEasFT == NULL) {
    return EFI_SUCCESS;
  }
  //
  // Free the items of Framework Table
  //
  if (gEasFT->DevicePath != NULL) {
    FreePool (gEasFT->DevicePath);
    gEasFT->DevicePath = NULL;
  }

  if (gEasFT->FilePath != NULL) {
    FreePool (gEasFT->FilePath);
    gEasFT->FilePath = NULL;
  }
  //
  // Free Command
  //
  if (gEasFT->Cmd != NULL) {
    FreePool (gEasFT->Cmd);
    gEasFT->Cmd = NULL;
  }

  //
  // Free Framework Table
  //
  FreePool (gEasFT);
  gEasFT = NULL;

  //
  // Done
  //
  return EFI_SUCCESS;
}

EFI_STATUS
EntsAttachSupportFiles (
  VOID
  )
/*++

Routine Description:

  Load support files.

Arguments:

  None

Returns:

  EFI_SUCCESS          - Operation succeeded.
  EFI_OUT_OF_RESOURCES - Memory allocation failed.
  Others               - Some failure happened.

--*/
{
  EFI_STATUS  Status;
  CHAR16      *FilePath;

  InitializeListHead (&gEasFT->SupportList);

  //
  // Create the test file path
  //
  FilePath = PoolPrint (L"%s\\%s", gEasFT->FilePath, EFI_NETWORK_PATH_SUPPORT);
  if (FilePath == NULL) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"Pool print - %r", EFI_OUT_OF_RESOURCES));
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Load test files
  //
  Status = EntsLoadSupportFiles (
            gEasFT->DevicePath,
            FilePath,
            TRUE  // Recursive
            );
  if (EFI_ERROR (Status)) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"Load support files - %r", Status));
    FreePool (FilePath);
    return Status;
  }

  FreePool (FilePath);

  //
  // Done
  //
  return EFI_SUCCESS;
}

EFI_STATUS
EntsDetachSupportFiles (
  VOID
  )
{
  EFI_STATUS                        Status;

  //
  // Unload support files
  //
  Status = EntsUnloadSupportFiles (&gEasFT->SupportList);

  return Status;
}

EFI_STATUS
EntsAttachTestFiles (
  VOID
  )
/*++

Routine Description:

  Load test files.

Arguments:

  None

Returns:

  EFI_SUCCESS - Operation succeeded.
  EFI_OUT_OF_RESOURCES - Memory allocation failed.
  Others      - Some failure happened.

--*/
{
  EFI_STATUS  Status;
  CHAR16      *FilePath;

  InitializeListHead (&gEasFT->TestAppList);

  //
  // Create the test file path
  //
  FilePath = PoolPrint (L"%s\\%s", gEasFT->FilePath, EFI_NETWORK_PATH_TEST);
  if (FilePath == NULL) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"Pool print - %r", EFI_OUT_OF_RESOURCES));
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Load test files
  //
  Status = EntsLoadTestFiles (
            gEasFT->DevicePath,
            FilePath,
            TRUE  // Recursive
            );
  if (EFI_ERROR (Status)) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"Load test files - %r", Status));
    FreePool (FilePath);
    return Status;
  }

  FreePool (FilePath);

  //
  // Done
  //
  return EFI_SUCCESS;
}

EFI_STATUS
EntsDetachTestFiles (
  VOID
  )
{
  EFI_STATUS                       Status;

  //
  // Unload test files
  //
  Status = EntsUnloadTestFiles (&gEasFT->TestAppList);

  return Status;
}

EFI_STATUS
EntsAttachMonitor (
  IN CHAR16                     *MonitorName
  )
/*++

Routine Description:

  Attach ENTS Monitor.

Arguments:

  MonitorName - Monitor name string.
  EntsMonitor - A pointer to the EFI_ENTS_MONITOR_PROTOCOL instance.

Returns:

  EFI_SUCCESS   - Operation succeeded.
  EFI_NOT_FOUND - The handle of EFI_ENTS_MONITOR_PROTOCOL was not found.
  Others        - Some failure happened.

--*/
{
  EFI_STATUS                    Status;
  EFI_ENTS_MONITOR_PROTOCOL     *EntsMonitorInterface;
  UINTN                         NoHandles;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         Index;

  EntsMonitorInterface = NULL;

  //
  // Locate all the handle of EFI_ENTS_MONITOR_PROTOCOL
  //
  Status = gBS->LocateHandleBuffer (
                ByProtocol,
                &gEfiEntsMonitorProtocolGuid,
                NULL,
                &NoHandles,
                &HandleBuffer
                );
  if (EFI_ERROR (Status) || (NoHandles == 0)) {
    EFI_ENTS_DEBUG((EFI_ENTS_D_ERROR, L"Can not locate any EFI_ENTS_MONITOR_PROTOCOL handles - %r", Status));
    Status = EFI_NOT_FOUND;
    goto AttachErr;
  }

  //
  // Search the proper EFI_ENTS_MONITOR_PROTOCOL
  //
  for (Index = 0; Index < NoHandles; Index++) {
    Status = gBS->HandleProtocol (
                  HandleBuffer[Index],
                  &gEfiEntsMonitorProtocolGuid,
                  &EntsMonitorInterface
                  );
    if (EFI_ERROR (Status)) {
      EFI_ENTS_DEBUG((EFI_ENTS_D_ERROR, L"Can not get ENTS Monitor interface - %r", Status));
      Status = EFI_NOT_FOUND;
      goto AttachErr;
    }

    if (StriCmp (MonitorName, EntsMonitorInterface->MonitorName) == 0) {
      break;
    }
  }

  FreePool(HandleBuffer);

  gEasFT->Monitor = EntsMonitorInterface;
  Status          = EntsMonitorInterface->InitMonitor (EntsMonitorInterface);
  if (EFI_ERROR (Status)) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"Fail to InitMonitor - %r", Status));
  goto AttachErr;
  }

  return EFI_SUCCESS;

AttachErr:
  gEasFT->Monitor = NULL;
  return Status;
}

EFI_STATUS
EntsDetachMonitor (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_ENTS_MONITOR_PROTOCOL     *EntsMonitor;

  Status = EFI_SUCCESS;
  if (gEasFT->Monitor != NULL) {
    EntsMonitor = gEasFT->Monitor;
    gEasFT->Monitor = NULL;
    Status = EntsMonitor->ResetMonitor (EntsMonitor);
  }

  return Status;
}

