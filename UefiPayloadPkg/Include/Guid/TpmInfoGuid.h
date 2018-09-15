/** @file
  This file defines the hob structure for TPM info.
  
  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __TPM_INFO_GUID_H__
#define __TPM_INFO_GUID_H__

///
/// TPM Information GUID
///
extern EFI_GUID gUefiTpmInfoGuid;

typedef struct {  
	UINTN             TpmTable12Base;
	UINTN             TpmTable12LAML;
	UINTN             TpmTable2Base;
	UINTN             TpmTable2LAML;
} TPM_INFO;  
  
#endif
