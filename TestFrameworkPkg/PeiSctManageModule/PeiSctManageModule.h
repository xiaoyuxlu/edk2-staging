/** @file
  Declarations and typedefs for EFI Application for management

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef  _MANAGE_MODULE_H_
#define  _MANAGE_MODULE_H_

#include "PeiTestPpi.h"

typedef struct SCT_ITEM_RECORD {
  INT8 ItemId;
  INT8 CheckNum;    //item need reboot if none zero
  EFI_GUID HobGuid; //mark the HOB we used to store check results during PEI phase.
}SCT_ITEM_RECORD;

EFI_STATUS
InitManageModule(VOID);

EFI_STATUS
SelectedItemAvailable (SCT_ITEM_RECORD*);

EFI_STATUS
RecordUserSelection(VOID);

EFI_STATUS
ConfigVar(VOID);

EFI_STATUS
DeleteVar(VOID);

EFI_STATUS
ParseHob(EFI_GUID*);

EFI_STATUS
ParseHobData (VOID*, EFI_FILE_HANDLE, UINTN);

EFI_STATUS
GenerateReport(VOID);


VOID
BurnFd(EFI_HANDLE, CHAR16*);

EFI_FILE_HANDLE
Fopen (
  CHAR16    *Name,
  UINT64    Mode
  );
EFI_STATUS
Fclose (
  EFI_FILE_HANDLE   Handle
  );
UINTN
Fwrite (
  EFI_FILE_HANDLE   Handle,
  UINT8             *Buffer,
  UINTN             BufferSize
  );
UINTN
Fread (
  EFI_FILE_HANDLE   Handle,
  UINT8             *Buffer,
  UINTN             BufferSize
  );
EFI_STATUS
Fseek (
  EFI_FILE_HANDLE   Handle,
  UINT64            Position
  );
EFI_STATUS
Flocate (
  EFI_FILE_HANDLE   Handle,
  UINT64            *Position
  );
EFI_STATUS
Fremove (CHAR16*);
#endif
