/** @file
  Consistency Test Cases of Timer Architectural Protocol

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include "ArchTimerBBTestMain.h"


/**
 *  Entrypoint for EFI_TIMER_ARCH_PROTOCOL.RegisterHandler() Consistency Test.
 *  @param This a pointer of EFI_BB_TEST_PROTOCOL.
 *  @param ClientInterface a pointer to the interface to be tested.
 *  @param TestLevel test "thoroughness" control.
 *  @param SupportHandle a handle containing protocols required.
 *  @return EFI_SUCCESS Finish the test successfully.
 */
//
// TDS 3.1
//
EFI_STATUS
BBTestRegisterHandlerConsistencyTest (
    IN EFI_BB_TEST_PROTOCOL       *This,
    IN VOID                       *ClientInterface,
    IN EFI_TEST_LEVEL             TestLevel,
    IN EFI_HANDLE                 SupportHandle
  )
{
  EFI_STANDARD_TEST_LIBRARY_PROTOCOL   *StandardLib;
  EFI_STATUS                           OldStatus;
  EFI_STATUS                           Status;
  EFI_TIMER_ARCH_PROTOCOL              *Timer;
  EFI_TEST_ASSERTION                   AssertionType;

  //
  // Get the Standard Library Interface
  //
  Status = gtBS->HandleProtocol (
                   SupportHandle,
                   &gEfiStandardTestLibraryGuid,
                   &StandardLib
                   );
  if (EFI_ERROR(Status)) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_FAILED,
                   gTestGenericFailureGuid,
                   L"BS.HandleProtocol - Handle standard test library",
                   L"%a:%d:Status - %r",
                   __FILE__,
                   __LINE__,
                   Status
                   );
    return Status;
  }

  Timer = (EFI_TIMER_ARCH_PROTOCOL *)ClientInterface;

  //
  //Target Assertion Point 3.1.2.1
  //

  //
  // Try to register a handler
  //
  OldStatus = Timer->RegisterHandler (Timer, TimerTestHandlerNotify);
  if (OldStatus== EFI_UNSUPPORTED) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_PASSED,
                   gTestGenericFailureGuid,
                   L"EFI_TIMER_ARCH_PROTOCOL.RegisterHandler - Register a handler",
                   L"%a:%d:Status - %r",
                   __FILE__,
                   __LINE__,
                   OldStatus
                   );
    return OldStatus;
  }
  if (!((OldStatus == EFI_SUCCESS) || (OldStatus == EFI_ALREADY_STARTED))) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_FAILED,
                   gTestGenericFailureGuid,
                   L"EFI_TIMER_ARCH_PROTOCOL.RegisterHandler - Register a handler",
                   L"%a:%d:Status - %r",
                   __FILE__,
                   __LINE__,
                   OldStatus
                   );
    return OldStatus;
  }

  //
  // Register a handler when another handler is already registered
  //
  Status = Timer->RegisterHandler (Timer, TimerTestHandlerNotify);
  if (Status == EFI_ALREADY_STARTED) {
    AssertionType = EFI_TEST_ASSERTION_PASSED;
  } else {
    AssertionType = EFI_TEST_ASSERTION_FAILED;
  }
  StandardLib->RecordAssertion (
                 StandardLib,
                 AssertionType,
                 gArchTimerConsistencyTestAssertionGuid001,
                 L"EFI_TIMER_ARCH_PROTOCOL.RegisterHandler - Register a handler when another handler is already registered",
                 L"%a:%d:Status - %r, Expected = %r",
                 __FILE__,
                 __LINE__,
                 Status,
                 EFI_ALREADY_STARTED
                 );

  //
  // Do not unregister other's Notify Function
  //
  if (OldStatus == EFI_ALREADY_STARTED) {
    return EFI_SUCCESS;
  }

  //
  //Target Assertion Point 3.1.2.2
  //

  //
  // Unregister a handle
  //
  Status = Timer->RegisterHandler (Timer, NULL);
  if (Status == EFI_UNSUPPORTED) {
    StandardLib->RecordAssertion (
                    StandardLib,
                    EFI_TEST_ASSERTION_FAILED,
                    gTestGenericFailureGuid,
                    L"EFI_TIMER_ARCH_PROTOCOL.RegisterHandler - Unregister a handler",
                    L"%a:%d,Status - %r",
                    __FILE__,
                    __LINE__,
                    Status
                    );
    return Status;
  }
  if (!(Status == EFI_SUCCESS)) {
    StandardLib->RecordAssertion (
                    StandardLib,
                    EFI_TEST_ASSERTION_FAILED,
                    gTestGenericFailureGuid,
                    L"EFI_TIMER_ARCH_PROTOCOL.RegisterHandler - Unregister a handler",
                    L"%a:%d,Status - %r",
                    __FILE__,
                    __LINE__,
                    Status
                    );
    return Status;
  }

  //
  // Unregister a handler when no handler is registered
  //
  Status = Timer->RegisterHandler (Timer, NULL);
  if (Status == EFI_INVALID_PARAMETER) {
    AssertionType = EFI_TEST_ASSERTION_PASSED;
  } else {
    AssertionType = EFI_TEST_ASSERTION_FAILED;
  }
  StandardLib->RecordAssertion (
                 StandardLib,
                 AssertionType,
                 gArchTimerConsistencyTestAssertionGuid002,
                 L"EFI_TIMER_ARCH_PROTOCOL.RegisterHandler - Unregister a handler when no handler is registered",
                 L"%a:%d:Status - %r, Expected = %r",
                 __FILE__,
                 __LINE__,
                 Status,
                 EFI_INVALID_PARAMETER
                 );

  return EFI_SUCCESS;
}

/**
 *  Entrypoint for EFI_TIMER_ARCH_PROTOCOL.GetTimerPeriod() Consistency Test.
 *  @param This a pointer of EFI_BB_TEST_PROTOCOL.
 *  @param ClientInterface a pointer to the interface to be tested.
 *  @param TestLevel test "thoroughness" control.
 *  @param SupportHandle a handle containing protocols required.
 *  @return EFI_SUCCESS Finish the test successfully.
 */
//
// TDS 3.2
//
EFI_STATUS
BBTestGetTimerPeriodConsistencyTest (
    IN EFI_BB_TEST_PROTOCOL       *This,
    IN VOID                       *ClientInterface,
    IN EFI_TEST_LEVEL             TestLevel,
    IN EFI_HANDLE                 SupportHandle
  )
{
  EFI_STANDARD_TEST_LIBRARY_PROTOCOL   *StandardLib;
  EFI_STATUS                           Status;
  EFI_TIMER_ARCH_PROTOCOL              *Timer;
  EFI_TEST_ASSERTION                   AssertionType;

  //
  // Get the Standard Library Interface
  //
  Status = gtBS->HandleProtocol (
                   SupportHandle,
                   &gEfiStandardTestLibraryGuid,
                   &StandardLib
                   );
  if (EFI_ERROR(Status)) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_FAILED,
                   gTestGenericFailureGuid,
                   L"BS.HandleProtocol - Handle standard test library",
                   L"%a:%d:Status - %r",
                   __FILE__,
                   __LINE__,
                   Status
                   );
    return Status;
  }

  Timer = (EFI_TIMER_ARCH_PROTOCOL *)ClientInterface;

  //
  //Target Assertion Point 3.2.2.1
  //

  //
  //Call GetTimerPeriod with TimerPeriod equal to NULL
  //
  Status = Timer->GetTimerPeriod (Timer, NULL);
  if (Status == EFI_INVALID_PARAMETER) {
    AssertionType = EFI_TEST_ASSERTION_PASSED;
  } else {
    AssertionType = EFI_TEST_ASSERTION_FAILED;
  }
  StandardLib->RecordAssertion (
                 StandardLib,
                 AssertionType,
                 gArchTimerConsistencyTestAssertionGuid003,
                 L"EFI_TIMER_ARCH_PROTOCOL.GetTimerPeriod - GetTimerPeriod With TimerPeriod equal to NULL",
                 L"%a:%d:Status - %r, Expected = %r",
                 __FILE__,
                 __LINE__,
                 Status,
                 EFI_INVALID_PARAMETER
                 );

  return EFI_SUCCESS;
}

