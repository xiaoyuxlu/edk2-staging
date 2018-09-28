/** @file
  64-bit Math Worker Function.
  The 32-bit versions of C compiler generate calls to library routines
  to handle 64-bit math. These functions use non-standard calling conventions.

  Copyright (c) 2009 - 2011, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

UINT64
EFIAPI
CustomMathMultU64x64 (
    IN UINT64 Multiplicand,
    IN UINT64 Multiplier
    )
{
  _asm {
    mov     ebx, dword ptr [Multiplicand + 0]
    mov     edx, dword ptr [Multiplier + 0]
    mov     ecx, ebx
    mov     eax, edx
    imul    ebx, dword ptr [Multiplier + 4]
    imul    edx, dword ptr [Multiplicand + 4]
    add     ebx, edx
    mul     ecx
    add     edx, ebx
  }
}

UINT64
EFIAPI
CustomMultU64x64 (
  IN      UINT64                    Multiplicand,
  IN      UINT64                    Multiplier
  )
{
  UINT64                            Result;

  Result = CustomMathMultU64x64 (Multiplicand, Multiplier);

  return Result;
}

INT64
EFIAPI
CustomMultS64x64 (
  IN      INT64                     Multiplicand,
  IN      INT64                     Multiplier
  )
{
  return (INT64)CustomMultU64x64((UINT64)Multiplicand, (UINT64)Multiplier);
}

/*
 * Multiplies a 64-bit signed or unsigned value by a 64-bit signed or unsigned value
 * and returns a 64-bit result.
 */
__declspec(naked) void __cdecl _allmul (void)
{
  //
  // Wrapper Implementation over our CustomMultS64x64() routine
  //    INT64
  //    EFIAPI
  //    CustomMultS64x64 (
  //      IN      INT64      Multiplicand,
  //      IN      INT64      Multiplier
  //      )
  //
  _asm {
    ; Original local stack when calling _allmul
    ;               -----------------
    ;               |               |
    ;               |---------------|
    ;               |               |
    ;               |--Multiplier --|
    ;               |               |
    ;               |---------------|
    ;               |               |
    ;               |--Multiplicand-|
    ;               |               |
    ;               |---------------|
    ;               |  ReturnAddr** |
    ;       ESP---->|---------------|
    ;

    ;
    ; Set up the local stack for Multiplicand parameter
    ;
    mov  eax, [esp + 16]
    push eax
    mov  eax, [esp + 16]
    push eax

    ;
    ; Set up the local stack for Multiplier parameter
    ;
    mov  eax, [esp + 16]
    push eax
    mov  eax, [esp + 16]
    push eax

    ;
    ; Call native MulS64x64 of BaseLib
    ;
    call CustomMultS64x64

    ;
    ; Adjust stack
    ;
    add  esp, 16

    ret  16
  }
}

/*
 * Shifts a 64-bit signed value left by a particular number of bits.
 */
__declspec(naked) void __cdecl _allshl(void)
{
  _asm {
    ;
    ; Handle shifting of 64 or more bits (return 0)
    ;
    cmp     cl, 64
    jae     short ReturnZero

    ;
    ; Handle shifting of between 0 and 31 bits
    ;
    cmp     cl, 32
    jae     short More32
    shld    edx, eax, cl
    shl     eax, cl
    ret

    ;
    ; Handle shifting of between 32 and 63 bits
    ;
More32:
    mov     edx, eax
    xor     eax, eax
    and     cl, 31
    shl     edx, cl
    ret

ReturnZero:
    xor     eax,eax
    xor     edx,edx
    ret
  }
}

/*
 * Shifts a 64-bit unsigned value right by a certain number of bits.
 */
__declspec(naked) void __cdecl _aullshr(void)
{
  _asm {
    ;
    ; Checking: Only handle 64bit shifting or more
    ;
    cmp     cl, 64
    jae     _Exit

    ;
    ; Handle shifting between 0 and 31 bits
    ;
    cmp     cl, 32
    jae     More32
    shrd    eax, edx, cl
    shr     edx, cl
    ret

    ;
    ; Handle shifting of 32-63 bits
    ;
More32:
    mov     eax, edx
    xor     edx, edx
    and     cl, 31
    shr     eax, cl
    ret

    ;
    ; Invalid number (less then 32bits), return 0
    ;
_Exit:
    xor     eax, eax
    xor     edx, edx
    ret
  }
}
