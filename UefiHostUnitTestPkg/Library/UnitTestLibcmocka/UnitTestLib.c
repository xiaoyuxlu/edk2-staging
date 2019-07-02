/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <Uefi.h>
#include <UnitTestTypes.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/OsServiceLib.h>

#define MAX_STRING_SIZE  1025

int GroupSetupTemplate (void **state);
int GroupTeardownTemplate (void **state);
void TestFuncTemplate (void **state);
int SetupFuncTemplate (void **state);
int TeardownFuncTemplate (void **state);

extern UINTN mGroupSetupTemplateSize;
extern UINTN mGroupTeardownTemplateSize;
extern UINTN mTestFuncTemplateSize;
extern UINTN mSetupFuncTemplateSize;
extern UINTN mTeardownFuncTemplateSize;

// NOTE: Changing structure below requires NASM file update
typedef struct {
  CHAR8                       *TestName;
  CMUnitTestFunction          TestFunc;
  CMFixtureFunction           SetupFunc;
  CMFixtureFunction           TeardownFunc;
  CHAR16                      *Description;
  CHAR16                      *ClassName;  //can't have spaces and should be short
  UNIT_TEST_FUNCTION          RunTest;
  UNIT_TEST_PREREQ            PreReq;
  UNIT_TEST_CLEANUP           CleanUp;
  UNIT_TEST_CONTEXT           Context;
  UNIT_TEST_SUITE_HANDLE      ParentSuite;
  UNIT_TEST_FRAMEWORK_HANDLE  ParentFramework;
} MY_UNIT_TEST;

typedef struct {
  LIST_ENTRY       Entry;
  MY_UNIT_TEST     UT;
} MY_UNIT_TEST_LIST_ENTRY;

// NOTE: Changing structure below requires NASM file update
typedef struct {
  UINTN                       NumTests;
  CMFixtureFunction           GroupSetup;
  CMFixtureFunction           GroupTeardown;
  CHAR16                      *Title;
  CHAR16                      *Package;
  UNIT_TEST_SUITE_SETUP       Setup;
  UNIT_TEST_SUITE_TEARDOWN    Teardown;
  LIST_ENTRY                  TestCaseList;     // MY_UNIT_TEST_LIST_ENTRY
  UNIT_TEST_FRAMEWORK_HANDLE  ParentFramework;
} MY_UNIT_TEST_SUITE;

typedef struct {
  LIST_ENTRY           Entry;
  MY_UNIT_TEST_SUITE   UTS;
} MY_UNIT_TEST_SUITE_LIST_ENTRY;

typedef struct {
  CHAR16                    *Title;
  CHAR16                    *ShortTitle;      // This title should contain NO spaces or non-filename charatecters. Is used in reporting and serialization.
  CHAR16                    *VersionString;
  LIST_ENTRY                TestSuiteList;    // MY_UNIT_TEST_SUITE_LIST_ENTRY
} MY_UNIT_TEST_FRAMEWORK;

//=============================================================================
//
// ----------------  TEST HELPER FUNCTIONS ------------------------------------
//
//=============================================================================

/**
  This function will determine whether the short name violates any rules that would
  prevent it from being used as a reporting name or as a serialization name.

  Example: If the name cannot be serialized to a filesystem file name.

  @param[in]  ShortTitleString  A pointer to the short title string to be evaluated.

  @retval     TRUE    The string is acceptable.
  @retval     FALSE   The string should not be used.

**/
STATIC
BOOLEAN
IsFrameworkShortNameValid (
  IN  CHAR16    *ShortTitleString
  )
{
  // TODO: Finish this function.
  return TRUE;
} // IsFrameworkShortNameValid()

STATIC
CHAR16*
AllocateAndCopyString (
  IN  CHAR16    *StringToCopy
  )
{
  CHAR16    *NewString = NULL;
  UINTN     NewStringLength;

  NewStringLength = StrnLenS( StringToCopy, UNIT_TEST_MAX_STRING_LENGTH ) + 1;
  NewString = AllocatePool( NewStringLength * sizeof( CHAR16 ) );
  if (NewString != NULL)
  {
    StrCpyS( NewString, NewStringLength, StringToCopy );
  }

  return NewString;
} // AllocateAndCopyString ()


EFI_STATUS
EFIAPI
FreeUnitTestFramework (
  IN UNIT_TEST_FRAMEWORK  *Framework
  )
{
  // TODO: Finish this function.
  return EFI_SUCCESS;
} // FreeUnitTestFramework()


STATIC
EFI_STATUS
FreeUnitTestSuiteEntry (
  IN UNIT_TEST_SUITE_LIST_ENTRY *SuiteEntry
  )
{
  // TODO: Finish this function.
  return EFI_SUCCESS;
} // FreeUnitTestSuiteEntry()


STATIC
EFI_STATUS
FreeUnitTestTestEntry (
  IN UNIT_TEST_LIST_ENTRY *TestEntry
  )
{
  // TODO: Finish this function.
  return EFI_SUCCESS;
} // FreeUnitTestTestEntry()



//=============================================================================
//
// ----------------  TEST SETUP FUNCTIONS -------------------------------------
//
//=============================================================================


/*
Method to Initialize the Unit Test framework

@retval Success - Unit Test init.
@retval EFI_ERROR - Unit Tests init failed.
*/
EFI_STATUS
EFIAPI
InitUnitTestFramework (
  OUT UNIT_TEST_FRAMEWORK   **Framework,
  IN  CHAR16                *Title,
  IN  CHAR16                *ShortTitle,
  IN  CHAR16                *VersionString
  )
{
  EFI_STATUS                Status;
  MY_UNIT_TEST_FRAMEWORK    *NewFramework;

  Status = EFI_SUCCESS;
  NewFramework = NULL;
  
  //
  // First, check all pointers and make sure nothing's broked.
  if (Framework == NULL || Title == NULL ||
      ShortTitle == NULL || VersionString == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Next, determine whether all of the strings are good to use.
  if (!IsFrameworkShortNameValid( ShortTitle ))
  {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Next, set aside some space to start messing with the framework.
  NewFramework = AllocateZeroPool( sizeof( MY_UNIT_TEST_FRAMEWORK ) );
  if (NewFramework == NULL)
  {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Next, set up all the test data.
  NewFramework->Title         = AllocateAndCopyString( Title );
  NewFramework->ShortTitle    = AllocateAndCopyString( ShortTitle );
  NewFramework->VersionString = AllocateAndCopyString( VersionString );
  if (NewFramework->Title == NULL || NewFramework->ShortTitle == NULL ||
      NewFramework->VersionString == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }
  InitializeListHead( &(NewFramework->TestSuiteList) );

Exit:
  //
  // If we're good, then let's copy the framework.
  if (!EFI_ERROR( Status ))
  {
    *Framework = (UNIT_TEST_FRAMEWORK *)NewFramework;
  }
  // Otherwise, we need to undo this horrible thing that we've done.
  else
  {
    FreeUnitTestFramework ((UNIT_TEST_FRAMEWORK *)NewFramework);
  }

  return Status;
}

EFI_STATUS
EFIAPI
CreateUnitTestSuite (
  OUT UNIT_TEST_SUITE           **Suite,
  IN UNIT_TEST_FRAMEWORK        *Framework,
  IN CHAR16                     *Title,
  IN CHAR16                     *Package,
  IN UNIT_TEST_SUITE_SETUP      Sup    OPTIONAL,
  IN UNIT_TEST_SUITE_TEARDOWN   Tdn    OPTIONAL
  )
{
  EFI_STATUS                       Status;
  MY_UNIT_TEST_SUITE_LIST_ENTRY    *NewSuiteEntry;
  MY_UNIT_TEST_FRAMEWORK           *MyFramework;

  Status = EFI_SUCCESS;
  MyFramework = (MY_UNIT_TEST_FRAMEWORK *)Framework;

  //
  // First, let's check to make sure that our parameters look good.
  if ((MyFramework == NULL) || (Title == NULL) || (Package == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Create the new entry.
  NewSuiteEntry = AllocateZeroPool( sizeof( MY_UNIT_TEST_SUITE_LIST_ENTRY ) );
  if (NewSuiteEntry == NULL)
  {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Copy the fields we think we need.
  NewSuiteEntry->UTS.NumTests         = 0;
  NewSuiteEntry->UTS.Title            = AllocateAndCopyString( Title );
  NewSuiteEntry->UTS.Package          = AllocateAndCopyString(Package);
  NewSuiteEntry->UTS.Setup            = Sup;
  NewSuiteEntry->UTS.Teardown         = Tdn;
  NewSuiteEntry->UTS.ParentFramework  = MyFramework;
  InitializeListHead( &(NewSuiteEntry->Entry) );             // List entry for sibling suites.
  InitializeListHead( &(NewSuiteEntry->UTS.TestCaseList) );  // List entry for child tests.
  if (NewSuiteEntry->UTS.Title == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  if (NewSuiteEntry->UTS.Package == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  NewSuiteEntry->UTS.GroupSetup = (CMFixtureFunction)(UINTN)AllocateExecutableMemory (mGroupSetupTemplateSize);
  ASSERT(NewSuiteEntry->UTS.GroupSetup != NULL);
  CopyMem ((VOID *)(UINTN)NewSuiteEntry->UTS.GroupSetup, (VOID *)(UINTN)GroupSetupTemplate, mGroupSetupTemplateSize);
  *(UINTN *)((UINTN)NewSuiteEntry->UTS.GroupSetup + sizeof(UINTN)/sizeof(UINT32)) = (UINTN)&NewSuiteEntry->UTS;
  
  NewSuiteEntry->UTS.GroupTeardown = (CMFixtureFunction)(UINTN)AllocateExecutableMemory (mGroupTeardownTemplateSize);
  ASSERT(NewSuiteEntry->UTS.GroupTeardown != NULL);
  CopyMem ((VOID *)(UINTN)NewSuiteEntry->UTS.GroupTeardown, (VOID *)(UINTN)GroupTeardownTemplate, mGroupTeardownTemplateSize);
  *(UINTN *)((UINTN)NewSuiteEntry->UTS.GroupTeardown + sizeof(UINTN)/sizeof(UINT32)) = (UINTN)&NewSuiteEntry->UTS;

Exit:
  //
  // If everything is going well, add the new suite to the tail list for the framework.
  if (!EFI_ERROR( Status ))
  {
    InsertTailList( &(MyFramework->TestSuiteList), (LIST_ENTRY*)NewSuiteEntry );
    *Suite = (UNIT_TEST_SUITE *)&NewSuiteEntry->UTS;
  }
  // Otherwise, make with the destruction.
  else
  {
    FreeUnitTestSuiteEntry( (UNIT_TEST_SUITE_LIST_ENTRY *)NewSuiteEntry );
  }
  
  return Status;
}


EFI_STATUS
EFIAPI
AddTestCase (
  IN UNIT_TEST_SUITE      *Suite,
  IN CHAR16               *Description,
  IN CHAR16               *ClassName,
  IN UNIT_TEST_FUNCTION   Func,
  IN UNIT_TEST_PREREQ     PreReq    OPTIONAL,
  IN UNIT_TEST_CLEANUP    CleanUp   OPTIONAL,
  IN UNIT_TEST_CONTEXT    Context   OPTIONAL
  )
{
  EFI_STATUS               Status;
  MY_UNIT_TEST_SUITE       *MySuite;
  MY_UNIT_TEST_LIST_ENTRY  *NewTestEntry;
  MY_UNIT_TEST_FRAMEWORK   *ParentFramework;
  UINTN                    TestNameSize;

  Status = EFI_SUCCESS;
  MySuite = (MY_UNIT_TEST_SUITE *)Suite;
  ParentFramework = (MY_UNIT_TEST_FRAMEWORK*)MySuite->ParentFramework;

  //
  // First, let's check to make sure that our parameters look good.
  if ((MySuite == NULL) || (Description == NULL) || (ClassName == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Create the new entry.
  NewTestEntry = AllocateZeroPool( sizeof( MY_UNIT_TEST_LIST_ENTRY ) );
  if (NewTestEntry == NULL)
  {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Copy the fields we think we need.
  NewTestEntry->UT.Description  = AllocateAndCopyString( Description );
  NewTestEntry->UT.ClassName    = AllocateAndCopyString(ClassName);
  NewTestEntry->UT.PreReq       = PreReq;
  NewTestEntry->UT.CleanUp      = CleanUp;
  NewTestEntry->UT.RunTest      = Func;
  NewTestEntry->UT.Context      = Context;
  NewTestEntry->UT.ParentSuite  = MySuite;
  NewTestEntry->UT.ParentFramework  = MySuite->ParentFramework;
  InitializeListHead( &(NewTestEntry->Entry) );      // List entry for sibling tests.
  if (NewTestEntry->UT.Description == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }
  
  NewTestEntry->UT.TestFunc = (CMUnitTestFunction)(UINTN)AllocateExecutableMemory (mTestFuncTemplateSize);
  ASSERT(NewTestEntry->UT.TestFunc != NULL);
  CopyMem ((VOID *)(UINTN)NewTestEntry->UT.TestFunc, (VOID *)(UINTN)TestFuncTemplate, mTestFuncTemplateSize);
  *(UINTN *)((UINTN)NewTestEntry->UT.TestFunc + sizeof(UINTN)/sizeof(UINT32)) = (UINTN)&NewTestEntry->UT;
  
  NewTestEntry->UT.SetupFunc = (CMFixtureFunction)(UINTN)AllocateExecutableMemory (mSetupFuncTemplateSize);
  ASSERT(NewTestEntry->UT.SetupFunc != NULL);
  CopyMem ((VOID *)(UINTN)NewTestEntry->UT.SetupFunc, (VOID *)(UINTN)SetupFuncTemplate, mSetupFuncTemplateSize);
  *(UINTN *)((UINTN)NewTestEntry->UT.SetupFunc + sizeof(UINTN)/sizeof(UINT32)) = (UINTN)&NewTestEntry->UT;
  
  NewTestEntry->UT.TeardownFunc = (CMFixtureFunction)(UINTN)AllocateExecutableMemory (mTeardownFuncTemplateSize);
  ASSERT(NewTestEntry->UT.TeardownFunc != NULL);
  CopyMem ((VOID *)(UINTN)NewTestEntry->UT.TeardownFunc, (VOID *)(UINTN)TeardownFuncTemplate, mTeardownFuncTemplateSize);
  *(UINTN *)((UINTN)NewTestEntry->UT.TeardownFunc + sizeof(UINTN)/sizeof(UINT32)) = (UINTN)&NewTestEntry->UT;

  TestNameSize = StrLen (Description) + 1;
  NewTestEntry->UT.TestName  = AllocatePool (TestNameSize);
  ASSERT (NewTestEntry->UT.TestName != NULL);

  Status = UnicodeStrToAsciiStrS (Description, NewTestEntry->UT.TestName, TestNameSize);
  ASSERT_EFI_ERROR(Status);


Exit:
  //
  // If everything is going well, add the new suite to the tail list for the framework.
  if (!EFI_ERROR( Status ))
  {
    InsertTailList( &(MySuite->TestCaseList), (LIST_ENTRY*)NewTestEntry );
    MySuite->NumTests ++;
  }
  // Otherwise, make with the destruction.
  else
  {
    FreeUnitTestTestEntry( (UNIT_TEST_LIST_ENTRY *)NewTestEntry );
  }

  return Status;
}


//=============================================================================
//
// ----------------  TEST EXECUTION FUNCTIONS ---------------------------------
//
//=============================================================================

EFI_STATUS
EFIAPI
RunAllTestSuites(
  IN UNIT_TEST_FRAMEWORK  *Framework
  )
{
  EFI_STATUS                    Status;
  MY_UNIT_TEST_FRAMEWORK        *MyFramework;
  LIST_ENTRY                    *TestSuiteList;
  LIST_ENTRY                    *TestSuiteLink;
  MY_UNIT_TEST_SUITE_LIST_ENTRY *TestSuiteListEntry;
  MY_UNIT_TEST_SUITE            *TestSuite;
  CHAR8                         GroupName[MAX_STRING_SIZE];
  struct CMUnitTest             *Tests;
  LIST_ENTRY                    *UnitTestList;
  LIST_ENTRY                    *UnitTestLink;
  MY_UNIT_TEST_LIST_ENTRY       *UnitTestListEntry;
  MY_UNIT_TEST                  *UnitTest;
  UINTN                         Index;

  if (PcdGet8 (HostUnitTestMode) == 1) {
    cmocka_set_message_output (CM_OUTPUT_XML);
  }

  MyFramework = (MY_UNIT_TEST_FRAMEWORK *)Framework;

  TestSuiteList = &MyFramework->TestSuiteList;
  for (TestSuiteLink = TestSuiteList->ForwardLink;
       TestSuiteLink != TestSuiteList;
       TestSuiteLink = TestSuiteLink->ForwardLink) {
    TestSuiteListEntry = BASE_CR (
                           TestSuiteLink,
                           MY_UNIT_TEST_SUITE_LIST_ENTRY,
                           Entry
                           );
    TestSuite = &TestSuiteListEntry->UTS;
    Status = UnicodeStrToAsciiStrS (TestSuite->Title, GroupName, sizeof(GroupName));
    ASSERT_EFI_ERROR(Status);

    Tests = AllocateZeroPool (TestSuite->NumTests * sizeof(struct CMUnitTest));
    ASSERT (Tests != NULL);
    Index = 0;

    UnitTestList = &TestSuite->TestCaseList;
    for (UnitTestLink = UnitTestList->ForwardLink;
         UnitTestLink != UnitTestList;
         UnitTestLink = UnitTestLink->ForwardLink) {
      UnitTestListEntry = BASE_CR (
                             UnitTestLink,
                             MY_UNIT_TEST_LIST_ENTRY,
                             Entry
                             );
      UnitTest = &UnitTestListEntry->UT;

      Tests[Index].name = UnitTest->TestName;
      Tests[Index].test_func = UnitTest->TestFunc;
      Tests[Index].setup_func = UnitTest->SetupFunc;
      Tests[Index].teardown_func = UnitTest->TeardownFunc;
      Tests[Index].initial_state = NULL;
      Index++;
    }
    ASSERT (Index == TestSuite->NumTests);

    _cmocka_run_group_tests (GroupName, Tests, TestSuite->NumTests, TestSuite->GroupSetup, TestSuite->GroupTeardown);
    FreePool (Tests);
  }

  return EFI_SUCCESS;
}

//=============================================================================
//
// ----------------  TEST UTILITY FUNCTIONS -----------------------------------
//
//=============================================================================

EFI_STATUS
EFIAPI
SaveFrameworkState (
  IN UNIT_TEST_FRAMEWORK_HANDLE FrameworkHandle,
  IN UNIT_TEST_CONTEXT          ContextToSave     OPTIONAL,
  IN UINTN                      ContextToSaveSize
  )
{
  return EFI_UNSUPPORTED;
} // SaveFrameworkState()


EFI_STATUS
EFIAPI
SaveFrameworkStateAndQuit (
  IN UNIT_TEST_FRAMEWORK_HANDLE FrameworkHandle,
  IN UNIT_TEST_CONTEXT          ContextToSave     OPTIONAL,
  IN UINTN                      ContextToSaveSize
  )
{
  return EFI_UNSUPPORTED;
} // SaveFrameworkStateAndQuit()


/**
  NOTE: Takes in a ResetType, but currently only supports EfiResetCold
        and EfiResetWarm. All other types will return EFI_INVALID_PARAMETER.
        If a more specific reset is required, use SaveFrameworkState() and
        call gRT->ResetSystem() directly.

**/
EFI_STATUS
EFIAPI
SaveFrameworkStateAndReboot (
  IN UNIT_TEST_FRAMEWORK_HANDLE FrameworkHandle,
  IN UNIT_TEST_CONTEXT          ContextToSave     OPTIONAL,
  IN UINTN                      ContextToSaveSize,
  IN EFI_RESET_TYPE             ResetType
  )
{
  return EFI_UNSUPPORTED;
} // SaveFrameworkStateAndReboot()
