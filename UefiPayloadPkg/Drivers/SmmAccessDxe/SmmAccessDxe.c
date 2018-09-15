/** @file
  This is the driver that publishes the SMM Access Protocol.

  Copyright (c) 2014 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>

#include <Protocol/SmmAccess2.h>
#include <Protocol/SmmControl2.h>
#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PlatformLib.h>
#include "SmmAccessDxe.h"
//
// Module variable hold the instance of SMM_ACCESS_PRIVATE_DATA
// 
static SMM_ACCESS_PRIVATE_DATA  mSmmAccess;

/**
  This is the constructor for the SMM Access protocol.

  This function installs EFI_SMM_ACCESS_PROTOCOL.

  @param  ImageHandle Handle for the image of this driver
  @param  SystemTable Pointer to the EFI System Table

  @return The status returned from InstallProtocolInterface().

--*/

EFI_STATUS
EFIAPI
SmmAccessDriverEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                      Status;
  UINTN                           SmramBase;
  UINTN                           SmramSize;

  //
  // Initialize Global variables
  //
  ZeroMem (&mSmmAccess, sizeof (mSmmAccess));
  mSmmAccess.Handle             = NULL;
  
  //
  // Allocate buffer for all the SMRAM descriptors
  // Here, we allocate only one SMM descriptor
  //
  mSmmAccess.SmramDesc = AllocateZeroPool (sizeof (EFI_SMRAM_DESCRIPTOR));
  ASSERT (mSmmAccess.SmramDesc != NULL);

  //
  // Fill the SMRAM descriptor
  //
  Status = GetSmramInfo (&SmramBase, &SmramSize);
  ASSERT_EFI_ERROR(Status);
  if (Status != EFI_SUCCESS) {
    return EFI_UNSUPPORTED;
  }
  mSmmAccess.SmramDesc->PhysicalStart = SmramBase;
  mSmmAccess.SmramDesc->CpuStart      = SmramBase;
  mSmmAccess.SmramDesc->PhysicalSize  = SmramSize;
  mSmmAccess.SmramDesc->RegionState   = EFI_SMRAM_CLOSED | EFI_CACHEABLE;
  
  mSmmAccess.NumberRegions               = 1;
  mSmmAccess.SmmAccess.Open              = Open;
  mSmmAccess.SmmAccess.Close             = Close;
  mSmmAccess.SmmAccess.Lock              = Lock;
  mSmmAccess.SmmAccess.GetCapabilities   = GetCapabilities;
  mSmmAccess.SmmAccess.LockState         = FALSE;
  mSmmAccess.SmmAccess.OpenState         = FALSE;
  mSmmAccess.SMMRegionState              = EFI_SMRAM_CLOSED;

  //
  // Install our protocol interfaces on the device's handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mSmmAccess.Handle,
                  &gEfiSmmAccess2ProtocolGuid, &mSmmAccess.SmmAccess,
                  NULL
                  );
                  
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Failed to install SmmAccess2 protocol ,returned status %r\n", Status));
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
   This routine accepts a request to "open" a region of SMRAM.
   The use of "open" means that the memory is visible from all boot-service
   and SMM agents.

   @param This                    Pointer to the SMM Access Interface.
   
   @retval EFI_SUCCESS            The region was successfully opened.
   @retval EFI_DEVICE_ERROR       The region could not be opened because locked by chipset.
   @retval EFI_INVALID_PARAMETER  The descriptor index was out of bounds.

**/
EFI_STATUS
EFIAPI
Open (
  IN EFI_SMM_ACCESS2_PROTOCOL  *This
  )
{
  DEBUG ((EFI_D_WARN, "Open SMRAM Region\n"));
  if (mSmmAccess.SMMRegionState & EFI_SMRAM_LOCKED) {
    //
    // Cannot open a "locked" region
    //
    DEBUG ((EFI_D_WARN, "Cannot open the locked SMRAM Region\n"));
    return EFI_DEVICE_ERROR;
  }

  mSmmAccess.SMMRegionState &= ~(UINT64)(UINT32)(EFI_SMRAM_CLOSED | EFI_ALLOCATED);
  SyncRegionState2SmramDesc(FALSE, (UINT64)(UINTN)(~(EFI_SMRAM_CLOSED | EFI_ALLOCATED)));

  mSmmAccess.SMMRegionState |= EFI_SMRAM_OPEN;
  SyncRegionState2SmramDesc(TRUE, EFI_SMRAM_OPEN);
  mSmmAccess.SmmAccess.OpenState = TRUE;

  return EFI_SUCCESS;

}

/**
   This routine accepts a request to "close" a region of SMRAM.
   The use of "close" means that the memory is only visible from SMM agents,
   not from BS or RT code.

   @param This                      Pointer to the SMM Access Interface.
   
   @retval EFI_SUCCESS              The region was successfully closed.
   @retval EFI_DEVICE_ERROR         The region could not be closed because locked by
                                    chipset.
   @retval EFI_INVALID_PARAMETER    The descriptor index was out of bounds.

**/
EFI_STATUS
EFIAPI
Close (
  IN EFI_SMM_ACCESS2_PROTOCOL    *This
  )
{

  DEBUG ((EFI_D_WARN, "Prepare to close the SMRAM\n"));

  if (mSmmAccess.SMMRegionState & EFI_SMRAM_LOCKED) {
    //
    // Cannot close a "locked" region
    //
    DEBUG ((EFI_D_WARN, "Cannot close the locked SMRAM Region\n"));
    return EFI_DEVICE_ERROR;
  }

  if (mSmmAccess.SMMRegionState & EFI_SMRAM_CLOSED) {
    return EFI_DEVICE_ERROR;
  }
    
  mSmmAccess.SMMRegionState &= ~(UINT64)(UINT32)EFI_SMRAM_OPEN;
  SyncRegionState2SmramDesc(FALSE, (UINT64)(UINTN)(~EFI_SMRAM_OPEN)); 
  mSmmAccess.SMMRegionState |= (UINT64)(UINT32)(EFI_SMRAM_CLOSED | EFI_ALLOCATED);
  SyncRegionState2SmramDesc(TRUE, EFI_SMRAM_CLOSED | EFI_ALLOCATED); 
  mSmmAccess.SmmAccess.OpenState = FALSE;
  DEBUG ((EFI_D_WARN, "Close SMRAM Region, success!\n"));
  
  return EFI_SUCCESS;

}

/**
   This routine accepts a request to "lock" SMRAM.
   The use of "lock" means that the memory can no longer be opened
   to BS state.

   @param This                     Pointer to the SMM Access Interface.
   
   @retval EFI_SUCCESS             The region was successfully locked.
   @retval EFI_DEVICE_ERROR        The region could not be locked because at least
                                   one range is still open.
   @retval EFI_INVALID_PARAMETER   The descriptor index was out of bounds.

**/
EFI_STATUS
EFIAPI
Lock (
  IN EFI_SMM_ACCESS2_PROTOCOL    *This
  )
{
  EFI_STATUS Status;

  DEBUG ((EFI_D_WARN, "Prepare to lock SMRAM Region\n"));
  
  if (mSmmAccess.SmmAccess.OpenState) {
    DEBUG ((EFI_D_WARN, "Cannot lock SMRAM when SMRAM regions are still open\n"));
    return EFI_DEVICE_ERROR;
  }

  Status = LockSmram();
  
  mSmmAccess.SMMRegionState |= EFI_SMRAM_LOCKED;
  SyncRegionState2SmramDesc(TRUE, EFI_SMRAM_LOCKED); 
  mSmmAccess.SmmAccess.LockState = TRUE;
  DEBUG ((EFI_D_WARN, "Locking SMRAM returns %r\n", Status));
  return Status;
}

/**
   This routine services a user request to discover the SMRAM
   capabilities of this platform.  This will report the possible
   ranges that are possible for SMRAM access, based upon the
   memory controller capabilities.

   @param This            Pointer to the SMRAM Access Interface.
   @param SmramMapSize    Pointer to the variable containing size of the
                          buffer to contain the description information.
   @param SmramMap        Buffer containing the data describing the Smram
                          region descriptors.
   
   @retval EFI_BUFFER_TOO_SMALL  The user did not provide a sufficient buffer.
   @retval EFI_SUCCESS           The user provided a sufficiently-sized buffer.

**/
EFI_STATUS
EFIAPI
GetCapabilities (
  IN CONST EFI_SMM_ACCESS2_PROTOCOL     *This,
  IN OUT   UINTN                       *SmramMapSize,
  IN OUT   EFI_SMRAM_DESCRIPTOR        *SmramMap
  )
{
  EFI_STATUS                Status;
  UINTN                     NecessaryBufferSize;

  NecessaryBufferSize = mSmmAccess.NumberRegions * sizeof(EFI_SMRAM_DESCRIPTOR);

  if (*SmramMapSize < NecessaryBufferSize) {
    DEBUG ((EFI_D_WARN, "SMRAM Map Buffer too small\n"));
    Status = EFI_BUFFER_TOO_SMALL;
  } else {
    CopyMem(SmramMap, mSmmAccess.SmramDesc, NecessaryBufferSize);
    Status = EFI_SUCCESS;
  }

  *SmramMapSize = NecessaryBufferSize;

  return Status;
}


/**
   This routine updates SMRAM regions' state with an OR'ed or AND'ed value

   @param OrAnd           Whether the value is to be OR'ed (TRUE) or AND'ed (FALSE)
   @param Value           The Value to be OR'ed or AND'ed to SMRAM Regions' state

**/
VOID
SyncRegionState2SmramDesc(
  IN BOOLEAN  OrAnd,
  IN UINT64   Value
  )
{
  UINT32 Index;

  for (Index = 0; Index < mSmmAccess.NumberRegions; Index++) {
    if (OrAnd) {
      mSmmAccess.SmramDesc[Index].RegionState |= Value;
    } else {
      mSmmAccess.SmramDesc[Index].RegionState &= Value;
    }
  }
}

