/** @file
  This file defines efi test item management component.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_FRAMEWORK_TEST_ITEM_MGMT_H
#define _EFI_FRAMEWORK_TEST_ITEM_MGMT_H

//
// Includes
//

#include "efi.h"
#include "ui.h"

//
// Prototypes
//

EFI_STATUS
AddTestItem (
  IN OUT EFI_TEST_ITEM            **TestItemList,
  IN EFI_TEST_ITEM                *TestItem
  );

EFI_STATUS
RemoveTestItemByGuid (
  IN OUT EFI_TEST_ITEM            **TestItemList,
  IN EFI_GUID                     *ItemGuid,
  IN EFI_GUID                     *CaseGuid
  );

EFI_STATUS
RemoveTestItemByName (
  IN OUT EFI_TEST_ITEM            **TestItemList,
  IN CHAR16                       *ItemName
  );

EFI_STATUS
QueryItemStatusByGuid (
  IN EFI_TEST_ITEM                *TestItemList,
  IN EFI_TEST_ITEM_TYPE           ItemType,
  IN EFI_GUID                     *ItemGuid,
  IN EFI_GUID                     *CaseGuid,
  OUT EFI_ITEM_SELECT_STATUS      *SelectStatus
  );

EFI_STATUS
QueryItemStatusByName (
  IN EFI_TEST_ITEM                *TestItemList,
  IN CHAR16                       *ItemName,
  OUT EFI_ITEM_SELECT_STATUS      *SelectStatus
  );

extern
EFI_STATUS
QueryItemIndexByGuid (
  IN EFI_TEST_ITEM                *TestItemList,
  IN EFI_GUID                     *ItemGuid,
  IN EFI_GUID                     *CaseGuid,
  OUT UINTN                       *SelectIndex
  );

extern
EFI_STATUS
QueryItemIndexByName (
  IN EFI_TEST_ITEM                *TestItemList,
  IN CHAR16                       *ItemName,
  OUT UINTN                       *SelectIndex
  );

EFI_STATUS
FreeTestItemList (
  IN EFI_TEST_ITEM                *TestItemList
  );

EFI_STATUS
QueryItemByType (
  IN EFI_TEST_ITEM_TYPE         ItemType,
  IN EFI_GUID                   *ItemGuid OPTIONAL,
  IN OUT EFI_TEST_ITEM          **ItemList
  );
#endif
