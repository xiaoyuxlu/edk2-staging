/** @file
  Deal with the user interface menu for test nodes.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_TEST_NODE_MENU_H_
#define _EFI_TEST_NODE_MENU_H_

//
//  Includes
//

#include "Sct.h"
#include "Ui.h"
#include "BuildMenu.h"
#include "Dialog.h"

#include <Protocol/DevicePath.h>

//
// Definitions
//

#define ITERATION_NUMBER_MAX     999
#define ITERATION_NUMBER_MIN     0
#define ITERATION_NUMBER_DEFAULT 0

//
// Functions
//

EFI_STATUS
DisplayTestNodeMenu (
  IN LIST_ENTRY               *Root,
  IN EFI_MENU_PAGE                *ParentPage
  );

UINTN
CalculatePassNumber (
  EFI_SCT_TEST_NODE               *TestNode
  );

UINTN
CalculateTotalPassNumber (
  LIST_ENTRY                  *Root
  );

UINTN
CalculateFailNumber (
  EFI_SCT_TEST_NODE               *TestNode
  );

UINTN
CalculateTotalFailNumber (
  LIST_ENTRY                  *Root
  );

UINT32
GetTestCaseIterations (
  IN EFI_GUID                     *Guid
  );

#endif
