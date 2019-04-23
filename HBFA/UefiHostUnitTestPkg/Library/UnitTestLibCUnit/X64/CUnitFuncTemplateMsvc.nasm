;------------------------------------------------------------------------------
;
; Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;------------------------------------------------------------------------------

SECTION .text

; This must be aligned to MY_UNIT_TEST_SUITE
%define OFFSET_OF_UNIT_TEST_SUITE_SETUP_IN_MY_UNIT_TEST_SUITE       (8 * 5)
%define OFFSET_OF_UNIT_TEST_SUITE_TEARDOWN_IN_MY_UNIT_TEST_SUITE    (8 * 6)
%define OFFSET_OF_UNIT_TEST_FRAMEWORK_HANDLE_IN_MY_UNIT_TEST_SUITE  (8 * 9)

; This must be aligned to MY_UNIT_TEST
%define OFFSET_OF_UNIT_TEST_FUNCTION_IN_MY_UNIT_TEST                (8 * 4)
%define OFFSET_OF_UNIT_TEST_PREREQ_IN_MY_UNIT_TEST                  (8 * 5)
%define OFFSET_OF_UNIT_TEST_CLEANUP_IN_MY_UNIT_TEST                 (8 * 6)
%define OFFSET_OF_UNIT_TEST_CONTEXT_IN_MY_UNIT_TEST                 (8 * 7)
%define OFFSET_OF_UNIT_TEST_FRAMEWORK_HANDLE_IN_MY_UNIT_TEST        (8 * 9)

;------------------------------------------------------------------------------
; int InitializeFuncTemplate (void);
;------------------------------------------------------------------------------
global ASM_PFX(InitializeFuncTemplate)
ASM_PFX(InitializeFuncTemplate):
    ; mov rax, <MY_UNIT_TEST_SUITE>
    DB 0x48, 0xB8
    DQ 0x0  ; to be patched
    push rbx
    mov  rbx, rax

    ; call UNIT_TEST_SUITE_SETUP(UNIT_TEST_FRAMEWORK_HANDLE)
    mov  rcx, [rbx + OFFSET_OF_UNIT_TEST_FRAMEWORK_HANDLE_IN_MY_UNIT_TEST_SUITE]
    mov  rax, [rbx + OFFSET_OF_UNIT_TEST_SUITE_SETUP_IN_MY_UNIT_TEST_SUITE]
    test rax, rax
    jz   InitializeDone
    push rbx
    push rbp
    push rsi
    push rdi
    call rax
    pop  rdi
    pop  rsi
    pop  rbp
    pop  rbx
InitializeDone:

    pop  rbx
    xor  rax, rax
    ret

global ASM_PFX(mInitializeFuncTemplateSize)
ASM_PFX(mInitializeFuncTemplateSize) DQ  $ - ASM_PFX(InitializeFuncTemplate)

;------------------------------------------------------------------------------
; int CleanupFuncTemplate (void);
;------------------------------------------------------------------------------
global ASM_PFX(CleanupFuncTemplate)
ASM_PFX(CleanupFuncTemplate):
    ; mov rax, <MY_UNIT_TEST_SUITE>
    DB 0x48, 0xB8
    DQ 0x0  ; to be patched
    push rbx
    mov  rbx, rax

    ; call UNIT_TEST_SUITE_TEARDOWN(UNIT_TEST_FRAMEWORK_HANDLE)
    mov  rcx, [rbx + OFFSET_OF_UNIT_TEST_FRAMEWORK_HANDLE_IN_MY_UNIT_TEST_SUITE]
    mov  rax, [rbx + OFFSET_OF_UNIT_TEST_SUITE_TEARDOWN_IN_MY_UNIT_TEST_SUITE]
    test rax, rax
    jz   CleanupDone
    push rbx
    push rbp
    push rsi
    push rdi
    call rax
    pop  rdi
    pop  rsi
    pop  rbp
    pop  rbx
CleanupDone:

    pop  rbx
    xor  rax, rax
    ret

global ASM_PFX(mCleanupFuncTemplateSize)
ASM_PFX(mCleanupFuncTemplateSize) DQ  $ - ASM_PFX(CleanupFuncTemplate)

;------------------------------------------------------------------------------
; void TestFuncTemplate (void);
;------------------------------------------------------------------------------
global ASM_PFX(TestFuncTemplate)
ASM_PFX(TestFuncTemplate):
    ; mov rax, <MY_UNIT_TEST>
    DB 0x48, 0xB8
    DQ 0x0  ; to be patched
    push rbx
    mov  rbx, rax

    ; call UNIT_TEST_PREREQ(UNIT_TEST_FRAMEWORK_HANDLE, UNIT_TEST_CONTEXT)
    mov  rcx, [rbx + OFFSET_OF_UNIT_TEST_FRAMEWORK_HANDLE_IN_MY_UNIT_TEST]
    mov  rdx, [rbx + OFFSET_OF_UNIT_TEST_CONTEXT_IN_MY_UNIT_TEST]
    mov  rax, [rbx + OFFSET_OF_UNIT_TEST_PREREQ_IN_MY_UNIT_TEST]
    test rax, rax
    jz   PreReqDone
    push rbx
    push rbp
    push rsi
    push rdi
    call rax
    pop  rdi
    pop  rsi
    pop  rbp
    pop  rbx
PreReqDone:
    test rax, rax
    jnz  CleanupDone

    ; call UNIT_TEST_FUNCTION(UNIT_TEST_FRAMEWORK_HANDLE, UNIT_TEST_CONTEXT)
    mov  rcx, [rbx + OFFSET_OF_UNIT_TEST_FRAMEWORK_HANDLE_IN_MY_UNIT_TEST]
    mov  rdx, [rbx + OFFSET_OF_UNIT_TEST_CONTEXT_IN_MY_UNIT_TEST]
    mov  rax, [rbx + OFFSET_OF_UNIT_TEST_FUNCTION_IN_MY_UNIT_TEST]
    test rax, rax
    jz   DeadLoop
    push rbx
    push rbp
    push rsi
    push rdi
    call rax
    pop  rdi
    pop  rsi
    pop  rbp
    pop  rbx

    ; call UNIT_TEST_CLEANUP(UNIT_TEST_FRAMEWORK_HANDLE, UNIT_TEST_CONTEXT)
    mov  rcx, [rbx + OFFSET_OF_UNIT_TEST_FRAMEWORK_HANDLE_IN_MY_UNIT_TEST]
    mov  rdx, [rbx + OFFSET_OF_UNIT_TEST_CONTEXT_IN_MY_UNIT_TEST]
    mov  rax, [rbx + OFFSET_OF_UNIT_TEST_CLEANUP_IN_MY_UNIT_TEST]
    test rax, rax
    jz   CleanUpDone
    push rbx
    push rbp
    push rsi
    push rdi
    call rax
    pop  rdi
    pop  rsi
    pop  rbp
    pop  rbx
CleanUpDone:

    pop  rbx
    ret

DeadLoop:
    jmp $
    ret
global ASM_PFX(mTestFuncTemplateSize)
ASM_PFX(mTestFuncTemplateSize) DQ  $ - ASM_PFX(TestFuncTemplate)

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

