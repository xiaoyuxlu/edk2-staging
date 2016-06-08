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

#ifndef __RISCV_H__
#define __RISCV_H__

#if defined(MDE_CPU_RISCV32)
#define RISC_V_XLEN_BITS 32
#elif defined (MDE_CPU_RISCV64)
#define RISC_V_XLEN_BITS 64
#elif defined (MDE_CPU_RISCV128)
#define RISC_V_XLEN_BITS 128
#else
#endif

//
// RISC-V CSR definitions.
//
//
// Machine Trap Setup.
//
#define RISCV_CSR_MACHINE_MCPUID        0xF00
#define RISCV_CSR_MACHINE_MIMPID        0xF01
#define RISCV_CSR_MACHINE_MHARTID       0xF10
//
// Machine Trap Setup.
//
#define RISCV_CSR_MACHINE_MSTATUS       0x300
#define RISCV_CSR_MACHINE_MTVEC         0x301
#define RISCV_CSR_MACHINE_MTDELEG       0x302
#define RISCV_CSR_MACHINE_MIE           0x304
#define RISCV_CSR_MACHINE_MTIMECMP      0x321
  #define RISCV_TIMER_COMPARE_BITS      32
//
// Machine Timer and Counter.
//
#define RISCV_CSR_MACHINE_MTIME         0x701
#define RISCV_CSR_MACHINE_MTIMEH        0x741
//
// Machine Trap Handling.
//
#define RISCV_CSR_MACHINE_MSCRATCH      0x340
#define RISCV_CSR_MACHINE_MEPC          0x341
#define RISCV_CSR_MACHINE_MCAUSE        0x342
  #define MACHINE_MCAUSE_EXCEPTION_ MASK 0x0f
  #define MACHINE_MCAUSE_INTERRUPT      (RISC_V_XLEN_BITS - 1)
#define RISCV_CSR_MACHINE_MBADADDR      0x343
#define RISCV_CSR_MACHINE_MIP           0x344

//
// Machine Protection and Translation.
//
#define RISCV_CSR_MACHINE_MBASE         0x380
#define RISCV_CSR_MACHINE_MBOUND        0x381
#define RISCV_CSR_MACHINE_MIBASE        0x382
#define RISCV_CSR_MACHINE_MIBOUND       0x383
#define RISCV_CSR_MACHINE_MDBASE        0x384
#define RISCV_CSR_MACHINE_MDBOUND       0x385
//
// Machine Read-Write Shadow of Hypervisor Read-Only Registers
//
#define RISCV_CSR_HTIMEW                0xB01
#define RISCV_CSR_HTIMEHW               0xB81
//
// Machine Host-Target Interface (Non-Standard Berkeley Extension)
//
#define RISCV_CSR_MTOHOST               0x780
#define RISCV_CSR_MFROMHOST             0x781

#define RISCV_MACHINE_CONTEXT_SIZE  0x1000
typedef struct _RISCV_MACHINE_MODE_CONTEXT RISCV_MACHINE_MODE_CONTEXT;

///
/// Exception handlers in context.
///
typedef struct _EXCEPTION_HANDLER_CONTEXT {
  EFI_PHYSICAL_ADDRESS InstAddressMisalignedHander;
  EFI_PHYSICAL_ADDRESS InstAccessFaultHander;
  EFI_PHYSICAL_ADDRESS IllegalInstHander;
  EFI_PHYSICAL_ADDRESS BreakpointHander;
  EFI_PHYSICAL_ADDRESS LoadAddrMisalignedHander;
  EFI_PHYSICAL_ADDRESS LoadAccessFaultHander;
  EFI_PHYSICAL_ADDRESS StoreAmoAddrMisalignedHander;
  EFI_PHYSICAL_ADDRESS StoreAmoAccessFaultHander;
  EFI_PHYSICAL_ADDRESS EnvCallFromUModeHander;
  EFI_PHYSICAL_ADDRESS EnvCallFromSModeHander;
  EFI_PHYSICAL_ADDRESS EnvCallFromHModeHander;
  EFI_PHYSICAL_ADDRESS EnvCallFromMModeHander;
} EXCEPTION_HANDLER_CONTEXT;

///
/// Exception handlers in context.
///
typedef struct _INTERRUPT_HANDLER_CONTEXT {
  EFI_PHYSICAL_ADDRESS SoftwareIntHandler;
  EFI_PHYSICAL_ADDRESS TimerIntHandler;
} INTERRUPT_HANDLER_CONTEXT;

///
/// Interrupt handlers in context.
///
typedef struct _TRAP_HANDLER_CONTEXT {
  EXCEPTION_HANDLER_CONTEXT ExceptionHandlerContext;
  INTERRUPT_HANDLER_CONTEXT IntHandlerContext;
} TRAP_HANDLER_CONTEXT;

///
/// Machine mode context used for saveing hart-local context.
///
typedef struct _RISCV_MACHINE_MODE_CONTEXT {
  EFI_PHYSICAL_ADDRESS PeiService;                /// PEI service.
  EFI_PHYSICAL_ADDRESS MachineModeTrapHandler;    /// Machine mode trap handler.
  EFI_PHYSICAL_ADDRESS HypervisorModeTrapHandler; /// Hypervisor mode trap handler.
  EFI_PHYSICAL_ADDRESS SupervisorModeTrapHandler; /// Supervisor mode trap handler.
  EFI_PHYSICAL_ADDRESS UserModeTrapHandler;       /// USer mode trap handler.
  TRAP_HANDLER_CONTEXT MModeHandler;              /// Handler for machine mode.
} RISCV_MACHINE_MODE_CONTEXT;

#endif
