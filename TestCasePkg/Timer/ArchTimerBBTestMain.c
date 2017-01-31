/** @file
  Test Driver of Timer Architectural Protocol

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ArchTimerBBTestMain.h"

BOOLEAN  EnterThisFunc;
UINTN Count;

//
// Build Data structure here
//

EFI_BB_TEST_PROTOCOL_FIELD gBBTestProtocolField = {
  TIMER_ARCHITECTURAL_PROTOCOL_TEST_REVISION,
  EFI_TIMER_ARCH_PROTOCOL_GUID,
  L"Timer Architectural Protocol Test",
  L"EFI2.0 Timer Architectural Protocol Test"
};

EFI_GUID gSupportProtocolGuid1[2] = {
  EFI_STANDARD_TEST_LIBRARY_GUID,
  ZERO_GUID
};

EFI_BB_TEST_ENTRY_FIELD gBBTestEntryField[] = {
  {
    TIMER_ARCHTICTURAL_PROTOCOL_REGISTERHANDLER_CONSISTENCY_GUID,
    L"RegisterHandler_Conf",
    L"Consistency Test for RegisterHandler",
    EFI_TEST_LEVEL_MINIMAL,
    gSupportProtocolGuid1,
    EFI_TEST_CASE_AUTO | EFI_TEST_CASE_RESET_REQUIRED,
    BBTestRegisterHandlerConsistencyTest
  },
  {
    TIMER_ARCHTICTURAL_PROTOCOL_GETTIMERPERIOD_CONSISTENCY_GUID,
    L"GetTimerPeriod_Conf",
    L"Consistency Test for GetTimerPeriod",
    EFI_TEST_LEVEL_MINIMAL,
    gSupportProtocolGuid1,
    EFI_TEST_CASE_AUTO,
    BBTestGetTimerPeriodConsistencyTest
  },
  {
    TIMER_ARCHTICTURAL_PROTOCOL_REGISTERHANDLER_INTERFACE_GUID,
    L"RegisterHandler_Func",
    L"Interface Test for RegisterHandler",
    EFI_TEST_LEVEL_DEFAULT,
    gSupportProtocolGuid1,
    EFI_TEST_CASE_AUTO | EFI_TEST_CASE_RESET_REQUIRED,
    BBTestRegisterHandlerInterfaceTest
  },
  {
    TIMER_ARCHTICTURAL_PROTOCOL_SETTIMERPERIOD_INTERFACE_GUID,
    L"SetTimerPeriod_Func",
    L"Interface Test for SetTimerPeriod",
    EFI_TEST_LEVEL_DEFAULT,
    gSupportProtocolGuid1,
    EFI_TEST_CASE_AUTO | EFI_TEST_CASE_RESET_REQUIRED,
    BBTestSetTimerPeriodInterfaceTest
  },
  {
    TIMER_ARCHTICTURAL_PROTOCOL_GETTIMERPERIOD_INTERFACE_GUID,
    L"GetTimerPeriod_Func",
    L"Interface Test for GetTimerPeriod",
    EFI_TEST_LEVEL_DEFAULT,
    gSupportProtocolGuid1,
    EFI_TEST_CASE_AUTO | EFI_TEST_CASE_RESET_REQUIRED,
    BBTestGetTimerPeriodInterfaceTest
  },
  {
    TIMER_ARCHTICTURAL_PROTOCOL_GENERATESOFTINTERRUPT_INTERFACE_GUID,
    L"GenerateSoftInterrupt_Func",
    L"Interface Test for GenerateSoftInterrupt",
    EFI_TEST_LEVEL_DEFAULT,
    gSupportProtocolGuid1,
    EFI_TEST_CASE_AUTO | EFI_TEST_CASE_RESET_REQUIRED,
    BBTestGenerateSoftInterruptInterfaceTest
  },
  {
  ZERO_GUID
  }
};


EFI_BB_TEST_PROTOCOL *gBBTestProtocolInterface;

EFI_STATUS
EFIAPI
BBTestTimerArchProtocolUnload (
  IN EFI_HANDLE       ImageHandle
  );

/**
 *  Timer Architecture Protocol Test Driver Entry point.
 *  @param ImageHandle the driver image handle.
 *  @param SystemTable the system table.
 *  @return EFI_SUCCESS the driver is loaded successfully.
 */
EFI_STATUS
EFIAPI
InitializeBBTestTimerArchProtocol (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EfiInitializeTestLib (ImageHandle, SystemTable);

  return EfiInitAndInstallBBTestInterface (
           &ImageHandle,
           &gBBTestProtocolField,
           gBBTestEntryField,
           BBTestTimerArchProtocolUnload,
           &gBBTestProtocolInterface
           );
}

/**
 *  The driver's Unload function.
 *  @param ImageHandle the test driver image handle.
 *  @return EFI_SUCCESS unload successfully.
 */
EFI_STATUS
EFIAPI
BBTestTimerArchProtocolUnload (
  IN EFI_HANDLE       ImageHandle
  )
{
  return EfiUninstallAndFreeBBTestInterface (
           ImageHandle,
           gBBTestProtocolInterface
           );
}

/**
 *  The Timer's Test Handler function.
 *  @param Time Time since the last timer interrupt.
 */
VOID
EFIAPI
TimerTestHandlerNotify (
  IN UINT64           Time
  )
{
  EnterThisFunc = TRUE;
  Count ++;

  return;
}
