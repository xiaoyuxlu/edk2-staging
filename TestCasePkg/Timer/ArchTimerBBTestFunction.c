/** @file
  Interface Test Cases of Timer Architectural Protocol

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
 *  Entrypoint for EFI_TIMER_ARCH_PROTOCOL.RegisterHandler() Interface Test.
 *  @param This a pointer of EFI_BB_TEST_PROTOCOL.
 *  @param ClientInterface a pointer to the interface to be tested.
 *  @param TestLevel test "thoroughness" control.
 *  @param SupportHandle a handle containing protocols required.
 *  @return EFI_SUCCESS Finish the test successfully.
 */
//
// TDS 4.1
//
EFI_STATUS
BBTestRegisterHandlerInterfaceTest (
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
  UINT64                               OldTimerPeriod;
  UINT64                               TimerPeriod;

  //
  // Get the Standard Library Interface
  //
  Status = gtBS->HandleProtocol (
                   SupportHandle,
                   &gEfiStandardTestLibraryGuid,
                   (VOID **)&StandardLib
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
  //Target Assertion Point 4.1.2.1
  //RegisterHandler must succeed to register a handler when there is no other handler previously registered
  //

  //
  // Try to register a handler
  //
  Count = 0;
  EnterThisFunc = FALSE;
  Status = Timer->RegisterHandler (Timer, TimerTestHandlerNotify);
  if (Status== EFI_UNSUPPORTED) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_PASSED,
                   gTestGenericFailureGuid,
                   L"EFI_TIMER_ARCH_PROTOCOL.RegisterHandler - Register a handler",
                   L"%a:%d:Status - %r",
                   __FILE__,
                   __LINE__,
                   Status
                   );
    return Status;
  }
  if ((Status == EFI_SUCCESS) || (Status == EFI_ALREADY_STARTED)) {
    AssertionType = EFI_TEST_ASSERTION_PASSED;
  } else {
    AssertionType = EFI_TEST_ASSERTION_FAILED;
  }
  StandardLib->RecordAssertion (
                 StandardLib,
                 AssertionType,
                 gArchTimerInterfaceTestAssertionGuid001,
                 L"EFI_TIMER_ARCH_PROTOCOL.RegisterHandler - Register a handler",
                 L"%a:%d:Status - %r, Expected = %r or %r",
                 __FILE__,
                 __LINE__,
                 Status,
                 EFI_SUCCESS,
                 EFI_ALREADY_STARTED
                 );

  //
  // If the handle already registered, stop test.
  //
  if (Status == EFI_ALREADY_STARTED) {
    return EFI_SUCCESS;
  }

  //
  //Record Old TimerPeriod
  //
  Status = Timer->GetTimerPeriod (Timer, &OldTimerPeriod);
  if (Status != EFI_SUCCESS) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_FAILED,
                   gTestGenericFailureGuid,
                   L"EFI_TIMER_ARCH_PROTOCOL.GetTimerPeriod - Get OldTimerPeriod",
                   L"%a:%d:Status - %r",
                   __FILE__,
                   __LINE__,
                   Status
                   );
    return Status;
  }

  //
  //Enable Timer with TimerPeriod 1 second
  //
  TimerPeriod = 10000;
  Status = Timer->SetTimerPeriod (Timer, TimerPeriod);
  if (Status != EFI_SUCCESS) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_FAILED,
                   gTestGenericFailureGuid,
                   L"EFI_TIMER_ARCH_PROTOCOL.SetTimerPeriod - Set TimerPeriod",
                   L"%a:%d:TimerPeriod:0x%lx,Status - %r",
                   __FILE__,
                   __LINE__,
                   TimerPeriod,
                   Status
                   );
    return Status;
  }

  //
  //Wait for 2s to verify whether NotifyFunction is invoked!
  //
  gtBS->Stall (2000000);
  if (EnterThisFunc == TRUE) {
    AssertionType = EFI_TEST_ASSERTION_PASSED;
  } else {
    AssertionType = EFI_TEST_ASSERTION_FAILED;
  }
  StandardLib->RecordAssertion (
                 StandardLib,
                 AssertionType,
                 gArchTimerInterfaceTestAssertionGuid002,
                 L"EFI_TIMER_ARCH_PROTOCOL.RegisterHandler - Register a handler with no previous handler installed",
                 L"%a:%d:Count:%d,Stall:2000000,TimerPeriod:10000",
                 __FILE__,
                 __LINE__,
                 Count
                 );

  //
  //Target Assertion 4.1.2.2
  //RegisterHandler must succeed to unregister a handler that is previously registered
  //

  //
  //Last step has confirm that a handler is already installed and Timer is enabled
  //Thus, it is no need to Register a handler for unregister..
  //If need, add RegisterHandler and SetTimerPeriod here
  //

  //
  //Call RegisterHandler with NotifyFunction being NULL
  //
  Count = 0;
  EnterThisFunc = FALSE;
  Status = Timer->RegisterHandler (Timer, NULL);
  if (Status== EFI_UNSUPPORTED) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_PASSED,
                   gTestGenericFailureGuid,
                   L"EFI_TIMER_ARCH_PROTOCOL.RegisterHandler - Unregister a handler",
                   L"%a:%d:Status - %r",
                   __FILE__,
                   __LINE__,
                   Status
                   );
    return Status;
  }
  if (Status == EFI_SUCCESS) {
    AssertionType = EFI_TEST_ASSERTION_PASSED;
  } else {
    AssertionType = EFI_TEST_ASSERTION_FAILED;
  }
  StandardLib->RecordAssertion (
                 StandardLib,
                 AssertionType,
                 gArchTimerInterfaceTestAssertionGuid003,
                 L"EFI_TIMER_ARCH_PROTOCOL.RegisterHandler - Unregister a handler that is previously registered",
                 L"%a:%d:Status - %r, Expected = %r",
                 __FILE__,
                 __LINE__,
                 Status,
                 EFI_SUCCESS
                 );

  //
  //Wait for 2s to verify whether NotifyFunction is invoked!
  //
  gtBS->Stall (2000000);
  if (EnterThisFunc == FALSE) {
    AssertionType = EFI_TEST_ASSERTION_PASSED;
  } else {
    AssertionType = EFI_TEST_ASSERTION_FAILED;
  }
  StandardLib->RecordAssertion (
                 StandardLib,
                 AssertionType,
                 gArchTimerInterfaceTestAssertionGuid004,
                 L"EFI_TIMER_ARCH_PROTOCOL.RegisterHandler - Unregister a handler that is previously registered",
                 L"%a:%d:Count:%d,Stall:2000000,TimerPeriod:10000",
                 __FILE__,
                 __LINE__,
                 Count
                 );

  //
  // Restore oldTimerPeriod
  //
  Timer->SetTimerPeriod (Timer, OldTimerPeriod);

  return EFI_SUCCESS;
}

/**
 *  Entrypoint for EFI_TIMER_ARCH_PROTOCOL.SetTimerPeriod() Interface Test.
 *  @param This a pointer of EFI_BB_TEST_PROTOCOL.
 *  @param ClientInterface a pointer to the interface to be tested.
 *  @param TestLevel test "thoroughness" control.
 *  @param SupportHandle a handle containing protocols required.
 *  @return EFI_SUCCESS Finish the test successfully.
 */
//
// TDS 4.2
//
EFI_STATUS
BBTestSetTimerPeriodInterfaceTest (
    IN EFI_BB_TEST_PROTOCOL       *This,
    IN VOID                       *ClientInterface,
    IN EFI_TEST_LEVEL             TestLevel,
    IN EFI_HANDLE                 SupportHandle
  )
{
  EFI_STANDARD_TEST_LIBRARY_PROTOCOL   *StandardLib;
  EFI_STATUS                           RegStatus;
  EFI_STATUS                           Status;
  EFI_TIMER_ARCH_PROTOCOL              *Timer;
  UINT64                               OldTimerPeriod;
  UINT64                               TimerPeriod;
  UINT64                               TimerPeriodVerify;
  EFI_TEST_ASSERTION                   AssertionType;

  //
  // Get the Standard Library Interface
  //
  Status = gtBS->HandleProtocol (
                   SupportHandle,
                   &gEfiStandardTestLibraryGuid,
                   (VOID **)&StandardLib
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

  Status = Timer->GetTimerPeriod (Timer, &OldTimerPeriod);
  if (EFI_ERROR(Status)) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_FAILED,
                   gTestGenericFailureGuid,
                   L"EFI_TIMER_ARCH_PROTOCOL.GetTimerPeriod - Get OldTimerPeriod",
                   L"%a:%d:Status - %r",
                   __FILE__,
                   __LINE__,
                   Status
                   );
    return EFI_SUCCESS;
  }

  //
  //Target Assertion Point 4.2.2.1
  //SetTimerPeriod must succed with valid TimerPeriod
  //

  TimerPeriod = 100000;

  //
  //Set Timer Period with specified value
  //
  Status = Timer->SetTimerPeriod (Timer, TimerPeriod);
  if (Status == EFI_UNSUPPORTED) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_PASSED,
                   gTestGenericFailureGuid,
                   L"EFI_TIMER_ARCH_PROTOCOL.SetTimerPeriod - SetTimerPeriod with valid TimerPeriod",
                   L"%a:%d:TimerPeriod:0x%lx,Status - %r",
                   __FILE__,
                   __LINE__,
                   TimerPeriod,
                   Status
                   );
    return Status;
  }
  if (Status == EFI_SUCCESS) {
    AssertionType = EFI_TEST_ASSERTION_PASSED;
  } else {
    AssertionType = EFI_TEST_ASSERTION_FAILED;
  }
  StandardLib->RecordAssertion (
                 StandardLib,
                 AssertionType,
                 gArchTimerInterfaceTestAssertionGuid005,
                 L"EFI_TIMER_ARCH_PROTOCOL.SetTimerPeriod - SetTimerPeriod with valid TimerPeriod",
                 L"%a:%d:TimerPeriod:0x%lx,Status - %r",
                 __FILE__,
                 __LINE__,
                 TimerPeriod,
                 Status
                 );

  //
  //Call GetTimerPeriod to verify the correctness of SetTimerPeriod
  //
  Status = Timer->GetTimerPeriod (Timer, &TimerPeriodVerify);
  if (EFI_ERROR(Status)) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_FAILED,
                   gTestGenericFailureGuid,
                   L"EFI_TIMER_ARCH_PROTOCOL.GetTimerPeriod - GetTimerPeriod",
                   L"%a:%d:TimerPeriod:0x%lx,Status - %r",
                   __FILE__,
                   __LINE__,
                   TimerPeriod,
                   Status
                   );
    return EFI_SUCCESS;
  }

  if (TimerPeriod == TimerPeriodVerify) {
    AssertionType = EFI_TEST_ASSERTION_PASSED;
  } else {
    AssertionType = EFI_TEST_ASSERTION_FAILED;
  }
  StandardLib->RecordAssertion (
                 StandardLib,
                 AssertionType,
                 gArchTimerInterfaceTestAssertionGuid006,
                 L"EFI_TIMER_ARCH_PROTOCOL.SetTimerPeriod - SetTimerPeriod with valid TimerPeriod, TimerPeriod verification",
                 L"%a:%d:TimerPeriod:0x%lx,TimerPeriodVerify:0x%lx",
                 __FILE__,
                 __LINE__,
                 TimerPeriod,
                 TimerPeriodVerify
                 );

  //
  //Register Ourselves NotifyFunction
  //
  Count = 0;
  RegStatus = Timer->RegisterHandler (Timer, TimerTestHandlerNotify);
  if (EFI_ERROR(RegStatus)) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_FAILED,
                   gTestGenericFailureGuid,
                   L"EFI_TIMER_ARCH_PROTOCOL.RegisterHandler - Register a handler",
                   L"%a:%d:Status - %r",
                   __FILE__,
                   __LINE__,
                   RegStatus
                   );
  }

  if (!EFI_ERROR(RegStatus)) {
    //
    //Wait for 1s to test correctness of function
    //
    gtBS->Stall(1000000);

    //
    //Verify whether NotifyFunction is invoked when Timer fires
    //
    if (Count <= 1) {
      AssertionType = EFI_TEST_ASSERTION_PASSED;
    } else {
      AssertionType = EFI_TEST_ASSERTION_FAILED;
    }
    StandardLib->RecordAssertion (
                   StandardLib,
                   AssertionType,
                   gArchTimerInterfaceTestAssertionGuid007,
                   L"EFI_TIMER_ARCH_PROTOCOL.SetTimerPeriod - SetTimerPeriod with valid TimerPeriod, NotifyFunction verification",
                   L"%a:%d:TimerPeriod:0x%lx, Count - %d",
                   __FILE__,
                   __LINE__,
                   TimerPeriod,
                   Count
                   );

    //
    //Wait for 25s to test correctness of function
    //
    gtBS->Stall(2500000);

    //
    //Verify whether NotifyFunction is invoked when Timer fires
    //
    if (Count >= 2) {
      AssertionType = EFI_TEST_ASSERTION_PASSED;
    } else {
      AssertionType = EFI_TEST_ASSERTION_FAILED;
    }
    StandardLib->RecordAssertion (
                   StandardLib,
                   AssertionType,
                   gArchTimerInterfaceTestAssertionGuid008,
                   L"EFI_TIMER_ARCH_PROTOCOL.SetTimerPeriod - SetTimerPeriod with valid TimerPeriod, NotifyFunction verification",
                   L"%a:%d:TimerPeriod:0x%lx, Count - %d",
                   __FILE__,
                   __LINE__,
                   TimerPeriod,
                   Count
                   );
  }

  //
  //Target Assertion 4.2.2.2
  //SetTimerPeriod must succeed to disable Timer
  //

  //
  //Last step has confirm that a handler is already installed and Timer is enabled
  //If need, add RegisterHandler and SetTimerPeriod here
  //

  //
  //Disable Timer by calling SetTimerPeriod with TimerPeriod being zero
  //
  Status = Timer->SetTimerPeriod (Timer, 0);
  if (Status == EFI_UNSUPPORTED) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_PASSED,
                   gTestGenericFailureGuid,
                   L"EFI_TIMER_ARCH_PROTOCOL.SetTimerPeriod - Disable Timer with TimerPeriod being zero",
                   L"%a:%d:Status - %r",
                   __FILE__,
                   __LINE__,
                   Status
                   );
    return Status;
  }
  if (Status == EFI_SUCCESS) {
    AssertionType = EFI_TEST_ASSERTION_PASSED;
  } else {
    AssertionType = EFI_TEST_ASSERTION_FAILED;
  }
  StandardLib->RecordAssertion (
                 StandardLib,
                 AssertionType,
                 gArchTimerInterfaceTestAssertionGuid009,
                 L"EFI_TIMER_ARCH_PROTOCOL.SetTimerPeriod - Disable Timer with TimerPeriod being zero",
                 L"%a:%d:Status - %r, Expected = %r",
                 __FILE__,
                 __LINE__,
                 Status,
                 EFI_SUCCESS
                 );

  //
  //Call GetTimerPeriod to verify correctness of TimerPeriod
  //
  Status = Timer->GetTimerPeriod (Timer, &TimerPeriodVerify);
  if (EFI_ERROR(Status)) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_FAILED,
                   gTestGenericFailureGuid,
                   L"EFI_TIMER_ARCH_PROTOCOL.GetTimerPeriod - Get Timer Period",
                   L"%a:%d:Status - %r",
                   __FILE__,
                   __LINE__,
                   Status
                   );
    return EFI_SUCCESS;
  }

  if (TimerPeriodVerify == 0)  {
    AssertionType = EFI_TEST_ASSERTION_PASSED;
  } else {
    AssertionType = EFI_TEST_ASSERTION_FAILED;
  }
  StandardLib->RecordAssertion (
                 StandardLib,
                 AssertionType,
                 gArchTimerInterfaceTestAssertionGuid010,
                 L"EFI_TIMER_ARCH_PROTOCOL.SetTimerPeriod - Disable Timer with TimerPeriod being zero, TimerPeriod verificatoin",
                 L"%a:%d:TimerPeriodVerify:%d",
                 __FILE__,
                 __LINE__,
                 TimerPeriodVerify
                 );

  if (!EFI_ERROR(RegStatus)) {
    Count = 0;

    //
    //Wait for 15s to verify that NotifyFunction is not invoked!
    //
    gtBS->Stall (15000000);
    if (Count == 0) {
      AssertionType = EFI_TEST_ASSERTION_PASSED;
    } else {
      AssertionType = EFI_TEST_ASSERTION_FAILED;
    }
    StandardLib->RecordAssertion (
                   StandardLib,
                   AssertionType,
                   gArchTimerInterfaceTestAssertionGuid011,
                   L"EFI_TIMER_ARCH_PROTOCOL.SetTimerPeriod - Disable Timer with TimerPeriod being zero, NotifyFunction verification",
                   L"%a:%d:Count - %d",
                   __FILE__,
                   __LINE__,
                   Count
                   );
  }

  //
  // Restore oldTimerPeriod
  //
  Timer->SetTimerPeriod (Timer, OldTimerPeriod);

  return EFI_SUCCESS;
}

/**
 *  Entrypoint for EFI_TIMER_ARCH_PROTOCOL.GetTimerPeriod() Interface Test.
 *  @param This a pointer of EFI_BB_TEST_PROTOCOL.
 *  @param ClientInterface a pointer to the interface to be tested.
 *  @param TestLevel test "thoroughness" control.
 *  @param SupportHandle a handle containing protocols required.
 *  @return EFI_SUCCESS Finish the test successfully.
 */
//
// TDS 4.3
//
EFI_STATUS
BBTestGetTimerPeriodInterfaceTest (
    IN EFI_BB_TEST_PROTOCOL       *This,
    IN VOID                       *ClientInterface,
    IN EFI_TEST_LEVEL             TestLevel,
    IN EFI_HANDLE                 SupportHandle
  )
{
  EFI_STANDARD_TEST_LIBRARY_PROTOCOL   *StandardLib;
  EFI_STATUS                           Status;
  EFI_TIMER_ARCH_PROTOCOL              *Timer;
  UINT64                               OldTimerPeriod;
  UINT64                               TimerPeriod;
  UINT64                               TimerPeriodVerify;
  UINT16                               Index;
  EFI_TEST_ASSERTION                   AssertionType;
  UINT64                               TimerPeriodArray[2] = {
    0, 10000
  };

  //
  // Get the Standard Library Interface
  //
  Status = gtBS->HandleProtocol (
                   SupportHandle,
                   &gEfiStandardTestLibraryGuid,
                   (VOID **)&StandardLib
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
  //Target Assertion Point 4.3.2.1
  //GetTimerPeriod must succeed with valid parameters
  //

  Status = Timer->GetTimerPeriod (Timer, &OldTimerPeriod);
  if (EFI_ERROR(Status)) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_FAILED,
                   gArchTimerInterfaceTestAssertionGuid012,
                   L"EFI_TIMER_ARCH_PROTOCOL.GetTimerPeriod - Get TimerPeriod",
                   L"%a:%d:Status - %r",
                   __FILE__,
                   __LINE__,
                   Status
                   );
    return EFI_SUCCESS;
  }

  //
  //For each different TimerPeriod to do following verification
  //
  for (Index = 0; Index < 2; Index++) {
    TimerPeriod = TimerPeriodArray[Index];
    Status = Timer->SetTimerPeriod (Timer, TimerPeriod);
    if (EFI_ERROR(Status)) {
      StandardLib->RecordAssertion (
                     StandardLib,
                     EFI_TEST_ASSERTION_FAILED,
                     gTestGenericFailureGuid,
                     L"EFI_TIMER_ARCH_PROTOCOL.SetTimerPeriod - SetTimerPeriod with valid TimerPeriod",
                     L"%a:%d:TimerPeriod:0x%lx",
                     __FILE__,
                     __LINE__,
                     TimerPeriod
                     );
      continue;
    }

    //
    //Call GetTimerPeriod
    //
    Status = Timer->GetTimerPeriod (Timer, &TimerPeriodVerify);
    if (Status == EFI_SUCCESS) {
      AssertionType = EFI_TEST_ASSERTION_PASSED;
    } else {
      AssertionType = EFI_TEST_ASSERTION_FAILED;
    }
    StandardLib->RecordAssertion (
                   StandardLib,
                   AssertionType,
                   gArchTimerInterfaceTestAssertionGuid012,
                   L"EFI_TIMER_ARCH_PROTOCOL.GetTimerPeriod - GetTimerPeriod with valid parameters,Status verification",
                   L"%a:%d:TimerPeriod:0x%lx, Status = %r, Expected = %r",
                   __FILE__,
                   __LINE__,
                   TimerPeriod,
                   Status,
                   EFI_SUCCESS
                   );

    if (TimerPeriod == TimerPeriodVerify) {
      AssertionType = EFI_TEST_ASSERTION_PASSED;
    } else {
      AssertionType = EFI_TEST_ASSERTION_FAILED;
    }
    StandardLib->RecordAssertion (
                   StandardLib,
                   AssertionType,
                   gArchTimerInterfaceTestAssertionGuid013,
                   L"EFI_TIMER_ARCH_PROTOCOL.GetTimerPeriod - GetTimerPeriod with valid parameters, TimerPeriod verification",
                   L"%a:%d:TimerPeriod:0x%lx,TimerPeriodVerify:0x%lx",
                   __FILE__,
                   __LINE__,
                   TimerPeriod,
                   TimerPeriodVerify
                   );
  }

  //
  // Restore oldTimerPeriod
  //
  Timer->SetTimerPeriod (Timer, OldTimerPeriod);

  return EFI_SUCCESS ;
}


/**
 *  Entrypoint for EFI_TIMER_ARCH_PROTOCOL.GenerateSoftInterrupt() Interface Test.
 *  @param This a pointer of EFI_BB_TEST_PROTOCOL.
 *  @param ClientInterface a pointer to the interface to be tested.
 *  @param TestLevel test "thoroughness" control.
 *  @param SupportHandle a handle containing protocols required.
 *  @return EFI_SUCCESS Finish the test successfully.
 */
//
// TDS 4.4
//
EFI_STATUS
BBTestGenerateSoftInterruptInterfaceTest (
    IN EFI_BB_TEST_PROTOCOL       *This,
    IN VOID                       *ClientInterface,
    IN EFI_TEST_LEVEL             TestLevel,
    IN EFI_HANDLE                 SupportHandle
  )
{
  EFI_STANDARD_TEST_LIBRARY_PROTOCOL   *StandardLib;
  EFI_STATUS                           RegStatus;
  EFI_STATUS                           Status;
  EFI_TIMER_ARCH_PROTOCOL              *Timer;
  UINT64                               TimerPeriod;
  UINT64                               OriginalTimerPeriod;
  EFI_TEST_ASSERTION                   AssertionType;

  //
  // Get the Standard Library Interface
  //
  Status = gtBS->HandleProtocol (
                   SupportHandle,
                   &gEfiStandardTestLibraryGuid,
                   (VOID **)&StandardLib
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
  //Target Assertion Point 4.4.2.1
  //

  //
  //Call GetTimerPeriod
  //
  Status = Timer->GetTimerPeriod (Timer, &OriginalTimerPeriod);
  if (EFI_ERROR(Status)) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_FAILED,
                   gTestGenericFailureGuid,
                   L"EFI_TIMER_ARCH_PROTOCOL.GetTimerPeriod - Get original timer period",
                   L"%a:%d:Status - %r",
                   __FILE__,
                   __LINE__,
                   Status
                   );
    return Status;
  }

  //
  //Register a handler with no previous handler installed
  //
  RegStatus = Timer->RegisterHandler (Timer, TimerTestHandlerNotify);
  if (EFI_ERROR(RegStatus)){
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_FAILED,
                   gTestGenericFailureGuid,
                   L"EFI_TIMER_ARCH_PROTOCOL.RegisterHandler - Register a handler",
                   L"%a:%d:Status - %r",
                   __FILE__,
                   __LINE__,
                   RegStatus
                   );
  }

  //
  //1: GenerateSoftInterrupt should notify timer handler
  //Enable Timer with TimerPeriod = 1000000, to enable interrupt
  //
  TimerPeriod = 1000000;
  Status = Timer->SetTimerPeriod (Timer, TimerPeriod);
  if (Status != EFI_SUCCESS) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_FAILED,
                   gTestGenericFailureGuid,
                   L"EFI_TIMER_ARCH_PROTOCOL.SetTimerPeriod - Enable Timer",
                   L"%a:%d:TimerPeriod:0x%lx,Status - %r",
                   __FILE__,
                   __LINE__,
                   TimerPeriod,
                   Status
                   );
    return Status;
  }

  EnterThisFunc = FALSE;
  Status = Timer->GenerateSoftInterrupt (Timer);
  if (Status == EFI_UNSUPPORTED) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_PASSED,
                   gTestGenericFailureGuid,
                   L"EFI_TIMER_ARCH_PROTOCOL.GenerateSoftInterrupt - Generate soft interrupt to notify handler when timer enabled",
                   L"%a:%d:Status = %r",
                   __FILE__,
                   __LINE__,
                   Status
                   );
    return EFI_SUCCESS;
  }
  if (Status == EFI_SUCCESS) {
    AssertionType = EFI_TEST_ASSERTION_PASSED;
  } else {
    AssertionType = EFI_TEST_ASSERTION_FAILED;
  }
  StandardLib->RecordAssertion (
                 StandardLib,
                 AssertionType,
                 gArchTimerInterfaceTestAssertionGuid014,
                 L"EFI_TIMER_ARCH_PROTOCOL.GenerateSoftInterrupt - Generate soft interrupt to notify handler when timer enabled",
                 L"%a:%d:Status = %r",
                 __FILE__,
                 __LINE__,
                 Status
                 );

  if (!EFI_ERROR(RegStatus)) {
    if (EnterThisFunc == TRUE) {
      AssertionType = EFI_TEST_ASSERTION_PASSED;
    } else {
      AssertionType = EFI_TEST_ASSERTION_FAILED;
    }
    StandardLib->RecordAssertion (
                   StandardLib,
                   AssertionType,
                   gArchTimerInterfaceTestAssertionGuid015,
                   L"EFI_TIMER_ARCH_PROTOCOL.GenerateSoftInterrupt - Generate soft interrupt to notify handler when timer enabled",
                   L"%a:%d:EnterThisFunc = %d",
                   __FILE__,
                   __LINE__,
                   EnterThisFunc
                   );
  }

  //
  //4.4.2.2: GenerateSoftInterrupt should not notify timer handler
  //Set Timer with TimerPeriod = 0, to disable interrupt
  //
  TimerPeriod = 0;
  Status = Timer->SetTimerPeriod (Timer, TimerPeriod);
  if (Status != EFI_SUCCESS) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_FAILED,
                   gTestGenericFailureGuid,
                   L"EFI_TIMER_ARCH_PROTOCOL.SetTimerPeriod - disable Timer",
                   L"%a:%d:TimerPeriod:0x%lx,Status - %r",
                   __FILE__,
                   __LINE__,
                   TimerPeriod,
                   Status
                   );
    return Status;
  }

  EnterThisFunc = FALSE;
  Status = Timer->GenerateSoftInterrupt (Timer);
  if (Status == EFI_UNSUPPORTED) {
    StandardLib->RecordAssertion (
                   StandardLib,
                   EFI_TEST_ASSERTION_PASSED,
                   gTestGenericFailureGuid,
                   L"EFI_TIMER_ARCH_PROTOCOL.GenerateSoftInterrupt - Generate soft interrupt but not notify handler when timer disabled",
                   L"%a:%d:Status = %r",
                   __FILE__,
                   __LINE__,
                   Status
                   );
    return EFI_SUCCESS;
  }
  if (Status == EFI_SUCCESS) {
    AssertionType = EFI_TEST_ASSERTION_PASSED;
  } else {
    AssertionType = EFI_TEST_ASSERTION_FAILED;
  }
  StandardLib->RecordAssertion (
                 StandardLib,
                 AssertionType,
                 gArchTimerInterfaceTestAssertionGuid016,
                 L"EFI_TIMER_ARCH_PROTOCOL.GenerateSoftInterrupt - Generate soft interrupt but not notify handler when timer disabled",
                 L"%a:%d:Status = %r",
                 __FILE__,
                 __LINE__,
                 Status
                 );

  if (!EFI_ERROR(RegStatus)) {
    if (EnterThisFunc == FALSE) {
      AssertionType = EFI_TEST_ASSERTION_PASSED;
    } else {
      AssertionType = EFI_TEST_ASSERTION_FAILED;
    }
    StandardLib->RecordAssertion (
                   StandardLib,
                   AssertionType,
                   gArchTimerInterfaceTestAssertionGuid017,
                   L"EFI_TIMER_ARCH_PROTOCOL.GenerateSoftInterrupt - Generate soft interrupt but not notify handler when timer disabled",
                   L"%a:%d:EnterThisFunc = %d",
                   __FILE__,
                   __LINE__,
                   EnterThisFunc
                   );
  }

  //
  // Restore timer
  //
  Timer->SetTimerPeriod (Timer, OriginalTimerPeriod);

  return EFI_SUCCESS ;
}
