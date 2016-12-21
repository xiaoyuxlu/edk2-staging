/** @file
  This file provides the services to load, save, and manage all kind of test
  data.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_SCT_DATA_H_
#define _EFI_SCT_DATA_H_

//
// External functions declaration
//

//
// Application test services
//

EFI_STATUS
LoadApTest (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevicePath,
  IN CHAR16                       *FileName,
  OUT EFI_AP_TEST_INTERFACE       **ApTest
  );

EFI_STATUS
FreeApTest (
  IN EFI_AP_TEST_INTERFACE        *ApTest
  );

//
// Configuration data services
//

EFI_STATUS
LoadConfigData (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevicePath,
  IN CHAR16                       *FileName,
  OUT EFI_SCT_CONFIG_DATA         *ConfigData
  );

EFI_STATUS
SaveConfigData (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevicePath,
  IN CHAR16                       *FileName,
  IN EFI_SCT_CONFIG_DATA          *ConfigData
  );

EFI_STATUS
FreeConfigData (
  IN EFI_SCT_CONFIG_DATA          *ConfigData
  );

//
// Category data services
//

EFI_STATUS
LoadCategoryData (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevicePath,
  IN CHAR16                       *FileName,
  OUT LIST_ENTRY              *CategoryList
  );

EFI_STATUS
FreeCategoryData (
  IN LIST_ENTRY               *CategoryList
  );

EFI_STATUS
FindCategoryByGuid (
  IN  EFI_GUID                    *Guid,
  OUT EFI_SCT_CATEGORY_DATA       **Category
);

//
// Test case services
//

EFI_STATUS
LoadTestCases (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevicePath,
  IN CHAR16                       *FileName,
  OUT LIST_ENTRY              *TestCaseList
  );

EFI_STATUS
SaveTestCases (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevicePath,
  IN CHAR16                       *FileName,
  IN LIST_ENTRY               *TestCaseList
  );

EFI_STATUS
LoadTestSequence (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevicePath,
  IN CHAR16                       *FileName,
  IN OUT LIST_ENTRY           *TestCaseList
  );

EFI_STATUS
SaveTestSequence (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevicePath,
  IN CHAR16                       *FileName,
  IN LIST_ENTRY               *TestCaseList
  );

EFI_STATUS
FreeTestCases (
  IN LIST_ENTRY               *TestCaseList
  );

EFI_STATUS
CreateTestCases (
  OUT LIST_ENTRY              *TestCaseList
  );

EFI_STATUS
MergeTestCases (
  IN OUT LIST_ENTRY           *DstTestCaseList,
  IN LIST_ENTRY               *SrcTestCaseList
  );

EFI_STATUS
FindTestCaseByGuid (
  IN EFI_GUID                     *Guid,
  OUT EFI_SCT_TEST_CASE           **TestCase
  );

EFI_STATUS
SaveCaseTree (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevicePath,
  IN CHAR16                       *FileName,
  IN LIST_ENTRY               *TestNodeList
  );

EFI_STATUS
GenerateCaseTreeFile(
  VOID
  );

//
// Test node services
//

EFI_STATUS
CreateTestNodes (
  OUT LIST_ENTRY              *TestNodeList
  );

EFI_STATUS
FreeTestNodes (
  IN LIST_ENTRY               *TestNodeList
  );

#endif
