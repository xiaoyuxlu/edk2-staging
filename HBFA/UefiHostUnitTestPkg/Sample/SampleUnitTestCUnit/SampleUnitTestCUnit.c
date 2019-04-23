/** @file -- SampleUnitTestHost.c

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.


Copyright (C) 2016 Microsoft Corporation. All Rights Reserved.

**/

#include <stdio.h>
#include <string.h>
#include <Basic.h>
#include <Automated.h>

#include <Uefi.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>

BOOLEAN       mSampleGlobalTestBoolean = FALSE;
VOID          *mSampleGlobalTestPointer = NULL;


///================================================================================================
///================================================================================================
///
/// HELPER FUNCTIONS
///
///================================================================================================
///================================================================================================


/**
  The suite initialization function.

  @retval zero on success
  @retval non-zero otherwise.
**/
int InitSuiteSampleUnitTest (void)
{
  return 0;
}

/**
  The suite cleanup function.

  @retval zero on success
  @retval non-zero otherwise.
**/
int CleanSuiteSampleUnitTest (void)
{
  return 0;
}

//
// Anything you think might be helpful that isn't a test itself.
//

/**
  Setup function
**/
int MakeSureThatPointerIsNull (void)
{
  CU_ASSERT_EQUAL(mSampleGlobalTestPointer, NULL);
  mSampleGlobalTestPointer = NULL;
  return 0;
} // ListsShouldHaveTheSameDescriptorSize()

/**
  Teardown function
**/
int ClearThePointer (void)
{
  mSampleGlobalTestPointer = NULL;
  return 0;
} // ClearThePointer()


///================================================================================================
///================================================================================================
///
/// TEST CASES
///
///================================================================================================
///================================================================================================


void OnePlusOneShouldEqualTwo (void)
{
  UINTN     A, B, C;

  A = 1;
  B = 1;
  C = A + B;

  CU_ASSERT_EQUAL(C, 2);
} // OnePlusOneShouldEqualTwo()


void GlobalBooleanShouldBeChangeable (void)
{
  mSampleGlobalTestBoolean = TRUE;
  CU_ASSERT_TRUE(mSampleGlobalTestBoolean);
  
  mSampleGlobalTestBoolean = FALSE;
  CU_ASSERT_FALSE(mSampleGlobalTestBoolean);
} // GlobalBooleanShouldBeChangeable()


void GlobalPointerShouldBeChangeable (void)
{
  mSampleGlobalTestPointer = (VOID*)-1;
  CU_ASSERT_EQUAL(mSampleGlobalTestPointer, ((VOID*)-1));
} // GlobalPointerShouldBeChangeable()


///================================================================================================
///================================================================================================
///
/// TEST ENGINE
///
///================================================================================================
///================================================================================================


/** 
  SampleUnitTestApp
  
  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point executed successfully.
  @retval other           Some error occured when executing this entry point.

**/
int main ()
{
  CU_pSuite SimpleMathSuite = NULL;
  CU_pSuite GlobalVariableSuite = NULL;
  CU_pSuite GlobalPointerSuite = NULL;

  if (CUE_SUCCESS != CU_initialize_registry()) {
    return CU_get_error();
  }

  SimpleMathSuite = CU_add_suite("Suite_SimpleMatch", InitSuiteSampleUnitTest, CleanSuiteSampleUnitTest);
  if (NULL == SimpleMathSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }
  
  if (NULL == CU_add_test(SimpleMathSuite, "test of Addition", OnePlusOneShouldEqualTwo)) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  GlobalVariableSuite = CU_add_suite("Suite_GlobalVariable", InitSuiteSampleUnitTest, CleanSuiteSampleUnitTest);
  if (NULL == GlobalVariableSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (NULL == CU_add_test(GlobalVariableSuite, "test of GlobalBoolean", GlobalBooleanShouldBeChangeable)) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  //
  // NOTE: In CUnit, the Setup/Teardown is suite-level, not case-level.
  //
  GlobalPointerSuite = CU_add_suite_with_setup_and_teardown("Suite_GlobalPointer", InitSuiteSampleUnitTest, CleanSuiteSampleUnitTest, MakeSureThatPointerIsNull, ClearThePointer);
  if (NULL == GlobalPointerSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }
  
  if (NULL == CU_add_test(GlobalPointerSuite, "test of GlobalPointer", GlobalPointerShouldBeChangeable)) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (PcdGet8 (HostUnitTestMode) == 1) {
    CU_set_output_filename("SampmeUnitTest");
    CU_list_tests_to_file();
    CU_automated_run_tests();
  } else {
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
  }

  CU_cleanup_registry();
  return CU_get_error();
}