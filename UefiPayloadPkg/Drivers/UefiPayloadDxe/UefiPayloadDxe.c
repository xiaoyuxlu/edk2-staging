/** @file
  This driver processes platform info handed from PEI.

  Copyright (c) 2014 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include "UefiPayloadDxe.h"
#include <Protocol/Tcg2Protocol.h>


/**
  Reserve MMIO/IO resource in GCD

  @param  IsMMIO        Flag of whether it is mmio resource or io resource.
  @param  GcdType       Type of the space.
  @param  BaseAddress   Base address of the space.
  @param  Length        Length of the space.
  @param  Alignment     Align with 2^Alignment
  @param  ImageHandle   Handle for the image of this driver.

  @retval EFI_SUCCESS   Reserve successful
**/
EFI_STATUS
PayloadReserveResourceInGcd (
  IN BOOLEAN               IsMMIO,
  IN UINTN                 GcdType,
  IN EFI_PHYSICAL_ADDRESS  BaseAddress,
  IN UINT64                Length,
  IN UINTN                 Alignment,
  IN EFI_HANDLE            ImageHandle
  )
{
  EFI_STATUS               Status;

  if (IsMMIO) {
    Status = gDS->AddMemorySpace (
                    GcdType,
                    BaseAddress,
                    Length,
                    EFI_MEMORY_UC
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((
        EFI_D_ERROR,
        "Failed to add memory space :0x%lx 0x%lx\n",
        BaseAddress,
        Length
        ));
    }
    ASSERT_EFI_ERROR (Status);
    Status = gDS->AllocateMemorySpace (
                    EfiGcdAllocateAddress,
                    GcdType,
                    Alignment,
                    Length,
                    &BaseAddress,
                    ImageHandle,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);
  } else {
    Status = gDS->AddIoSpace (
                    GcdType,
                    BaseAddress,
                    Length
                    );
    ASSERT_EFI_ERROR (Status);
    Status = gDS->AllocateIoSpace (
                    EfiGcdAllocateAddress,
                    GcdType,
                    Alignment,
                    Length,
                    &BaseAddress,
                    ImageHandle,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);
  }
  return Status;
}


/**
  Notification function of END_OF_DXE event group.

  This function calls Platform Lib function to perform any additional tasks
  needed at the END_OF_DXE event.  

  @param  Event        Event whose notification function is being invoked.
  @param  Context      Pointer to the notification function's context.

**/
VOID
EFIAPI
OnEndOfDxe (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  )
{
  DEBUG ((EFI_D_INFO, "Uefi Payload DXE: On END_OF_DXE\n"));
  PlatformLibEndOfDxeHook();
  return;
}


/**
  Notification function of EVT_GROUP_READY_TO_BOOT event group.

  This is a notification function registered on EVT_GROUP_READY_TO_BOOT event group.
  When the Boot Manager is about to load and execute a boot option, it reclaims variable
  storage if free size is below the threshold.

  @param  Event        Event whose notification function is being invoked.
  @param  Context      Pointer to the notification function's context.

**/
VOID
EFIAPI
OnReadyToBoot (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  EFI_HOB_GUID_TYPE        *GuidHob;
  ACPI_BOARD_INFO          *pAcpiBoardInfo;
  UINT64                   *TpmLasa;
  EFI_STATUS               Status;
  EFI_TCG2_PROTOCOL        *Tcg2Protocol;
  EFI_PHYSICAL_ADDRESS     EventLogLocation;
  EFI_PHYSICAL_ADDRESS     EventLogLastEntry;
  BOOLEAN                  EventLogTruncated;
  UINT8                    Checksum;
  UINT8                    *ChecksumAddress;

  DEBUG ((EFI_D_INFO, "Uefi Payload DXE: On Ready_To_Boot\n"));

  //
  // Find the acpi board information guid hob
  //
  GuidHob = GetFirstGuidHob(&gUefiAcpiBoardInfoGuid);
  ASSERT(GuidHob != NULL);
  pAcpiBoardInfo = (ACPI_BOARD_INFO *)GET_GUID_HOB_DATA(GuidHob);

  TpmLasa = (UINT64*)(UINTN)(pAcpiBoardInfo->TpmLasa);
  ChecksumAddress = (UINT8*)(UINTN)(pAcpiBoardInfo->TpmChecksum);

  if (TpmLasa) {
    Status = gBS->LocateProtocol(&gEfiTcg2ProtocolGuid, NULL, (VOID **)&Tcg2Protocol);
    if (Status == EFI_SUCCESS) {
      Status = Tcg2Protocol->GetEventLog(
        Tcg2Protocol,
        EFI_TCG2_EVENT_LOG_FORMAT_TCG_2,
        &EventLogLocation,
        &EventLogLastEntry,
        &EventLogTruncated
        );
      if (Status == EFI_SUCCESS) {
        //
        // Update TPM Table's LASA and checksum
        //
        Checksum = *ChecksumAddress;
        Checksum -= (UINT8)(*(UINT8*)&EventLogLocation + *((UINT8*)&EventLogLocation + 1) + 
                             *((UINT8*)&EventLogLocation + 2) + *((UINT8*)&EventLogLocation + 3) +
                             *((UINT8*)&EventLogLocation + 4) + *((UINT8*)&EventLogLocation + 5) +
                             *((UINT8*)&EventLogLocation + 6) + *((UINT8*)&EventLogLocation + 7)
                              - *(UINT8*)(TpmLasa) - *((UINT8*)(TpmLasa) + 1) 
                              - *((UINT8*)(TpmLasa) + 2) - *((UINT8*)(TpmLasa) + 3)
                              - *((UINT8*)(TpmLasa) + 4) - *((UINT8*)(TpmLasa) + 5)
                              - *((UINT8*)(TpmLasa) + 6) - *((UINT8*)(TpmLasa) + 7));
        *ChecksumAddress = Checksum;
        *TpmLasa = (UINT64)EventLogLocation;
      }
    }
  }
  
  PlatformLibReadyToBootHook();
}


/**
  MP function to be passed to MP Services Protocol

  This function wraps PlatformLib's hook function for ExitBootServices event

  @param  Buffer        MP function parameter. Not used.

**/
VOID
EFIAPI
ExitBootServicesMpFunc (
  IN VOID *Buffer
  )
{
  PlatformLibEndOfBootHookMp ();
}


/**
  Notification function of EXIT_BOOT_SERVICES event group.

  This function calls Platform Lib function to perform any additional tasks
  needed at the EXIT_BOOT_SERVICES event.  

  @param  Event        Event whose notification function is being invoked.
  @param  Context      Pointer to the notification function's context.

**/
VOID
EFIAPI
OnExitBootServices (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  )
{
  EFI_MP_SERVICES_PROTOCOL *MpService;
  EFI_STATUS               Status;

  //
  // Calls Platform hook
  //
  PlatformLibEndOfBootHook ();

  //
  // Runs Platform MP hook on every CPU
  //
  Status = gBS->LocateProtocol (
                  &gEfiMpServiceProtocolGuid,
                  NULL,
                  (VOID **) &MpService
                  );

  if (!EFI_ERROR (Status)) {
    ExitBootServicesMpFunc (NULL);
    Status = MpService->StartupAllAPs (
                          MpService,
                          ExitBootServicesMpFunc,
                          TRUE,
                          NULL,
                          0,
                          NULL,
                          NULL
                          );
//    ASSERT_EFI_ERROR (Status);
  }

  return;
}

/**
  Main entry for the Coreboot Support DXE module.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
PayloadDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS         Status;
  EFI_HOB_GUID_TYPE  *GuidHob;
  SYSTEM_TABLE_INFO  *pSystemTableInfo;
  FRAME_BUFFER_INFO  *FbInfo;
  EFI_EVENT          EndOfDxeEvent;
  EFI_EVENT          ReadyToBootEvent;
  EFI_EVENT          ExitBootServicesEvent;
  EFI_HANDLE         Handle;

  Status = EFI_SUCCESS;
  //
  // Report MMIO/IO Resources
  //
  Status = PayloadReserveResourceInGcd (TRUE, EfiGcdMemoryTypeMemoryMappedIo, 0xFEC00000, SIZE_4KB, 0, SystemTable); // IOAPIC
  ASSERT_EFI_ERROR (Status);

  Status = PayloadReserveResourceInGcd (TRUE, EfiGcdMemoryTypeMemoryMappedIo, 0xFED00000, SIZE_1KB, 0, SystemTable); // HPET
  ASSERT_EFI_ERROR (Status);

  //
  // Find the system table information guid hob
  //
  GuidHob = GetFirstGuidHob (&gUefiSystemTableInfoGuid);
  ASSERT (GuidHob != NULL);
  pSystemTableInfo = (SYSTEM_TABLE_INFO *)GET_GUID_HOB_DATA (GuidHob);

  //
  // Install gEfiPciEnumerationCompleteProtocolGuid to inform IntelVTdDxe driver
  // that PCI enumeration is done.
  //
  // UEFI Payload assumes PCI enumeration is completed in previous stage
  // firmware. The UEFI PCI Bus Driver does NOT do a full enumeration and 
  // does NOT install the gEfiPciEnumerationCompleteProtocolGuid. That is why the
  // GUID is installed here.
  //
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiPciEnumerationCompleteProtocolGuid, NULL,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Install Acpi Table
  //
  if (pSystemTableInfo->AcpiTableBase != 0 && pSystemTableInfo->AcpiTableSize != 0) {
    DEBUG ((EFI_D_ERROR, "Install Acpi Table at 0x%lx, length 0x%x\n", pSystemTableInfo->AcpiTableBase, pSystemTableInfo->AcpiTableSize));
    Status = gBS->InstallConfigurationTable (&gEfiAcpiTableGuid, (VOID *)(UINTN)pSystemTableInfo->AcpiTableBase);
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Install Smbios Table
  //
  if (pSystemTableInfo->SmbiosTableBase != 0 && pSystemTableInfo->SmbiosTableSize != 0) {
    DEBUG ((EFI_D_ERROR, "Install Smbios Table at 0x%lx, length 0x%x\n", pSystemTableInfo->SmbiosTableBase, pSystemTableInfo->SmbiosTableSize));
    Status = gBS->InstallConfigurationTable (&gEfiSmbiosTableGuid, (VOID *)(UINTN)pSystemTableInfo->SmbiosTableBase);
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Find the frame buffer information and update PCDs
  //
  GuidHob = GetFirstGuidHob (&gUefiFrameBufferInfoGuid);
  if (GuidHob != NULL) {
    FbInfo  = (FRAME_BUFFER_INFO *)GET_GUID_HOB_DATA (GuidHob);
    Status = PcdSet32S (PcdVideoHorizontalResolution, FbInfo->HorizontalResolution);
    ASSERT_EFI_ERROR (Status);
    Status = PcdSet32S (PcdVideoVerticalResolution, FbInfo->VerticalResolution);
    ASSERT_EFI_ERROR (Status);
    Status = PcdSet32S (PcdSetupVideoHorizontalResolution, FbInfo->HorizontalResolution);
    ASSERT_EFI_ERROR (Status);
    Status = PcdSet32S (PcdSetupVideoVerticalResolution, FbInfo->VerticalResolution);
    ASSERT_EFI_ERROR (Status);
  }

  EndOfDxeEvent = NULL;
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  OnEndOfDxe,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  ReadyToBootEvent = NULL;
  Status = EfiCreateEventReadyToBootEx (
                    TPL_CALLBACK,
                    OnReadyToBoot,
                    NULL,
                    &ReadyToBootEvent
                    );
  ASSERT_EFI_ERROR (Status);

  ExitBootServicesEvent = NULL;
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  OnExitBootServices,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &ExitBootServicesEvent
                  );
  ASSERT_EFI_ERROR (Status);
  
  return EFI_SUCCESS;
}

