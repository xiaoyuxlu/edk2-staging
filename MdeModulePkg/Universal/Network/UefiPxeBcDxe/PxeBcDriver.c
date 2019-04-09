/** @file
  The driver binding for UEFI PXEBC protocol.

Copyright (c) 2007 - 2015, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include "PxeBcImpl.h"

EFI_DRIVER_BINDING_PROTOCOL gPxeBcDriverBinding = {
  PxeBcDriverBindingSupported,
  PxeBcDriverBindingStart,
  PxeBcDriverBindingStop,
  0xa,
  NULL,
  NULL
};

/**
  This is the declaration of an EFI image entry point. This entry point is
  the same for UEFI Applications, UEFI OS Loaders, and UEFI Drivers including
  both device drivers and bus drivers.

  @param  ImageHandle           The firmware allocated handle for the UEFI image.
  @param  SystemTable           A pointer to the EFI System Table.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
PxeBcDriverEntryPoint (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  return EfiLibInstallDriverBindingComponentName2 (
          ImageHandle,
          SystemTable,
          &gPxeBcDriverBinding,
          ImageHandle,
          &gPxeBcComponentName,
          &gPxeBcComponentName2
          );
}


/**
  Test to see if this driver supports ControllerHandle. This service
  is called by the EFI boot service ConnectController(). In
  order to make drivers as small as possible, there are a few calling
  restrictions for this service. ConnectController() must
  follow these calling restrictions. If any other agent wishes to call
  Supported() it must also follow these calling restrictions.
  PxeBc requires DHCP4 and MTFTP4 protocols.

  @param  This                Protocol instance pointer.
  @param  ControllerHandle    Handle of device to test
  @param  RemainingDevicePath Optional parameter use to pick a specific child
                              device to start.

  @retval EFI_SUCCESS         This driver supports this device
  @retval EFI_ALREADY_STARTED This driver is already running on this device
  @retval other               This driver does not support this device

**/
EFI_STATUS
EFIAPI
PxeBcDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  * This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     * RemainingDevicePath OPTIONAL
  )
{
  EFI_PXE_BASE_CODE_PROTOCOL  *PxeBc;
  EFI_STATUS                  Status;

  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiPxeBaseCodeProtocolGuid,
                  (VOID **) &PxeBc,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (!EFI_ERROR (Status)) {
    return EFI_ALREADY_STARTED;
  }

  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiDhcp4ServiceBindingProtocolGuid,
                  NULL,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );

  return Status;
}


/**
  Start this driver on ControllerHandle. This service is called by the
  EFI boot service ConnectController(). In order to make
  drivers as small as possible, there are a few calling restrictions for
  this service. ConnectController() must follow these
  calling restrictions. If any other agent wishes to call Start() it
  must also follow these calling restrictions.

  @param  This                 Protocol instance pointer.
  @param  ControllerHandle     Handle of device to bind driver to
  @param  RemainingDevicePath  Optional parameter use to pick a specific child
                               device to start.

  @retval EFI_SUCCESS          This driver is added to ControllerHandle
  @retval EFI_ALREADY_STARTED  This driver is already running on ControllerHandle
  @retval other                This driver does not support this device

**/
EFI_STATUS
EFIAPI
PxeBcDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  * This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     * RemainingDevicePath OPTIONAL
  )
{
  PXEBC_PRIVATE_DATA  *Private;
  UINTN               Index;
  EFI_STATUS          Status;
//  EFI_IP4_MODE_DATA   Ip4ModeData;

  DEBUG ((EFI_D_INFO, "[PXE BC] PxeBcDriverBindingStart\n"));

  Private = AllocateZeroPool (sizeof (PXEBC_PRIVATE_DATA));
  if (Private == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Private->Signature                    = PXEBC_PRIVATE_DATA_SIGNATURE;
  Private->Controller                   = ControllerHandle;
  Private->Image                        = This->DriverBindingHandle;
  CopyMem (&Private->PxeBc, &mPxeBcProtocolTemplate, sizeof (Private->PxeBc));
  Private->PxeBc.Mode                   = &Private->Mode;
  CopyMem (&Private->LoadFile, &mLoadFileProtocolTemplate, sizeof (Private->LoadFile));

  Private->ProxyOffer.Packet.Offer.Size = PXEBC_CACHED_DHCP4_PACKET_MAX_SIZE;
  Private->Dhcp4Ack.Packet.Ack.Size     = PXEBC_CACHED_DHCP4_PACKET_MAX_SIZE;
  Private->PxeReply.Packet.Ack.Size     = PXEBC_CACHED_DHCP4_PACKET_MAX_SIZE;

  for (Index = 0; Index < PXEBC_MAX_OFFER_NUM; Index++) {
    Private->Dhcp4Offers[Index].Packet.Offer.Size = PXEBC_CACHED_DHCP4_PACKET_MAX_SIZE;
  }

  //
  // Get the NII interface if it exists.
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiNetworkInterfaceIdentifierProtocolGuid_31,
                  (VOID **) &Private->Nii,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "[PXE BC] PxeBcDriverBindingStart: NII not found.\n"));
    Private->Nii = NULL;
  }

  Status = NetLibCreateServiceChild (
            ControllerHandle,
            This->DriverBindingHandle,
            &gEfiDhcp4ServiceBindingProtocolGuid,
            &Private->Dhcp4Child
            );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "[PXE BC] PxeBcDriverBindingStart: Failed to create DHCP child: %r\n", Status));
    goto ON_ERROR;
  }

  Status = gBS->OpenProtocol (
                  Private->Dhcp4Child,
                  &gEfiDhcp4ProtocolGuid,
                  (VOID **) &Private->Dhcp4,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "[PXE BC] PxeBcDriverBindingStart: Failed to open DHCP protocol: %r\n", Status));
    goto ON_ERROR;
  }

  Private->Ip4MaxPacketSize = PXEBC_DEFAULT_PACKET_SIZE;

  {
    EFI_HANDLE    Buffer[2];
    UINTN         BufferSize;
    EFI_SERVICE_BINDING_PROTOCOL  *MtftpSb;

    BufferSize = sizeof(Buffer);

    Status = gBS->LocateHandle (
                    ByProtocol,
                    &gEfiMtftp4ServiceBindingProtocolGuid,
                    NULL,
                    &BufferSize,
                    Buffer
                    );

    ASSERT_EFI_ERROR (Status);
    ASSERT (BufferSize == sizeof(EFI_HANDLE));

    MtftpSb = NULL;

    Status = gBS->OpenProtocol (
                    Buffer[0],
                    &gEfiMtftp4ServiceBindingProtocolGuid,
                    &MtftpSb,
                    This->DriverBindingHandle,
                    Buffer[0],
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );

    ASSERT_EFI_ERROR (Status);
    ASSERT (MtftpSb != NULL);

    Status = MtftpSb->CreateChild (
                        MtftpSb,
                        &Private->Mtftp4Child
                        );

    ASSERT_EFI_ERROR (Status);
    ASSERT (Private->Mtftp4Child != NULL);
  }

  Status = gBS->OpenProtocol (
                  Private->Mtftp4Child,
                  &gEfiMtftp4ProtocolGuid,
                  (VOID **) &Private->Mtftp4,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  DEBUG ((EFI_D_INFO, "[PXE BC] Opening MTFTP4 protocol: %r\n", Status));

  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  Status = gBS->LocateProtocol (
                  &gEfiMpSocketProtocolGuid,
                  NULL,
                  (VOID**) &Private->Sockets
                  );

  DEBUG ((EFI_D_INFO, "[PXE BC] Opening LWIP socket protocol: %r\n", Status));

  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  //
  // Create socket for PxeBc->UdpRead()
  //
  Private->InputSocket = -1;

  //
  // Do not create socket for PxeBc->UdpWrite() yet.
  //
  Private->OutputSocket   = -1;

  PxeBcInitSeedPacket (&Private->SeedPacket, Private->Dhcp4);
  Private->MacLen = Private->SeedPacket.Dhcp4.Header.HwAddrLen;
  CopyMem (&Private->Mac, &Private->SeedPacket.Dhcp4.Header.ClientHwAddr[0], Private->MacLen);

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ControllerHandle,
                  &gEfiPxeBaseCodeProtocolGuid,
                  &Private->PxeBc,
                  &gEfiLoadFileProtocolGuid,
                  &Private->LoadFile,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "[PXE BC] PxeBcDriverBindingStart: Failed to install PXE BC: %r\n", Status));
    goto ON_ERROR;
  }

  DEBUG ((EFI_D_INFO, "[PXE BC] PxeBcDriverBindingStart: OK\n"));

  return EFI_SUCCESS;

ON_ERROR:

  if (Private->Sockets) {
    if (Private->InputSocket != -1) {
      Private->Sockets->Close (
                          Private->Sockets,
                          Private->InputSocket
                          );
    }
  }

  if (Private->Mtftp4Child != NULL) {
    gBS->CloseProtocol (
          Private->Mtftp4Child,
          &gEfiMtftp4ProtocolGuid,
          This->DriverBindingHandle,
          ControllerHandle
          );

    NetLibDestroyServiceChild (
      ControllerHandle,
      This->DriverBindingHandle,
      &gEfiMtftp4ServiceBindingProtocolGuid,
      Private->Mtftp4Child
      );
  }

  if (Private->Dhcp4Child != NULL) {
    gBS->CloseProtocol (
          Private->Dhcp4Child,
          &gEfiDhcp4ProtocolGuid,
          This->DriverBindingHandle,
          ControllerHandle
          );

    NetLibDestroyServiceChild (
      ControllerHandle,
      This->DriverBindingHandle,
      &gEfiDhcp4ServiceBindingProtocolGuid,
      Private->Dhcp4Child
      );
  }

  FreePool (Private);

  return Status;
}


/**
  Stop this driver on ControllerHandle. This service is called by the
  EFI boot service DisconnectController(). In order to
  make drivers as small as possible, there are a few calling
  restrictions for this service. DisconnectController()
  must follow these calling restrictions. If any other agent wishes
  to call Stop() it must also follow these calling restrictions.

  @param  This              Protocol instance pointer.
  @param  ControllerHandle  Handle of device to stop driver on
  @param  NumberOfChildren  Number of Handles in ChildHandleBuffer. If number of
                            children is zero stop the entire bus driver.
  @param  ChildHandleBuffer List of Child Handles to Stop.

  @retval EFI_SUCCESS       This driver is removed ControllerHandle
  @retval other             This driver was not removed from this device

**/
EFI_STATUS
EFIAPI
PxeBcDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN UINTN                        NumberOfChildren,
  IN EFI_HANDLE                   *ChildHandleBuffer
  )
{
  PXEBC_PRIVATE_DATA          *Private;
  EFI_PXE_BASE_CODE_PROTOCOL  *PxeBc;
  EFI_HANDLE                  NicHandle;
  EFI_STATUS                  Status;

  NicHandle = NetLibGetNicHandle (ControllerHandle, &gEfiArpProtocolGuid);
  if (NicHandle == NULL) {
    NicHandle = NetLibGetNicHandle (ControllerHandle, &gEfiDhcp4ProtocolGuid);

    if (NicHandle == NULL) {
      NicHandle = NetLibGetNicHandle (ControllerHandle, &gEfiIp4ProtocolGuid);

      if (NicHandle == NULL) {
        NicHandle = NetLibGetNicHandle (ControllerHandle, &gEfiUdp4ProtocolGuid);

        if (NicHandle == NULL) {
          NicHandle = NetLibGetNicHandle (ControllerHandle, &gEfiMtftp4ProtocolGuid);

          if (NicHandle == NULL) {
            return EFI_SUCCESS;
          }
        }
      }
    }
  }

  Status = gBS->OpenProtocol (
                  NicHandle,
                  &gEfiPxeBaseCodeProtocolGuid,
                  (VOID **) &PxeBc,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Stop functionality of PXE Base Code protocol
  //
  Status = PxeBc->Stop (PxeBc);
  if (Status != EFI_SUCCESS && Status != EFI_NOT_STARTED) {
    return Status;
  }

  Private = PXEBC_PRIVATE_DATA_FROM_PXEBC (PxeBc);

  Status = gBS->UninstallMultipleProtocolInterfaces (
                  NicHandle,
                  &gEfiPxeBaseCodeProtocolGuid,
                  &Private->PxeBc,
                  &gEfiLoadFileProtocolGuid,
                  &Private->LoadFile,
                  NULL
                  );

  if (!EFI_ERROR (Status)) {
    gBS->CloseProtocol (
          Private->Dhcp4Child,
          &gEfiDhcp4ProtocolGuid,
          This->DriverBindingHandle,
          NicHandle
          );
    NetLibDestroyServiceChild (
      NicHandle,
      This->DriverBindingHandle,
      &gEfiDhcp4ServiceBindingProtocolGuid,
      Private->Dhcp4Child
      );

    gBS->CloseProtocol (
          Private->Mtftp4Child,
          &gEfiMtftp4ProtocolGuid,
          This->DriverBindingHandle,
          NicHandle
          );
    NetLibDestroyServiceChild (
      NicHandle,
      This->DriverBindingHandle,
      &gEfiMtftp4ServiceBindingProtocolGuid,
      Private->Mtftp4Child
      );

    FreePool (Private);
  }

  return Status;
}


