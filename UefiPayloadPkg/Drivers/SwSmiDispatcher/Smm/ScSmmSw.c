/** @file
  SMI context handling for the Software SMI dispatch driver.

  Copyright (c) 2012 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScSmmHelpers.h"
#include <Protocol/SmmCpu.h>

//
// to-do: retrieve from PlatformLib
//
#define R_APM_STS           0xB3
#define R_APM_CNT           0xB2

//
// There is only one instance for SwCommBuffer.
// It's safe in SMM since there is no re-entry for the function.
//
EFI_SMM_SW_CONTEXT            mScSwCommBuffer;
EFI_SMM_CPU_PROTOCOL          *mSmmCpuProtocol;
SC_SMM_SOURCE_DESC            mSwSmiSrcDescriptor = {
  SC_SMM_NO_FLAGS,
  {
    NULL_BIT_DESC_INITIALIZER,    
    NULL_BIT_DESC_INITIALIZER
  },
  {
    NULL_BIT_DESC_INITIALIZER
  }
};

/**
  Get the Software Smi value

  @param[in] Record               No use
  @param[in] Context              The context that includes Software Smi value to be filled

  @retval    None

**/
VOID
EFIAPI
SwGetContext (
  IN  DATABASE_RECORD    *Record,
  OUT SC_SMM_CONTEXT     *Context
  )
{
  UINT8 ApmCnt;

  ApmCnt                      = IoRead8 ((UINTN) R_APM_CNT);

  Context->Sw.SwSmiInputValue = ApmCnt;
}


/**
  Check whether software SMI value of two contexts match

  @param[in] Context1             Context 1 that includes software SMI value 1
  @param[in] Context2             Context 2 that includes software SMI value 2

  @retval    FALSE                Software SMI value match
  @retval    TRUE                 Software SMI value don't match

**/
BOOLEAN
EFIAPI
SwCmpContext (
  IN SC_SMM_CONTEXT              *Context1,
  IN SC_SMM_CONTEXT              *Context2
  )
{
  return (BOOLEAN) (Context1->Sw.SwSmiInputValue == Context2->Sw.SwSmiInputValue);
}


/**
  Gather the CommBuffer information of SmmSwDispatch2.

  @param[in]  Record              No use
  @param[out] CommBuffer          Point to the CommBuffer structure
  @param[out] CommBufferSize      Point to the Size of CommBuffer structure

**/
VOID
EFIAPI
SwGetCommBuffer (
  IN  DATABASE_RECORD    *Record,
  OUT VOID               **CommBuffer,
  OUT UINTN              *CommBufferSize
  )
{
  EFI_STATUS                            Status;
  EFI_SMM_SAVE_STATE_IO_INFO            SmiIoInfo;
  UINTN                                 Index;

  ASSERT (Record->ProtocolType == SwType);

  mScSwCommBuffer.CommandPort = IoRead8 (R_APM_CNT);
  mScSwCommBuffer.DataPort    = IoRead8 (R_APM_STS);

  //
  // Try to find which CPU trigger SWSMI
  //
  mScSwCommBuffer.SwSmiCpuIndex = 0;
  for (Index = 0; Index < gSmst->NumberOfCpus; Index++) {
    Status = mSmmCpuProtocol->ReadSaveState (
                                mSmmCpuProtocol,
                                sizeof (EFI_SMM_SAVE_STATE_IO_INFO),
                                EFI_SMM_SAVE_STATE_REGISTER_IO,
                                Index,
                                &SmiIoInfo
                                );
    if (EFI_ERROR (Status)) {
      continue;
    }
    if (SmiIoInfo.IoPort == R_APM_CNT) {
      //
      // Find matched CPU.
      //
      mScSwCommBuffer.SwSmiCpuIndex = Index;
      break;
    }
  }

  //
  // Return the CommBuffer
  //
  *CommBuffer = (VOID *) &mScSwCommBuffer;
  *CommBufferSize  = sizeof (EFI_SMM_SW_CONTEXT);
}


/**
  Initialize Software SMI Dispatch protocol.

**/
VOID
ScSwDispatchInit (
  VOID
  )
{
  EFI_STATUS    Status;
  UINT16        StsRegAcpiOffset;
  UINT8         StsRegSizeInBytes;
  UINT8         StsBit;

  //
  // Locate PI SMM CPU protocol
  //
  Status = gSmst->SmmLocateProtocol (&gEfiSmmCpuProtocolGuid, NULL, (VOID **) &mSmmCpuProtocol);
  ASSERT_EFI_ERROR (Status);
  
  //
  // Populate mSwSmiSrcDescriptor with SW SMI Status Register and Bit Info
  //
  Status = GetSwSmiStatusBit (
    &StsRegAcpiOffset,
    &StsRegSizeInBytes,
    &StsBit
    );
  ASSERT_EFI_ERROR (Status);
  mSwSmiSrcDescriptor.Sts[0].Reg.Type = ACPI_ADDR_TYPE;
  mSwSmiSrcDescriptor.Sts[0].Reg.Data.acpi = StsRegAcpiOffset;
  mSwSmiSrcDescriptor.Sts[0].SizeInBytes = StsRegSizeInBytes;
  mSwSmiSrcDescriptor.Sts[0].Bit = StsBit;
  
  return;
}

