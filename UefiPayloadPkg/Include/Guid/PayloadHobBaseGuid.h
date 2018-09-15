/** @file
  This file defines the hob structure for system tables like ACPI, SMBIOS tables.
  
  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __PAYLOAD_HOB_BASE_GUID_H__
#define __PAYLOAD_HOB_BASE_GUID_H__

///
/// This service provides the pointer to the base of the HOB list created by the previous stage
/// firmware.
/// This PPI is optional.
///
typedef struct {
  UINT8  Revision;
  UINT8  Reserved0[3];
  UINT32 PayloadHobBase;
} EFI_PEI_PAYLOAD_HOB_BASE_PPI;

extern EFI_GUID gEfiPayLoadHobBasePpiGuid;
  
#endif
