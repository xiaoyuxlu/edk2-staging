/** @file
  Support routines for PxeBc.

Copyright (c) 2007 - 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include "PxeBcImpl.h"


/**
  The common notify function associated with various PxeBc events.

  @param  Event     The event signaled.
  @param  Context   The context.

**/
VOID
EFIAPI
PxeBcCommonNotify (
  IN EFI_EVENT           Event,
  IN VOID                *Context
  )
{
  *((BOOLEAN *) Context) = TRUE;
}

/**
  This function is to display the IPv4 address.

  @param[in]  Ip        The pointer to the IPv4 address.

**/
VOID
PxeBcShowIp4Addr (
  IN EFI_IPv4_ADDRESS   *Ip
  )
{
  UINTN                 Index;

  for (Index = 0; Index < 4; Index++) {
    AsciiPrint ("%d", Ip->Addr[Index]);
    if (Index < 3) {
      AsciiPrint (".");
    }
  }
}

/**
  Convert number to ASCII value.

  @param  Number              Numeric value to convert to decimal ASCII value.
  @param  Buffer              Buffer to place ASCII version of the Number.
  @param  Length              Length of Buffer.

**/
VOID
CvtNum (
  IN UINTN  Number,
  IN UINT8  *Buffer,
  IN UINTN   Length
  )
{
  UINTN Remainder;

  for (; Length > 0; Length--) {
    Remainder = Number % 10;
    Number /= 10;
    Buffer[Length - 1] = (UINT8) ('0' + Remainder);
  }
}


/**
  Convert unsigned int number to decimal number.

  @param      Number         The unsigned int number will be converted.
  @param      Buffer         Pointer to the buffer to store the decimal number after transform.
  @param[in]  BufferSize     The maxsize of the buffer.
  
  @return the length of the number after transform.

**/
UINTN
UtoA10 (
  IN UINTN Number,
  IN CHAR8 *Buffer,
  IN UINTN BufferSize
  )
{
  UINTN Index;
  CHAR8 TempStr[64];

  Index           = 63;
  TempStr[Index]  = 0;

  do {
    Index--;
    TempStr[Index]  = (CHAR8) ('0' + (Number % 10));
    Number          = Number / 10;
  } while (Number != 0);

  AsciiStrCpyS (Buffer, BufferSize, &TempStr[Index]);

  return AsciiStrLen (Buffer);
}


/**
  Convert ASCII numeric string to a UINTN value.

  @param  Buffer  Pointer to the 8-byte unsigned int value.

  @return UINTN value of the ASCII string.

**/
UINT64
AtoU64 (
  IN UINT8 *Buffer
  )
{
  UINT64  Value;
  UINT8   Character;

  Value = 0;
  while ((Character = *Buffer++) != '\0') {
    Value = MultU64x32 (Value, 10) + (Character - '0');
  }

  return Value;
}

