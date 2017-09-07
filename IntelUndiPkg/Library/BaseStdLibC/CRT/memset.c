/** @file
    Miscellaneous Functions for <string.h>.

    Copyright (c) 2010, Intel Corporation. All rights reserved.<BR>
    This program and the accompanying materials are licensed and made available under
    the terms and conditions of the BSD License that accompanies this distribution.
    The full text of the license may be found at
    http://opensource.org/licenses/bsd-license.php.

    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#ifdef _SIZE_T_NOTDEFINED
typedef unsigned int size_t;
#endif

/** The memset function copies the value of c (converted to an unsigned char)
    into each of the first n characters of the object pointed to by s.

    @return   The memset function returns the value of s.
**/
#include  <Uefi.h>
#include  <Library/BaseLib.h>
#include  <Library/BaseMemoryLib.h>

void *
memset(void *s, int c, size_t n)
{
  return SetMem( s, (UINTN)n, (UINT8)c);
}