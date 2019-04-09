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

#ifndef __EFI_PXEBC_SUPPORT_H__
#define __EFI_PXEBC_SUPPORT_H__


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
  );

/**
  This function is to display the IPv4 address.

  @param[in]  Ip        The pointer to the IPv4 address.

**/
VOID
PxeBcShowIp4Addr (
  IN EFI_IPv4_ADDRESS   *Ip
  );
  

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
  );


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
  
  );


/**
  Convert ASCII numeric string to a UINTN value.

  @param  Buffer  Pointer to the 8-byte unsigned int value.

  @return UINTN value of the ASCII string.

**/
UINT64
AtoU64 (
  IN UINT8 *Buffer
  );


#endif

