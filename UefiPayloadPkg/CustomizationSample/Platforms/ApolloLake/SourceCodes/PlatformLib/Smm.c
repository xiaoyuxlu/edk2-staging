/** @file
  Platform specific SMM functions 
  
  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include "Include/ScAccess.h"
#include <Library/PlatformLib.h>

UINT16 mAcpiBaseAddr = 0x400;

/**
  Returns the ACPI Power Management Base I/O address

  @retval ACPI Power Management Base I/O address

--*/
UINT16
EFIAPI
GetAcpiBaseAddress (
  VOID
  )
{
  return mAcpiBaseAddr;
}    

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
  )
{
  *AcpiIoOffset = R_SMI_STS;
  *SizeInBytes = S_SMI_STS;
  *Bit = N_SMI_STS_APM;

  return EFI_SUCCESS;
}


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
  )
{
  *Base = 0x7B000000;
  *Size  = 0x800000;

  return EFI_SUCCESS;
}


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
  )
{
  MmioAndThenOr64 (MCH_BASE_ADDRESS + 0x6838, 0, 0x00);

  return EFI_SUCCESS;
}


/**
  Enables the South Cluster to generate SMIs.

  @retval EFI_SUCCESS             Enable Global Smi Bit completed

**/
EFI_STATUS
ScSmmEnableGlobalSmiBit (
  VOID
  )
{
  UINT32  SmiEn;

  SmiEn = IoRead32 ((UINTN)(UINT32)(mAcpiBaseAddr + R_SMI_EN));

  //
  // Set the "global SMI enable" bit
  //
  SmiEn |= B_SMI_EN_GBL_SMI;
  IoWrite32 ((UINTN)(UINT32)(mAcpiBaseAddr + R_SMI_EN), SmiEn);

  return EFI_SUCCESS;
}


/**
  Clears the SMI after all SMI source have been processed.
  Note that this function will not work correctly (as it is
  written) unless all SMI sources have been processed.
  A revision of this function could manually clear all SMI
  status bits to guarantee success.

  @retval  EFI_SUCCESS             Clears the SMIs completed
  @retval  EFI_DEVICE_ERROR        EOS was not set to a 1

**/
EFI_STATUS
ScSmmClearSmi (
  VOID
  )
{
  BOOLEAN   EosSet;
  BOOLEAN   SciEn;
  UINT32    Pm1Cnt;
  UINT16    Pm1Sts;
  UINT32    Gpe0aStsLow;
  UINT32    SmiSts;
  UINT32    TcoSts;

  //
  // Determine whether an ACPI OS is present (via the SCI_EN bit)
  //
  Pm1Cnt  = IoRead32 ((UINTN)(UINT32)(mAcpiBaseAddr + R_ACPI_PM1_CNT));
  SciEn   = (BOOLEAN) ((Pm1Cnt & B_ACPI_PM1_CNT_SCI_EN) == B_ACPI_PM1_CNT_SCI_EN);
  if (!SciEn) {
    //
    // Clear any SMIs that double as SCIs (when SCI_EN==0)
    //
    Pm1Sts        = IoRead16 ((UINTN)(UINT32)(mAcpiBaseAddr + R_ACPI_PM1_STS));
    Gpe0aStsLow   = IoRead32 ((UINTN)(UINT32)(mAcpiBaseAddr + R_ACPI_GPE0a_STS));

    Pm1Sts |=
      (
       B_ACPI_PM1_STS_WAK |
       B_ACPI_PM1_STS_WAK_PCIE0 |
       B_ACPI_PM1_STS_PRBTNOR |
       B_ACPI_PM1_STS_RTC |
       B_ACPI_PM1_STS_PWRBTN |
       B_ACPI_PM1_STS_GBL |
       B_ACPI_PM1_STS_WAK_PCIE3 |
       B_ACPI_PM1_STS_WAK_PCIE2 |
       B_ACPI_PM1_STS_WAK_PCIE1 |
       B_ACPI_PM1_STS_TMROF
       );

    Gpe0aStsLow |=
      (
       B_ACPI_GPE0a_STS_PME_B0 |
       B_ACPI_GPE0a_STS_BATLOW |
       B_ACPI_GPE0a_STS_PCI_EXP |
       B_ACPI_GPE0a_STS_GUNIT_SCI |
       B_ACPI_GPE0a_STS_PUNIT_SCI |
       B_ACPI_GPE0a_STS_SWGPE |
       B_ACPI_GPE0a_STS_HOT_PLUG
       );

    IoWrite16 ((UINTN)(UINT32)(mAcpiBaseAddr + R_ACPI_PM1_STS), (UINT16) Pm1Sts);
    IoWrite32 ((UINTN)(UINT32)(mAcpiBaseAddr + R_ACPI_GPE0a_STS), (UINT32) Gpe0aStsLow);
  }

  //
  // Clear all SMIs that are unaffected by SCI_EN
  //
  SmiSts = IoRead32 ((UINTN)(UINT32)(mAcpiBaseAddr + R_SMI_STS));
  TcoSts = IoRead32 ((UINTN)(UINT32)(mAcpiBaseAddr + R_TCO_STS));

  SmiSts |=
    (
     B_SMI_STS_PERIODIC |
     B_SMI_STS_TCO |
     B_SMI_STS_SWSMI_TMR |
     B_SMI_STS_APM |
     B_SMI_STS_ON_SLP_EN |
     B_SMI_STS_BIOS
     );

  TcoSts |=
    (
     B_TCO_STS_SECOND_TO |
     B_TCO_STS_TIMEOUT
     );

//to-do GpioClearAllGpiSmiSts ();
  IoWrite32 ((UINTN)(UINT32)(mAcpiBaseAddr + R_SMI_STS), SmiSts);

  //
  // Try to clear the EOS bit. ASSERT on an error
  //
  EosSet = SwSmiSetAndCheckEos ();
  ASSERT (EosSet);
  if (!EosSet) {
    return EFI_DEVICE_ERROR;
  } else {
    return EFI_SUCCESS;
  }
}

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
  )
{
  EFI_STATUS  Status;

  //
  // Clear all SMIs
  //
  ScSmmClearSmi ();

  Status = ScSmmEnableGlobalSmiBit ();
  ASSERT_EFI_ERROR (Status);

  return Status;
}

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
  )
{
  UINT32  SmiEn;

  //
  // to-do: check this
  //
  IoWrite32 (0x444, 0xFFFFFFFF); // clear SMI_STS
  IoWrite32 (0x440, 0x63); // set APMC, Global SMI, and EOS

  SmiEn = IoRead32 ((UINTN)(UINT32)(mAcpiBaseAddr + R_SMI_EN));

  //
  // Reset the SC to generate subsequent SMIs
  //
  SmiEn |= B_SMI_EN_EOS;
  IoWrite32 ((UINTN)(UINT32)(mAcpiBaseAddr + R_SMI_EN), SmiEn);

  //
  // Double check that the assert worked
  //
  SmiEn = IoRead32 ((UINTN)(UINT32)(mAcpiBaseAddr + R_SMI_EN));

  //
  // Return TRUE if EOS is set correctly
  //
  if ((SmiEn & B_SMI_EN_EOS) == 0) {
    //
    // EOS was not set to a 1; this is an error
    //
    return FALSE;
  } else {
    //
    // EOS was correctly set to a 1
    //
    return TRUE;
  }
}


/**
  Determine whether an ACPI OS is present (via the SCI_EN bit)

  @retval TRUE                    ACPI OS is present
  @retval FALSE                   ACPI OS is not present

**/
BOOLEAN
EFIAPI
IsSciEnabled (
  VOID
  )
{
  BOOLEAN    SciEn;
  UINT32     Pm1Cnt;

  //
  // Determine whether an ACPI OS is present (via the SCI_EN bit)
  //
  Pm1Cnt  = IoRead32 ((UINTN)(UINT32)(mAcpiBaseAddr + R_ACPI_PM1_CNT));
  SciEn   = (BOOLEAN) ((Pm1Cnt & B_ACPI_PM1_CNT_SCI_EN) == B_ACPI_PM1_CNT_SCI_EN);

  return SciEn;
}

/**
  Specific programming done before exiting SMI

**/
VOID
EFIAPI
BeforeExitSmi (
  VOID
  )
{
  if ((AsmReadMsr64(0x8b) >> 32) >= 8) {
    UINT32 SpiBar0 = 0;

    SpiBar0 = (MmioRead32 (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_SPI, PCI_FUNCTION_NUMBER_SPI, R_SPI_BASE))) & B_SPI_BASE_BAR;
    SpiBar0 |= BIT0; // Set MSR valid
    AsmWriteMsr32 (0x124, SpiBar0);
  }
}
