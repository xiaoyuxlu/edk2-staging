/** @file
  This is the driver that publishes the SMM Control Protocol that allows generating
  software SMIs

  Copyright (c) 2014 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include <PiDxe.h>

#include <Protocol/SmmControl2.h>

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/PlatformLib.h>

EFI_STATUS
EFIAPI
Activate (
  IN CONST EFI_SMM_CONTROL2_PROTOCOL     *This,
  IN OUT  UINT8                          *CommandPort       OPTIONAL,
  IN OUT  UINT8                          *DataPort          OPTIONAL,
  IN      BOOLEAN                        Periodic           OPTIONAL,
  IN      EFI_SMM_PERIOD                 ActivationInterval OPTIONAL
  );

/**
  Clears an SMI.

  @param  This      Pointer to an instance of EFI_SMM_CONTROL2_PROTOCOL
  @param  Periodic  TRUE to indicate a periodical SMI

  @return Return value from SmmClear()

**/
EFI_STATUS
EFIAPI
Deactivate (
  IN CONST     EFI_SMM_CONTROL2_PROTOCOL  *This,
  IN      BOOLEAN                         Periodic OPTIONAL
  );

///
/// Handle for the SMM Control2 Protocol
///
EFI_HANDLE  mSmmControl2Handle = NULL;

//
// SMM Control2 Protocol instance
//
EFI_SMM_CONTROL2_PROTOCOL mSmmControl2 = {
  Activate,
  Deactivate,
  0
};

//to-do: retrieve from PlatformLib
#define SMM_DATA_PORT       0xB3
#define SMM_ACTIVATION_PORT 0xB2

/**
  Invokes SMI activation from either the preboot or runtime environment.

  This function generates an SMI.

  @param[in]     This                The EFI_SMM_CONTROL2_PROTOCOL instance.
  @param[in,out] CommandPort         The value written to the command port.
  @param[in,out] DataPort            The value written to the data port.
  @param[in]     Periodic            Optional mechanism to engender a periodic stream.
  @param[in]     ActivationInterval  Optional parameter to repeat at this period one
                                     time or, if the Periodic Boolean is set, periodically.

  @retval EFI_SUCCESS            The SMI has been engendered.
  @retval EFI_INVALID_PARAMETER  Periodic generation of SMIs is not supported

**/
EFI_STATUS
EFIAPI
Activate (
  IN CONST EFI_SMM_CONTROL2_PROTOCOL     *This,
  IN OUT  UINT8                          *CommandPort       OPTIONAL,
  IN OUT  UINT8                          *DataPort          OPTIONAL,
  IN      BOOLEAN                        Periodic           OPTIONAL,
  IN      EFI_SMM_PERIOD                 ActivationInterval OPTIONAL
  )
{
  if (Periodic) {
    return EFI_INVALID_PARAMETER;
  }

  SwSmiInitHardware();

  //
  // Set APMC_STS
  //
  if (DataPort == NULL) {
    IoWrite8 (SMM_DATA_PORT, 0xFF);
  } else {
    IoWrite8 (SMM_DATA_PORT, *DataPort);
  }

  //
  // Generate the APMC SMI
  //
  if (CommandPort == NULL) {
    IoWrite8 (SMM_ACTIVATION_PORT, 0xFF);
  } else {
    IoWrite8 (SMM_ACTIVATION_PORT, *CommandPort);
  }

  return EFI_SUCCESS;
}


/**
  Clears any system state that was created in response to the Trigger() call.

  This function acknowledges and causes the deassertion of the SMI activation source.

  @param[in] This                The EFI_SMM_CONTROL2_PROTOCOL instance.
  @param[in] Periodic            Optional mechanism to engender a periodic stream.

  @retval EFI_SUCCESS            The SMI has been engendered.
  @retval EFI_INVALID_PARAMETER  Periodic generation of SMIs is not supported.

**/
EFI_STATUS
EFIAPI
Deactivate (
  IN CONST EFI_SMM_CONTROL2_PROTOCOL     *This,
  IN       BOOLEAN                       Periodic
  )
{
  if (Periodic) {
    return EFI_INVALID_PARAMETER;
  }
  
  //
  // Do nothing here currently
  //
  return EFI_SUCCESS;
}

/**
  This is the constructor for the SMM Control protocol.

  This function installs EFI_SMM_CONTROL2_PROTOCOL.

  @param  ImageHandle Handle for the image of this driver
  @param  SystemTable Pointer to the EFI System Table

  @return The status returned from InstallProtocolInterface().

--*/
EFI_STATUS
EFIAPI
SmmControl2Init (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  //
  // Install our protocol interfaces on the device's handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mSmmControl2Handle,
                  &gEfiSmmControl2ProtocolGuid,  &mSmmControl2,
                  NULL
                  );

  return Status;
}
