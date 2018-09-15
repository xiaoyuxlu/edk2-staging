/** @file
  Helper functions that access register bits

  Copyright (c) 2012 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_SMM_HELPERS_H_
#define _SC_SMM_HELPERS_H_

#include "ScSmm.h"

/**
  Check if a specific bit in a register is set

  @param[in] BitDesc              The struct that includes register address, size in byte and bit number

  @retval    TRUE                 The bit is enabled
  @retval    FALSE                The bit is disabled

**/
BOOLEAN
ReadBitDesc (
  IN CONST SC_SMM_BIT_DESC        *BitDesc
  );

/**
  Write a specific bit in a register

  @param[in] BitDesc              The struct that includes register address, size in byte and bit number
  @param[in] ValueToWrite         The value to be written
  @param[in] WriteClear           If the rest bits of the register is write clear

**/
VOID
WriteBitDesc (
  IN CONST SC_SMM_BIT_DESC        *BitDesc,
  IN CONST BOOLEAN                ValueToWrite,
  IN CONST BOOLEAN                WriteClear
  );

#endif

