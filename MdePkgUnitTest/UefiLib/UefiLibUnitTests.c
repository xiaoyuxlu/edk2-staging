/** @file
  Uefi Shell based Application that Unit Tests the UefiLib

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <UnitTestTypes.h>
#include <Library/UnitTestLib.h>
#include <Library/UnitTestAssertLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/LoadedImage.h>


#define UNIT_TEST_APP_NAME        L"Uefi Lib Unit Test Application"
#define UNIT_TEST_APP_VERSION     L"0.1"

EFI_GUID  mUnitTestProtocolGuid = { 0xe5f282af, 0x895c, 0x4ece, { 0xae, 0xf0, 0x19, 0xbf, 0x6f, 0xdd, 0x41, 0x2 } };

//
// Conversion function tests:
//
UNIT_TEST_STATUS
EFIAPI
TestEfiLocateProtocolBuffer (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS  Status;
  UINTN       NoProtocols;
  VOID        **Buffer;
  EFI_HANDLE  Handle1;
  EFI_HANDLE  Handle2;
  EFI_HANDLE  Handle3;
  UINT32      Instance2;
  UINT32      Instance3;

  //
  // NULL Protocol should result in EFI_INVALID_PARAMETER
  //
  Status = EfiLocateProtocolBuffer (NULL, &NoProtocols, &Buffer);
  UT_ASSERT_EQUAL (EFI_INVALID_PARAMETER, Status);

  //
  // NULL NoProtocols should result in EFI_INVALID_PARAMETER
  //
  Status = EfiLocateProtocolBuffer (&gEfiCallerIdGuid, NULL, &Buffer);
  UT_ASSERT_EQUAL (EFI_INVALID_PARAMETER, Status);

  //
  // NULL Buffer should result in EFI_INVALID_PARAMETER
  //
  Status = EfiLocateProtocolBuffer (&gEfiCallerIdGuid, &NoProtocols, NULL);
  UT_ASSERT_EQUAL (EFI_INVALID_PARAMETER, Status);

  //
  // All NULL should result in EFI_INVALID_PARAMETER
  //
  Status = EfiLocateProtocolBuffer (NULL, NULL, NULL);
  UT_ASSERT_EQUAL (EFI_INVALID_PARAMETER, Status);

  //
  // Request for unknown protocol should result in EFI_NOT_FOUND
  //
  NoProtocols = 0;
  Buffer = NULL;
  Status = EfiLocateProtocolBuffer (&mUnitTestProtocolGuid, &NoProtocols, &Buffer);
  UT_ASSERT_EQUAL (EFI_NOT_FOUND, Status);

  //
  // Request for Loaded Image Protocol should result in EFI_SUCCESS
  //
  NoProtocols = 0;
  Buffer = NULL;
  Status = EfiLocateProtocolBuffer (&gEfiLoadedImageProtocolGuid, &NoProtocols, &Buffer);
  UT_ASSERT_EQUAL (EFI_SUCCESS, Status);
  UT_ASSERT_TRUE (NoProtocols > 0);
  UT_ASSERT_NOT_NULL (Buffer);

  if (Buffer != NULL) {
    FreePool (Buffer);
  }

  //
  // Install 1 instance of a protocol
  //
  Handle1 = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle1,
                  &mUnitTestProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );

  //
  // Verify 1 instance found
  //
  NoProtocols = 0;
  Buffer = NULL;
  Status = EfiLocateProtocolBuffer (&mUnitTestProtocolGuid, &NoProtocols, &Buffer);
  UT_ASSERT_EQUAL (EFI_SUCCESS, Status);
  UT_ASSERT_EQUAL (NoProtocols, 1);
  UT_ASSERT_EQUAL ((UINTN)Buffer[0], (UINTN)NULL);

  if (Buffer != NULL) {
    FreePool (Buffer);
  }

  //
  // Install a 2nd instance of a protocol
  //
  Handle2 = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle2,
                  &mUnitTestProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &Instance2
                  );

  //
  // Verify 2 instances found
  //
  NoProtocols = 0;
  Buffer = NULL;
  Status = EfiLocateProtocolBuffer (&mUnitTestProtocolGuid, &NoProtocols, &Buffer);
  UT_ASSERT_EQUAL (EFI_SUCCESS, Status);
  UT_ASSERT_EQUAL (NoProtocols, 2);
  UT_ASSERT_EQUAL ((UINTN)Buffer[0], (UINTN)NULL);
  UT_ASSERT_EQUAL ((UINTN)Buffer[1], (UINTN)&Instance2);

  if (Buffer != NULL) {
    FreePool (Buffer);
  }

  //
  // Install a 3rd instance of a protocol
  //
  Handle3 = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle3,
                  &mUnitTestProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &Instance3
                  );

  //
  // Verify 3 instances found
  //
  NoProtocols = 0;
  Buffer = NULL;
  Status = EfiLocateProtocolBuffer (&mUnitTestProtocolGuid, &NoProtocols, &Buffer);
  UT_ASSERT_EQUAL (EFI_SUCCESS, Status);
  UT_ASSERT_EQUAL (NoProtocols, 3);
  UT_ASSERT_EQUAL ((UINTN)Buffer[0], (UINTN)NULL);
  UT_ASSERT_EQUAL ((UINTN)Buffer[1], (UINTN)&Instance2);
  UT_ASSERT_EQUAL ((UINTN)Buffer[2], (UINTN)&Instance3);

  if (Buffer != NULL) {
    FreePool (Buffer);
  }

  //
  // Uninstall test protocols
  //
  Status = gBS->UninstallProtocolInterface (
                  Handle3,
                  &mUnitTestProtocolGuid,
                  &Instance3
                  );

  Status = gBS->UninstallProtocolInterface (
                  Handle2,
                  &mUnitTestProtocolGuid,
                  &Instance2
                  );

  Status = gBS->UninstallProtocolInterface (
                  Handle1,
                  &mUnitTestProtocolGuid,
                  NULL
                  );

  return UNIT_TEST_PASSED;
}

/**

  Main fuction sets up the unit test environment

**/
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  UNIT_TEST_FRAMEWORK  *Fw;
  UNIT_TEST_SUITE      *ProtocolTestSuite;
  CHAR16               ShortName[100];


  DEBUG ((DEBUG_INFO, "%s v%s\n", UNIT_TEST_APP_NAME, UNIT_TEST_APP_VERSION));

  ShortName[0] = L'\0';
  UnicodeSPrint (&ShortName[0], sizeof (ShortName), L"%a", gEfiCallerBaseName);

  //
  // Start setting up the test framework for running the tests.
  //
  Fw = NULL;
  Status = InitUnitTestFramework (&Fw, UNIT_TEST_APP_NAME, ShortName, UNIT_TEST_APP_VERSION);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed in InitUnitTestFramework. Status = %r\n", Status));
    goto EXIT;
  }

  //
  // Test the protocol functions
  //
  Status = CreateUnitTestSuite (
             &ProtocolTestSuite,
             Fw,
             L"Uefi Lib Test Suite",
             L"Common.UefiLib.Protocol",
             NULL, NULL
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed in CreateUnitTestSuite for Protocol Test Suite\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  //
  // Add test protocol test cases
  //
  AddTestCase (ProtocolTestSuite, L"Test EfiLocateProtocolBuffer",    L"Common.UefiLib.Protocol.EfiLocateProtocolBuffer",    TestEfiLocateProtocolBuffer,    NULL, NULL, NULL);

  //
  // Execute the tests.
  //
  Status = RunAllTestSuites (Fw);

EXIT:
  if (Fw) {
    FreeUnitTestFramework (Fw);
  }

  return Status;
}
