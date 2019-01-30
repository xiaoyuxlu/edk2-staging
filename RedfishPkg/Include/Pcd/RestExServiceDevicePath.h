/** @file
  This library defines the UEFI device path data of network device for REST
  service to decide which should be used as the Redfish host interface.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __REST_EX_SERVICE_DEVICE_PATH_H__
#define __REST_EX_SERVICE_DEVICE_PATH_H__

#include <Protocol/DevicePath.h>

typedef enum {
  DEVICE_PATH_MATCH_MAC_NODE = 1,
  DEVICE_PATH_MATCH_PCI_NODE = 2,
  DEVICE_PATH_MATCH_MODE_MAX
} DEVICE_PATH_MATCH_MODE;

typedef struct {
  DEVICE_PATH_MATCH_MODE        DevicePathMatchMode;
  UINTN                         DevicePathNum;
  //
  // Example:
  //   {DEVICE_PATH("PciRoot(0)/Pci(0,0)/MAC(005056C00002,0x1)")}
  // DevicePath will be parsed as below:
  //   {0x02,0x01,0x0c,0x00,0xd0,0x41,0x03,0x0a,0x00,0x00,0x00,0x00,
  //    0x01,0x01,0x06,0x00,0x00,0x00,
  //    0x03,0x0b,0x25,0x00,0x00,0x50,0x56,0xc0,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
  //    0x7f,0xff,0x04,0x00}
  //
  EFI_DEVICE_PATH_PROTOCOL      DevicePath[];
} REST_EX_SERVICE_IN_BAND_DEVICE_PATH_DATA;

#endif
