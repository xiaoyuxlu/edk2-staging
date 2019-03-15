/** @file

Sample driver for exposing capabilities through UEFI Runtime Driver.

Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _RT_SAMPLE_H_
#define _RT_SAMPLE_H_

#include <Uefi.h>
#include <Protocol/Variable.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Guid/EventGroup.h>

// {09763E5D-FA8C-4554-8EF6-C6E457CD62FA}
#define GET_VAR_HOOK_GUID { 0x9763e5d, 0xfa8c, 0x4554, { 0x8e, 0xf6, 0xc6, 0xe4, 0x57, 0xcd, 0x62, 0xfa } }
#define MMIO_BASE_ADDRESS 0xfd110000
#define R_REGISTER        0x128

/**
  A service function provided to RT driver. Prints a string 

  @param  String              Formatted string.

  @retval (VOID)

**/
typedef 
VOID
(EFIAPI *CORE_PRINT_FUNC) (
  IN CHAR8 *String
  );

#endif
