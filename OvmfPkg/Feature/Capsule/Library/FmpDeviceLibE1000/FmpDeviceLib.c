/**

Copyright (c) 2016, Microsoft Corporation. All rights reserved.<BR>
Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>

SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>

#include <Library/FmpDeviceLib.h>

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiLib.h>

#include <Protocol/DevicePath.h>
#include <Protocol/PciIo.h>

#include <IndustryStandard/Pci.h>

#define E1000_FMP_PAYLOAD_SIZE  sizeof (E1000_FMP_PAYLOAD)

typedef struct {
  UINT8   Data[0x18];
  UINT32  Version;
  UINT32  LowestSupportedVersion;
} E1000_FMP_PAYLOAD;

extern FMP_DEVICE_LIB_REGISTER_FMP_INSTALLER    mFmpDeviceLibRegisterFmpInstaller;
extern FMP_DEVICE_LIB_REGISTER_FMP_UNINSTALLER  mFmpDeviceLibRegisterFmpUninstaller;

typedef struct {
  EFI_HANDLE                Handle;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  UINT64                    HardwareInstance;
  CHAR16                    *PayloadVariableName;
} E1000_FMP_DEVICE_LIB_PRIVATE_CONTEXT;

E1000_FMP_DEVICE_LIB_PRIVATE_CONTEXT  *mCurrentE1000FmpDevice = NULL;

/**
  Used to pass the FMP install function to this lib.
  This allows the library to have control of the handle
  that the FMP instance is installed on.  This allows the library
  to use DriverBinding protocol model to locate its device(s) in the
  system.

  @param[in]  Function pointer to FMP install function.

  @retval EFI_SUCCESS      Library has saved function pointer and will call function pointer on each DriverBinding Start.
  @retval EFI_UNSUPPORTED  Library doesn't use driver binding and only supports a single instance.
  @retval other error      Error occurred.  Don't install FMP

**/
EFI_STATUS
EFIAPI
RegisterFmpInstaller (
  IN FMP_DEVICE_LIB_REGISTER_FMP_INSTALLER Function
  )
{
  mFmpDeviceLibRegisterFmpInstaller = Function;
  return EFI_SUCCESS;
}

/**
  Provide a function to uninstall the Firmware Management Protocol instance from a
  device handle when the device is managed by a driver that follows the UEFI
  Driver Model.  If the device is not managed by a driver that follows the UEFI
  Driver Model, then EFI_UNSUPPORTED is returned.

  @param[in] FmpUninstaller  Function that installs the Firmware Management
                             Protocol.

  @retval EFI_SUCCESS      The device is managed by a driver that follows the
                           UEFI Driver Model.  FmpUinstaller must be called on
                           each Driver Binding Stop().
  @retval EFI_UNSUPPORTED  The device is not managed by a driver that follows
                           the UEFI Driver Model.
  @retval other            The Firmware Management Protocol for this firmware
                           device is not installed.  The firmware device is
                           still locked using FmpDeviceLock().

**/
EFI_STATUS
EFIAPI
RegisterFmpUninstaller (
  IN FMP_DEVICE_LIB_REGISTER_FMP_UNINSTALLER  Function
  )
{
  mFmpDeviceLibRegisterFmpUninstaller = Function;
  return EFI_SUCCESS;
}

/**
  Set the device context for the FmpDeviceLib services when the device is
  managed by a driver that follows the UEFI Driver Model.  If the device is not
  managed by a driver that follows the UEFI Driver Model, then EFI_UNSUPPORTED
  is returned.  Once a device context is set, the FmpDeviceLib services
  operate on the currently set device context.

  @param[in]      Handle   Device handle for the FmpDeviceLib services.
                           If Handle is NULL, then Context is freed.
  @param[in, out] Context  Device context for the FmpDeviceLib services.
                           If Context is NULL, then a new context is allocated
                           for Handle and the current device context is set and
                           returned in Context.  If Context is not NULL, then
                           the current device context is set.

  @retval EFI_SUCCESS      The device is managed by a driver that follows the
                           UEFI Driver Model.
  @retval EFI_UNSUPPORTED  The device is not managed by a driver that follows
                           the UEFI Driver Model.
  @retval other            The Firmware Management Protocol for this firmware
                           device is not installed.  The firmware device is
                           still locked using FmpDeviceLock().

**/
EFI_STATUS
EFIAPI
FmpDeviceSetContext (
  IN EFI_HANDLE  Handle,
  OUT VOID       **Context
  )
{
  EFI_STATUS                            Status;
  E1000_FMP_DEVICE_LIB_PRIVATE_CONTEXT  *E1000Device;
  PCI_TYPE00                            PciHeader;

  if (Handle == NULL) {
    E1000Device = (E1000_FMP_DEVICE_LIB_PRIVATE_CONTEXT  *)(*Context);
    if (E1000Device != NULL) {
      if (E1000Device->PayloadVariableName != NULL) {
        FreePool (E1000Device->PayloadVariableName);
      }
      FreePool (E1000Device);
    }
    mCurrentE1000FmpDevice = NULL;
    return EFI_SUCCESS;
  }
  if (*Context == NULL) {
    E1000Device = (E1000_FMP_DEVICE_LIB_PRIVATE_CONTEXT  *)AllocatePool (sizeof (E1000_FMP_DEVICE_LIB_PRIVATE_CONTEXT));
    if (E1000Device == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    E1000Device->Handle = Handle;
    Status = gBS->OpenProtocol (
                    Handle,
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&E1000Device->PciIo,
                    NULL,
                    Handle,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (EFI_ERROR (Status)) {
      FreePool (E1000Device);
      return EFI_INVALID_PARAMETER;
    }
    Status = gBS->OpenProtocol (
                    Handle,
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&E1000Device->DevicePath,
                    NULL,
                    Handle,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (EFI_ERROR (Status)) {
      FreePool (E1000Device);
      return EFI_INVALID_PARAMETER;
    }

    //
    // Zero a PCI Header and fill in Vendor IDs and Device IDs.
    //
    ZeroMem (&PciHeader, sizeof (PciHeader));
    E1000Device->PciIo->Pci.Read (
                              E1000Device->PciIo,
                              EfiPciIoWidthUint16,
                              PCI_VENDOR_ID_OFFSET,
                              1,
                              &PciHeader.Hdr.VendorId
                              );
    E1000Device->PciIo->Pci.Read (
                              E1000Device->PciIo,
                              EfiPciIoWidthUint16,
                              PCI_DEVICE_ID_OFFSET,
                              1,
                              &PciHeader.Hdr.DeviceId
                              );
    E1000Device->PciIo->Pci.Read (
                              E1000Device->PciIo,
                              EfiPciIoWidthUint16,
                              PCI_SVID_OFFSET,
                              1,
                              &PciHeader.Device.SubsystemVendorID
                              );
    E1000Device->PciIo->Pci.Read (
                              E1000Device->PciIo,
                              EfiPciIoWidthUint16,
                              PCI_SID_OFFSET,
                              1,
                              &PciHeader.Device.SubsystemID
                              );
    //
    // Build 64-bit Hardware Instance from CRC32 of PciHeader and CRC32 of
    // the device path.  This provides a unique value based on the slot the
    // PCI device is located and the Vendor/Device IDs
    //
    E1000Device->HardwareInstance = (UINT64)CalculateCrc32 (&PciHeader, sizeof (PciHeader));
    E1000Device->HardwareInstance = LShiftU64 (E1000Device->HardwareInstance, 32);
    E1000Device->HardwareInstance = E1000Device->HardwareInstance |
                                    (UINT64)CalculateCrc32 (
                                      E1000Device->DevicePath,
                                      GetDevicePathSize (E1000Device->DevicePath)
                                      );

    E1000Device->PayloadVariableName = CatSPrint (NULL, L"Payload%016lx", E1000Device->HardwareInstance);

    *Context = E1000Device;
  }
  mCurrentE1000FmpDevice = (E1000_FMP_DEVICE_LIB_PRIVATE_CONTEXT *)(*Context);
  return EFI_SUCCESS;
}

/**
Used to get the size of the image in bytes.
NOTE - Do not return zero as that will identify the device as
not updatable.

@retval UINTN that represents the size of the firmware.

**/
EFI_STATUS
EFIAPI
FmpDeviceGetSize (
  IN UINTN  *Size
  )
{
  if (Size == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *Size = E1000_FMP_PAYLOAD_SIZE;
  return EFI_SUCCESS;
}

/**
Used to return a library supplied guid that will be the ImageTypeId guid of the FMP descriptor.
This is optional but can be used if at runtime the guid needs to be determined.

@param  Guid:  Double Guid Ptr that will be updated to point to guid.  This should be from static memory
and will not be freed.
@return EFI_UNSUPPORTED: if you library instance doesn't need dynamic guid return this.
@return Error: Any error will cause the wrapper to use the GUID defined by PCD
@return EFI_SUCCESS:  Guid ptr should be updated to point to static memeory which contains a valid guid
**/
EFI_STATUS
EFIAPI
FmpDeviceGetImageTypeIdGuidPtr (
  OUT EFI_GUID  **Guid
  )
{
  //this instance doesn't need dynamic guid detection.
  return EFI_UNSUPPORTED;
}

/**
  Returns values used to fill in the AttributesSupported and AttributesSettings
  fields of the EFI_FIRMWARE_IMAGE_DESCRIPTOR structure that is returned by the
  GetImageInfo() service of the Firmware Management Protocol.  The following
  bit values from the Firmware Management Protocol may be combined:
    IMAGE_ATTRIBUTE_IMAGE_UPDATABLE
    IMAGE_ATTRIBUTE_RESET_REQUIRED
    IMAGE_ATTRIBUTE_AUTHENTICATION_REQUIRED
    IMAGE_ATTRIBUTE_IN_USE
    IMAGE_ATTRIBUTE_UEFI_IMAGE

  @param[out] Supported  Attributes supported by this firmware device.
  @param[out] Setting    Attributes settings for this firmware device.

  @retval EFI_SUCCESS            The attributes supported by the firmware
                                 device were returned.
  @retval EFI_INVALID_PARAMETER  Supported is NULL.
  @retval EFI_INVALID_PARAMETER  Setting is NULL.

**/
EFI_STATUS
EFIAPI
FmpDeviceGetAttributes (
  IN OUT UINT64  *Supported,
  IN OUT UINT64  *Setting
  )
{
  if (Supported == NULL || Setting == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *Supported = (IMAGE_ATTRIBUTE_IMAGE_UPDATABLE         |
                IMAGE_ATTRIBUTE_RESET_REQUIRED          |
                IMAGE_ATTRIBUTE_AUTHENTICATION_REQUIRED |
                IMAGE_ATTRIBUTE_IN_USE
                );
  *Setting   = (IMAGE_ATTRIBUTE_IMAGE_UPDATABLE         |
                IMAGE_ATTRIBUTE_RESET_REQUIRED          |
                IMAGE_ATTRIBUTE_AUTHENTICATION_REQUIRED |
                IMAGE_ATTRIBUTE_IN_USE
                );
  return EFI_SUCCESS;
}

/**
Gets the current Lowest Supported Version.
This is a protection mechanism so that a previous version with known issue is not
applied.

ONLY implement this if your running firmware has a method to return this at runtime.

@param[out] Version           On return this value represents the
current Lowest Supported Version (in same format as GetVersion).

@retval EFI_SUCCESS           The Lowest Supported Version was correctly retrieved
@retval EFI_UNSUPPORTED       Device firmware doesn't support reporting LSV
@retval EFI_DEVICE_ERROR      Error occurred when trying to get the LSV
**/
EFI_STATUS
EFIAPI
FmpDeviceGetLowestSupportedVersion (
  IN OUT UINT32  *LowestSupportedVersion
  )
{
  EFI_STATUS         Status;
  UINTN              Size;
  E1000_FMP_PAYLOAD  *E1000FmpPayload;

  E1000FmpPayload         = NULL;
  Size                    = 0;
  *LowestSupportedVersion = 0;

  Status = GetVariable2 (
             mCurrentE1000FmpDevice->PayloadVariableName,
             &gEfiCallerIdGuid,
             (VOID **)&E1000FmpPayload,
             &Size
             );
  if (EFI_ERROR (Status) || (E1000FmpPayload == NULL)) {
    DEBUG ((DEBUG_WARN, "Warning: Failed to get the E1000 FMP Payload from variable.  Status = %r\n", Status));
  } else {
    //
    // No error from call
    //
    if (Size != E1000_FMP_PAYLOAD_SIZE) {
      //
      // Return default since size was unknown
      //
      DEBUG ((DEBUG_ERROR, "Getting E1000 FMP Payload Variable returned a size different than expected. Size = 0x%x\n", Size));
    } else {
      //
      // Successful read
      //
      *LowestSupportedVersion = E1000FmpPayload->LowestSupportedVersion;
    }
  }

  if (E1000FmpPayload != NULL) {
    FreePool (E1000FmpPayload);
  }

  return EFI_SUCCESS;
}

/**
  Returns the Null-terminated Unicode string that is used to fill in the
  VersionName field of the EFI_FIRMWARE_IMAGE_DESCRIPTOR structure that is
  returned by the GetImageInfo() service of the Firmware Management Protocol.
  The returned string must be allocated using EFI_BOOT_SERVICES.AllocatePool().

  @note It is recommended that all firmware devices support a method to report
        the VersionName string from the currently stored firmware image.

  @param[out] VersionString  The version string retrieved from the currently
                             stored firmware image.

  @retval EFI_SUCCESS            The version string of currently stored
                                 firmware image was returned in Version.
  @retval EFI_INVALID_PARAMETER  VersionString is NULL.
  @retval EFI_UNSUPPORTED        The firmware device does not support a method
                                 to report the version string of the currently
                                 stored firmware image.
  @retval EFI_DEVICE_ERROR       An error occurred attempting to retrieve the
                                 version string of the currently stored
                                 firmware image.
  @retval EFI_OUT_OF_RESOURCES   There are not enough resources to allocate the
                                 buffer for the version string of the currently
                                 stored firmware image.

**/
EFI_STATUS
EFIAPI
FmpDeviceGetVersionString (
  OUT CHAR16  **VersionString
  )
{
  EFI_STATUS  Status;
  UINT32      Version;

  if (VersionString == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  Status = FmpDeviceGetVersion (&Version);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  *VersionString = CatSPrint (NULL, L"E1000 Firmware Version %08x", Version);
  if (*VersionString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}

/**
Gets the current running version.
ONLY implement this if your running firmware has a method to return this at runtime.

@param[out] Version           On return this value represents the current running version

@retval EFI_SUCCESS           The version was correctly retrieved
@retval EFI_UNSUPPORTED       Device firmware doesn't support reporting current version
@retval EFI_DEVICE_ERROR      Error occurred when trying to get the version
**/
EFI_STATUS
EFIAPI
FmpDeviceGetVersion (
  IN OUT UINT32  *Version
  )
{
  EFI_STATUS         Status;
  UINTN              Size;
  E1000_FMP_PAYLOAD  *E1000FmpPayload;

  E1000FmpPayload = NULL;
  Size            = 0;
  *Version        = 0;

  Status = GetVariable2 (
             mCurrentE1000FmpDevice->PayloadVariableName,
             &gEfiCallerIdGuid,
             (VOID **)&E1000FmpPayload,
             &Size
             );
  if (EFI_ERROR (Status) || (E1000FmpPayload == NULL)) {
    DEBUG ((DEBUG_WARN, "Warning: Failed to get the E1000 FMP Payload from variable.  Status = %r\n", Status));
  } else {
    //
    // No error from call
    //
    if (Size != E1000_FMP_PAYLOAD_SIZE) {
      //
      // Return default since size was unknown
      //
      DEBUG ((DEBUG_ERROR, "Getting E1000 FMP Payload returned a size different than expected. Size = 0x%x\n", Size));
    } else {
      //
      // Successful read
      //
      *Version = E1000FmpPayload->Version;
    }
  }

  if (E1000FmpPayload != NULL) {
    FreePool (E1000FmpPayload);
  }

  return EFI_SUCCESS;
}

/**
  Returns the value used to fill in the HardwareInstance field of the
  EFI_FIRMWARE_IMAGE_DESCRIPTOR structure that is returned by the GetImageInfo()
  service of the Firmware Management Protocol.  If EFI_SUCCESS is returned, then
  the firmware device supports a method to report the HardwareInstance value.
  If the value can not be reported for the firmware device, then EFI_UNSUPPORTED
  must be returned.  EFI_DEVICE_ERROR is returned if an error occurs attempting
  to retrieve the HardwareInstance value for the firmware device.

  @param[out] HardwareInstance  The hardware instance value for the firmware
                                device.

  @retval EFI_SUCCESS       The hardware instance for the current firmware
                            devide is returned in HardwareInstance.
  @retval EFI_UNSUPPORTED   The firmware device does not support a method to
                            report the hardware instance value.
  @retval EFI_DEVICE_ERROR  An error occurred attempting to retrieve the hardware
                            instance value.

**/
EFI_STATUS
EFIAPI
FmpDeviceGetHardwareInstance (
  OUT UINT64  *HardwareInstance
  )
{
  if (HardwareInstance == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *HardwareInstance = mCurrentE1000FmpDevice->HardwareInstance;
  return EFI_SUCCESS;
}

/**
Retrieves a copy of the current firmware image of the device.

This function allows a copy of the current firmware image to be created and saved.
The saved copy could later been used, for example, in firmware image recovery or rollback.

@param[out] Image              Points to the buffer where the current image is copied to.
@param[out] ImageSize          On entry, points to the size of the buffer pointed to by Image, in bytes.
On return, points to the length of the image, in bytes.

@retval EFI_SUCCESS            The device was successfully updated with the new image.
@retval EFI_BUFFER_TOO_SMALL   The buffer specified by ImageSize is too small to hold the
image. The current buffer size needed to hold the image is returned
in ImageSize.
@retval EFI_INVALID_PARAMETER  The Image was NULL.
@retval EFI_NOT_FOUND          The current image is not copied to the buffer.
@retval EFI_UNSUPPORTED        The operation is not supported.

**/
EFI_STATUS
EFIAPI
FmpDeviceGetImage (
  IN  OUT  VOID   *Image,
  IN  OUT  UINTN  *ImageSize
  )
{
  EFI_STATUS  Status;
  UINTN       Size;
  VOID        *E1000FmpPayload;

  E1000FmpPayload = NULL;
  Size            = 0;

  Status = GetVariable2 (
             mCurrentE1000FmpDevice->PayloadVariableName,
             &gEfiCallerIdGuid,
             (VOID **)&E1000FmpPayload,
             &Size
             );
  if (EFI_ERROR (Status) || (E1000FmpPayload == NULL)) {
    DEBUG ((DEBUG_WARN, "Warning: Failed to get the E1000 FMP Payload from variable.  Status = %r\n", Status));
    if (!EFI_ERROR (Status)) {
      Status = EFI_NOT_FOUND;
    }
  } else {
    //
    // No error from call
    //
    if (Size != E1000_FMP_PAYLOAD_SIZE) {
      DEBUG ((DEBUG_ERROR, "Getting E1000 FMP Payload returned a size different than expected. Size = 0x%x\n", Size));
      Status = EFI_NOT_FOUND;
    } else {
      //
      // Successful read
      //
      if (*ImageSize < Size) {
        *ImageSize = Size;
        Status = EFI_BUFFER_TOO_SMALL;
      } else {
        *ImageSize = Size;
        CopyMem (Image, E1000FmpPayload, *ImageSize);
        Status = EFI_SUCCESS;
      }
    }
  }

  if (E1000FmpPayload != NULL) {
    FreePool (E1000FmpPayload);
  }

  return Status;
}

/**
Updates the firmware image of the device.

This function updates the hardware with the new firmware image.
This function returns EFI_UNSUPPORTED if the firmware image is not updatable.
If the firmware image is updatable, the function should perform the following minimal validations
before proceeding to do the firmware image update.
- Validate the image is a supported image for this device.  The function returns EFI_ABORTED if
the image is unsupported.  The function can optionally provide more detailed information on
why the image is not a supported image.
- Validate the data from VendorCode if not null.  Image validation must be performed before
VendorCode data validation.  VendorCode data is ignored or considered invalid if image
validation failed.  The function returns EFI_ABORTED if the data is invalid.

VendorCode enables vendor to implement vendor-specific firmware image update policy.  Null if
the caller did not specify the policy or use the default policy.  As an example, vendor can implement
a policy to allow an option to force a firmware image update when the abort reason is due to the new
firmware image version is older than the current firmware image version or bad image checksum.
Sensitive operations such as those wiping the entire firmware image and render the device to be
non-functional should be encoded in the image itself rather than passed with the VendorCode.
AbortReason enables vendor to have the option to provide a more detailed description of the abort
reason to the caller.

@param[in]  Image              Points to the new image.
@param[in]  ImageSize          Size of the new image in bytes.
@param[in]  VendorCode         This enables vendor to implement vendor-specific firmware image update policy.
Null indicates the caller did not specify the policy or use the default policy.
@param[in]  Progress           A function used by the driver to report the progress of the firmware update.
@param[in]  CapsuleFwVersion   FMP Payload Header version of the image
@param[out] AbortReason        A pointer to a pointer to a null-terminated string providing more
details for the aborted operation. The buffer is allocated by this function
with AllocatePool(), and it is the caller's responsibility to free it with a
call to FreePool().

@retval EFI_SUCCESS            The device was successfully updated with the new image.
@retval EFI_ABORTED            The operation is aborted.
@retval EFI_INVALID_PARAMETER  The Image was NULL.
@retval EFI_UNSUPPORTED        The operation is not supported.

**/
EFI_STATUS
EFIAPI
FmpDeviceSetImage (
  IN  CONST VOID                                     *Image,
  IN  UINTN                                          ImageSize,
  IN  CONST VOID                                     *VendorCode,
  IN  EFI_FIRMWARE_MANAGEMENT_UPDATE_IMAGE_PROGRESS  Progress,
  IN  UINT32                                         CapsuleFwVersion,
  OUT CHAR16                                         **AbortReason
  )
{
  EFI_STATUS  Status;
  UINT32      Updateable;

  Status     = EFI_SUCCESS;
  Updateable = 0;

  Status = FmpDeviceCheckImage (Image, ImageSize, &Updateable);
  if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, "SetImage - Check Image failed with %r.\n", Status));
      goto cleanup;
  }

  if (Updateable != IMAGE_UPDATABLE_VALID) {
      DEBUG((DEBUG_ERROR, "SetImage - Check Image returned that the Image was not valid for update.  Updatable value = 0x%X.\n", Updateable));
      Status = EFI_ABORTED;
      goto cleanup;
  }

  if (Progress == NULL) {
      DEBUG((DEBUG_ERROR, "SetImage - Invalid progress callback\n"));
      Status = EFI_INVALID_PARAMETER;
      goto cleanup;
  }

  Status = gRT->SetVariable (
                  mCurrentE1000FmpDevice->PayloadVariableName,
                  &gEfiCallerIdGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  ImageSize,
                  (VOID *)Image
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to set E1000 FMP Payload.  Status = %r\n", Status));
    Status = EFI_ABORTED;
    goto cleanup;
  }

  Status = Progress(15);
  if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, "SetImage - Progress Callback failed with Status %r.\n", Status));
  }

  {
    UINTN  p;

    for (p = 20; p < 100; p++) {
      gBS->Stall (10000);  //us  = 0.01 seconds
      Progress (p);
    }
  }

  //TODO: add support for VendorCode, and AbortReason
cleanup:
  return Status;
}

/**
Checks if the firmware image is valid for the device.

This function allows firmware update application to validate the firmware image without
invoking the SetImage() first.

@param[in]  Image              Points to the new image.
@param[in]  ImageSize          Size of the new image in bytes.
@param[out] ImageUpdatable     Indicates if the new image is valid for update. It also provides,
if available, additional information if the image is invalid.

@retval EFI_SUCCESS            The image was successfully checked.
@retval EFI_INVALID_PARAMETER  The Image was NULL.

**/
EFI_STATUS
EFIAPI
FmpDeviceCheckImage (
  IN  CONST VOID  *Image,
  IN  UINTN       ImageSize,
  OUT UINT32      *ImageUpdateable
  )
{
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;

  if (ImageUpdateable == NULL) {
      DEBUG((DEBUG_ERROR, "CheckImage - ImageUpdateable Pointer Parameter is NULL.\n"));
      Status = EFI_INVALID_PARAMETER;
      goto cleanup;
  }

  //
  //Set to valid and then if any tests fail it will update this flag.
  //
  *ImageUpdateable = IMAGE_UPDATABLE_VALID;

  if (Image == NULL) {
      DEBUG((DEBUG_ERROR, "CheckImage - Image Pointer Parameter is NULL.\n"));
      *ImageUpdateable = IMAGE_UPDATABLE_INVALID; //not sure if this is needed
      return EFI_INVALID_PARAMETER;
  }

  if (ImageSize != E1000_FMP_PAYLOAD_SIZE) {
      DEBUG((DEBUG_ERROR, "CheckImage - Image Size is incorrect.  ImageSize = 0x%x\n", ImageSize));
      *ImageUpdateable = IMAGE_UPDATABLE_INVALID; //not sure if this is needed
      return EFI_INVALID_PARAMETER;
  }

cleanup:
  return Status;
}

/**
Device firmware should trigger lock mechanism so that device fw can not be updated or tampered with.
This lock mechanism is generally only cleared by a full system reset (not just sleep state/low power mode)

@retval EFI_SUCCESS           The device was successfully locked.
@retval EFI_UNSUPPORTED       The hardware device/firmware doesn't support locking

**/
EFI_STATUS
EFIAPI
FmpDeviceLock (
  VOID
  )
{
  return EFI_SUCCESS;
}
