/** @file
Platform Library APIs used by UEFI Payload

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef __PLATFORM_LIB_H__
#define __PLATFORM_LIB_H__

/**
  Platform specific initialization for TPM functions

  @retval EFI_SUCCESS           Initialization succeeded
  @retval EFI_DEVICE_ERROR      Some hardware error occurred
  @retval EFI_OUT_OF_RESOURCES  There are not enough resources to complete initialization

**/
EFI_STATUS
EFIAPI
PlatformLibInitializeTpm (
  VOID
  );

/**
  Platform specific initialization for variable services

  @retval EFI_SUCCESS           Initialization succeeded
  @retval EFI_DEVICE_ERROR      Some hardware error occurred
  @retval EFI_OUT_OF_RESOURCES  There are not enough resources to complete initialization

**/
EFI_STATUS
EFIAPI
PlatformLibInitializeVariable (
  VOID
  );

/**
  Platform specific tasks to be completed at the End of DXE event 

  @retval EFI_SUCCESS           Initialization succeeded
  @retval EFI_DEVICE_ERROR      Some hardware error occurred
  @retval EFI_OUT_OF_RESOURCES  There are not enough resources to complete the tasks

**/
EFI_STATUS
EFIAPI
PlatformLibEndOfDxeHook (
  VOID
  );

/**
  Platform specific tasks to be completed in System Management Mode at the End of DXE event 

  @retval EFI_SUCCESS           Initialization succeeded
  @retval EFI_DEVICE_ERROR      Some hardware error occurred
  @retval EFI_OUT_OF_RESOURCES  There are not enough resources to complete the tasks

**/
EFI_STATUS
EFIAPI
PlatformLibEndOfDxeHookSmm (
  VOID
  );

/**
  Platform specific tasks to be completed at the Ready to Boot event 

  @retval EFI_SUCCESS           Initialization succeeded
  @retval EFI_DEVICE_ERROR      Some hardware error occurred
  @retval EFI_OUT_OF_RESOURCES  There are not enough resources to complete the tasks

**/
EFI_STATUS
EFIAPI
PlatformLibReadyToBootHook (
  VOID
  );


/**
  Platform specific tasks to be completed at the end of boot (i.e., at the Exit Boot Services
  event)

  @retval EFI_SUCCESS           Initialization succeeded
  @retval EFI_DEVICE_ERROR      Some hardware error occurred
  @retval EFI_OUT_OF_RESOURCES  There are not enough resources to complete the tasks

**/
EFI_STATUS
EFIAPI
PlatformLibEndOfBootHook (
  VOID
  );

/**
  Platform specific tasks to be completed on each CPU thread at the end of boot
  (i.e., at the Exit Boot Services event)

  @retval EFI_SUCCESS           Initialization succeeded
  @retval EFI_DEVICE_ERROR      Some hardware error occurred
  @retval EFI_OUT_OF_RESOURCES  There are not enough resources to complete the tasks

**/
EFI_STATUS
EFIAPI
PlatformLibEndOfBootHookMp (
  VOID
  );

/**
  Platform specific tasks to be completed in System Management Mode at the end of boot
  (i.e., at the Exit Boot Services event)

  @retval EFI_SUCCESS           Initialization succeeded
  @retval EFI_DEVICE_ERROR      Some hardware error occurred
  @retval EFI_OUT_OF_RESOURCES  There are not enough resources to complete the tasks

**/
EFI_STATUS
EFIAPI
PlatformLibEndOfBootHookSmm (
  VOID
  );


/**
  Set a Variable's content (Volatile or Non-Volatile).

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode, and datasize and data are external input.
  This function will do basic validation, before parse the data.
  This function will parse the authentication carefully to avoid security issues, like
  buffer overflow, integer overflow.
  This function will check attribute carefully to avoid authentication bypass.

  @param[in] VariableName                     Name of Variable to be found.
  @param[in] VendorGuid                       Variable vendor GUID.
  @param[in] Attributes                       Attribute value of the variable found
  @param[in] DataSize                         Size of Data found. If size is less than the
                                              data, this value contains the required size.
  @param[in] Data                             Data pointer.

  @return    EFI_INVALID_PARAMETER            Invalid parameter.
  @return    EFI_SUCCESS                      Set successfully.
  @return    EFI_OUT_OF_RESOURCES             Resource not enough to set variable.
  @return    EFI_NOT_FOUND                    Not found.
  @return    EFI_WRITE_PROTECTED              Variable is read-only.

**/
EFI_STATUS
EFIAPI
VariableServiceSetVariable (
  IN CHAR16                  *VariableName,
  IN EFI_GUID                *VendorGuid,
  IN UINT32                  Attributes,
  IN UINTN                   DataSize,
  IN VOID                    *Data
  );

/**
  Get a variable's content (Volatile or Non-Volatile).

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode, and datasize and data are external input.
  This function will do basic validation, before parse the data.

  @param[in]      VariableName               Name of Variable to be found.
  @param[in]      VendorGuid                 Variable vendor GUID.
  @param[out]     Attributes                 Attribute value of the variable found.
  @param[in, out] DataSize                   Size of Data found. If size is less than the
                                             data, this value contains the required size.
  @param[out]     Data                       Data pointer.

  @return         EFI_INVALID_PARAMETER      Invalid parameter.
  @return         EFI_SUCCESS                Find the specified variable.
  @return         EFI_NOT_FOUND              Not found.
  @return         EFI_BUFFER_TO_SMALL        DataSize is too small for the result.

**/
EFI_STATUS
EFIAPI
VariableServiceGetVariable (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          *VendorGuid,
  OUT     UINT32            *Attributes OPTIONAL,
  IN OUT  UINTN             *DataSize,
  OUT     VOID              *Data
  );

/**

  Find the next available variable.

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode. This function will do basic validation, before parse the data.

  @param[in, out] VariableNameSize           Size of the variable name.
  @param[in, out] VariableName               Pointer to variable name.
  @param[in, out] VendorGuid                 Variable Vendor Guid.

  @return         EFI_INVALID_PARAMETER      Invalid parameter.
  @return         EFI_SUCCESS                Find the specified variable.
  @return         EFI_NOT_FOUND              Not found.
  @return         EFI_BUFFER_TO_SMALL        DataSize is too small for the result.

**/
EFI_STATUS
EFIAPI
VariableServiceGetNextVariableName (
  IN OUT  UINTN             *VariableNameSize,
  IN OUT  CHAR16            *VariableName,
  IN OUT  EFI_GUID          *VendorGuid
  );

/**
  Return information about the UEFI variables.

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode. This function will do basic validation, before parse the data.

  @param[in] Attributes                     Attributes bitmask to specify the type of variables
                                            on which to return information.
  @param[out] MaximumVariableStorageSize    Pointer to the maximum size of the storage space available
                                            for the EFI variables associated with the attributes specified.
  @param[out] RemainingVariableStorageSize  Pointer to the remaining size of the storage space available
                                            for EFI variables associated with the attributes specified.
  @param[out] MaximumVariableSize           Pointer to the maximum size of an individual EFI variables
                                            associated with the attributes specified.

  @return     EFI_INVALID_PARAMETER         An invalid combination of attribute bits was supplied.
  @return     EFI_SUCCESS                   Query successfully.
  @return     EFI_UNSUPPORTED               The attribute is not supported on this platform.

**/
EFI_STATUS
EFIAPI
VariableServiceQueryVariableInfo (
  IN  UINT32                 Attributes,
  OUT UINT64                 *MaximumVariableStorageSize,
  OUT UINT64                 *RemainingVariableStorageSize,
  OUT UINT64                 *MaximumVariableSize
  );

/**
  Get the size of implementation specific variable header

  @return Size of variable header in bytes in type UINTN.

**/
UINTN
GetVariableHeaderSize (
  VOID
  );

/**
  Get maxim size of a non-volatile Variable.

  @return Non-volatile maximum variable size.

**/
UINTN
GetNonVolatileMaxVariableSize (
  VOID
  );

/**
  Initialize variable quota at End Of Dxe

**/
VOID
InitializeVariableQuota (
  VOID
  );

/**
  This function reclaims variable storage if free space size is below the threshold for OS

**/
VOID
ReclaimForOS (
  VOID
  );

/**
  Mark a variable that will become read-only after leaving the DXE phase of execution.

  @param[in] VariableName          A pointer to the variable name that will be made read-only subsequently.
  @param[in] VendorGuid            A pointer to the vendor GUID that will be made read-only subsequently.

  @retval    EFI_SUCCESS           The variable specified by the VariableName and the VendorGuid was marked
                                   as pending to be read-only.
  @retval    EFI_INVALID_PARAMETER VariableName or VendorGuid is NULL.
                                   Or VariableName is an empty string.
  @retval    EFI_ACCESS_DENIED     EFI_END_OF_DXE_EVENT_GROUP_GUID or EFI_EVENT_GROUP_READY_TO_BOOT has
                                   already been signaled.
  @retval    EFI_OUT_OF_RESOURCES  There is not enough resource to hold the lock request.

**/
EFI_STATUS
EFIAPI
VariableLockRequestToLock (
  IN       CHAR16                       *VariableName,
  IN       EFI_GUID                     *VendorGuid
  );


/**
  Returns the ACPI Power Management Base I/O address

  @retval ACPI Power Management Base I/O address

--*/
UINT16
EFIAPI
GetAcpiBaseAddress (
  VOID
  );

/*
  This function returns the location (register & bit) for getting
  software SMI status

  @param[out]  AcpiIoOffset     The register address relative to ACPI bases
  @param[out]  SizeInBytes      The size of the register
  @param[out]  Bit              The bit position within the register

  @retval      EFI_SUCCESS      The location is successfully returned
  @retval      EFI_DEVICE_ERROR Some hardware error occurred

*/
EFI_STATUS
EFIAPI
GetSwSmiStatusBit (
  OUT UINT16 *AcpiIoOffset,
  OUT UINT8  *SizeInBytes,
  OUT UINT8  *Bit
  );

/**
  This function gets the SMRAM info.
  to-do: Use an array to support multiple SMRAM ranges?
  to-do: Get SMRAM info through previous stage firmware

  @param[out]  Base   The base address of SMRAM
  @param[out]  Size   The size of the SMRAM

  @retval      EFI_SUCCESS      SMRAM info is successfully returned
  @retval      EFI_DEVICE_ERROR Some hardware error occurred

--*/
EFI_STATUS
EFIAPI
GetSmramInfo (
  OUT UINTN *Base,
  OUT UINTN *Size
  );

/**
  Locks SMRAM such that the SMRAM can no longer be opened for access by non SMM
  code.

  @retval EFI_SUCCESS           Locking was successful
  @retval EFI_DEVICE_ERROR      Some hardware error occurred
  @retval EFI_OUT_OF_RESOURCES  There are not enough resources to complete locking

**/
EFI_STATUS
EFIAPI
LockSmram (
  VOID
  );

/**
  Initialize platform hardware for SMM

  @retval EFI_SUCCESS           Initialization completed.
  @retval EFI_DEVICE_ERROR      Some hardware error occurred
  @retval EFI_OUT_OF_RESOURCES  There are not enough resources to complete initialization

**/
EFI_STATUS
EFIAPI
SwSmiInitHardware (
  VOID
  );

/**
  Set the SMI EOS bit after all SMI sources have been processed. This is done
  at the end of handling the SMI

  @retval FALSE                   EOS was not set to a 1; this is an error
  @retval TRUE                    EOS was correctly set to a 1

**/
BOOLEAN
EFIAPI
SwSmiSetAndCheckEos (
  VOID
  );


/**
  Determine whether an ACPI OS is present (via the SCI_EN bit)

  @retval TRUE                    ACPI OS is present
  @retval FALSE                   ACPI OS is not present

**/
BOOLEAN
EFIAPI
IsSciEnabled (
  VOID
  );

/**
  Specific programming done before exiting an System Management Interrupt

**/
VOID
EFIAPI
BeforeExitSmi (
  VOID
  );

/**
  Sends formatted command to TPM for execution and returns formatted response data.

  @param[in]  InputBuffer       Buffer for the input data.
  @param[in]  InputBufferSize   Size of the input buffer.
  @param[out] ReturnBuffer      Buffer for the output data.
  @param[out] ReturnBufferSize  Size of the output buffer.

  @retval     EFI_SUCCESS       Operation completed successfully.
  @retval     EFI_TIMEOUT       The register can't run into the expected status in time.

**/
EFI_STATUS
EFIAPI
Tpm2PlatformSubmitCommand (
  IN      UINT8     *InputBuffer,
  IN      UINT32     InputBufferSize,
  OUT     UINT8     *ReturnBuffer,
  OUT     UINT32    *ReturnBufferSize
  );

/**
  Requests to use TPM2.

  @retval EFI_SUCCESS      Get the control of TPM2 chip.
  @retval EFI_NOT_FOUND    TPM2 not found.
  @retval EFI_DEVICE_ERROR Unexpected device behavior.
**/
EFI_STATUS
EFIAPI
Tpm2PlatformRequestUseTpm (
  VOID
  );

#endif