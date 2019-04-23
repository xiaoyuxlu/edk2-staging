/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <stdio.h>
#include <string.h>
#include <Basic.h>

#include <Uefi.h>
#include <UnitTestTypes.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#define MAX_STRING_SIZE  1025

BOOLEAN
EFIAPI
UnitTestAssertTrue (
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN BOOLEAN                    Expression,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *FileName,
  IN CONST CHAR8                *Description
  )
{
  CHAR8  TempStr[MAX_STRING_SIZE];

  sprintf (TempStr, "UT_ASSERT_TRUE(%s:%x)", Description, Expression);
  CU_assertImplementation (Expression, (UINT32)LineNumber, TempStr, FileName, FunctionName, FALSE);

  return Expression;
}


BOOLEAN
EFIAPI
UnitTestAssertFalse (
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN BOOLEAN                    Expression,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *FileName,
  IN CONST CHAR8                *Description
  )
{
  CHAR8  TempStr[MAX_STRING_SIZE];

  sprintf (TempStr, "UT_ASSERT_FALSE(%s:%x)", Description, Expression);
  CU_assertImplementation (!Expression, (UINT32)LineNumber, TempStr, FileName, FunctionName, FALSE);

  return !Expression;
}


BOOLEAN
EFIAPI
UnitTestAssertNotEfiError (
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN EFI_STATUS                 Status,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *FileName,
  IN CONST CHAR8                *Description
  )
{
  CHAR8  TempStr[MAX_STRING_SIZE];

  sprintf (TempStr, "UT_ASSERT_NOT_EFI_ERROR(%s:%p)", Description, (void *)Status);
  CU_assertImplementation (!EFI_ERROR (Status), (UINT32)LineNumber, TempStr, FileName, FunctionName, FALSE);

  return !EFI_ERROR (Status);
}


BOOLEAN
EFIAPI
UnitTestAssertEqual (
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN UINT64                     ValueA,
  IN UINT64                     ValueB,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *FileName,
  IN CONST CHAR8                *DescriptionA,
  IN CONST CHAR8                *DescriptionB
  )
{
  CHAR8  TempStr[MAX_STRING_SIZE];

  sprintf (TempStr, "UT_ASSERT_EQUAL(%s:%llx, %s:%llx)", DescriptionA, ValueA, DescriptionB, ValueB);
  CU_assertImplementation (ValueA == ValueB, (UINT32)LineNumber, TempStr, FileName, FunctionName, FALSE);

  return (ValueA == ValueB);
}

BOOLEAN
EFIAPI
UnitTestAssertMemEqual(
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN UINTN                      ValueA,
  IN UINTN                      ValueB,
  IN UINTN                      Length,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *FileName,
  IN CONST CHAR8                *DescriptionA,
  IN CONST CHAR8                *DescriptionB
  )
{
  CHAR8  TempStr[MAX_STRING_SIZE];
  BOOLEAN Result;

  Result = (CompareMem((VOID*)ValueA, (VOID*)ValueB, Length) == 0);

  sprintf (TempStr, "UT_ASSERT_MEM_EQUAL(%s:%p, %s:%p)", DescriptionA, (VOID *)ValueA, DescriptionB, (VOID *)ValueB);
  CU_assertImplementation (Result, (UINT32)LineNumber, TempStr, FileName, FunctionName, FALSE);

  return Result;
}


BOOLEAN
EFIAPI
UnitTestAssertNotEqual (
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN UINT64                     ValueA,
  IN UINT64                     ValueB,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *FileName,
  IN CONST CHAR8                *DescriptionA,
  IN CONST CHAR8                *DescriptionB
  )
{
  CHAR8  TempStr[MAX_STRING_SIZE];

  sprintf (TempStr, "UT_ASSERT_NOT_EQUAL(%s:%llx, %s:%llx)", DescriptionA, ValueA, DescriptionB, ValueB);
  CU_assertImplementation (ValueA != ValueB, (UINT32)LineNumber, TempStr, FileName, FunctionName, FALSE);

  return (ValueA != ValueB);
}


BOOLEAN
EFIAPI
UnitTestAssertStatusEqual (
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN EFI_STATUS                 Status,
  IN EFI_STATUS                 Expected,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *FileName,
  IN CONST CHAR8                *Description
  )
{
  CHAR8  TempStr[MAX_STRING_SIZE];

  sprintf (TempStr, "UT_ASSERT_STATUS_EQUAL(%s:%p)", Description, (VOID *)Status);
  CU_assertImplementation (Status == Expected, (UINT32)LineNumber, TempStr, FileName, FunctionName, FALSE);

  return (Status == Expected);
}

BOOLEAN
EFIAPI
UnitTestAssertNotNull(
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN VOID*                      Pointer,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *FileName,
  IN CONST CHAR8                *PointerName
  )
{
  CHAR8  TempStr[MAX_STRING_SIZE];

  sprintf (TempStr, "UT_ASSERT_NOT_NULL(%s:%p)", PointerName, Pointer);
  CU_assertImplementation (Pointer != NULL, (UINT32)LineNumber, TempStr, FileName, FunctionName, FALSE);

  return (Pointer != NULL);
}
