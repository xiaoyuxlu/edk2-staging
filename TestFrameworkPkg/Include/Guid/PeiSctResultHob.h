/** @file
  This file defines the PEI SCT Result HOB.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_SCT_RESULT_HOB_H_
#define _PEI_SCT_RESULT_HOB_H_

#define PEI_SCT_RESULT_HOB_GUID = \
  {0x8186bb6d, 0xd805, 0x4e1f, {0x8e, 0x6, 0x1a, 0x83, 0xc8, 0xed, 0x18, 0x54}}

typedef struct PEI_SCT_RESULT_HOB {
  EFI_GUID  AssertionGuid;
  INT8      Result;
  //
  // Zero if only one procedure for this AssertionGuid
  //
  INT8      ProcedureNumber;
} PEI_SCT_RESULT_HOB;

extern EFI_GUID gPeiSctResultHobGuid;

#endif
