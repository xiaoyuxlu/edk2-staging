/** @file
  ENTS misc services implementations.

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
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Library/EntsLib.h>

EFI_STATUS
GetImageDevicePath (
  IN EFI_HANDLE                   ImageHandle,
  OUT EFI_DEVICE_PATH_PROTOCOL    **DevicePath,
  OUT CHAR16                      **FilePath
  )
/*++

Routine Description:

  Get device path and file path from the image handle.

Arguments:

  ImageHandle - The image handle.
  DevicePath  - The device path of the image handle.
  FilePath    - The file path of the image handle.

Returns:

  EFI_SUCCESS          - Operation succeeded.
  EFI_OUT_OF_RESOURCES - Memory allocation failed.
  EFI_NOT_FOUND        - File path not found.
  Others               - Some failure happened.

--*/
{
  EFI_STATUS                Status;
  EFI_LOADED_IMAGE_PROTOCOL *Image;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *TempDeviceNode;
  CHAR16                    *TempFilePath;

  //
  // Get the image instance from the image handle
  //
  Status = gntBS->HandleProtocol (
                    ImageHandle,
                    &gEfiLoadedImageProtocolGuid,
                    &Image
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Get the device instance from the device handle
  //
  Status = gntBS->HandleProtocol (
                    Image->DeviceHandle,
                    &gEfiDevicePathProtocolGuid,
                    &TempDevicePath
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *DevicePath = EntsDuplicateDevicePath (TempDevicePath);
  if (*DevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Get the file name from the image instance
  //
  TempDevicePath  = Image->FilePath;

  TempFilePath    = NULL;
  TempDeviceNode  = TempDevicePath;
  while (!IsDevicePathEnd (TempDeviceNode)) {
    if ((DevicePathType (TempDeviceNode) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (TempDeviceNode) == MEDIA_FILEPATH_DP)
        ) {
      TempFilePath = EntsStrDuplicate (((FILEPATH_DEVICE_PATH *) TempDeviceNode)->PathName);
      if (TempFilePath == NULL) {
        FreePool (*DevicePath);
        return EFI_OUT_OF_RESOURCES;
      }
      break;
    }

    TempDeviceNode = NextDevicePathNode (TempDeviceNode);
  }

  if (TempFilePath == NULL) {
    FreePool (*DevicePath);
    return EFI_NOT_FOUND;
  }
  //
  // If the file path is only a root directory "\\", remove it
  //
  if ((TempFilePath[0] == L'\\') && (TempFilePath[1] == L'\0')) {
    TempFilePath[0] = L'\0';
  }

  *FilePath = TempFilePath;

  //
  // Done
  //
  return EFI_SUCCESS;
}

BOOLEAN
EntsGrowBuffer (
  IN OUT EFI_STATUS   *Status,
  IN OUT VOID         **Buffer,
  IN UINTN            BufferSize
  )
/*++

Routine Description:

    Helper function called as part of the code needed
    to allocate the proper sized buffer for various
    EFI interfaces.

Arguments:

    Status      - Current status

    Buffer      - Current allocated buffer, or NULL

    BufferSize  - Current buffer size needed

Returns:

    TRUE - if the buffer was reallocated and the caller
    should try the API again.

--*/
{
  BOOLEAN TryAgain;

  //
  // If this is an initial request, buffer will be null with a new buffer size
  //
  if (!*Buffer && BufferSize) {
    *Status = EFI_BUFFER_TOO_SMALL;
  }
  //
  // If the status code is "buffer too small", resize the buffer
  //
  TryAgain = FALSE;
  if (*Status == EFI_BUFFER_TOO_SMALL) {

    if (*Buffer) {
      FreePool (*Buffer);
    }

    *Buffer = AllocatePool (BufferSize);

    if (*Buffer) {
      TryAgain = TRUE;
    } else {
      *Status = EFI_OUT_OF_RESOURCES;
    }
  }
  //
  // If there's an error, free the buffer
  //
  if (!TryAgain && EFI_ERROR (*Status) && *Buffer) {
    FreePool (*Buffer);
    *Buffer = NULL;
  }

  return TryAgain;
}
