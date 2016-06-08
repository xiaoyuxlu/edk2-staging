/**@file
  Defined the platform specific device path which will be filled to
  ConIn/ConOut variables.

Copyright (c) 2016, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>
Copyright (c) 2015, Intel Corporation. All rights reserved.<BR>

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#include "PlatformBootManager.h"

//
// Platform specific keyboard device path
// 
PLATFORM_PCI_DEVICE_PATH gGopDevicePath0 = {
  { 
    {
      ACPI_DEVICE_PATH, ACPI_DP,
        {
          (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)),
          (UINT8)((sizeof (ACPI_HID_DEVICE_PATH)) >> 8)
        }
    },
    EISA_PNP_ID (0x0A03),
    0
  },
  {
    {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
      {
        (UINT8) (sizeof (PCI_DEVICE_PATH)),
        (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8)
      }
    },
    0,   // Function 0.
    2    // Device 2.
  },
  {
    {
     ACPI_DEVICE_PATH,
     ACPI_ADR_DP,
       {
         (UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
         (UINT8)((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8)
       }
    },
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0)
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      END_DEVICE_PATH_LENGTH,
      0
    } \
  }
}; 

//
// Predefined platform default console device path
//
PLATFORM_CONSOLE_CONNECT_ENTRY   gPlatformConsole[] = {
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &gGopDevicePath0,
    CONSOLE_OUT
  },
  {
    NULL,
    0
  }
};
