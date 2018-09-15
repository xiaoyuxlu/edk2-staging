/** @file
This is the main include file for User Extension lib. All the API functions for
users are listed in the files included in this file.

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef __Custom_H_LIB__
#define __Custom_H_LIB__

#include <Uefi.h>

#include "UserExtensionApi/BaseLib.h"
#include "UserExtensionApi/BaseMemoryLib.h"
#include "UserExtensionApi/ConsoleInOut.h"
#include "UserExtensionApi/DebugLib.h"
#include "UserExtensionApi/FsAccessLib.h"
#include "UserExtensionApi/IoLib.h"
#include "UserExtensionApi/MemoryAllocationLib.h"
#include "UserExtensionApi/PciExpressLib.h"
#include "UserExtensionApi/PciLib.h"
#include "UserExtensionApi/TimerEventLib.h"
#include "UserExtensionApi/TimerLib.h"

#define Custom_File_0 {0x15893854, 0x5682, 0x491F, { 0xB4, 0x21, 0xB4, 0x39, 0x1E, 0x45, 0xC4, 0x6B }}
#define Custom_File_1 {0x23C2F11D, 0x39DB, 0x4F39, { 0x89, 0xB9, 0xA1, 0x5F, 0xB4, 0xC9, 0x46, 0xD3 }}
#define Custom_File_2 {0xD0C7FA2C, 0xBB32, 0x4CB5, { 0x80, 0x8C, 0x00, 0x96, 0x2C, 0x04, 0x86, 0x73 }}

#endif