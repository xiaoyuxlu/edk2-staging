/** @file
  This file defines the EFI_REDFISH_CONFIG_HANDLER_PROTOCOL interface.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __EFI_REDFISH_CONFIG_HANDLER_H__
#define __EFI_REDFISH_CONFIG_HANDLER_H__

#include <IndustryStandard/RedfishHostInterface.h>

typedef struct _EFI_REDFISH_CONFIG_HANDLER_PROTOCOL EFI_REDFISH_CONFIG_HANDLER_PROTOCOL;

#define EFI_REDFISH_CONFIG_HANDLER_PROTOCOL_GUID \
    {  \
      0xbc0fe6bb, 0x2cc9, 0x463e, { 0x90, 0x82, 0xfa, 0x11, 0x76, 0xfc, 0x67, 0xde }  \
    }

/**
  Initialize a Redfish configure handler.

  This function will be called by the Redfish config driver to initialize each Redfish configure
  handler.

  @param[in]   This                    Pointer to EFI_REDFISH_CONFIG_HANDLER_PROTOCOL instance.
  @param[in]   DriverBindingHandle     The driver binding handle used to open the REST EX service.
                                       This parameter is passed to config handler protocol for
                                       building the UEFI driver model relationship.
  @param[in]   Controller              The controller which has the REST EX service installed.
  @param[in]   RedfishData             The Redfish service data.

  @retval EFI_SUCCESS                  The handler has been initialized successfully.
  @retval EFI_DEVICE_ERROR             Failed to create or configure the REST EX protocol instance.
  @retval EFI_ALREADY_STARTED          This handler has already been initialized.
  @retval Other                        Error happens during the initialization.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_REDFISH_CONFIG_HANDLER_PROTOCOL_INIT) (
  IN  EFI_REDFISH_CONFIG_HANDLER_PROTOCOL       *This,
  IN  EFI_HANDLE                                DriverBindingHandle,
  IN  EFI_HANDLE                                ControllerHandle,
  IN  REDFISH_OVER_IP_PROTOCOL_DATA             *RedfishData
  );

/**
  Stop a Redfish configure handler.

  @param[in]   This                Pointer to EFI_REDFISH_CONFIG_HANDLER_PROTOCOL instance.

  @retval EFI_SUCCESS              This handler has been stoped successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_REDFISH_CONFIG_HANDLER_PROTOCOL_STOP) (
  IN     EFI_REDFISH_CONFIG_HANDLER_PROTOCOL    *This
  );

struct _EFI_REDFISH_CONFIG_HANDLER_PROTOCOL {
  EFI_REDFISH_CONFIG_HANDLER_PROTOCOL_INIT      Init;
  EFI_REDFISH_CONFIG_HANDLER_PROTOCOL_STOP      Stop;
};


extern EFI_GUID gEfiRedfishConfigHandlerProtocolGuid;

#endif
