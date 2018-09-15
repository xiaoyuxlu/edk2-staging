/** @file
  Helper functions that access register bits

  Copyright (c) 2012 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScSmmHelpers.h"

#define BIT_ZERO  0x00000001

/**
  Check if a specific bit in a register is set

  @param[in] BitDesc                 The struct that includes register address, size in byte and bit number

  @retval    TRUE                    The bit is enabled
  @retval    FALSE                   The bit is disabled

**/
BOOLEAN
ReadBitDesc (
  IN CONST SC_SMM_BIT_DESC           *BitDesc
  )
{
  EFI_STATUS  Status;
  UINT64      Register;
  UINT32      PciBus;
  UINT32      PciDev;
  UINT32      PciFun;
  UINT32      PciReg;
  UINTN       RegSize;
  BOOLEAN     BitWasOne;
  UINTN       ShiftCount;
  UINTN       RegisterOffset;

  if ((BitDesc == NULL) || IS_BIT_DESC_NULL (*BitDesc)) {
    ASSERT (FALSE);
    return FALSE;
  }

  RegSize     = 0;
  Register    = 0;
  ShiftCount  = 0;
  BitWasOne   = FALSE;

  switch (BitDesc->Reg.Type) {
    case ACPI_ADDR_TYPE:
      switch (BitDesc->SizeInBytes) {
        case 0:
          //
          // Chances are that this field didn't get initialized.
          // Check your assignments to bit descriptions.
          //
          ASSERT (FALSE);
          break;

        case 1:
          RegSize = SMM_IO_UINT8;
          break;

        case 2:
          RegSize = SMM_IO_UINT16;
          break;

        case 4:
          RegSize = SMM_IO_UINT32;
          break;

        case 8:
          RegSize = SMM_IO_UINT64;
          break;

        default:
          //
          // Unsupported or invalid register size
          //
          ASSERT (FALSE);
          break;
      }

      //
      // Double check that we correctly read in the ACPI base address
      //

      RegisterOffset  = BitDesc->Reg.Data.acpi;
      ShiftCount      = BitDesc->Bit;

      //
      // As current CPU Smm Io can only support at most
      // 32-bit read/write,if Operation is 64 bit,
      // we do a 32 bit operation according to BitDesc->Bit
      //
      if (RegSize == SMM_IO_UINT64) {
        RegSize = SMM_IO_UINT32;
        //
        // If the operation is for high 32 bits
        //
        if (BitDesc->Bit >= 32) {
          RegisterOffset += 4;
          ShiftCount -= 32;
        }
      }

      Status = gSmst->SmmIo.Io.Read (
                                 &gSmst->SmmIo,
                                 RegSize,
                                 GetAcpiBaseAddress() + RegisterOffset,
                                 1,
                                 &Register
                                 );
      ASSERT_EFI_ERROR (Status);

      if ((Register & (LShiftU64 (BIT_ZERO, ShiftCount))) != 0) {
        BitWasOne = TRUE;
      } else {
        BitWasOne = FALSE;
      }
      break;

    case MEMORY_MAPPED_IO_ADDRESS_TYPE:
    case GPIO_ADDR_TYPE:
      //
      // Read the register, and it with the bit to read
      //
      switch (BitDesc->SizeInBytes) {
        case 1:
          Register = (UINT64) MmioRead8 ((UINTN) BitDesc->Reg.Data.Mmio);
          break;

        case 2:
          Register = (UINT64) MmioRead16 ((UINTN) BitDesc->Reg.Data.Mmio);
          break;

        case 4:
          Register = (UINT64) MmioRead32 ((UINTN) BitDesc->Reg.Data.Mmio);
          break;

        case 8:
          Register                      = (UINT64) MmioRead32 ((UINTN) BitDesc->Reg.Data.Mmio);
          *((UINT32 *) (&Register) + 1) = MmioRead32 ((UINTN) BitDesc->Reg.Data.Mmio + 4);
          break;

        default:
          //
          // Unsupported or invalid register size
          //
          ASSERT (FALSE);
          break;
      }

      Register = Register & (LShiftU64 (BIT0, BitDesc->Bit));
      if (Register) {
        BitWasOne = TRUE;
      } else {
        BitWasOne = FALSE;
      }
      break;

    case PCIE_ADDR_TYPE:
      PciBus  = BitDesc->Reg.Data.pcie.Fields.Bus;
      PciDev  = BitDesc->Reg.Data.pcie.Fields.Dev;
      PciFun  = BitDesc->Reg.Data.pcie.Fields.Fnc;
      PciReg  = BitDesc->Reg.Data.pcie.Fields.Reg;
      switch (BitDesc->SizeInBytes) {
        case 0:
          //
          // Chances are that this field didn't get initialized.
          // Check your assignments to bit descriptions.
          //
          ASSERT (FALSE);
          break;

        case 1:
          Register = (UINT64) MmioRead8 (MmPciAddress (0, PciBus, PciDev, PciFun, PciReg));
          break;

        case 2:
          Register = (UINT64) MmioRead16 (MmPciAddress (0, PciBus, PciDev, PciFun, PciReg));
          break;

        case 4:
          Register = (UINT64) MmioRead32 (MmPciAddress (0, PciBus, PciDev, PciFun, PciReg));
          break;

        default:
          //
          // Unsupported or invalid register size
          //
          ASSERT (FALSE);
          break;
      }

      if ((Register & (LShiftU64 (BIT_ZERO, BitDesc->Bit))) != 0) {
        BitWasOne = TRUE;
      } else {
        BitWasOne = FALSE;
      }
      break;
    case PCR_ADDR_TYPE:
      ASSERT (FALSE);
      break;
    default:
      //
      // This address type is not yet implemented
      //
      ASSERT (FALSE);
      break;
  }

  return BitWasOne;
}


/**
  Write a specific bit in a register

  @param[in] BitDesc              The struct that includes register address, size in byte and bit number
  @param[in] ValueToWrite         The value to be written
  @param[in] WriteClear           If the rest bits of the register is write clear

**/
VOID
WriteBitDesc (
  IN CONST SC_SMM_BIT_DESC        *BitDesc,
  IN CONST BOOLEAN                ValueToWrite,
  IN CONST BOOLEAN                WriteClear
  )
{
  EFI_STATUS  Status;
  UINT64      Register;
  UINT64      AndVal;
  UINT64      OrVal;
  UINT32      RegSize;
  UINT32      PciBus;
  UINT32      PciDev;
  UINT32      PciFun;
  UINT32      PciReg;
  UINTN       RegisterOffset;

  ASSERT (BitDesc != NULL);

  if ((BitDesc == NULL) || IS_BIT_DESC_NULL (*BitDesc)) {
    ASSERT (FALSE);
    return;
  }

  RegSize   = 0;
  Register  = 0;

  if (WriteClear) {
    AndVal = LShiftU64 (BIT_ZERO, BitDesc->Bit);
  } else {
    AndVal = ~(LShiftU64 (BIT_ZERO, BitDesc->Bit));
  }

  OrVal = (LShiftU64 ((UINT32) ValueToWrite, BitDesc->Bit));

  switch (BitDesc->Reg.Type) {

    case ACPI_ADDR_TYPE:
      switch (BitDesc->SizeInBytes) {
        case 0:
          //
          // Chances are that this field didn't get initialized.
          // Check your assignments to bit descriptions.
          //
          ASSERT (FALSE);
          break;

        case 1:
          RegSize = SMM_IO_UINT8;
          break;

        case 2:
          RegSize = SMM_IO_UINT16;
          break;

        case 4:
          RegSize = SMM_IO_UINT32;
          break;

        case 8:
          RegSize = SMM_IO_UINT64;
          break;

        default:
          //
          // Unsupported or invalid register size
          //
          ASSERT (FALSE);
          break;
      }
      //
      // Double check that we correctly read in the ACPI base address
      //
      RegisterOffset = BitDesc->Reg.Data.acpi;

      //
      // As current CPU Smm Io can only support at most
      // 32-bit read/write,if Operation is 64 bit,
      // we do a 32 bit operation according to BitDesc->Bit
      //
      if (RegSize == SMM_IO_UINT64) {
        RegSize = SMM_IO_UINT32;

        //
        // If the operation is for high 32 bits
        //
        if (BitDesc->Bit >= 32) {
          RegisterOffset += 4;

          if (WriteClear) {
            AndVal = LShiftU64 (BIT_ZERO, BitDesc->Bit - 32);
          } else {
            AndVal = ~(LShiftU64 (BIT_ZERO, BitDesc->Bit - 32));
          }

          OrVal = LShiftU64 ((UINT32) ValueToWrite, BitDesc->Bit - 32);
        }
      }

      Status = gSmst->SmmIo.Io.Read (
                                 &gSmst->SmmIo,
                                 RegSize,
                                 GetAcpiBaseAddress() + RegisterOffset,
                                 1,
                                 &Register
                                 );
      ASSERT_EFI_ERROR (Status);

      Register &= AndVal;
      Register |= OrVal;

      Status = gSmst->SmmIo.Io.Write (
                                 &gSmst->SmmIo,
                                 RegSize,
                                 GetAcpiBaseAddress() + RegisterOffset,
                                 1,
                                 &Register
                                 );
      ASSERT_EFI_ERROR (Status);
      break;

    case MEMORY_MAPPED_IO_ADDRESS_TYPE:
    case GPIO_ADDR_TYPE:
      //
      // Read the register, or it with the bit to set, then write it back.
      //
      switch (BitDesc->SizeInBytes) {
        case 1:
          Register = (UINT64) MmioRead8 ((UINTN) BitDesc->Reg.Data.Mmio);
          break;

        case 2:
          Register = (UINT64) MmioRead16 ((UINTN) BitDesc->Reg.Data.Mmio);
          break;

        case 4:
          Register = (UINT64) MmioRead32 ((UINTN) BitDesc->Reg.Data.Mmio);
          break;

        case 8:
          Register                      = (UINT64) MmioRead32 ((UINTN) BitDesc->Reg.Data.Mmio);
          *((UINT32 *) (&Register) + 1) = MmioRead32 ((UINTN) BitDesc->Reg.Data.Mmio + 4);
          break;

        default:
          //
          // Unsupported or invalid register size
          //
          ASSERT (FALSE);
          break;
      }

      Register &= AndVal;
      Register |= OrVal;
      //
      // Read the register, or it with the bit to set, then write it back.
      //
      switch (BitDesc->SizeInBytes) {
        case 1:
          MmioWrite8 ((UINTN) BitDesc->Reg.Data.Mmio, (UINT8) Register);
          break;

        case 2:
          MmioWrite16 ((UINTN) BitDesc->Reg.Data.Mmio, (UINT16) Register);
          break;

        case 4:
          MmioWrite32 ((UINTN) BitDesc->Reg.Data.Mmio, (UINT32) Register);
          break;

        case 8:
          MmioWrite32 ((UINTN) BitDesc->Reg.Data.Mmio, (UINT32) Register);
          MmioWrite32 ((UINTN) BitDesc->Reg.Data.Mmio + 4, *((UINT32 *) (&Register) + 1));
          break;

        default:
          //
          // Unsupported or invalid register size
          //
          ASSERT (FALSE);
          break;
      }
      break;

    case PCIE_ADDR_TYPE:
      PciBus  = BitDesc->Reg.Data.pcie.Fields.Bus;
      PciDev  = BitDesc->Reg.Data.pcie.Fields.Dev;
      PciFun  = BitDesc->Reg.Data.pcie.Fields.Fnc;
      PciReg  = BitDesc->Reg.Data.pcie.Fields.Reg;
      switch (BitDesc->SizeInBytes) {

        case 0:
          //
          // Chances are that this field didn't get initialized -- check your assignments
          // to bit descriptions.
          //
          ASSERT (FALSE);
          break;

        case 1:
          MmioAndThenOr8 (MmPciAddress (0, PciBus, PciDev, PciFun, PciReg), (UINT8) AndVal, (UINT8) OrVal);
          break;

        case 2:
          MmioAndThenOr16 (MmPciAddress (0, PciBus, PciDev, PciFun, PciReg), (UINT16) AndVal, (UINT16) OrVal);
          break;

        case 4:
          MmioAndThenOr32 (MmPciAddress (0, PciBus, PciDev, PciFun, PciReg), (UINT32) AndVal, (UINT32) OrVal);
          break;

        default:
          //
          // Unsupported or invalid register size
          //
          ASSERT (FALSE);
          break;
      }
      break;
    case PCR_ADDR_TYPE:
      ASSERT (FALSE);
      break;
    default:
      //
      // This address type is not yet implemented
      //
      ASSERT (FALSE);
      break;
  }
}

