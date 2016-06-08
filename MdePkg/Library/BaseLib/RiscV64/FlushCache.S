//------------------------------------------------------------------------------
//
// RISC-V cache operation.
//
// Copyright (c) 2016, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>
//
// This program and the accompanying materials
// are licensed and made available under the terms and conditions of the BSD License
// which accompanies this distribution.  The full text of the license may be found at
// http://opensource.org/licenses/bsd-license.php.
//
// THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
// WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
//
//------------------------------------------------------------------------------

.align 3
ASM_GLOBAL ASM_PFX(RiscVInvdInstCacheAsm)
ASM_GLOBAL ASM_PFX(RiscVInvdDataCacheAsm)


ASM_PFX(RiscVInvdInstCacheAsm):
    //fence.i
    ret

ASM_PFX(RiscVInvdDataCacheAsm):
    //fence
    ret
