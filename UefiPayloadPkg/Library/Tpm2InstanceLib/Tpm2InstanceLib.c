/** @file
  TPM2 Lib instance that calls Platform Lib for platform specific operations

  Copyright (c) 2012 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <IndustryStandard/Tpm12.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/Tpm2DeviceLib.h>
#include <Library/PciLib.h>
#include <Library/PlatformLib.h>

extern EFI_GUID  gPayloadTpm2DeviceInstanceGuid;

/**
  This service enables the sending of commands to the TPM2.

  @param[in]  InputParameterBlockSize  Size of the TPM2 input parameter block.
  @param[in]  InputParameterBlock      Pointer to the TPM2 input parameter block.
  @param[in]  OutputParameterBlockSize Size of the TPM2 output parameter block.
  @param[in]  OutputParameterBlock     Pointer to the TPM2 output parameter block.

  @retval     EFI_SUCCESS              The command byte stream was successfully sent to the device and a response was successfully received.
  @retval     EFI_DEVICE_ERROR         The command was not successfully sent to the device or a response was not successfully received from the device.
  @retval     EFI_BUFFER_TOO_SMALL     The output parameter block is too small.

**/
EFI_STATUS
EFIAPI
Tpm2InstanceSubmitCommand (
  IN UINT32            InputParameterBlockSize,
  IN UINT8             *InputParameterBlock,
  IN OUT UINT32        *OutputParameterBlockSize,
  IN UINT8             *OutputParameterBlock
  )
{
  return Tpm2PlatformSubmitCommand (InputParameterBlock, InputParameterBlockSize, OutputParameterBlock, OutputParameterBlockSize);
}

EFI_STATUS
EFIAPI
Tpm2InstanceRequestUseTpm (
  VOID
  )
{
  return Tpm2PlatformRequestUseTpm();
}

TPM2_DEVICE_INTERFACE  mTpm2DeviceInstance = {
  { 0x0, 0x0, 0x0, { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 } },
  Tpm2InstanceSubmitCommand,
  Tpm2InstanceRequestUseTpm,
};

EFI_STATUS
EFIAPI
Tpm2InstanceLibConstructor (
  VOID
  )
{
  EFI_STATUS  Status;

  //
  // Set up the GUID of Tmp2 Device Instance
  //
  CopyGuid (&mTpm2DeviceInstance.ProviderGuid, &gPayloadTpm2DeviceInstanceGuid);

  PlatformLibInitializeTpm ();

  Status = Tpm2RegisterTpm2DeviceLib (&mTpm2DeviceInstance);
  if (Status == EFI_SUCCESS) {
  } else if (Status == EFI_UNSUPPORTED){
    //
    // Unsupported means platform policy does not need this instance enabled.
    //
    Status = EFI_SUCCESS;
 }
  return Status;
}
