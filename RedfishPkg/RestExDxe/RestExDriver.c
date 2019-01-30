/** @file
  The driver binding and service binding protocol for RestExDxe driver.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "RestExImpl.h"

EFI_DRIVER_BINDING_PROTOCOL gRestExDriverBinding = {
  RestExDriverBindingSupported,
  RestExDriverBindingStart,
  RestExDriverBindingStop,
  RESTEX_VERSION,
  NULL,
  NULL
};


EFI_SERVICE_BINDING_PROTOCOL mRestExServiceBinding = {
  RestExServiceBindingCreateChild,
  RestExServiceBindingDestroyChild
};

/**
  Callback function which provided by user to remove one node in NetDestroyLinkList process.

  @param[in]    Entry           The entry to be removed.
  @param[in]    Context         Pointer to the callback context corresponds to the Context in NetDestroyLinkList.

  @retval EFI_SUCCESS           The entry has been removed successfully.
  @retval Others                Fail to remove the entry.

**/
EFI_STATUS
EFIAPI
RestExDestroyChildEntryInHandleBuffer (
  IN LIST_ENTRY         *Entry,
  IN VOID               *Context
  )
{
  RESTEX_INSTANCE               *Instance;
  EFI_SERVICE_BINDING_PROTOCOL  *ServiceBinding;
  UINTN                         NumberOfChildren;
  EFI_HANDLE                    *ChildHandleBuffer;

  if (Entry == NULL || Context == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Instance = NET_LIST_USER_STRUCT_S (Entry, RESTEX_INSTANCE, Link, RESTEX_INSTANCE_SIGNATURE);
  ServiceBinding    = ((RESTEX_DESTROY_CHILD_IN_HANDLE_BUF_CONTEXT *) Context)->ServiceBinding;
  NumberOfChildren  = ((RESTEX_DESTROY_CHILD_IN_HANDLE_BUF_CONTEXT *) Context)->NumberOfChildren;
  ChildHandleBuffer = ((RESTEX_DESTROY_CHILD_IN_HANDLE_BUF_CONTEXT *) Context)->ChildHandleBuffer;

  if (!NetIsInHandleBuffer (Instance->ChildHandle, NumberOfChildren, ChildHandleBuffer)) {
    return EFI_SUCCESS;
  }

  return ServiceBinding->DestroyChild (ServiceBinding, Instance->ChildHandle);
}

/**
  Destroy the RestEx instance and recycle the resources.

  @param[in]  Instance        The pointer to the RestEx instance.

**/
VOID
RestExDestroyInstance (
  IN RESTEX_INSTANCE         *Instance
  )
{
  HttpIoDestroyIo (&(Instance->HttpIo));

  FreePool (Instance);
}

/**
  Create the RestEx instance and initialize it.

  @param[in]  Service              The pointer to the RestEx service.
  @param[out] Instance             The pointer to the RestEx instance.

  @retval EFI_OUT_OF_RESOURCES   Failed to allocate resources.
  @retval EFI_SUCCESS            The RestEx instance is created.

**/
EFI_STATUS
RestExCreateInstance (
  IN  RESTEX_SERVICE         *Service,
  OUT RESTEX_INSTANCE        **Instance
  )
{
  RESTEX_INSTANCE            *RestExIns;
  EFI_STATUS                 Status;

  *Instance = NULL;
  Status    = EFI_SUCCESS;

  RestExIns = AllocateZeroPool (sizeof (RESTEX_INSTANCE));
  if (RestExIns == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  RestExIns->Signature = RESTEX_INSTANCE_SIGNATURE;
  InitializeListHead (&RestExIns->Link);
  RestExIns->InDestroy = FALSE;
  RestExIns->Service   = Service;

  CopyMem (&RestExIns->RestEx, &mRestExProtocol, sizeof (RestExIns->RestEx));

  //
  // Create a HTTP_IO to access the HTTP service.
  //
  Status = HttpIoCreateIo (
             RestExIns->Service->ImageHandle,
             RestExIns->Service->ControllerHandle,
             IP_VERSION_4,
             NULL,
             &(RestExIns->HttpIo)
             );
  if (EFI_ERROR (Status)) {
    FreePool (RestExIns);
    return Status;
  }

  *Instance = RestExIns;

  return EFI_SUCCESS;
}

/**
  Release all the resource used the RestEx service binding instance.

  @param  RestExSb                The RestEx service binding instance.

**/
VOID
RestExDestroyService (
  IN RESTEX_SERVICE     *RestExSb
  )
{
  if (RestExSb->HttpChildHandle != NULL) {
    gBS->CloseProtocol (
           RestExSb->HttpChildHandle,
           &gEfiHttpProtocolGuid,
           RestExSb->ImageHandle,
           RestExSb->ControllerHandle
           );

    NetLibDestroyServiceChild (
      RestExSb->ControllerHandle,
      RestExSb->ImageHandle,
      &gEfiHttpServiceBindingProtocolGuid,
      RestExSb->HttpChildHandle
      );

    RestExSb->HttpChildHandle = NULL;
  }

  gBS->UninstallProtocolInterface (
         RestExSb->ControllerHandle,
         &gEfiCallerIdGuid,
         &RestExSb->Id
         );

  FreePool (RestExSb);
}

/**
  Check the NIC controller handle represents an in-band or out-of-band Redfish host
  interface device. If not in-band, treat it as out-of-band interface device.

  @param  Controller             The NIC controller handle needs to be checked.

  @return     EFI_REST_EX_SERVICE_ACCESS_MODE of the device.

**/
EFI_REST_EX_SERVICE_ACCESS_MODE
RestExServiceAccessMode (
  IN     EFI_HANDLE            Controller
  )
{
  EFI_DEVICE_PATH_PROTOCOL                 *ControllerDevicePath;
  ACPI_HID_DEVICE_PATH                     *Acpi;
  PCI_DEVICE_PATH                          *Pci;
  MAC_ADDR_DEVICE_PATH                     *Mac;
  REST_EX_SERVICE_IN_BAND_DEVICE_PATH_DATA *RestExServiceInBandDevicePathData;
  EFI_DEVICE_PATH_PROTOCOL                 *RestExServiceInBandDevicePath;
  UINTN                                    RemainingDevicePathNum;

  ControllerDevicePath               = NULL;
  Acpi                               = NULL;
  Pci                                = NULL;
  Mac                                = NULL;
  RestExServiceInBandDevicePathData  = NULL;
  RestExServiceInBandDevicePath      = NULL;

  //
  // Retrieve NIC controller device path:
  //   PciRoot(...)/Pci(...)/Mac(...)[/Vlan(...)]
  //
  ControllerDevicePath = DevicePathFromHandle (Controller);
  if (ControllerDevicePath == NULL || !IsDevicePathValid (ControllerDevicePath, 0)) {
    goto RETURN_OOB;
  }

  //
  // Retrieve RestEx Service In Band DevicePath.
  //
  RestExServiceInBandDevicePathData = (REST_EX_SERVICE_IN_BAND_DEVICE_PATH_DATA *)PcdGetPtr(PcdRestExServiceInBandDevicePath);
  if (RestExServiceInBandDevicePathData == NULL ||
      RestExServiceInBandDevicePathData->DevicePathNum == 0 ||
      !IsDevicePathValid (RestExServiceInBandDevicePathData->DevicePath, 0)) {
    goto RETURN_OOB;
  }

  RestExServiceInBandDevicePath = RestExServiceInBandDevicePathData->DevicePath;
  RemainingDevicePathNum        = RestExServiceInBandDevicePathData->DevicePathNum;

  //
  // Parse controller device path.
  //
  while (!IsDevicePathEnd (ControllerDevicePath)) {
    if ((DevicePathType (ControllerDevicePath) == ACPI_DEVICE_PATH) &&
        (DevicePathSubType (ControllerDevicePath) == ACPI_DP)) {
      Acpi = (ACPI_HID_DEVICE_PATH *) ControllerDevicePath;
    } else if ((DevicePathType (ControllerDevicePath) == HARDWARE_DEVICE_PATH) &&
               (DevicePathSubType (ControllerDevicePath) == HW_PCI_DP)) {
      Pci = (PCI_DEVICE_PATH *) ControllerDevicePath;
    } else if ((DevicePathType (ControllerDevicePath) == MESSAGING_DEVICE_PATH) &&
               (DevicePathSubType (ControllerDevicePath) == MSG_MAC_ADDR_DP)) {
      Mac = (MAC_ADDR_DEVICE_PATH *) ControllerDevicePath;
    }

    ControllerDevicePath = NextDevicePathNode (ControllerDevicePath);
  }

  //
  // Match In Band DevicePath.
  //
  switch (RestExServiceInBandDevicePathData->DevicePathMatchMode) {
  case DEVICE_PATH_MATCH_MAC_NODE:
    if (Mac == NULL) {
      goto RETURN_OOB;
    }

    while (RemainingDevicePathNum > 0) {
      //
      // Find Mac DevicePath Node.
      //
      while (!IsDevicePathEnd (RestExServiceInBandDevicePath) &&
             ((DevicePathType (RestExServiceInBandDevicePath) != MESSAGING_DEVICE_PATH) ||
              (DevicePathSubType (RestExServiceInBandDevicePath) != MSG_MAC_ADDR_DP))) {
        RestExServiceInBandDevicePath = NextDevicePathNode (RestExServiceInBandDevicePath);
      }

      if (!IsDevicePathEnd (RestExServiceInBandDevicePath)) {
        if (CompareMem (Mac, RestExServiceInBandDevicePath, DevicePathNodeLength (RestExServiceInBandDevicePath)) == 0) {
          return EFI_REST_EX_SERVICE_IN_BAND_ACCESS;
        }

        //
        // Skip to End DevicePath Node.
        //
        while (!IsDevicePathEnd (RestExServiceInBandDevicePath)) {
          RestExServiceInBandDevicePath = NextDevicePathNode (RestExServiceInBandDevicePath);
        }
      }

      RemainingDevicePathNum --;
      if (RemainingDevicePathNum > 0) {
        //
        // Skip current End DevicePath Node since we still have another DevicePath need to be checked.
        //
        RestExServiceInBandDevicePath = NextDevicePathNode (RestExServiceInBandDevicePath);
      }
    }

    break;
  case DEVICE_PATH_MATCH_PCI_NODE:
    //
    // Since the PCI device path entry must be preceded by an ACPI device path entry
    // that uniquely identifies the PCI root bus, we also parse ACPI Device Path node.
    //
    if (Acpi == NULL || Pci ==  NULL) {
      goto RETURN_OOB;
    }

    while (RemainingDevicePathNum > 0) {
      //
      // Find Acpi/Pci DevicePath Nodes.
      //
      while (!IsDevicePathEnd (RestExServiceInBandDevicePath) &&
             ((DevicePathType (RestExServiceInBandDevicePath) != ACPI_DEVICE_PATH) ||
              (DevicePathSubType (RestExServiceInBandDevicePath) != ACPI_DP)) &&
             ((DevicePathType (NextDevicePathNode (RestExServiceInBandDevicePath)) != HARDWARE_DEVICE_PATH) ||
              (DevicePathSubType (NextDevicePathNode (RestExServiceInBandDevicePath)) != HW_PCI_DP))) {
        RestExServiceInBandDevicePath = NextDevicePathNode (RestExServiceInBandDevicePath);
      }

      if (!IsDevicePathEnd (RestExServiceInBandDevicePath)) {
        if ((CompareMem (Acpi, RestExServiceInBandDevicePath, DevicePathNodeLength (RestExServiceInBandDevicePath)) == 0) &&
            (CompareMem (Pci, NextDevicePathNode (RestExServiceInBandDevicePath), DevicePathNodeLength (NextDevicePathNode (RestExServiceInBandDevicePath))) == 0)) {
          return EFI_REST_EX_SERVICE_IN_BAND_ACCESS;
        }

        //
        // Skip to End DevicePath Node.
        //
        while (!IsDevicePathEnd (RestExServiceInBandDevicePath)) {
          RestExServiceInBandDevicePath = NextDevicePathNode (RestExServiceInBandDevicePath);
        }
      }

      RemainingDevicePathNum --;
      if (RemainingDevicePathNum > 0) {
        //
        // Skip current End DevicePath Node since we still have another DevicePath need to be checked.
        //
        RestExServiceInBandDevicePath = NextDevicePathNode (RestExServiceInBandDevicePath);
      }
    }

    break;
  default:
    goto RETURN_OOB;
  }

RETURN_OOB:
  return EFI_REST_EX_SERVICE_OUT_OF_BAND_ACCESS;
}

/**
  Create then initialize a RestEx service binding instance.

  @param  Controller             The controller to install the RestEx service
                                 binding on.
  @param  Image                  The driver binding image of the RestEx driver.
  @param  Service                The variable to receive the created service
                                 binding instance.

  @retval EFI_OUT_OF_RESOURCES   Failed to allocate resource to create the instance.
  @retval EFI_SUCCESS            The service instance is created for the controller.

**/
EFI_STATUS
RestExCreateService (
  IN     EFI_HANDLE            Controller,
  IN     EFI_HANDLE            Image,
     OUT RESTEX_SERVICE        **Service
  )
{
  EFI_STATUS                Status;
  RESTEX_SERVICE            *RestExSb;

  Status       = EFI_SUCCESS;
  RestExSb     = NULL;

  *Service  = NULL;

  RestExSb = AllocateZeroPool (sizeof (RESTEX_SERVICE));
  if (RestExSb == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  RestExSb->Signature = RESTEX_SERVICE_SIGNATURE;

  RestExSb->ServiceBinding = mRestExServiceBinding;

  RestExSb->RestExChildrenNum = 0;
  InitializeListHead (&RestExSb->RestExChildrenList);

  RestExSb->ControllerHandle = Controller;
  RestExSb->ImageHandle      = Image;

  RestExSb->RestExServiceInfo.EfiRestExServiceInfoV10.EfiRestExServiceInfoHeader.Length = sizeof (EFI_REST_EX_SERVICE_INFO);
  RestExSb->RestExServiceInfo.EfiRestExServiceInfoV10.EfiRestExServiceInfoHeader.RestServiceInfoVer.Major = 1;
  RestExSb->RestExServiceInfo.EfiRestExServiceInfoV10.EfiRestExServiceInfoHeader.RestServiceInfoVer.Minor = 1;
  RestExSb->RestExServiceInfo.EfiRestExServiceInfoV10.RestServiceType = EFI_REST_EX_SERVICE_REDFISH;
  RestExSb->RestExServiceInfo.EfiRestExServiceInfoV10.RestServiceAccessMode = RestExServiceAccessMode (Controller);
  RestExSb->RestExServiceInfo.EfiRestExServiceInfoV10.RestExConfigType = EFI_REST_EX_CONFIG_TYPE_HTTP;
  RestExSb->RestExServiceInfo.EfiRestExServiceInfoV10.RestExConfigDataLength = sizeof (EFI_REST_EX_HTTP_CONFIG_DATA);

  Status = gBS->InstallProtocolInterface (
                  &Controller,
                  &gEfiCallerIdGuid,
                  EFI_NATIVE_INTERFACE,
                  &RestExSb->Id
                  );
  if (EFI_ERROR (Status)) {
    FreePool (RestExSb);
    RestExSb = NULL;
  }

  *Service = RestExSb;
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
RestExDriverEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;

  //
  // Install the RestEx Driver Binding Protocol.
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &gRestExDriverBinding,
             ImageHandle,
             &gRestExComponentName,
             &gRestExComponentName2
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

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
RestExDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  )
{

  //
  // Test for the HttpServiceBinding Protocol.
  //
  return gBS->OpenProtocol (
                ControllerHandle,
                &gEfiHttpServiceBindingProtocolGuid,
                NULL,
                This->DriverBindingHandle,
                ControllerHandle,
                EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                );

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
RestExDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  )
{
  RESTEX_SERVICE         *RestExSb;
  EFI_STATUS             Status;
  UINT32                 *Id;
  VOID                   *Interface;

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

  Status = RestExCreateService (ControllerHandle, This->DriverBindingHandle, &RestExSb);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (RestExSb != NULL);

  //
  // Create a Http child instance, but do not configure it.
  // This will establish the parent-child relationship.
  //
  Status = NetLibCreateServiceChild (
             ControllerHandle,
             This->DriverBindingHandle,
             &gEfiHttpServiceBindingProtocolGuid,
             &RestExSb->HttpChildHandle
             );
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  Status = gBS->OpenProtocol (
                  RestExSb->HttpChildHandle,
                  &gEfiHttpProtocolGuid,
                  &Interface,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  //
  // Install the RestEx ServiceBinding Protocol onto ControllerHandle.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ControllerHandle,
                  &gEfiRestExServiceBindingProtocolGuid,
                  &RestExSb->ServiceBinding,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  return EFI_SUCCESS;

ON_ERROR:
  RestExDestroyService (RestExSb);

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
RestExDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN UINTN                        NumberOfChildren,
  IN EFI_HANDLE                   *ChildHandleBuffer OPTIONAL
  )
{
  EFI_SERVICE_BINDING_PROTOCOL               *ServiceBinding;
  RESTEX_SERVICE                             *RestExSb;
  EFI_HANDLE                                 NicHandle;
  EFI_STATUS                                 Status;
  LIST_ENTRY                                 *List;
  RESTEX_DESTROY_CHILD_IN_HANDLE_BUF_CONTEXT Context;

  //
  // RestEx driver opens HTTP child, So, Controller is a HTTP
  // child handle. Locate the Nic handle first. Then get the
  // RestEx private data back.
  //
  NicHandle = NetLibGetNicHandle (ControllerHandle, &gEfiHttpProtocolGuid);
  if (NicHandle == NULL) {
    return EFI_SUCCESS;
  }

  Status = gBS->OpenProtocol (
                  NicHandle,
                  &gEfiRestExServiceBindingProtocolGuid,
                  (VOID **) &ServiceBinding,
                  This->DriverBindingHandle,
                  NicHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  RestExSb = RESTEX_SERVICE_FROM_THIS (ServiceBinding);

  if (!IsListEmpty (&RestExSb->RestExChildrenList)) {
    //
    // Destroy the RestEx child instance in ChildHandleBuffer.
    //
    List = &RestExSb->RestExChildrenList;
    Context.ServiceBinding    = ServiceBinding;
    Context.NumberOfChildren  = NumberOfChildren;
    Context.ChildHandleBuffer = ChildHandleBuffer;
    Status = NetDestroyLinkList (
               List,
               RestExDestroyChildEntryInHandleBuffer,
               &Context,
               NULL
               );
  }

  if (NumberOfChildren == 0 && IsListEmpty (&RestExSb->RestExChildrenList)) {
    gBS->UninstallProtocolInterface (
           NicHandle,
           &gEfiRestExServiceBindingProtocolGuid,
           ServiceBinding
           );

    RestExDestroyService (RestExSb);

    if (gRestExControllerNameTable != NULL) {
      FreeUnicodeStringTable (gRestExControllerNameTable);
      gRestExControllerNameTable = NULL;
    }

    Status = EFI_SUCCESS;
  }

  return Status;
}

/**
  Creates a child handle and installs a protocol.

  The CreateChild() function installs a protocol on ChildHandle.
  If ChildHandle is a pointer to NULL, then a new handle is created and returned in ChildHandle.
  If ChildHandle is not a pointer to NULL, then the protocol installs on the existing ChildHandle.

  @param[in] This        Pointer to the EFI_SERVICE_BINDING_PROTOCOL instance.
  @param[in] ChildHandle Pointer to the handle of the child to create. If it is NULL,
                         then a new handle is created. If it is a pointer to an existing UEFI handle,
                         then the protocol is added to the existing UEFI handle.

  @retval EFI_SUCCES            The protocol was added to ChildHandle.
  @retval EFI_INVALID_PARAMETER ChildHandle is NULL.
  @retval EFI_OUT_OF_RESOURCES  There are not enough resources available to create
                                the child
  @retval other                 The child handle was not created

**/
EFI_STATUS
EFIAPI
RestExServiceBindingCreateChild (
  IN EFI_SERVICE_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                    *ChildHandle
  )
{
  RESTEX_SERVICE               *RestExSb;
  RESTEX_INSTANCE              *Instance;
  EFI_STATUS                   Status;
  EFI_TPL                      OldTpl;
  VOID                         *Http;

  if ((This == NULL) || (ChildHandle == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  RestExSb = RESTEX_SERVICE_FROM_THIS (This);

  Status = RestExCreateInstance (RestExSb, &Instance);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ASSERT (Instance != NULL);

  //
  // Install the RestEx protocol onto ChildHandle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  ChildHandle,
                  &gEfiRestExProtocolGuid,
                  &Instance->RestEx,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  Instance->ChildHandle = *ChildHandle;

  //
  // Open the Http protocol BY_CHILD.
  //
  Status = gBS->OpenProtocol (
                  RestExSb->HttpChildHandle,
                  &gEfiHttpProtocolGuid,
                  (VOID **) &Http,
                  gRestExDriverBinding.DriverBindingHandle,
                  Instance->ChildHandle,
                  EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                  );
  if (EFI_ERROR (Status)) {
    gBS->UninstallMultipleProtocolInterfaces (
           Instance->ChildHandle,
           &gEfiRestExProtocolGuid,
           &Instance->RestEx,
           NULL
           );

    goto ON_ERROR;
  }

  //
  // Open the Http protocol by child.
  //
  Status = gBS->OpenProtocol (
                  Instance->HttpIo.Handle,
                  &gEfiHttpProtocolGuid,
                  (VOID **) &Http,
                  gRestExDriverBinding.DriverBindingHandle,
                  Instance->ChildHandle,
                  EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                  );
  if (EFI_ERROR (Status)) {
    //
    // Close the Http protocol.
    //
    gBS->CloseProtocol (
           RestExSb->HttpChildHandle,
           &gEfiHttpProtocolGuid,
           gRestExDriverBinding.DriverBindingHandle,
           ChildHandle
           );

     gBS->UninstallMultipleProtocolInterfaces (
            Instance->ChildHandle,
            &gEfiRestExProtocolGuid,
            &Instance->RestEx,
            NULL
            );

    goto ON_ERROR;
  }

  //
  // Add it to the parent's child list.
  //
  OldTpl = gBS->RaiseTPL (TPL_CALLBACK);

  InsertTailList (&RestExSb->RestExChildrenList, &Instance->Link);
  RestExSb->RestExChildrenNum++;

  gBS->RestoreTPL (OldTpl);

  return EFI_SUCCESS;

ON_ERROR:

  RestExDestroyInstance (Instance);
  return Status;
}

/**
  Destroys a child handle with a protocol installed on it.

  The DestroyChild() function does the opposite of CreateChild(). It removes a protocol
  that was installed by CreateChild() from ChildHandle. If the removed protocol is the
  last protocol on ChildHandle, then ChildHandle is destroyed.

  @param[in] This        Pointer to the EFI_SERVICE_BINDING_PROTOCOL instance.
  @param[in] ChildHandle Handle of the child to destroy

  @retval EFI_SUCCES            The protocol was removed from ChildHandle.
  @retval EFI_UNSUPPORTED       ChildHandle does not support the protocol that is being removed.
  @retval EFI_INVALID_PARAMETER Child handle is NULL.
  @retval EFI_ACCESS_DENIED     The protocol could not be removed from the ChildHandle
                                because its services are being used.
  @retval other                 The child handle was not destroyed

**/
EFI_STATUS
EFIAPI
RestExServiceBindingDestroyChild (
  IN EFI_SERVICE_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                    ChildHandle
  )
{
  RESTEX_SERVICE               *RestExSb;
  RESTEX_INSTANCE              *Instance;

  EFI_REST_EX_PROTOCOL         *RestEx;
  EFI_STATUS                   Status;
  EFI_TPL                      OldTpl;

  if ((This == NULL) || (ChildHandle == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Retrieve the private context data structures
  //
  Status = gBS->OpenProtocol (
                  ChildHandle,
                  &gEfiRestExProtocolGuid,
                  (VOID **) &RestEx,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Instance  = RESTEX_INSTANCE_FROM_THIS (RestEx);
  RestExSb  = RESTEX_SERVICE_FROM_THIS (This);

  if (Instance->Service != RestExSb) {
    return EFI_INVALID_PARAMETER;
  }

  if (Instance->InDestroy) {
    return EFI_SUCCESS;
  }

  OldTpl = gBS->RaiseTPL (TPL_CALLBACK);

  Instance->InDestroy = TRUE;

  //
  // Close the Http protocol.
  //
  gBS->CloseProtocol (
         RestExSb->HttpChildHandle,
         &gEfiHttpProtocolGuid,
         gRestExDriverBinding.DriverBindingHandle,
         ChildHandle
         );

  gBS->CloseProtocol (
         Instance->HttpIo.Handle,
         &gEfiHttpProtocolGuid,
         gRestExDriverBinding.DriverBindingHandle,
         ChildHandle
         );


  gBS->RestoreTPL (OldTpl);

  //
  // Uninstall the RestEx protocol first to enable a top down destruction.
  //
  Status = gBS->UninstallProtocolInterface (
                  ChildHandle,
                  &gEfiRestExProtocolGuid,
                  RestEx
                  );

  OldTpl = gBS->RaiseTPL (TPL_CALLBACK);

  if (EFI_ERROR (Status)) {
    Instance->InDestroy = FALSE;
    gBS->RestoreTPL (OldTpl);
    return Status;
  }

  RemoveEntryList (&Instance->Link);
  RestExSb->RestExChildrenNum--;

  gBS->RestoreTPL (OldTpl);

  RestExDestroyInstance (Instance);
  return EFI_SUCCESS;
}

