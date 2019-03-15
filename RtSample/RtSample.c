/** @file

Sample driver for exposing capabilities through UEFI Runtime Driver

Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "RtSample.h"

//
// Global variables
//
EFI_GUID            mGetVarHookGuid = GET_VAR_HOOK_GUID;
EFI_GET_VARIABLE    mOrigGetVariable;
EFI_EVENT           mRtSampleVirtualAddressChangeEvent;
EFI_EVENT           mRtSampleEndOfDxeEvent;
UINT64              mMmioBase = MMIO_BASE_ADDRESS;

/**
  Notification function of EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE.

  This is a notification function registered on EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE event.
  It converts pointers to new virtual addresses in OS
  
  Addresses that are not automatically fixed up by PE/COFF image relocation need 
  to be handled here, e.g., MMIO address accessed by this driver, pointers to functions
  or memories outside of this driver, or pointers to dynamically allocated EfiRuntimeServicesData
  memories

  @param  Event        Event whose notification function is being invoked.
  @param  Context      Pointer to the notification function's context.
  
  @retval VOID

**/
VOID
EFIAPI
RtSampleFixVirtualAddresses (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS Status;
  //
  // Convert addresses used at Runtime
  //
  Status = gRT->ConvertPointer (0, (VOID **) &mMmioBase);
  ASSERT_EFI_ERROR (Status);
  Status = gRT->ConvertPointer (0, (VOID **) &mOrigGetVariable);
  ASSERT_EFI_ERROR (Status);
}

/**
  Calculate the 32-bit CRC in a EFI table using the service provided by the
  gRuntime service.

  @param  Hdr                    Pointer to an EFI standard header

**/
VOID
CalculateEfiHdrCrc (
  IN  OUT EFI_TABLE_HEADER    *Hdr
  )
{
  UINT32 Crc;

  Hdr->CRC32 = 0;

  //
  // If gBS->CalculateCrc32 () == CoreEfiNotAvailableYet () then
  //  Crc will come back as zero if we set it to zero here
  //
  Crc = 0;
  gBS->CalculateCrc32 ((UINT8 *)Hdr, Hdr->HeaderSize, &Crc);
  Hdr->CRC32 = Crc;
}

/**
  PRM Handler that is called by the GetVariable Wrapper function
  
  @param  DataSize          Pointer to the size of the parameters
  @param  Data              Pointer to parameters
  
  @retval EFI_SUCCESS       PRM Handler completes successfully
  @retval EFI Error Codes   Some error occurred

**/
EFI_STATUS
EFIAPI
RuntimeFunction (
  UINTN   *DataSize,
  VOID    *Data
  ) 
{
  UINT8 Buffer[512];
  UINTN StringLen;

  //
  // Use Spin Lock to serialize this function if the resources accessed 
  // do not tolerate reentrance
  //

  StringLen = AsciiSPrint (
                Buffer,
                512,
                "Runtime function output: Value at MmioAddress == 0x%lx\n", 
                (UINTN)*(UINT32*)(mMmioBase + R_REGISTER)
              );

  if (Data) {
    //
    // Output a message using the passed in OS debug message printing function
    //
    ((CORE_PRINT_FUNC)(UINTN)Data)(Buffer);
  }

  return EFI_SUCCESS;
}

/**
  GetVariable Wrapper function. This function calls PRM Handler if
  it receives a GET_VAR_HOOK_GUID; otherwise it calls the normal 
  GetVariable service. This function is called by the OS PRM Driver.

  @param[in]      VariableName       Name of Variable to be found.
  @param[in]      VendorGuid         Variable vendor GUID.
  @param[out]     Attributes         Attribute value of the variable found.
  @param[in, out] DataSize           Size of Data found. If size is less than the
                                     data, this value contains the required size.
  @param[out]     Data               Data pointer.
  
  @retval EFI_SUCCESS                GetVariable Wrapper completes successfully
  @retval EFI Error Codes            Some error occurred

**/
EFI_STATUS
EFIAPI
RtGetVariableWrapper (
  IN      CHAR16                            *VariableName,
  IN      EFI_GUID                          *VendorGuid,
  OUT     UINT32                            *Attributes OPTIONAL,
  IN OUT  UINTN                             *DataSize,
  OUT     VOID                              *Data
  )
{
  if (CompareGuid (VendorGuid, &mGetVarHookGuid)) {
    return RuntimeFunction (DataSize, Data);
  }
  return mOrigGetVariable (VariableName, VendorGuid, Attributes, DataSize, Data);
}

/**
  Notification function of EFI_END_OF_DXE_EVENT_GROUP_GUID event group.

  This is a notification function registered on EFI_END_OF_DXE_EVENT_GROUP_GUID event group.
  
  This function hooks the UEFI GetVariable service

  @param  Event        Event whose notification function is being invoked.
  @param  Context      Pointer to the notification function's context.

**/
VOID
EFIAPI
RtSampleEndOfDxeHandler (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  //
  // Hook GetVariable
  //
  mOrigGetVariable         = gRT->GetVariable; // make sure we retrieve the final variable service pointer
  gRT->GetVariable         = RtGetVariableWrapper;

  CalculateEfiHdrCrc (&gRT->Hdr);

  DEBUG ((DEBUG_ERROR, "GetVariable hooked ...\n"));  
}


/**
  Driver entry point

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       Variable service successfully initialized.

**/
EFI_STATUS
EFIAPI
RtSampleInit (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                      Status;
  UINT64                          BaseAddress;
  UINT64                          Length;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR MemorySpaceDescriptor;
  UINT64                          Attributes;

  //
  // Register EFI_END_OF_DXE_EVENT_GROUP_GUID event.
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  RtSampleEndOfDxeHandler,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &mRtSampleEndOfDxeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Create virtual address change event handler
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  RtSampleFixVirtualAddresses,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &mRtSampleVirtualAddressChangeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Set the GCD Attribute of mMmioBase to be Runtime
  //
  BaseAddress = mMmioBase;
  Length      = EFI_PAGE_SIZE;

  Status      = gDS->GetMemorySpaceDescriptor (BaseAddress, &MemorySpaceDescriptor);
  ASSERT_EFI_ERROR (Status);
  ASSERT (MemorySpaceDescriptor.Length >= Length);

  Attributes = MemorySpaceDescriptor.Attributes | EFI_MEMORY_RUNTIME;

  Status = gDS->SetMemorySpaceAttributes (
                  BaseAddress,
                  Length,
                  Attributes
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
