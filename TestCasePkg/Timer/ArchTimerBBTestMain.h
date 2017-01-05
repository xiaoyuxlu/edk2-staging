/** @file
  BB test header file of Timer Architectural Protocol

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#ifndef _ARCH_TIMER_BBTEST_H_
#define _ARCH_TIMER_BBTEST_H_

#include <PiDxe.h>
#include "Guid.h"
#include <Protocol/Timer.h>

#include <Library/EfiTestLib.h>
#include <Guid/ZeroGuid.h>

#define TIMER_ARCHITECTURAL_PROTOCOL_TEST_REVISION 0x00010000

extern BOOLEAN EnterThisFunc;
extern UINTN Count;

EFI_STATUS
EFIAPI
BBTestRegisterHandlerConsistencyTest (
  IN EFI_BB_TEST_PROTOCOL       *This,
  IN VOID                       *ClientInterface,
  IN EFI_TEST_LEVEL             TestLevel,
  IN EFI_HANDLE                 SupportHandle
  );

EFI_STATUS
EFIAPI
BBTestGetTimerPeriodConsistencyTest (
  IN EFI_BB_TEST_PROTOCOL       *This,
  IN VOID                       *ClientInterface,
  IN EFI_TEST_LEVEL             TestLevel,
  IN EFI_HANDLE                 SupportHandle
  );

EFI_STATUS
EFIAPI
BBTestRegisterHandlerInterfaceTest (
  IN EFI_BB_TEST_PROTOCOL       *This,
  IN VOID                       *ClientInterface,
  IN EFI_TEST_LEVEL             TestLevel,
  IN EFI_HANDLE                 SupportHandle
  );

EFI_STATUS
EFIAPI
BBTestSetTimerPeriodInterfaceTest (
    IN EFI_BB_TEST_PROTOCOL       *This,
    IN VOID                       *ClientInterface,
    IN EFI_TEST_LEVEL             TestLevel,
    IN EFI_HANDLE                 SupportHandle
  );

EFI_STATUS
EFIAPI
BBTestGetTimerPeriodInterfaceTest (
    IN EFI_BB_TEST_PROTOCOL       *This,
    IN VOID                       *ClientInterface,
    IN EFI_TEST_LEVEL             TestLevel,
    IN EFI_HANDLE                 SupportHandle
  );

EFI_STATUS
EFIAPI
BBTestGenerateSoftInterruptInterfaceTest (
    IN EFI_BB_TEST_PROTOCOL       *This,
    IN VOID                       *ClientInterface,
    IN EFI_TEST_LEVEL             TestLevel,
    IN EFI_HANDLE                 SupportHandle
  );


EFI_STATUS
EFIAPI
InitializeBBTestTimerArchProtocol (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
EFIAPI
BBTestTimerArchProtocolUnload (
  IN EFI_HANDLE       ImageHandle
  );

//
// Entry GUIDs
//

#define TIMER_ARCHTICTURAL_PROTOCOL_REGISTERHANDLER_CONSISTENCY_GUID\
  { 0xbec1c453, 0x6d56, 0x4b0f, { 0x96, 0x13, 0x44, 0xb4, 0x56, 0xfd, 0xc9, 0xea } }

#define TIMER_ARCHTICTURAL_PROTOCOL_GETTIMERPERIOD_CONSISTENCY_GUID\
  { 0xc99ba244, 0xc93e, 0x402b, { 0xac, 0x39, 0x40, 0x89, 0x52, 0x3f, 0xd7, 0x68 } }

#define TIMER_ARCHTICTURAL_PROTOCOL_REGISTERHANDLER_INTERFACE_GUID\
  { 0x8e58aa40, 0xe506, 0x4a9c, { 0xbe, 0xc, 0xba, 0xc0, 0x14, 0x44, 0xab, 0xe7 } }

#define TIMER_ARCHTICTURAL_PROTOCOL_SETTIMERPERIOD_INTERFACE_GUID\
  { 0xc3fb1fc7, 0xdb71, 0x4651, { 0x89, 0x63, 0xbf, 0x15, 0xf6, 0xcb, 0xe9, 0xd } }

#define TIMER_ARCHTICTURAL_PROTOCOL_GETTIMERPERIOD_INTERFACE_GUID\
  { 0x2a123900, 0x661a, 0x4047, { 0xa1, 0x73, 0xed, 0x21, 0x7b, 0x3d, 0x80, 0x61 } }

#define TIMER_ARCHTICTURAL_PROTOCOL_GENERATESOFTINTERRUPT_INTERFACE_GUID\
  { 0x7626a07e, 0x644a, 0x44db, { 0x85, 0xcb, 0xf6, 0x63, 0x85, 0xc, 0xe2, 0x34 } }

VOID
EFIAPI
TimerTestHandlerNotify (
  IN UINT64           Time
  );

#endif

