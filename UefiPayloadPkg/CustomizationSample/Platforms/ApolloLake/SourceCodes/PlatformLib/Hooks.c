/** @file
  
  Implementation for hooks called at various Payload events.

  ** NOTE ** This implementation is currently NOT complete, and only serves
  as an illustration. This implementation is NOT intended for production use.

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
#include "Include/SaAccess.h"
#include <Library/PlatformLib.h>
#include <Library/SerialPortLib.h>

VOID
EFIAPI
ScOnEndOfDxe (
  VOID
  );

/**
  Returns PCIE address

  @param[in] Bus                  Pci Bus Number
  @param[in] Device               Pci Device Number
  @param[in] Function             Pci Function Number

  @retval    PCIE address

**/
UINTN
MmPciBase (
  IN UINT32                       Bus,
  IN UINT32                       Device,
  IN UINT32                       Function
  )
{
  UINTN  PcieAddress;

  ASSERT ((Bus <= 0xFF) && (Device <= 0x1F) && (Function <= 0x7));
  PcieAddress = (UINTN)(UINT32)PCIEX_BASE_ADDRESS + (UINTN)(UINT32)((Bus << 20) + (Device << 15) + (Function << 12));

  return PcieAddress;
}

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
  )
{
  UINTN                           PciSpiRegBase;
  UINTN                           LpcBaseAddress;

  //
  // Lock down SPI access
  //
  
  //
  // Get SPI PCI Config Space base
  //
  PciSpiRegBase = MmPciBase (
                       DEFAULT_PCI_BUS_NUMBER_SC,
                       PCI_DEVICE_NUMBER_SPI,
                       PCI_FUNCTION_NUMBER_SPI
                       );

  LpcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_SC,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC
                     );

  DEBUG ((EFI_D_INFO, "EndOfDxe PciSpiRegBase == 0x%X, LpcBaseAddress == 0x%X\n", PciSpiRegBase, LpcBaseAddress));
  DEBUG ((EFI_D_INFO, "EndOfDxe hook: SPI BCR register initial value == 0x%X\n", (UINTN)MmioRead32((UINTN) (PciSpiRegBase + R_SPI_BCR))));
  DEBUG ((EFI_D_INFO, "EndOfDxe hook: LPC BCR register initial value == 0x%X\n", (UINTN)MmioRead8((UINTN) (LpcBaseAddress + R_PCH_LPC_BC))));

  //
  // Disable BIOS write
  //
  MmioAnd32 ((UINTN) (PciSpiRegBase + R_SPI_BCR), (UINT32) (~B_SPI_BCR_BIOSWE));
  MmioAnd8 ((UINTN) (LpcBaseAddress + R_PCH_LPC_BC), (UINT8) (~B_PCH_LPC_BC_WPD));

  //
  // Forbid none-SMM SPI writes
  //
  MmioOr32 ((UINTN) (PciSpiRegBase + R_SPI_BCR), (UINT32) B_SPI_BCR_SMM_BWP);
  MmioOr8 ((UINTN) (LpcBaseAddress + R_PCH_LPC_BC), (UINT8) (B_PCH_LPC_BC_EISS));

  //
  // Lock down
  //
  MmioOr32 ((UINTN) (PciSpiRegBase + R_SPI_BCR), (UINT32) B_SPI_BCR_BLE);
  MmioOr32 ((UINTN) (PciSpiRegBase + R_SPI_BCR), (UINT32) B_SPI_BCR_BILD);
  MmioOr8 ((UINTN) (LpcBaseAddress + R_PCH_LPC_BC), (UINT8) (B_PCH_LPC_BC_LE));
  MmioOr8 ((UINTN) (LpcBaseAddress + R_PCH_LPC_BC), (UINT8) (B_PCH_LPC_BC_BILD));
  
  DEBUG ((EFI_D_INFO, "EndOfDxe hook: SPI BCR register configured value == 0x%X\n", (UINTN)MmioRead32((UINTN) (PciSpiRegBase + R_SPI_BCR))));
  DEBUG ((EFI_D_INFO, "EndOfDxe hook: LPC BCR register configured value == 0x%X\n", (UINTN)MmioRead8((UINTN) (LpcBaseAddress + R_PCH_LPC_BC))));

  //
  // Call South Cluster settings
  //
  ScOnEndOfDxe();

  return EFI_SUCCESS;
}

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
  )
{
  UINT32                          PmcBase;
  UINT32                          Data;

  DEBUG ((DEBUG_INFO, "LockConfig () - Start\n"));

  PmcBase        = PMC_BASE_ADDRESS;

  //
  // Lock the "CF9h Global reset" field by setting CF9LOCK bit (PBASE + 0x48[31]) for production build,
  // but this field should not be locked for manufacturing mode. When the Manufacturing Mode is closed,
  // CF9h Global Reset should be cleared (step#1) and CF9LOCK bit should be set (step#2).
  //
  Data = B_PMC_PMIR_CF9LOCK;
  DEBUG ((DEBUG_INFO, "Pmc Base Address: 0x%X \n", (UINTN)PmcBase));
  DEBUG ((DEBUG_INFO, "R_PMC_PMIR orig value: 0x%X, Data to be OR'ed: 0x%X\n", MmioRead32 ((UINTN) (PmcBase + R_PMC_PMIR)), 
  (UINTN)Data));
  
  MmioAndThenOr32 (
    (UINTN)(UINT32)(PmcBase + R_PMC_PMIR),
    (UINT32) (~(B_PMC_PMIR_CF9LOCK | B_PMC_PMIR_CF9GR)),
    (UINT32) Data
    );

  DEBUG ((DEBUG_INFO, "R_PMC_PMIR configured value: 0x%X\n", MmioRead32 ((UINTN) (PmcBase + R_PMC_PMIR))));
  return EFI_SUCCESS;
}

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
  )
{
  return EFI_SUCCESS;
}

/**
  Outputs a Quad Word value in Hex format through serial port

  @param[in] Qword          The Quad Word value to output

**/
VOID
OutputQword (
  UINT64 Qw
  )
{
  STATIC UINT8 Map[0x10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  UINT8 Prefix[0x2] = {'0', 'x'};
  DEBUG_CODE (
    UINT8 Buffer[16];
    UINT8 Index;

    SerialPortWrite (Prefix, 2);
    for (Index = 0; Index < 16; Index ++) {
      Buffer[15 - Index] = Map [Qw & 0xF];
      Qw >>= 4;
    }
    SerialPortWrite (Buffer, 16);
  );
  return;
}

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
  )
{
  UINT64    Data;
  UINT8 MpHook[] = {'M', 'p', 'H', 'o', 'o', 'k', ' '};
  UINT8 MSR0x3A[] = {'M', 'S', 'R', '0', 'x', '3', 'A', ' ', '=', '=', ' '};
  Data = AsmReadMsr64 (0x120);
  
  OutputQword (Data);
  Data |= BIT6;
  AsmWriteMsr64 (0x120, Data);
  OutputQword (AsmReadMsr64 (0x120));
  SerialPortWrite (MpHook, 7);
  SerialPortWrite (MSR0x3A, 7);
  OutputQword (AsmReadMsr64 (0x3A));
  return EFI_SUCCESS;
}

///
/// BXT Series
///
typedef enum {
  Bxt          = 0x00,
  Bxt1,
  BxtX,
  BxtP,
  BxtSeriesMax = 0xFF
} BXT_SERIES;

/**
  Return SOC series type

  @retval  BXT_SERIES      SOC series type

**/
BXT_SERIES
EFIAPI
GetBxtSeries (
  VOID
  )
{
  UINTN   McD0Base;
  UINT16  VenId;
  UINT16  DevId;

  McD0Base = MmPciBase (
               SA_MC_BUS,
               SA_MC_DEV,
               SA_MC_FUN
               );

  VenId = MmioRead16 (McD0Base + PCI_VENDOR_ID_OFFSET);
  DevId = MmioRead16 (McD0Base + PCI_DEVICE_ID_OFFSET);
  if (VenId == V_SA_MC_VID) {
    switch (DevId) {
      case V_SA_MC_DID0:
        return Bxt;
        break;
      case V_SA_MC_DID1:
        return Bxt1;
        break;
      case V_SA_MC_DID3:
        return BxtP;
        break;
      default:
        DEBUG ((DEBUG_ERROR, "Unsupported BXT Series.\n"));
        return BxtSeriesMax;
        break;
    }
  }
  return BxtSeriesMax;
}

/**
  Perform the remaining configuration on SC SATA to perform device detection
  at end of Dxe, then set the SATA SPD and PxE corresponding, and set the Register Lock

  @param[in] ScPolicy                   The SC Policy instance

  @retval    EFI_SUCCESS                The function completed successfully

**/
EFI_STATUS
EFIAPI
ConfigureSataDxe (
  VOID
  )
{
  UINTN          PciSataRegBase;

  DEBUG ((DEBUG_INFO, "ConfigureSataDxe() Start\n"));

  PciSataRegBase = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_SC,
                     PCI_DEVICE_NUMBER_SATA,
                     PCI_FUNCTION_NUMBER_SATA
                     );

  //
  // Program SATA PCI offset 9Ch [31] to 1b
  //
  MmioOr32 ((UINTN) (PciSataRegBase + R_SATA_SATAGC), BIT31);
  DEBUG ((DEBUG_INFO, "SATAGC = 0x%x\n", MmioRead32 (PciSataRegBase + R_SATA_SATAGC)));
  DEBUG ((DEBUG_INFO, "ConfigureSataDxe() End\n"));

  return EFI_SUCCESS;
}

/**
  Lock USB registers before boot

**/
VOID
EFIAPI
UsbInitBeforeBoot (
  VOID
 )
{
  UINTN         XhciPciMmBase;

  XhciPciMmBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_SC,
                    PCI_DEVICE_NUMBER_XHCI,
                    PCI_FUNCTION_NUMBER_XHCI
                    );

  //
  // Set xHCI MSI_MCTL According to OS:
  //  Windows: D3 Supported - Set to 1h to enable MSI
  //  Android: D0i3/DevIdle Supported (legacy interrupts used in D0i3 flows) - Set to 0h to disable MSI
  //
  if (MmioRead32((UINTN)(XhciPciMmBase)) != 0xFFFFFFFF) {
    MmioOr16(XhciPciMmBase + R_XHCI_MSI_MCTL, B_XHCI_MSI_MCTL_MSIENABLE);
  } else {
    DEBUG ((EFI_D_ERROR, "xHCI not present, cannot disable the xHCI MSI capability structure for Android.\n"));
  }
  
  MmioOr32 (XhciPciMmBase + R_XHCI_XHCC2, B_XHCI_XHCC2_OCCFDONE);
  
  MmioOr32 (XhciPciMmBase + R_XHCI_XHCC1, B_XHCI_XHCC1_ACCTRL | B_XHCI_XHCC1_URD);
}

#define SC_PCR_BASE_ADDRESS            0xD0000000     ///< SBREG MMIO base address
#define SC_PCR_ADDRESS(Pid, Offset)    (SC_PCR_BASE_ADDRESS | ((UINT8) (Pid) << 16) | (UINT16) (Offset))
typedef enum {
  PID0                               = 0xD1,
  PID1                               = 0xD0,
  PID2                               = 0xC6,
  PID3                               = 0xB0,
  PID4                               = 0xB3,
  PID5                               = 0xB4,
  PID6                               = 0xB6,
  PID7                               = 0xA9,
} SC_SBI_PID;

/**
  Read PCR register.
  It returns PCR register and size in 1byte/2bytes/4bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of this Port ID
  @param[in]  Size                      Size for read. Must be 1 or 2 or 4.
  @param[out] OutData                   Buffer of Output Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
STATIC
EFI_STATUS
PchPcrRead (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINTN                             Size,
  OUT UINT32                            *OutData
  )
{
  if ((Offset & (Size - 1)) != 0) {
    DEBUG ((DEBUG_ERROR, "PchPcrRead error. Invalid Offset: %x Size: %x", Offset, Size));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }
  switch (Size) {
    case 4:
      *(UINT32 *) OutData = MmioRead32 (SC_PCR_ADDRESS (Pid, Offset));
      break;
    case 2:
      *(UINT16 *) OutData = MmioRead16 (SC_PCR_ADDRESS (Pid, Offset));
      break;
    case 1:
      *(UINT8 *) OutData = MmioRead8 (SC_PCR_ADDRESS (Pid, Offset));
      break;
    default:
      break;
  }

  return EFI_SUCCESS;
}


/**
  Write PCR register.
  It programs PCR register and size in 1byte/2bytes/4bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  Size                      Size for read. Must be 1 or 2 or 4.
  @param[in]  InData                    Input Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
STATIC
EFI_STATUS
PchPcrWrite (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINTN                             Size,
  IN  UINT32                            InData
  )
{
  if ((Offset & (Size - 1)) != 0) {
    DEBUG ((DEBUG_ERROR, "PchPcrWrite error. Invalid Offset: %x Size: %x", Offset, Size));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }
#ifdef EFI_DEBUG
  if (!PchPcrWriteMmioCheck (Pid, Offset)) {
    DEBUG ((DEBUG_ERROR, "PchPcrWrite error. Pid: %x Offset: %x should access through SBI interface", Pid, Offset));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }
#endif

  //
  // Write the PCR register with provided data
  // Then read back PCR register to prevent from back to back write.
  //
  switch (Size) {
    case 4:
      MmioWrite32 (SC_PCR_ADDRESS (Pid, Offset), (UINT32)InData);
      break;
    case 2:
      MmioWrite16 (SC_PCR_ADDRESS (Pid, Offset), (UINT16)InData);
      break;
    case 1:
      MmioWrite8  (SC_PCR_ADDRESS (Pid, Offset), (UINT8) InData);
      break;
    default:
      break;
  }

  return EFI_SUCCESS;
}

/**
  Reads an 4-byte Pcr register, performs a bitwise AND followed by a bitwise
  inclusive OR, and writes the result back to the 4-byte Pcr register.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  AndData                   AND Data. Must be the same size as Size parameter.
  @param[in]  OrData                    OR Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrAndThenOr32 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINT32                            AndData,
  IN  UINT32                            OrData
  )
{
  EFI_STATUS                            Status;
  UINT32                                Data32;

  Status  = PchPcrRead (Pid, Offset, 4, &Data32);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Data32 &= AndData;
  Data32 |= OrData;
  Status  = PchPcrWrite (Pid, Offset, 4, Data32);

  return Status;
}

/**
  South Cluster initialization on End-Of-DXE event

**/
VOID
EFIAPI
ScOnEndOfDxe (
  VOID
  )
{
  UINT32                     FuncDisableReg;
  UINT32                     Data32And;
  UINT32                     Data32Or;
  UINT16                     AcpiBaseAddr;
  UINT32                     PmcBase;
  UINTN                      PciLpcRegBase;
  UINTN                      PciSpiRegBase;
  BXT_SERIES                 BxtSeries;
  UINT8                      Data8;
  UINT16                     Data16Or;
  UINT16                     Data16And;
  UINT16                     Data16;
  UINTN                      SpiBar0;

  Data8 = 0;
  SpiBar0 = 0;
  PciLpcRegBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_SC,
                    PCI_DEVICE_NUMBER_PCH_LPC,
                    PCI_FUNCTION_NUMBER_PCH_LPC
                    );
  PciSpiRegBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_SC,
                    PCI_DEVICE_NUMBER_SPI,
                    PCI_FUNCTION_NUMBER_SPI
                    );

  DEBUG ((DEBUG_INFO, "ScOnEndOfDxe() Start\n"));

  BxtSeries = GetBxtSeries ();
  if (BxtSeries == BxtP) {
    ConfigureSataDxe ();
  }
  AcpiBaseAddr   = GetAcpiBaseAddress();
  PmcBase        = PMC_BASE_ADDRESS;
  FuncDisableReg = MmioRead32 (PmcBase + R_PMC_FUNC_DIS);
  DEBUG ((DEBUG_INFO, "0x%X\n", FuncDisableReg));
  MmioAnd32 (
    (UINTN)(UINT32)(PmcBase + R_PMC_PMIR),
    (UINT32) ~(B_PMC_PMIR_CF9GR)
    );

  if (BxtSeries == BxtP){
    SpiBar0 = MmioRead32 (PciSpiRegBase + R_SPI_BASE) &~(B_SPI_BAR0_MASK);

    Data16 = (UINT16) (B_SPI_HSFS_WRSDIS);
    MmioWrite16 ((UINTN) (SpiBar0 + R_SPI_HSFS), Data16);
  }
  
  //
  // Enables SMI_LOCK
  //
  DEBUG ((DEBUG_INFO, "Enable SMI Lock\n"));
  MmioOr8 ((UINTN)(UINT32)(PmcBase + R_PMC_GEN_PMCON_2), B_PMC_GEN_PMCON_SMI_LOCK);

  if (GetBxtSeries () == BxtP) {
    DEBUG ((DEBUG_INFO, "Configure SPI\n"));
    //
    // LPC
    //
    if (! (MmioRead8 (PciLpcRegBase + R_PCH_LPC_BC) & B_PCH_LPC_BC_LE)) {
      DEBUG ((DEBUG_INFO, "Set LPC bios lock\n"));
      MmioOr8 ((UINTN) (PciLpcRegBase + R_PCH_LPC_BC), B_PCH_LPC_BC_LE);
    }
    
    //
    // SPI
    //
    if (! (MmioRead8 (PciSpiRegBase + R_SPI_BCR) & B_SPI_BCR_BLE)) {
      DEBUG ((DEBUG_INFO, "Set SPI bios lock\n"));
      MmioOr8 ((UINTN) (PciSpiRegBase + R_SPI_BCR), (UINT8) B_SPI_BCR_BLE);
    }

    //
    // LPC
    //
    MmioOr8 ((UINTN) (PciLpcRegBase + R_PCH_LPC_BC), (UINT8) B_PCH_LPC_BC_BILD);
    
    //
    // Reads back for posted write to take effect
    //
    Data8 = MmioRead8 ((UINTN) (PciLpcRegBase + R_PCH_LPC_BC));
    
    //
    // SPI
    //
    MmioOr8 ((UINTN) (PciSpiRegBase + R_SPI_BCR), (UINT8) B_SPI_BCR_BILD);
    
    //
    // Reads back for posted write to take effect
    //
    Data8 = MmioRead8 ((UINTN) (PciSpiRegBase + R_SPI_BCR));
    
    //
    // Locks down VSCC
    //
    MmioOr32 ((UINTN) (SpiBar0 + R_SPI_LVSCC), B_SPI_LVSCC_VCL);
  }

  DEBUG ((DEBUG_INFO, "0x%x\n", Data8));
  //
  // Lock Down TCO
  //
  DEBUG ((DEBUG_INFO, "Disable TCO SMI and Lock Down TCO\n"));
  DEBUG ((DEBUG_INFO, "R_SMI_EN original value == 0x%X\n", (UINT32)IoRead16 (AcpiBaseAddr + R_SMI_EN)));
  Data16And = (UINT16)~B_SMI_EN_TCO;
  IoAnd16 (AcpiBaseAddr + R_SMI_EN, Data16And);
  DEBUG ((DEBUG_INFO, "R_SMI_EN configured value == 0x%X\n", (UINT32)IoRead16 (AcpiBaseAddr + R_SMI_EN)));

  Data16Or = B_TCO_CNT_LOCK;
  IoOr16 (AcpiBaseAddr + R_TCO_CNT, Data16Or);

  DEBUG ((DEBUG_INFO, "USB init before boot\n"));
  UsbInitBeforeBoot ();
  DEBUG ((DEBUG_INFO, "Enable TCO and Lock Down TCO ---- END\n"));

  Data32And = 0xFFFFFFFF;
  Data32Or  = (B_PCH_PCR_RTC_CONF_UCMOS_LOCK | B_PCH_PCR_RTC_CONF_LCMOS_LOCK | B_PCH_PCR_RTC_CONF_BILD);
  PchPcrAndThenOr32 (
    0xD1,
    R_PCH_PCR_RTC_CONF,
    Data32And,
    Data32Or
    );

  DEBUG ((DEBUG_INFO, "ScOnEndOfDxe() End\n"));

  return;
}

