/** @file
  An UEFI driver model driver which is responsible for locating the correct
  Redfish host interface NIC device and executing Redfish config handlers.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "RedfishConfigDxe.h"

REDFISH_OVER_IP_PROTOCOL_DATA       *mRedfishData = NULL;
EFI_REDFISH_CREDENTIAL_PROTOCOL     *mCredential = NULL;
EFI_EVENT                           mEndOfDxeEvent = NULL;
EFI_EVENT                           mExitBootServiceEvent = NULL;

///
/// Driver Binding Protocol instance
///
EFI_DRIVER_BINDING_PROTOCOL gRedfishConfigDriverBinding = {
  RedfishConfigDriverBindingSupported,
  RedfishConfigDriverBindingStart,
  RedfishConfigDriverBindingStop,
  REDFISH_CONFIG_VERSION,
  NULL,
  NULL
};

/**
  Check if the REST service is preferred by this implementation.

  @param[in]       RestExServiceInfo     Pointer to the REST EX service info returned from
                                         RestEx->GetService() function.

  @retval TRUE           The REST service is supported by this implementation.
  @retval FALSE          The REST service is NOT supported by this implementation.

**/
BOOLEAN
RedfishIsPreferredRestExService (
  IN EFI_REST_EX_SERVICE_INFO          *RestExServiceInfo
  )
{
  if (RestExServiceInfo == NULL) {
    return FALSE;
  }

  //
  // Check whether this REST service is preferred:
  // 1. RestServiceAccessMode is EFI_REST_EX_SERVICE_OUT_OF_BAND_ACCESS.
  // 2. RestServiceType is EFI_REST_EX_SERVICE_REDFISH.
  // 3. RestExConfigType is EFI_REST_EX_CONFIG_TYPE_HTTP.
  // 4. RestExConfigDataLength is the length of EFI_REST_EX_HTTP_CONFIG_DATA.
  //
  if (RestExServiceInfo->EfiRestExServiceInfoV10.RestServiceAccessMode != EFI_REST_EX_SERVICE_IN_BAND_ACCESS ||
      RestExServiceInfo->EfiRestExServiceInfoV10.RestServiceType != EFI_REST_EX_SERVICE_REDFISH ||
      RestExServiceInfo->EfiRestExServiceInfoV10.RestExConfigType != EFI_REST_EX_CONFIG_TYPE_HTTP ||
      RestExServiceInfo->EfiRestExServiceInfoV10.RestExConfigDataLength != sizeof (EFI_REST_EX_HTTP_CONFIG_DATA)) {
    return FALSE;
  }

  return TRUE;
}

/**
  Callback function executed when a Redfish Config Handler Protocol is installed.

  @param[in]  Event    Event whose notification function is being invoked.
  @param[in]  Context  Pointer to the REDFISH_CONFIG_DRIVER_DATA buffer.

**/
VOID
EFIAPI
RedfishConfigHandlerInstalledCallback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            *HandleBuffer;
  UINTN                                 NumberOfHandles;
  EFI_REDFISH_CONFIG_HANDLER_PROTOCOL   *ConfigHandler;
  REDFISH_CONFIG_DRIVER_DATA            *Private;
  UINTN                                 Index;

  Private = (REDFISH_CONFIG_DRIVER_DATA *) Context;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiRedfishConfigHandlerProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return;
  }


  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                     HandleBuffer[Index],
                     &gEfiRedfishConfigHandlerProtocolGuid,
                     (VOID**) &ConfigHandler
                     );
    ASSERT_EFI_ERROR (Status);

    Status = ConfigHandler->Init (ConfigHandler, Private->Image, Private->Controller, mRedfishData);
    if (EFI_ERROR (Status) && Status != EFI_ALREADY_STARTED) {
      DEBUG ((EFI_D_ERROR, "ERROR: Failed to init Redfish config handler %p.\n", ConfigHandler));
    }
  }
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
RedfishConfigDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  )
{
  EFI_REST_EX_PROTOCOL             *RestEx;
  EFI_STATUS                       Status;
  EFI_HANDLE                       ChildHandle;
  EFI_REST_EX_SERVICE_INFO         *RestExServiceInfo;

  ChildHandle       = NULL;
  RestExServiceInfo = NULL;

  Status = NetLibCreateServiceChild (
             ControllerHandle,
             This->ImageHandle,
             &gEfiRestExServiceBindingProtocolGuid,
             &ChildHandle
             );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->OpenProtocol(
                  ChildHandle,
                  &gEfiRestExProtocolGuid,
                  (VOID**) &RestEx,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    Status = EFI_UNSUPPORTED;
    goto ON_EXIT;
  }

  Status = RestEx->GetService (RestEx, &RestExServiceInfo);
  if (EFI_ERROR (Status)) {
    Status = EFI_UNSUPPORTED;
    goto ON_EXIT;
  }

  //
  // Check whether this REST service is preferred:
  // 1. RestServiceAccessMode is EFI_REST_EX_SERVICE_OUT_OF_BAND_ACCESS.
  // 2. RestServiceType is EFI_REST_EX_SERVICE_REDFISH.
  // 3. RestExConfigType is EFI_REST_EX_CONFIG_TYPE_HTTP.
  // 4. RestExConfigDataLength is the length of EFI_REST_EX_HTTP_CONFIG_DATA.
  //
  if (!RedfishIsPreferredRestExService (RestExServiceInfo)) {
    Status = EFI_UNSUPPORTED;
    goto ON_EXIT;
  }

ON_EXIT:
  if (RestExServiceInfo != NULL) {
    FreePool (RestExServiceInfo);
  }

  NetLibDestroyServiceChild (
    ControllerHandle,
    This->ImageHandle,
    &gEfiRestExServiceBindingProtocolGuid,
    ChildHandle
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
RedfishConfigDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  )
{
  EFI_STATUS                      Status;
  REDFISH_CONFIG_DRIVER_DATA      *Private;
  UINT32                          Id;
  VOID                            *ConfigHandlerRegistration;

  ConfigHandlerRegistration = NULL;

  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiCallerIdGuid,
                  (VOID **) &Id,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (!EFI_ERROR (Status)) {
    return EFI_ALREADY_STARTED;
  }

  //
  // Test for the REST EX service binding Protocol
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiRestExServiceBindingProtocolGuid,
                  NULL,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Private = AllocateZeroPool (sizeof (REDFISH_CONFIG_DRIVER_DATA));
  if (Private == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Private->Signature  = REDFISH_CONFIG_DRIVER_DATA_SIGNATURE;
  Private->Image      = This->DriverBindingHandle;
  Private->Controller = ControllerHandle;

  //
  // Install a protocol with Caller Id Guid to the ControllerHandle, this is just to
  // build the relationship between the ControllerHandle and the private data so as
  // to avoid the driver reentry.
  //
  Status = gBS->InstallProtocolInterface (
                  &ControllerHandle,
                  &gEfiCallerIdGuid,
                  EFI_NATIVE_INTERFACE,
                  &Private->Id
                  );
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  Private->Event = EfiCreateProtocolNotifyEvent (
                     &gEfiRedfishConfigHandlerProtocolGuid,
                     TPL_CALLBACK,
                     RedfishConfigHandlerInstalledCallback,
                     Private,
                     &ConfigHandlerRegistration
                     );

  return EFI_SUCCESS;

ON_ERROR:

  if (Private != NULL) {
    gBS->UninstallProtocolInterface (
           ControllerHandle,
           &gEfiCallerIdGuid,
           &Private->Id
           );

    FreePool (Private);
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
RedfishConfigDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN UINTN                        NumberOfChildren,
  IN EFI_HANDLE                   *ChildHandleBuffer OPTIONAL
  )
{
  EFI_STATUS                            Status;
  UINT32                                *Id;
  REDFISH_CONFIG_DRIVER_DATA            *Private;
  EFI_HANDLE                            *HandleBuffer;
  UINTN                                 NumberOfHandles;
  EFI_REDFISH_CONFIG_HANDLER_PROTOCOL   *ConfigHandler;
  UINTN                                 Index;
  EFI_HANDLE                            NicHandle;


  //
  // RedfishConfigDxe driver opens RestEx child, So, Controller is a RestEx
  // child handle. Locate the Nic handle first.
  //
  NicHandle = NetLibGetNicHandle (ControllerHandle, &gEfiRestExProtocolGuid);
  if (NicHandle == NULL) {
    return EFI_SUCCESS;
  }

  Status = gBS->OpenProtocol (
                  NicHandle,
                  &gEfiCallerIdGuid,
                  (VOID **) &Id,
                  This->DriverBindingHandle,
                  NicHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  Private = REDFISH_CONFIG_DRIVER_DATA_FROM_ID (Id);

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiRedfishConfigHandlerProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status) && Status != EFI_NOT_FOUND) {
    return Status;
  }

  Status = EFI_SUCCESS;
  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                     HandleBuffer[Index],
                     &gEfiRedfishConfigHandlerProtocolGuid,
                     (VOID**) &ConfigHandler
                     );
    ASSERT_EFI_ERROR (Status);

    Status = ConfigHandler->Stop (ConfigHandler);
    if (EFI_ERROR (Status) && Status != EFI_ALREADY_STARTED) {
      DEBUG ((EFI_D_ERROR, "ERROR: Failed to stop Redfish config handler %p.\n", ConfigHandler));
      break;
    }
  }

  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  gBS->UninstallProtocolInterface (
         NicHandle,
         &gEfiCallerIdGuid,
         &Private->Id
         );

  gBS->CloseEvent (Private->Event);

  FreePool (Private);

  return EFI_SUCCESS;
}

/**
  Retrieve platform's Redfish Host Interface information.

  This functions returns the Redfish Host Interface information as defined in DSP0270
  specification. Only Network HI is supported.

  Callers are responsible for freeing the returned storage.

  @param[out]  Data                The pointer to store the returned host interface record.

  @retval EFI_SUCCESS              Get the Redfish Host Interface info successfully.
  @retval EFI_INVALID_PARAMETER    Data is NULL.
  @retval EFI_OUT_OF_RESOURCES     There are not enough memory resources.
  @retval EFI_NOT_FOUND            Can't find the HI info.

**/
EFI_STATUS
RedfishGetNetworkHostInterfaceInfo (
  OUT REDFISH_OVER_IP_PROTOCOL_DATA    **Data
  )
{
  EFI_SMBIOS_PROTOCOL               *Smbios;
  EFI_STATUS                        Status;
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  EFI_SMBIOS_TABLE_HEADER           *Record;
  SMBIOS_TABLE_TYPE42               *Type42Record;
  UINT16                            Offset;
  UINT8                             *RecordTmp;
  UINT8                             ProtocolLength;
  REDFISH_OVER_IP_PROTOCOL_DATA     *ProtocolData;
  UINT8                             SpecificDataLen;

  if (Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Locate SMBIOS protocol.
  //
  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **)&Smbios);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
  while (!EFI_ERROR (Status) && SmbiosHandle != SMBIOS_HANDLE_PI_RESERVED) {
    if (Record->Type == EFI_SMBIOS_TYPE_MANAGEMENT_CONTROLLER_HOST_INTERFACE) {
      //
      // Check Interface Type, should be Network Host Interface = 40h
      //
      Type42Record = (SMBIOS_TABLE_TYPE42 *) Record;
      if (Type42Record->InterfaceType == REDFISH_NETWORK_HOST_INTERFACE_TYPE) {
        ASSERT (Record->Length >= 9);
        Offset = 5;
        RecordTmp = (UINT8 *) Record + Offset;
        //
        // Get interface specific data length.
        //
        SpecificDataLen = *RecordTmp;
        Offset += 1;
        RecordTmp = (UINT8 *) Record + Offset;

        //
        // Check Device Type, only PCI/PCIe Network Interface is supported now.
        //
        if (*RecordTmp == REDFISH_HOST_INTERFACE_DEVICE_TYPE_PCI_PCIE) {
          ASSERT (SpecificDataLen == sizeof (PCI_OR_PCIE_INTERFACE_DEVICE_DESCRIPTOR) + 1);
          Offset = Offset + SpecificDataLen;
          RecordTmp = (UINT8 *) Record + Offset;
          //
          // Check Protocol count. if > 1, only use the first protocol.
          //
          ASSERT (*RecordTmp == 1);
          Offset += 1;
          RecordTmp = (UINT8 *) Record + Offset;
          //
          // Check protocol identifier.
          //
          if (*RecordTmp == REDFISH_HOST_INTERFACE_PROTOCOL_ID_OVER_IP) {
            Offset += 1;
            RecordTmp = (UINT8 *) Record + Offset;
            ProtocolLength = *RecordTmp;

            Offset += 1;
            RecordTmp = (UINT8 *) Record + Offset;

            //
            // This SMBIOS record is invalid, if the length of protocol specific data for
            // Redfish Over IP protocol is wrong.
            //
            if ((*(RecordTmp + 90) + sizeof (REDFISH_OVER_IP_PROTOCOL_DATA) - 1) != ProtocolLength) {
              return EFI_SECURITY_VIOLATION;
            }

            Offset += ProtocolLength;
            //
            // This SMBIOS record is invalid, if the length is smaller than the offset.
            //
            if (Offset > Type42Record->Hdr.Length) {
              return EFI_SECURITY_VIOLATION;
            }
            //
            // Get the Redfish over IP protocol data.
            //
            ProtocolData = (VOID *) AllocateZeroPool (ProtocolLength);
            if (ProtocolData == NULL) {
              return EFI_OUT_OF_RESOURCES;
            }

            CopyMem (ProtocolData, RecordTmp, ProtocolLength);
            *Data = ProtocolData;
            return EFI_SUCCESS;
          }
        }
      }
    }
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
  }

  *Data = NULL;
  return EFI_NOT_FOUND;
}

/**
  Callback function executed when the EndOfDxe event group is signaled.

  @param[in]   Event    Event whose notification function is being invoked.
  @param[OUT]  Context  Pointer to the Context buffer.

**/
VOID
EFIAPI
RedfishConfigOnEndOfDxe (
  IN  EFI_EVENT  Event,
  OUT VOID       *Context
  )
{
  EFI_STATUS                          Status;
  UINT8                               *SecureBootVar;

  //
  // Check Secure Boot status and lock Redfish service if Secure Boot is disabled.
  //
  Status = GetVariable2 (EFI_SECURE_BOOT_MODE_NAME, &gEfiGlobalVariableGuid, (VOID**)&SecureBootVar, NULL);
  if (EFI_ERROR (Status) || (*SecureBootVar != SECURE_BOOT_MODE_ENABLE)) {
    Status = mCredential->StopService (mCredential);
    if (EFI_ERROR(Status)) {
      DEBUG ((EFI_D_ERROR, "Redfish credential protocol faied to stop service on EndOfDxe: %r", Status));
    }
  }

  //
  // Close event, so it will not be invoked again.
  //
  gBS->CloseEvent (mEndOfDxeEvent);
  mEndOfDxeEvent = NULL;
}

/**
  Callback function executed when the ExitBootService event group is signaled.

  @param[in]   Event    Event whose notification function is being invoked.
  @param[OUT]  Context  Pointer to the Context buffer

**/
VOID
EFIAPI
RedfishConfigOnExitBootService (
  IN  EFI_EVENT  Event,
  OUT VOID       *Context
  )
{
  EFI_STATUS                          Status;

  Status = mCredential->StopService (mCredential);
  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "Redfish credential protocol faied to stop service on ExitBootService: %r", Status));
  }

}

/**
  Unloads an image.

  @param  ImageHandle           Handle that identifies the image to be unloaded.

  @retval EFI_SUCCESS           The image has been unloaded.
  @retval EFI_INVALID_PARAMETER ImageHandle is not a valid image handle.

**/
EFI_STATUS
EFIAPI
RedfishConfigDriverUnload (
  IN EFI_HANDLE  ImageHandle
  )
{
  EFI_STATUS                      Status;

  if (mRedfishData != NULL) {
    FreePool (mRedfishData);
  }

  if (mEndOfDxeEvent != NULL) {
    gBS->CloseEvent (mEndOfDxeEvent);
    mEndOfDxeEvent = NULL;
  }

  if (mExitBootServiceEvent != NULL) {
    gBS->CloseEvent (mExitBootServiceEvent);
    mExitBootServiceEvent = NULL;
  }

  Status = NetLibDefaultUnload (ImageHandle);

  return Status;
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
RedfishConfigDriverEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS    Status;

  //
  // Discover Redfish Host Interface by SMBIOS table
  //
  Status = RedfishGetNetworkHostInterfaceInfo (&mRedfishData);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Failed to get Redfish Host Interface info from SMBIOS: %r", Status));
    return Status;
  }

  Status = gBS->LocateProtocol (&gEfiRedfishCredentialProtocolGuid, NULL, (VOID **) &mCredential);
  if (EFI_ERROR (Status)) {
    FreePool (mRedfishData);
    return Status;
  }

  //
  // Create EndOfDxe Event.
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  RedfishConfigOnEndOfDxe,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &mEndOfDxeEvent
                  );
  if (EFI_ERROR (Status)) {
    FreePool (mRedfishData);
    return Status;
  }

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  RedfishConfigOnExitBootService,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &mExitBootServiceEvent
                  );
  if (EFI_ERROR (Status)) {
    FreePool (mRedfishData);
    gBS->CloseEvent (mEndOfDxeEvent);
    mEndOfDxeEvent = NULL;
    return Status;
  }

  //
  // Install UEFI Driver Model protocol(s).
  //
  Status = EfiLibInstallDriverBinding (
             ImageHandle,
             SystemTable,
             &gRedfishConfigDriverBinding,
             ImageHandle
             );
  if (EFI_ERROR (Status)) {
    FreePool (mRedfishData);
    gBS->CloseEvent (mEndOfDxeEvent);
    mEndOfDxeEvent = NULL;
    gBS->CloseEvent (mExitBootServiceEvent);
    mExitBootServiceEvent = NULL;
    return Status;
  }

  return EFI_SUCCESS;
}

