;------------------------------------------------------------------------------
;
; Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;------------------------------------------------------------------------------

SECTION .text

; This must be aligned to MY_UNIT_TEST_SUITE
%define OFFSET_OF_UNIT_TEST_SUITE_SETUP_IN_MY_UNIT_TEST_SUITE       (4 * 5)
%define OFFSET_OF_UNIT_TEST_SUITE_TEARDOWN_IN_MY_UNIT_TEST_SUITE    (4 * 6)
%define OFFSET_OF_UNIT_TEST_FRAMEWORK_HANDLE_IN_MY_UNIT_TEST_SUITE  (4 * 9)

; This must be aligned to MY_UNIT_TEST
%define OFFSET_OF_UNIT_TEST_FUNCTION_IN_MY_UNIT_TEST                (4 * 4)
%define OFFSET_OF_UNIT_TEST_PREREQ_IN_MY_UNIT_TEST                  (4 * 5)
%define OFFSET_OF_UNIT_TEST_CLEANUP_IN_MY_UNIT_TEST                 (4 * 6)
%define OFFSET_OF_UNIT_TEST_CONTEXT_IN_MY_UNIT_TEST                 (4 * 7)
%define OFFSET_OF_UNIT_TEST_FRAMEWORK_HANDLE_IN_MY_UNIT_TEST        (4 * 9)

;------------------------------------------------------------------------------
; int InitializeFuncTemplate (void);
;------------------------------------------------------------------------------
global ASM_PFX(InitializeFuncTemplate)
ASM_PFX(InitializeFuncTemplate):
    ; mov eax, <MY_UNIT_TEST_SUITE>
    DB 0xB8
    DD 0x0  ; to be patched
    push ebx
    mov  ebx, eax

    ; call UNIT_TEST_SUITE_SETUP(UNIT_TEST_FRAMEWORK_HANDLE)
    mov  ecx, [ebx + OFFSET_OF_UNIT_TEST_FRAMEWORK_HANDLE_IN_MY_UNIT_TEST_SUITE]
    mov  eax, [ebx + OFFSET_OF_UNIT_TEST_SUITE_SETUP_IN_MY_UNIT_TEST_SUITE]
    test eax, eax
    jz   InitializeDone
    push ecx
    call eax
    add  esp, 4
InitializeDone:

    pop  ebx
    xor  eax, eax
    ret

global ASM_PFX(mInitializeFuncTemplateSize)
ASM_PFX(mInitializeFuncTemplateSize) DD  $ - ASM_PFX(InitializeFuncTemplate)

;------------------------------------------------------------------------------
; int CleanupFuncTemplate (void);
;------------------------------------------------------------------------------
global ASM_PFX(CleanupFuncTemplate)
ASM_PFX(CleanupFuncTemplate):
    ; mov eax, <MY_UNIT_TEST_SUITE>
    DB 0xB8
    DD 0x0  ; to be patched
    push ebx
    mov  ebx, eax

    ; call UNIT_TEST_SUITE_TEARDOWN(UNIT_TEST_FRAMEWORK_HANDLE)
    mov  ecx, [ebx + OFFSET_OF_UNIT_TEST_FRAMEWORK_HANDLE_IN_MY_UNIT_TEST_SUITE]
    mov  eax, [ebx + OFFSET_OF_UNIT_TEST_SUITE_TEARDOWN_IN_MY_UNIT_TEST_SUITE]
    test eax, eax
    jz   CleanupDone
    push ecx
    call eax
    add  esp, 4
CleanupDone:

    pop  ebx
    xor  eax, eax
    ret

global ASM_PFX(mCleanupFuncTemplateSize)
ASM_PFX(mCleanupFuncTemplateSize) DD  $ - ASM_PFX(CleanupFuncTemplate)

;------------------------------------------------------------------------------
; void TestFuncTemplate (void);
;------------------------------------------------------------------------------
global ASM_PFX(TestFuncTemplate)
ASM_PFX(TestFuncTemplate):
    ; mov eax, <MY_UNIT_TEST>
    DB 0xB8
    DD 0x0  ; to be patched
    push ebx
    mov  ebx, eax

    ; call UNIT_TEST_PREREQ(UNIT_TEST_FRAMEWORK_HANDLE, UNIT_TEST_CONTEXT)
    mov  ecx, [ebx + OFFSET_OF_UNIT_TEST_FRAMEWORK_HANDLE_IN_MY_UNIT_TEST]
    mov  edx, [ebx + OFFSET_OF_UNIT_TEST_CONTEXT_IN_MY_UNIT_TEST]
    mov  eax, [ebx + OFFSET_OF_UNIT_TEST_PREREQ_IN_MY_UNIT_TEST]
    test eax, eax
    jz   PreReqDone
    push edx
    push ecx
    call eax
    add  esp, 8
PreReqDone:
    test eax, eax
    jnz  CleanupDone

    ; call UNIT_TEST_FUNCTION(UNIT_TEST_FRAMEWORK_HANDLE, UNIT_TEST_CONTEXT)
    mov  ecx, [ebx + OFFSET_OF_UNIT_TEST_FRAMEWORK_HANDLE_IN_MY_UNIT_TEST]
    mov  edx, [ebx + OFFSET_OF_UNIT_TEST_CONTEXT_IN_MY_UNIT_TEST]
    mov  eax, [ebx + OFFSET_OF_UNIT_TEST_FUNCTION_IN_MY_UNIT_TEST]
    test eax, eax
    jz   DeadLoop
    push edx
    push ecx
    call eax
    add  esp, 8

    ; call UNIT_TEST_CLEANUP(UNIT_TEST_FRAMEWORK_HANDLE, UNIT_TEST_CONTEXT)
    mov  ecx, [ebx + OFFSET_OF_UNIT_TEST_FRAMEWORK_HANDLE_IN_MY_UNIT_TEST]
    mov  edx, [ebx + OFFSET_OF_UNIT_TEST_CONTEXT_IN_MY_UNIT_TEST]
    mov  eax, [ebx + OFFSET_OF_UNIT_TEST_CLEANUP_IN_MY_UNIT_TEST]
    test eax, eax
    jz   CleanUpDone
    push edx
    push ecx
    call eax
    add  esp, 8
CleanUpDone:

    pop  ebx
    ret

DeadLoop:
    jmp $
    ret
global ASM_PFX(mTestFuncTemplateSize)
ASM_PFX(mTestFuncTemplateSize) DD  $ - ASM_PFX(TestFuncTemplate)

;------------------------------------------------------------------------------
; void SetUpFuncTemplate (void);
;------------------------------------------------------------------------------
global ASM_PFX(SetUpFuncTemplate)
ASM_PFX(SetUpFuncTemplate):
    ; TBD
    ret
    
;------------------------------------------------------------------------------
; void TearDownFuncTemplate (void);
;------------------------------------------------------------------------------
global ASM_PFX(TearDownFuncTemplate)
ASM_PFX(TearDownFuncTemplate):
    ; TBD
    ret

