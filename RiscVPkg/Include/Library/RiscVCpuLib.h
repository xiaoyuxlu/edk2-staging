/** @file
  RISC-V package definitions.

  Copyright (c) 2016, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#ifndef __RISCV_CPU_LIB_H__
#define __RISCV_CPU_LIB_H__

#include "RiscV.h"

/**
  RISCV_TRAP_HANDLER
**/
typedef
VOID
(EFIAPI *RISCV_TRAP_HANDLER)(
  VOID
  );

VOID
RiscVSetScratch (RISCV_MACHINE_MODE_CONTEXT *RiscvContext);

UINT32
RiscVGetScratch (VOID);

UINT32
RiscVGetTrapCause (VOID);

UINT32
RiscVReadMachineTimer (VOID);

VOID
RiscVSetMachineTimerCmp (UINT32);
#endif
