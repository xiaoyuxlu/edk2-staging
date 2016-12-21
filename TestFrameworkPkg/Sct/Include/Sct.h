/** @file
  SCT master include file.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_SCT_H_
#define _EFI_SCT_H_

//
// Includes
//
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>
#include <Library/FileHandleLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ShellLib.h>
#include <Library/ShellCommandLib.h>
#include <Library/EntsLib.h>
#include <Protocol/ShellParameters.h>
#include <Protocol/UnicodeCollation.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SimpleNetwork.h>
#include <Protocol/PciIo.h>

#include <Protocol/LibPrivate.h>
#include <Protocol/BbTest.h>
#include <Protocol/WbTest.h>
#include <Protocol/StandardTestLibrary.h>
#include <Protocol/TestProfileLibrary.h>
#include <Protocol/TestRecoveryLibrary.h>
#include <Protocol/TestLoggingLibrary.h>
#include <Protocol/TestOutputLibrary.h>

#include "ApTest.h"
#include "SctTypes.h"
#include "SctCore.h"
#include "SctData.h"
#include "SctDataEx.h"
#include "SctDebug.h"
#include "SctExecute.h"
#include "SctLoad.h"
#include "SctMisc.h"
#include "SctOperation.h"
#include "SctOutput.h"
#include "SctReport.h"
#include "SctDeviceConfig.h"
#include "SctUi.h"
#include "SctDef.h"
#include "ExecuteSupport.h"
#include "ENTS/EasDispatcher/Include/Eas.h"

#define StrDuplicate(Src)  AllocateCopyPool (StrSize (Src), Src)

extern EFI_SHELL_PROTOCOL                *gShell;
extern EFI_UNICODE_COLLATION_PROTOCOL    *gtUnicodeCollation;

UINTN
SPrint (
    OUT CHAR16  *Str,
    IN UINTN    StrSize,
    IN CHAR16   *fmt,
    ...
    );

UINTN
VSPrint (
    OUT CHAR16  *Str,
    IN UINTN    StrSize,
    IN CHAR16   *fmt,
    IN VA_LIST  vargs
    );

EFI_FILE_HANDLE
LibOpenRoot (
  IN EFI_HANDLE                   DeviceHandle
  );

#endif
