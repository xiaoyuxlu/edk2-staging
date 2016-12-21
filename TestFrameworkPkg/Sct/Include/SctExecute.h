/** @file
  This file provides the test execution services.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_SCT_EXECUTE_H_
#define _EFI_SCT_EXECUTE_H_

//
// External functions declaration
//

//
// Execution services
//

EFI_STATUS
SctExecutePrepare (
  VOID
  );

EFI_STATUS
SctExecuteStart (
  VOID
  );

EFI_STATUS
SctExecute (
  VOID
  );

EFI_STATUS
SctContinueExecute (
  VOID
  );

//
// Reset services
//

EFI_STATUS
RemoveRecoveryFile (
  VOID
  );

EFI_STATUS
ResetAllTestResults (
  VOID
  );

#endif
