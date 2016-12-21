/** @file
  Implementation of recording result services

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>
#include <Library/MemoryAllocationLib.h>

#include <Library/EntsLib.h>

EFI_STATUS
RecordMessage (
  IN  OUT CHAR16**ResultBuffer,
  IN  OUT UINTN *ResultBufferSize,
  IN     CHAR16 *Format,
  ...
  )
/*++

Routine Description:

  Record runtime information to a buffer.

Arguments:

  ResultBuffer      - Buffer space.
  ResultBufferSize  - Result buffer size in octets.
  Format            - Format string.
  ...               - Variables.

Returns:

  EFI_SUCCESS - Operation succeeded.
  EFI_OUT_OF_RESOURCES - Memory allocation failed.

--*/
{
  UINTN   Size;
  VA_LIST Marker;
  CHAR16  *Buffer;

  Buffer = AllocatePool (ENTS_MAX_BUFFER_SIZE * 4 * 8);
  if (Buffer == NULL) {
    EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"Error in RecordMessage Status - EFI_OUT_OF_RESOURCES"));
    return EFI_OUT_OF_RESOURCES;
  }

  VA_START (Marker, Format);
  EntsVSPrint (Buffer, ENTS_MAX_BUFFER_SIZE * 4 * 8, Format, Marker);
  VA_END (Marker);

  if (*ResultBuffer == NULL) {
    Size          = StrLen (Buffer) * 2 + 4 + ENTS_MAX_BUFFER_SIZE * 4;
    *ResultBuffer = AllocatePool (Size);
    if (*ResultBuffer == NULL) {
      EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"Error in RecordMessage Status - EFI_OUT_OF_RESOURCES"));
      goto RecordMessageFail1;
    }
    (*ResultBuffer)[0]  = L'\0';
    *ResultBufferSize   = Size;
  }

  Size = StrLen (*ResultBuffer) * 2 + StrLen (Buffer) * 2 + 4;
  if ((*ResultBufferSize) < Size) {
    *ResultBuffer = ReallocatePool (*ResultBufferSize, Size + ENTS_MAX_BUFFER_SIZE * 4, *ResultBuffer);
    if (*ResultBuffer == NULL) {
      EFI_ENTS_DEBUG ((EFI_ENTS_D_ERROR, L"Error in RecordMessage Status - EFI_OUT_OF_RESOURCES"));
      goto RecordMessageFail2;
    }

    *ResultBufferSize = Size + ENTS_MAX_BUFFER_SIZE * 4;
  }

  StrCat (*ResultBuffer, Buffer);
  StrCat (*ResultBuffer, L"\n");

  FreePool (Buffer);
  return EFI_SUCCESS;

RecordMessageFail2:
  if(*ResultBuffer != NULL) {
    FreePool (*ResultBuffer);
  }
RecordMessageFail1:
  if (Buffer != NULL) {
    FreePool (Buffer);
  }

  return EFI_OUT_OF_RESOURCES;
}
