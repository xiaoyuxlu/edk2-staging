/** @file
  E1000 driver with Firmware Management Protocol that supports firmware updates.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "E1000Fmp.h"

///
/// Driver Binding Protocol instance
///
EFI_DRIVER_BINDING_PROTOCOL gE1000FmpDriverBinding = {
  E1000FmpDriverBindingSupported,
  E1000FmpDriverBindingStart,
  E1000FmpDriverBindingStop,
  E1000FMP_VERSION,
  NULL,
  NULL
};

FMP_DEVICE_LIB_REGISTER_FMP_INSTALLER    mFmpDeviceLibRegisterFmpInstaller   = NULL;
FMP_DEVICE_LIB_REGISTER_FMP_UNINSTALLER  mFmpDeviceLibRegisterFmpUninstaller = NULL;

EFI_STATUS
IntelE1000UninstallFmp (
  EFI_HANDLE  ControllerHandle
  )
{
  EFI_STATUS                                   Status;
  EFI_FIRMWARE_MANAGEMENT_PROTOCOL             *Fmp;
  EDKII_FIRMWARE_MANAGEMENT_PROGRESS_PROTOCOL  *FmpProgress;

  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiFirmwareManagementProtocolGuid,
                  (VOID **) &Fmp,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEdkiiFirmwareManagementProgressProtocolGuid,
                  (VOID **) &FmpProgress,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->UninstallMultipleProtocolInterfaces (
                  ControllerHandle,
                  &gEfiFirmwareManagementProtocolGuid, Fmp,
                  &gEdkiiFirmwareManagementProgressProtocolGuid, FmpProgress,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  FreePool (Fmp);
  FreePool (FmpProgress);

  return EFI_SUCCESS;
}

/**
  Unloads an image.

  @param  ImageHandle           Handle that identifies the image to be unloaded.

  @retval EFI_SUCCESS           The image has been unloaded.
  @retval EFI_INVALID_PARAMETER ImageHandle is not a valid image handle.

**/
EFI_STATUS
EFIAPI
E1000FmpUnload (
  IN EFI_HANDLE  ImageHandle
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  *HandleBuffer;
  UINTN       HandleCount;
  UINTN       Index;

  Status = EFI_SUCCESS;
  //
  // Retrieve array of all handles in the handle database
  //
  Status = gBS->LocateHandleBuffer (
                  AllHandles,
                  NULL,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Disconnect the current driver from handles in the handle database
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->DisconnectController (HandleBuffer[Index], gImageHandle, NULL);
  }

  //
  // Free the array of handles
  //
  FreePool (HandleBuffer);

  //
  // Uninstall protocols installed in the driver entry point
  //
  Status = gBS->UninstallMultipleProtocolInterfaces (
                  ImageHandle,
                  &gEfiDriverBindingProtocolGuid,  &gE1000FmpDriverBinding,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  This is the declaration of an EFI image entry point. This entry point is
  the same for UEFI Applications, UEFI OS Loaders, and UEFI Drivers including
  both device drivers and bus drivers.

  @param  ImageHandle           The firmware allocated handle for the UEFI image.
  @param  SystemTable           A pointer to the EFI System Table.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval Others                An unexpected error occurred.
**/
EFI_STATUS
EFIAPI
E1000FmpDriverEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;

  //
  // Install UEFI Driver Model protocol(s).
  //
  Status = EfiLibInstallDriverBinding (
             ImageHandle,
             SystemTable,
             &gE1000FmpDriverBinding,
             ImageHandle
             );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Tests to see if this driver supports a given controller. If a child device is provided,
  it further tests to see if this driver supports creating a handle for the specified child device.

  This function checks to see if the driver specified by This supports the device specified by
  ControllerHandle. Drivers will typically use the device path attached to
  ControllerHandle and/or the services from the bus I/O abstraction attached to
  ControllerHandle to determine if the driver supports ControllerHandle. This function
  may be called many times during platform initialization. In order to reduce boot times, the tests
  performed by this function must be very small, and take as little time as possible to execute. This
  function must not change the state of any hardware devices, and this function must be aware that the
  device specified by ControllerHandle may already be managed by the same driver or a
  different driver. This function must match its calls to AllocatePages() with FreePages(),
  AllocatePool() with FreePool(), and OpenProtocol() with CloseProtocol().
  Because ControllerHandle may have been previously started by the same driver, if a protocol is
  already in the opened state, then it must not be closed with CloseProtocol(). This is required
  to guarantee the state of ControllerHandle is not modified by this function.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to test. This handle
                                   must support a protocol interface that supplies
                                   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
                                   parameter is ignored by device drivers, and is optional for bus
                                   drivers. For bus drivers, if this parameter is not NULL, then
                                   the bus driver must determine if the bus controller specified
                                   by ControllerHandle and the child controller specified
                                   by RemainingDevicePath are both supported by this
                                   bus driver.

  @retval EFI_SUCCESS              The device specified by ControllerHandle and
                                   RemainingDevicePath is supported by the driver specified by This.
  @retval EFI_ALREADY_STARTED      The device specified by ControllerHandle and
                                   RemainingDevicePath is already being managed by the driver
                                   specified by This.
  @retval EFI_ACCESS_DENIED        The device specified by ControllerHandle and
                                   RemainingDevicePath is already being managed by a different
                                   driver or an application that requires exclusive access.
                                   Currently not implemented.
  @retval EFI_UNSUPPORTED          The device specified by ControllerHandle and
                                   RemainingDevicePath is not supported by the driver specified by This.
**/
EFI_STATUS
EFIAPI
E1000FmpDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  )
{
  EFI_STATUS           Status;
  EFI_PCI_IO_PROTOCOL  *PciIo;
  UINT16               VendorId;
  UINT16               DeviceId;

  //
  // Open the PCI I/O Protocol on ControllerHandle
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **)&PciIo,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Read 16-bit Vendor ID from the PCI configuration header at offset 0x00
  //
  Status = PciIo->Pci.Read (
                        PciIo,                 // This
                        EfiPciIoWidthUint16,   // Width
                        PCI_VENDOR_ID_OFFSET,  // Offset
                        1,                     // Count
                        &VendorId              // Buffer
                        );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Read 16-bit Device ID from the PCI configuration header at offset 0x02
  //
  Status = PciIo->Pci.Read (
                        PciIo,                 // This
                        EfiPciIoWidthUint16,   // Width
                        PCI_DEVICE_ID_OFFSET,  // Offset
                        1,                     // Count
                        &DeviceId              // Buffer
                        );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Evaluate Vendor ID and Device ID
  //
  Status = EFI_SUCCESS;
  if (VendorId != INTEL_E1000_VENDOR_ID || DeviceId != INTEL_E1000_DEVICE_ID) {
    Status = EFI_UNSUPPORTED;
  }

Done:
  //
  // Close the PCI I/O Protocol
  //
  gBS->CloseProtocol (
         ControllerHandle,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         ControllerHandle
         );

  return Status;
}

/**
  Starts a device controller or a bus controller.

  The Start() function is designed to be invoked from the EFI boot service ConnectController().
  As a result, much of the error checking on the parameters to Start() has been moved into this
  common boot service. It is legal to call Start() from other locations,
  but the following calling restrictions must be followed, or the system behavior will not be deterministic.
  1. ControllerHandle must be a valid EFI_HANDLE.
  2. If RemainingDevicePath is not NULL, then it must be a pointer to a naturally aligned
     EFI_DEVICE_PATH_PROTOCOL.
  3. Prior to calling Start(), the Supported() function for the driver specified by This must
     have been called with the same calling parameters, and Supported() must have returned EFI_SUCCESS.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to start. This handle
                                   must support a protocol interface that supplies
                                   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
                                   parameter is ignored by device drivers, and is optional for bus
                                   drivers. For a bus driver, if this parameter is NULL, then handles
                                   for all the children of Controller are created by this driver.
                                   If this parameter is not NULL and the first Device Path Node is
                                   not the End of Device Path Node, then only the handle for the
                                   child device specified by the first Device Path Node of
                                   RemainingDevicePath is created by this driver.
                                   If the first Device Path Node of RemainingDevicePath is
                                   the End of Device Path Node, no child handle is created by this
                                   driver.

  @retval EFI_SUCCESS              The device was started.
  @retval EFI_DEVICE_ERROR         The device could not be started due to a device error.Currently not implemented.
  @retval EFI_OUT_OF_RESOURCES     The request could not be completed due to a lack of resources.
  @retval Others                   The driver failded to start the device.

**/
EFI_STATUS
EFIAPI
E1000FmpDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  )
{
  EFI_STATUS                Status;
  INTEL_E1000_PRIVATE_DATA  *Private;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  UINT64                    PciSupports;

  //
  // Open the PCI I/O Protocol
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **)&PciIo,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Private = (INTEL_E1000_PRIVATE_DATA *)AllocateZeroPool (sizeof (INTEL_E1000_PRIVATE_DATA));
  if (Private == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }
  Private->Signature = INTEL_E1000_PRIVATE_DATA_SIGNATURE;
  Private->PciIo     = PciIo;

  //
  // Retrieve original PCI attributes and save them in the private context data
  // structure.
  //
  Status = PciIo->Attributes (
                    PciIo,
                    EfiPciIoAttributeOperationGet,
                    0,
                    &Private->OriginalPciAttributes
                    );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Retrieve attributes that the PCI Controller supports
  //
  Status = PciIo->Attributes (
                    PciIo,
                    EfiPciIoAttributeOperationSupported,
                    0,
                    &PciSupports
                    );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Enable Command register
  //
  Status = PciIo->Attributes (
                    PciIo,
                    EfiPciIoAttributeOperationEnable,
                    (PciSupports & EFI_PCI_DEVICE_ENABLE),
                    NULL
                    );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Install Simple Network Protocol to this instance
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ControllerHandle,
                  &gEfiCallerIdGuid, Private,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  if (mFmpDeviceLibRegisterFmpInstaller != NULL) {
    Status = mFmpDeviceLibRegisterFmpInstaller (ControllerHandle);
    if (EFI_ERROR (Status)) {
      goto Done;
    }
  }

Done:
  if (EFI_ERROR (Status)) {
    if (Private != NULL) {
      FreePool (Private);
    }

    //
    // Close the PCI I/O Protocol
    //
    gBS->CloseProtocol (
           ControllerHandle,
           &gEfiPciIoProtocolGuid,
           This->DriverBindingHandle,
           ControllerHandle
           );
  }

  return Status;
}

/**
  Stops a device controller or a bus controller.

  The Stop() function is designed to be invoked from the EFI boot service DisconnectController().
  As a result, much of the error checking on the parameters to Stop() has been moved
  into this common boot service. It is legal to call Stop() from other locations,
  but the following calling restrictions must be followed, or the system behavior will not be deterministic.
  1. ControllerHandle must be a valid EFI_HANDLE that was used on a previous call to this
     same driver's Start() function.
  2. The first NumberOfChildren handles of ChildHandleBuffer must all be a valid
     EFI_HANDLE. In addition, all of these handles must have been created in this driver's
     Start() function, and the Start() function must have called OpenProtocol() on
     ControllerHandle with an Attribute of EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER.

  @param[in]  This              A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle  A handle to the device being stopped. The handle must
                                support a bus specific I/O protocol for the driver
                                to use to stop the device.
  @param[in]  NumberOfChildren  The number of child device handles in ChildHandleBuffer.
  @param[in]  ChildHandleBuffer An array of child handles to be freed. May be NULL
                                if NumberOfChildren is 0.

  @retval EFI_SUCCESS           The device was stopped.
  @retval EFI_DEVICE_ERROR      The device could not be stopped due to a device error.

**/
EFI_STATUS
EFIAPI
E1000FmpDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN UINTN                        NumberOfChildren,
  IN EFI_HANDLE                   *ChildHandleBuffer OPTIONAL
  )
{
  EFI_STATUS                Status;
  INTEL_E1000_PRIVATE_DATA  *Private;
  EFI_PCI_IO_PROTOCOL       *PciIo;

  //
  // Open the Simple Network Protocol
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  (EFI_GUID *)&gEfiCallerIdGuid,
                  (VOID **)&Private,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  PciIo = Private->PciIo;

  //
  // Restore original PCI attributes
  //
  PciIo->Attributes (
           PciIo,
           EfiPciIoAttributeOperationSet,
           Private->OriginalPciAttributes,
           NULL
           );
  //
  // Close the PCI I/O Protocol
  //
  gBS->CloseProtocol (
         ControllerHandle,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         ControllerHandle
         );

  if (mFmpDeviceLibRegisterFmpUninstaller != NULL) {
    Status = mFmpDeviceLibRegisterFmpUninstaller (ControllerHandle);
  }

  Status = gBS->UninstallMultipleProtocolInterfaces (
                  ControllerHandle,
                  &gEfiCallerIdGuid, Private,
                  NULL
                  );

  FreePool (Private);

  return Status;
}
