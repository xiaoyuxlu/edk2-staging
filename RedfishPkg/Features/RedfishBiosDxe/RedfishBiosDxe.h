/** @file
  Header file for RedfishBiosDxe driver private data.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __EFI_REDFISH_BIOS_DXE_H__
#define __EFI_REDFISH_BIOS_DXE_H__

//
// Libraries
//
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/RedfishLib.h>
#include <Library/HttpLib.h>

//
// Consumed Protocols
//
#include <Protocol/Smbios.h>

//
// Produced Protocols
//
#include <Protocol/RedfishConfigHandler.h>

#include "HiiRedfishLib.h"

#define REDFISH_BIOS_PRIVATE_DATA_SIGNATURE   SIGNATURE_32 ('R', 'B', 'P', 'D')

typedef struct {
  UINT32                                 Signature;
  EFI_REDFISH_CONFIG_HANDLER_PROTOCOL    ConfigHandler;

  REDFISH_SERVICE                        RedfishService;
  EFI_EVENT                              Event;
} REDFISH_BIOS_PRIVATE_DATA;

#define REDFISH_BIOS_PRIVATE_DATA_FROM_PROTOCOL(This) \
          CR ((This), REDFISH_BIOS_PRIVATE_DATA, ConfigHandler, REDFISH_BIOS_PRIVATE_DATA_SIGNATURE)

#endif
