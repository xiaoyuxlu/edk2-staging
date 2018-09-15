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

#ifndef _SMM_ACCESS_DRIVER_H
#define _SMM_ACCESS_DRIVER_H

//
// Private data
//
typedef struct {
    UINTN                           Signature;
    EFI_HANDLE                      Handle;
    EFI_SMM_ACCESS2_PROTOCOL        SmmAccess;
    //
    // Local Data for SMM Access interface goes here
    //
    UINTN                           NumberRegions;
    UINT64                          SMMRegionState;        
    EFI_SMRAM_DESCRIPTOR            *SmramDesc;
} SMM_ACCESS_PRIVATE_DATA;

//
// Prototypes
// Driver model protocol interface
//

EFI_STATUS
EFIAPI
Open (
  IN EFI_SMM_ACCESS2_PROTOCOL *This
  );

EFI_STATUS
EFIAPI
Close (
  IN EFI_SMM_ACCESS2_PROTOCOL *This
  );

EFI_STATUS
EFIAPI
Lock (
  IN EFI_SMM_ACCESS2_PROTOCOL *This
  );

EFI_STATUS
EFIAPI
GetCapabilities (
  IN CONST EFI_SMM_ACCESS2_PROTOCOL     *This,
  IN OUT   UINTN                       *SmramMapSize,
  IN OUT   EFI_SMRAM_DESCRIPTOR        *SmramMap
  );

VOID
SyncRegionState2SmramDesc(
  IN BOOLEAN  OrAnd,
  IN UINT64    Value
  );
  
#endif
