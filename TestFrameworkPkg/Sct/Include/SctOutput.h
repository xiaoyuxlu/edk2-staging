/** @file
  This file provides a default test output library.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_SCT_OUTPUT_H_
#define _EFI_SCT_OUTPUT_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/TestOutputLibrary.h>

//
// Private interface data structures of TestOutput protocol
//

#define TEST_OUTPUT_PRIVATE_DATA_SIGNATURE  SIGNATURE_32('T','O','L','I')

#define TEST_OUTPUT_PRIVATE_DATA_REVISION   0x00010000

//
// Forward reference for pure ANSI compatibility
//
typedef struct _TEST_OUTPUT_FILE  TEST_OUTPUT_FILE;

struct _TEST_OUTPUT_FILE {
  TEST_OUTPUT_FILE            *Next;
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  CHAR16                      *FileName;
  EFI_FILE_HANDLE             FileHandle;
  UINTN                       OpenCount;
};

typedef struct {
  UINT32                                    Signature;
  EFI_TEST_OUTPUT_LIBRARY_PROTOCOL          TestOutput;
  TEST_OUTPUT_FILE                          *OutputFileList;
} TEST_OUTPUT_PRIVATE_DATA;

#define TEST_OUTPUT_PRIVATE_DATA_FROM_THIS(a) \
  CR(a, TEST_OUTPUT_PRIVATE_DATA, TestOutput, TEST_OUTPUT_PRIVATE_DATA_SIGNATURE)

//
// Global variables
//

extern EFI_TEST_OUTPUT_LIBRARY_PROTOCOL *gOutputProtocol;

#endif
