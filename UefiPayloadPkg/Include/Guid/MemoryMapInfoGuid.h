/** @file
  This file defines the hob structure for memory map information.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __MEMORY_MAP_INFO_GUID_H__
#define __MEMORY_MAP_INFO_GUID_H__

#include <Library/PcdLib.h>

extern EFI_GUID gLoaderMemoryMapInfoGuid;

#pragma pack(1)
typedef struct {
  UINT64 Base;
  UINT64 Size;
  UINT8  Type;
  UINT8  Flag;
  UINT8  Reserved[6];
} MEMROY_MAP_ENTRY;

typedef struct {
  UINT8  Revision;
  UINT8  Reserved0[3];
  UINT32 Count;
  MEMROY_MAP_ENTRY  Entry[0];
} MEMROY_MAP_INFO;
#pragma pack()

#endif
