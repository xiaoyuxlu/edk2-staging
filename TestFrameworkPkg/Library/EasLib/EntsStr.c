/** @file
  Ents String implementations.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Library/EntsLib.h>

#define IsDigit(c)    ((c) >= L'0' && (c) <= L'9')
#define IsHexDigit(c) (((c) >= L'a' && (c) <= L'f') || ((c) >= L'A' && (c) <= L'F'))

struct {
  EFI_STATUS  Status;
  CHAR16      *String;
}
EntsStatusString[] = {
  {
    EFI_SUCCESS,
    L"EFI_SUCCESS"
  },
  {
    EFI_LOAD_ERROR,
    L"EFI_LOAD_ERROR"
  },
  {
    EFI_INVALID_PARAMETER,
    L"EFI_INVALID_PARAMETER"
  },
  {
    EFI_UNSUPPORTED,
    L"EFI_UNSUPPORTED"
  },
  {
    EFI_BAD_BUFFER_SIZE,
    L"EFI_BAD_BUFFER_SIZE"
  },
  {
    EFI_BUFFER_TOO_SMALL,
    L"EFI_BUFFER_TOO_SMALL"
  },
  {
    EFI_NOT_READY,
    L"EFI_NOT_READY"
  },
  {
    EFI_DEVICE_ERROR,
    L"EFI_DEVICE_ERROR"
  },
  {
    EFI_WRITE_PROTECTED,
    L"EFI_WRITE_PROTECTED"
  },
  {
    EFI_OUT_OF_RESOURCES,
    L"EFI_OUT_OF_RESOURCES"
  },
  {
    EFI_VOLUME_CORRUPTED,
    L"EFI_VOLUME_CORRUPTED"
  },
  {
    EFI_VOLUME_FULL,
    L"EFI_VOLUME_FULL"
  },
  {
    EFI_NO_MEDIA,
    L"EFI_NO_MEDIA"
  },
  {
    EFI_MEDIA_CHANGED,
    L"EFI_MEDIA_CHANGED"
  },
  {
    EFI_NOT_FOUND,
    L"EFI_NOT_FOUND"
  },
  {
    EFI_ACCESS_DENIED,
    L"EFI_ACCESS_DENIED"
  },
  {
    EFI_NO_RESPONSE,
    L"EFI_NO_RESPONSE"
  },
  {
    EFI_NO_MAPPING,
    L"EFI_NO_MAPPING"
  },
  {
    EFI_TIMEOUT,
    L"EFI_TIMEOUT"
  },
  {
    EFI_NOT_STARTED,
    L"EFI_NOT_STARTED"
  },
  {
    EFI_ALREADY_STARTED,
    L"EFI_ALREADY_STARTED"
  },
  {
    EFI_ABORTED,
    L"EFI_ABORTED"
  },
  {
    EFI_ICMP_ERROR,
    L"EFI_ICMP_ERROR"
  },
  {
    EFI_TFTP_ERROR,
    L"EFI_TFTP_ERROR"
  },
  {
    EFI_PROTOCOL_ERROR,
    L"EFI_PROTOCOL_ERROR"
  },
  {
    EFI_INCOMPATIBLE_VERSION,
    L"EFI_INCOMPATIBLE_VERSION"
  },
  {
    EFI_SECURITY_VIOLATION,
    L"EFI_SECURITY_VIOLATION"
  },
  {
    EFI_CRC_ERROR,
    L"EFI_CRC_ERROR"
  },
  {
    EFI_NOT_AVAILABLE_YET,
    L"EFI_NOT_AVAILABLE_YET"
  },
  {
    EFI_REQUEST_UNLOAD_IMAGE,
    L"EFI_REQUEST_UNLOAD_IMAGE"
  },
  {
    EFI_WARN_UNKNOWN_GLYPH,
    L"EFI_WARN_UNKNOWN_GLYPH"
  },
  {
    EFI_WARN_DELETE_FAILURE,
    L"EFI_WARN_DELETE_FAILURE"
  },
  {
    EFI_WARN_WRITE_FAILURE,
    L"EFI_WARN_WRITE_FAILURE"
  },
  {
    EFI_WARN_BUFFER_TOO_SMALL,
    L"EFI_WARN_BUFFER_TOO_SMALL"
  },
  {
    RETURN_WARN_STALE_DATA,
    L"RETURN_WARN_STALE_DATA"
  },
  {
    MAX_BIT,
    NULL
  }
};

CHAR16  *___strtok_line   = NULL;
CHAR16  *___strtok_arg    = NULL;
CHAR16  *___strtok_field  = NULL;

VOID
EntsStrTrim (
  IN OUT CHAR16   *str,
  IN     CHAR16   c
  )
{
  CHAR16  *p1;
  CHAR16  *p2;

  ASSERT (str != NULL);

  if (*str == 0) {
    return ;
  }
  //
  // Trim off the leading characters c
  //
  for (p1 = str; *p1 && *p1 == c; p1++) {
    ;
  }

  p2 = str;
  if (p2 == p1) {
    while (*p1) {
      p2++;
      p1++;
    }
  } else {
    while (*p1) {
      *p2 = *p1;
      p1++;
      p2++;
    }

    *p2 = 0;
  }

  for (p1 = str + StrLen (str) - 1; p1 >= str && *p1 == c; p1--) {
    ;
  }

  if (p1 != str + StrLen (str) - 1) {
    *(p1 + 1) = 0;
  }
}

CHAR16 *
EntsStrDuplicate (
  IN CHAR16   *Src
  )
{
  CHAR16  *Dest;
  UINTN   Size;

  Size  = StrSize (Src);
  Dest  = AllocatePool (Size);
  if (Dest) {
    CopyMem (Dest, Src, Size);
  }

  return Dest;
}

INTN
EFIAPI
StriCmp (
  IN CHAR16   *s1,
  IN CHAR16   *s2
  )
{
  return EntsUnicodeInterface->StriColl (EntsUnicodeInterface, s1, s2);
}

UINTN
Atoi (
  CHAR16  *str
  )
{
  UINTN   u;
  CHAR16  c;

  //
  // skip preceeding white space
  //
  while (*str && *str == ' ') {
    str += 1;
  }
  //
  // convert digits
  //
  u = 0;
  c = *(str++);
  while (c) {
    if (c >= '0' && c <= '9') {
      u = (u * 10) + c - '0';
    } else {
      break;
    }

    c = *(str++);
  }

  return u;
}

EFI_STATUS
Char16ToChar8 (
  IN  CHAR16 *Src,
  OUT CHAR8  *Dest,
  OUT UINTN  Size
  )
/*++

Routine Description:

  Convert a NULL-ended string of CHAR16 to CHAR8.

Arguments:

  Src   - NULL ended string of CHAR16.
  Dest  - String of CHAR8.
  Size  - The length of CHAR8 string

Returns:

  EFI_SUCCESS - Operation succeeded.
  EFI_INVALID_PARAMETER - Parameter invalid.
  EFI_OUT_OF_RESOURCES  - Memory allocation failed.
  Others      - Some failure happened.

--*/
{
  CHAR8   *Tmp8;
  CHAR16  *Tmp16;

  if ((Src == NULL) || (Dest == NULL) || (Size < StrLen(Src))) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (Dest, Size + 1);

  for (Tmp16 = Src; *Tmp16; Tmp16++) {
    Tmp8  = (CHAR8 *) Tmp16;
    *Dest  = *Tmp8;
    Dest++;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
Char8ToChar16 (
  IN  CHAR8  *Src,
  IN  UINTN  Size,
  OUT CHAR16 *Dest
  )
/*++

Routine Description:

  Convert a NULL-ended string of CHAR8 to CHAR16.

Arguments:

  Src   - String of CHAR8 which is not necessarily null ended.
  Size  - The byte number of CHAR8 string.
  Dest  - String of coresponding CHAR16,

Returns:

  EFI_INVALID_PARAMETER - Parameter invalid.
  EFI_OUT_OF_RESOURCES - Memory allocation failed.
  EFI_SUCCESS - Operation succeeded.

--*/
{
  CHAR8 *Tmp8;
  CHAR8 *Ptr;

  if ((Src == NULL) || (Dest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (Dest, Size * 2 + 2);

  Tmp8  = Src;
  Ptr   = (CHAR8 *) Dest;
  while (Size) {
    *Ptr++  = *Tmp8;
    *Ptr++  = 0;
    Size--;
    Tmp8++;
  }

  return EFI_SUCCESS;
}

UINTN
EntsStrStr (
  IN  CHAR16  *Str,
  IN  CHAR16  *Pat
  )
/*++

Routine Description:

  Search Pat in Str.

Arguments:

  Str - String to be searched.
  Pat - Search pattern.

Returns:

  0 : not found
  >= 1 : found position + 1

--*/
{
  INTN  *Failure;
  INTN  i;
  INTN  j;
  INTN  Lenp;
  INTN  Lens;

  //
  // this function copies from some lib
  //
  Lenp        = StrLen (Pat);
  Lens        = StrLen (Str);

  Failure     = AllocatePool (Lenp * sizeof (INTN));
  Failure[0]  = -1;

  for (j = 1; j < Lenp; j++) {
    i = Failure[j - 1];
    while ((Pat[j] != Pat[i + 1]) && (i >= 0)) {
      i = Failure[i];
    }

    if (Pat[i] == Pat[i + 1]) {
      Failure[j] = i + 1;
    } else {
      Failure[j] = -1;
    }
  }

  i = 0;
  j = 0;
  while (i < Lens && j < Lenp) {
    if (Str[i] == Pat[j]) {
      i++;
      j++;
    } else if (j == 0) {
      i++;
    } else {
      j = Failure[j - 1] + 1;
    }
  }

  FreePool (Failure);

  //
  // 0: not found
  // >= 1: found position + 1
  //
  return ((j == Lenp) ? (i - Lenp) : -1) + 1;

}

CHAR16 *
strtok_line (
  CHAR16       *s,
  const CHAR16 *ct
  )
{
  CHAR16  *sbegin;

  CHAR16  *send;

  sbegin = s ? s : ___strtok_line;
  if (!sbegin) {
    return NULL;
  }

  sbegin += strspn (sbegin, ct);
  if (*sbegin == '\0') {
    ___strtok_line = NULL;
    return NULL;
  }

  send = strpbrk (sbegin, ct);
  if (send && (*send != '\0')) {
    *send++ = '\0';
  }

  ___strtok_line = send;
  return sbegin;
}

CHAR16 *
strtok_field (
  CHAR16       *s,
  const CHAR16 *ct
  )
/*++

Routine Description:

  Find the next token in a string.

--*/
{
  CHAR16  *sbegin;

  CHAR16  *send;

  sbegin = s ? s : ___strtok_field;
  if (!sbegin) {
    return NULL;
  }
  //
  // sbegin += strspn (sbegin, ct);
  // Different with strtok_line()
  // do not find the first substring, just use this char as sbegin
  //
  if (*sbegin == '\0') {
    ___strtok_field = NULL;
    return NULL;
  }

  send = strpbrk (sbegin, ct);
  if (send && (*send != '\0')) {
    *send++ = '\0';
  }

  ___strtok_field = send;
  return sbegin;
}

UINTN
strspn (
  const CHAR16 *s,
  const CHAR16 *accept
  )
/*++

Routine Description:

  Find the first substring.

--*/
{
  const CHAR16  *p;
  const CHAR16  *a;
  UINTN         count;

  count = 0;
  for (p = s; *p != '\0'; ++p) {
    for (a = accept; *a != '\0'; ++a) {
      if (*p == *a) {
        break;
      }
    }

    if (*a == '\0') {
      return count;
    }

    ++count;
  }

  return count;
}

CHAR16 *
strpbrk (
  const CHAR16 *cs,
  const CHAR16 *ct
  )
/*++

Routine Description:

  Scan strings for characters in specified character sets.

--*/
{
  const CHAR16  *sc1;

  const CHAR16  *sc2;

  for (sc1 = cs; *sc1 != '\0'; ++sc1) {
    for (sc2 = ct; *sc2 != '\0'; ++sc2) {
      if (*sc1 == *sc2) {
        return (CHAR16 *) sc1;
      }
    }
  }

  return NULL;
}

INTN
EntsStrToUINTN (
  IN CHAR16        *Str,
  IN OUT UINTN     *Value
  )
{
  INTN  Index;
  UINTN Temp;

  Index = 0;
  Temp  = 0;

  if (Str[Index] == L'0' && (Str[Index + 1] == L'x' || Str[Index + 1] == L'X')) {
    Index += 2;
    while (IsDigit (Str[Index]) || IsHexDigit (Str[Index])) {
      if (a2i (Str[Index]) < 0) {
        return -1;
      }

      Temp = Temp * 0x10 + a2i (Str[Index]);
      Index++;
    }

    *Value = Temp;
    return Index;
  }

  while (IsDigit (Str[Index])) {
    if (a2i (Str[Index]) < 0) {
      return -1;
    }

    Temp = Temp * 10 + a2i (Str[Index]);
    Index++;
  }

  *Value = Temp;
  return Index;
}

INTN
EntsHexStrToUINTN (
  IN CHAR16        *Str,
  IN OUT UINTN     *Value
  )
{
  INTN  Index;
  UINTN Temp;

  Index = 0;
  Temp  = 0;

  while (IsDigit (Str[Index]) || IsHexDigit (Str[Index])) {
    if (a2i (Str[Index]) < 0) {
      return -1;
    }

    Temp = Temp * 0x10 + a2i (Str[Index]);
    Index++;
  }

  *Value = Temp;
  return Index;
}

BOOLEAN
EntsStrEndWith (
  IN CHAR16             *Str,
  IN CHAR16             *SubStr
  )
/*++

Routine Description:

  Test if the string is end with the sub string.

Arguments:

  Str     - NULL ended string.
  SubStr  - NULL ended string.

Returns:

  TRUE    - Str ends with SubStr.
  FALSE   - Str does not end with SubStr.

--*/
{
  CHAR16  *Temp;

  if ((Str == NULL) || (SubStr == NULL) || (StrLen (Str) < StrLen (SubStr))) {
    return FALSE;
  }

  Temp = Str + StrLen (Str) - StrLen (SubStr);

  //
  // Compare
  //
  if (StriCmp (Temp, SubStr) == 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}
//
// Internal functions implementations
//
EFI_STATUS
EntsValueToStr (
  IN UINTN              Value,
  OUT CHAR16            *Buffer
  )
/*++

Routine Description:

  Convert a value to a string.

Arguments:

  Value   - Value to be converted.
  Buffer  - Receive string buffer pointer.

Returns:

  EFI_INVALID_PARAMETER - Parameter invalid.
  EFI_SUCCESS - Operation succeeded.

--*/
{
  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Convert a value to a string
  //
  EntsSPrint (Buffer, 0, L"%d", Value);

  return EFI_SUCCESS;
}

INTN
a2i (
  IN CHAR16       Ch
  )
{
  if (Ch >= L'0' && Ch <= L'9') {
    return Ch - L'0';
  }

  if (Ch >= L'a' && Ch <= L'f') {
    return Ch - L'a' + 0xa;
  }

  if (Ch >= L'A' && Ch <= L'F') {
    return Ch - L'A' + 0xA;
  }

  return -1;
}

EFI_STATUS
EntsTimeToStr (
  IN EFI_TIME            *Time,
  IN UINTN               StrSize,
  IN OUT CHAR16          *Str
  )
{
  EntsSPrint (
    Str,
    StrSize,
    L"%02d-%02d-%04d,%02d:%02d:%02d",
    Time->Day,
    Time->Month,
    Time->Year,
    Time->Hour,
    Time->Minute,
    Time->Second
    );

  return EFI_SUCCESS;
}

EFI_STATUS
EntsStatusToStr (
  IN EFI_STATUS          Status,
  IN UINTN               StrSize,
  IN OUT CHAR16          *Str
  )
{
  BOOLEAN Found;
  UINTN   Index;

  Found = FALSE;
  Index = 0;
  while (Found != TRUE) {
    if ((Status == EntsStatusString[Index].Status) || (MAX_BIT == EntsStatusString[Index].Status)) {
      Found = TRUE;
      EntsSPrint (Str, StrSize, L"%s", EntsStatusString[Index].String);
      break;
    } else {
      Index++;
    }
  }

  return EFI_SUCCESS;
}

CHAR8
M2S (
  IN CHAR8                     Ch
  )
{
  if (Ch <= 0x09) {
    return Ch + '0';
  }

  if (Ch >= 0xa && Ch <= 0xf) {
    return Ch - 0xa + 'a';
  }

  return 0xff;
}

EFI_STATUS
MemToString (
  IN VOID                      *Mem,
  IN OUT CHAR16                *String,
  IN UINT32                    MemSize
  )
/*++

Routine Description:

  Convert Memory to a Unicode string.

Arguments:

  Mem     - Memory buffer to be converted.
  String  - Receive string buffer pointer.
  MemSize - Memory buffer size.

Returns:

  EFI_SUCCESS - Operation succeeded.

--*/
{
  UINT32  Index;

  for (Index = 0; Index < MemSize; Index++) {
    *((CHAR16 *) String + Index * 2)      = (CHAR16) M2S ((*((CHAR8 *) Mem + Index) & 0xF0) >> 4);
    *((CHAR16 *) String + Index * 2 + 1)  = (CHAR16) M2S (*((CHAR8 *) Mem + Index) & 0x0F);
  }

  *(String + Index * 2) = L'\0';

  return EFI_SUCCESS;
}

CHAR8
S2M (
  IN CHAR8                     Ch
  )
{
  if (Ch >= '0' && Ch <= '9') {
    return Ch - '0';
  }

  if (Ch >= 'a' && Ch <= 'f') {
    return Ch - 'a' + 0xa;
  }

  if (Ch >= 'A' && Ch <= 'F') {
    return Ch - 'A' + 0xa;
  }

  return 0xff;
}

EFI_STATUS
StringToMem (
  IN CHAR16                    *String,
  IN OUT VOID                  *Mem,
  IN UINT32                    MemSize
  )
/*++

Routine Description:

  Convert a Unicode string to Memory.

Arguments:

  String  - Unicode string buffer.
  Mem     - Receive memory buffer pointer.
  MemSize - Receive memory buffer size.

Returns:

  EFI_SUCCESS - Operation succeeded.

--*/
{
  UINT32  Index;

  for (Index = 0; (Index < MemSize) && (*(String + Index * 2) != L'\0'); Index++) {
    *((CHAR8 *) Mem + Index) = 0;
    *((CHAR8 *) Mem + Index) |= S2M ((CHAR8) *((CHAR16 *) String + Index * 2)) << 4;
    *((CHAR8 *) Mem + Index) |= S2M ((CHAR8) *((CHAR16 *) String + Index * 2 + 1));
  }

  return EFI_SUCCESS;
}

