## @file
# UEFI Payload Package
#
# Provides drivers and definitions to build a UEFI payload.
#
# Copyright (c) 2014 - 2018, Intel Corporation. All rights reserved.<BR>
# This program and the accompanying materials are licensed and made available under
# the terms and conditions of the BSD License that accompanies this distribution.
# The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php.
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                       = UefiPayloadPkg
  PLATFORM_GUID                       = E95370CC-A4E6-4308-98DC-599D38519DC4
  PLATFORM_VERSION                    = 0.1
  DSC_SPECIFICATION                   = 0x00010005
  SUPPORTED_ARCHITECTURES             = IA32|X64
  BUILD_TARGETS                       = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER                    = DEFAULT
  OUTPUT_DIRECTORY                    = Build/UefiPayloadPkgX64
  FLASH_DEFINITION                    = UefiPayloadPkg/UefiPayloadPkg.fdf

  DEFINE SECURE_BOOT_ENABLE      = FALSE
  DEFINE SOURCE_DEBUG_ENABLE     = FALSE
  DEFINE FTPM_ENABLE             = FALSE
  DEFINE SMM_ENABLE              = FALSE

  #
  # CPU options
  #
  DEFINE MAX_LOGICAL_PROCESSORS  = 64

  #
  # PCI options
  #
  DEFINE PCIE_BASE                        = 0x0
###  DEFINE PCIE_BASE                        = 0x0

  #
  # Serial port set up
  #
  DEFINE BAUD_RATE                        = 115200
  DEFINE SERIAL_CLOCK_RATE                = 1843200
  DEFINE SERIAL_LINE_CONTROL              = 3 # 8-bits, no parity
  DEFINE SERIAL_HARDWARE_FLOW_CONTROL     = FALSE
  DEFINE SERIAL_DETECT_CABLE              = FALSE
  DEFINE SERIAL_FIFO_CONTROL              = 7 # Enable FIFO
  DEFINE SERIAL_EXTENDED_TX_FIFO_SIZE     = 16
  DEFINE UART_DEFAULT_BAUD_RATE           = $(BAUD_RATE)
  DEFINE UART_DEFAULT_DATA_BITS           = 8
  DEFINE UART_DEFAULT_PARITY              = 1
  DEFINE UART_DEFAULT_STOP_BITS           = 1
  DEFINE DEFAULT_TERMINAL_TYPE            = 0

  #
  #  typedef struct {
  #    UINT16  VendorId;          ///< Vendor ID to match the PCI device.  The value 0xFFFF terminates the list of entries.
  #    UINT16  DeviceId;          ///< Device ID to match the PCI device
  #    UINT32  ClockRate;         ///< UART clock rate.  Set to 0 for default clock rate of 1843200 Hz
  #    UINT64  Offset;            ///< The byte offset into to the BAR
  #    UINT8   BarIndex;          ///< Which BAR to get the UART base address
  #    UINT8   RegisterStride;    ///< UART register stride in bytes.  Set to 0 for default register stride of 1 byte.
  #    UINT16  ReceiveFifoDepth;  ///< UART receive FIFO depth in bytes. Set to 0 for a default FIFO depth of 16 bytes.
  #    UINT16  TransmitFifoDepth; ///< UART transmit FIFO depth in bytes. Set to 0 for a default FIFO depth of 16 bytes.
  #    UINT8   Reserved[2];
  #  } PCI_SERIAL_PARAMETER;
  #
  # Vendor FFFF Device 0000 Prog Interface 1, BAR #0, Offset 0, Stride = 1, Clock 1843200 (0x1c2000)
  #
  #                                           [Vendor]   [Device]  [----ClockRate---]  [------------Offset-----------] [Bar] [Stride] [RxFifo] [TxFifo]   [Rsvd]   [Vendor]
  DEFINE PCI_SERIAL_PARAMETERS            = {0xff,0xff, 0x00,0x00, 0x0,0x20,0x1c,0x00, 0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, 0x00,    0x01, 0x0,0x0, 0x0,0x0, 0x0,0x0, 0xff,0xff}

  #
  # Chipset options
  #
  DEFINE USE_HPET_TIMER                   = TRUE

  #
  # Shell options: [BUILD_SHELL, FULL_BIN, MIN_BIN, NONE, UEFI]
  #
  DEFINE SHELL_TYPE              = UEFI_BIN

  #
  # Use Emulated Variable Service or Not
  #
  DEFINE EMU_VARIABLE            = TRUE

  #
  # Special Pool is a debug feature to catch memory access errors
  # Turning this to TRUE impacts performance.
  #
  DEFINE SPECIAL_POOL             = FALSE

[BuildOptions]
  MSFT:*_*_*_CC_FLAGS            = /D DISABLE_NEW_DEPRECATED_INTERFACES
  GCC:*_*_*_CC_FLAGS             = -D DISABLE_NEW_DEPRECATED_INTERFACES
  GCC:*_UNIXGCC_*_CC_FLAGS       = -DMDEPKG_NDEBUG
  GCC:RELEASE_*_*_CC_FLAGS       = -DMDEPKG_NDEBUG
  INTEL:RELEASE_*_*_CC_FLAGS     = /D MDEPKG_NDEBUG
  MSFT:RELEASE_*_*_CC_FLAGS      = /D MDEPKG_NDEBUG

[BuildOptions.common.EDKII.DXE_RUNTIME_DRIVER]
  MSFT:*_*_*_DLINK_FLAGS         = /ALIGN:4096

################################################################################
#
# SKU Identification section - list of all SKU IDs supported by this Platform.
#
################################################################################
[SkuIds]
  0|DEFAULT

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses]
  #
  # Entry point
  #
  PeiCoreEntryPoint|MdePkg/Library/PeiCoreEntryPoint/PeiCoreEntryPoint.inf
  PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  DxeCoreEntryPoint|MdePkg/Library/DxeCoreEntryPoint/DxeCoreEntryPoint.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf

  #
  # Basic
  #
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  CpuLib|MdePkg/Library/BaseCpuLib/BaseCpuLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
!if $(PCIE_BASE) == 0
  PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
!else
  PciLib|MdePkg/Library/BasePciLibPciExpress/BasePciLibPciExpress.inf
  PciExpressLib|MdePkg/Library/BasePciExpressLib/BasePciExpressLib.inf
!endif
  PciSegmentLib|MdePkg/Library/BasePciSegmentLibPci/BasePciSegmentLibPci.inf
  PeCoffLib|MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  CacheMaintenanceLib|MdePkg/Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf

  #
  # UEFI & PI
  #
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiDecompressLib|MdePkg/Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLibIdt/PeiServicesTablePointerLibIdt.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  UefiCpuLib|UefiCpuPkg/Library/BaseUefiCpuLib/BaseUefiCpuLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf

  #
  # Generic Modules
  #
  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf
  UefiScsiLib|MdePkg/Library/UefiScsiLib/UefiScsiLib.inf
  OemHookStatusCodeLib|MdeModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
  SecurityManagementLib|MdeModulePkg/Library/DxeSecurityManagementLib/DxeSecurityManagementLib.inf
  UefiBootManagerLib|MdeModulePkg/Library/UefiBootManagerLib/UefiBootManagerLib.inf
  CustomizedDisplayLib|MdeModulePkg/Library/CustomizedDisplayLib/CustomizedDisplayLib.inf
  FspWrapperApiLib|IntelFsp2WrapperPkg/Library/BaseFspWrapperApiLib/BaseFspWrapperApiLib.inf
  FspWrapperPlatformLib|IntelFsp2WrapperPkg/Library/BaseFspWrapperPlatformLibSample/BaseFspWrapperPlatformLibSample.inf

  #
  # CPU
  #
  MtrrLib|UefiCpuPkg/Library/MtrrLib/MtrrLib.inf
  LocalApicLib|UefiCpuPkg/Library/BaseXApicLib/BaseXApicLib.inf

  #
  # Platform
  #
  TimerLib|UefiPayloadPkg/Library/AcpiTimerLib/AcpiTimerLib.inf
  ResetSystemLib|UefiPayloadPkg/Library/ResetSystemLib/ResetSystemLib.inf
  SerialPortLib|UefiPayloadPkg/Library/BaseSerialPortLib16550/BaseSerialPortLib16550.inf
  PlatformHookLib|UefiPayloadPkg/Library/PlatformHookLib/PlatformHookLib.inf
  PlatformBootManagerLib|UefiPayloadPkg/Library/PlatformBootManagerLib/PlatformBootManagerLib.inf
  IoApicLib|PcAtChipsetPkg/Library/BaseIoApicLib/BaseIoApicLib.inf

!if $(FTPM_ENABLE) == TRUE
  TcgPpVendorLib|SecurityPkg/Library/TcgPpVendorLibNull/TcgPpVendorLibNull.inf
  Tpm2CommandLib|SecurityPkg/Library/Tpm2CommandLib/Tpm2CommandLib.inf
  Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibRouter/Tpm2DeviceLibRouterDxe.inf
#  PttPtpLib|UefiPayloadPkg/Library/BroxtonSiPkg/Txe/Library/PeiDxePttPtpLib/PeiDxePttPtpLib.inf
  Tcg2PhysicalPresenceLib|SecurityPkg/Library/DxeTcg2PhysicalPresenceLib/DxeTcg2PhysicalPresenceLib.inf
  TpmMeasurementLib|SecurityPkg/Library/DxeTpmMeasurementLib/DxeTpmMeasurementLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
  Tcg2PpVendorLib|SecurityPkg/Library/Tcg2PpVendorLibNull/Tcg2PpVendorLibNull.inf
!endif
  #
  # Misc
  #
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
!if $(SOURCE_DEBUG_ENABLE) == TRUE
  PeCoffExtraActionLib|SourceLevelDebugPkg/Library/PeCoffExtraActionLibDebug/PeCoffExtraActionLibDebug.inf
  DebugCommunicationLib|SourceLevelDebugPkg/Library/DebugCommunicationLibSerialPort/DebugCommunicationLibSerialPort.inf
!else
  PeCoffExtraActionLib|MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
!endif
  PlatformInfoParseLib|UefiPayloadPkg/Library/PlatformInfoParseLib/PlatformInfoParseLib.inf
  DebugLib|MdeModulePkg/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  # DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  LockBoxLib|MdeModulePkg/Library/LockBoxNullLib/LockBoxNullLib.inf
  FileExplorerLib|MdeModulePkg/Library/FileExplorerLib/FileExplorerLib.inf
  #
  #API
  #
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  UserExtensionLib|UefiPayloadPkg/Library/UserExtensionLib/UserExtensionLib.inf
#  BaseServiceLib|UefiPayloadPkg/Library/APILib/BaseServiceLib/BaseServiceLib.inf
#  CustomLib|UefiPayloadPkg/Library/APILib/CustomLib/CustomLib.inf
#  ConsoleInOutLib|UefiPayloadPkg/Library/APILib/ConsoleInOutLib/ConsoleInOutLib.inf
#  FsAccessLib|UefiPayloadPkg/Library/APILib/FSaccessLib/FsAccessLib.inf
#  TimerEventLib|UefiPayloadPkg/Library/APILib/TimerEvent/TimerEvent.inf
  OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLib.inf
  IntrinsicLib|CryptoPkg/Library/IntrinsicLib/IntrinsicLib.inf
#  MmPciLib|UefiPayloadPkg/Library/PeiDxeSmmMmPciLib/PeiDxeSmmMmPciLib.inf
  
  #
  # Custom Platform Library
  #
  CustomPlatformLib|UefiPayloadPkg/Library/CustomPlatformLib/CustomPlatformLib.inf

!if $(FTPM_ENABLE) == FALSE
  TpmMeasurementLib|MdeModulePkg/Library/TpmMeasurementLibNull/TpmMeasurementLibNull.inf
!endif
  VarCheckLib|MdeModulePkg/Library/VarCheckLib/VarCheckLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
!if $(SECURE_BOOT_ENABLE) == TRUE
  AuthVariableLib|SecurityPkg/Library/AuthVariableLib/AuthVariableLib.inf
  PlatformSecureLib|SecurityPkg/Library/PlatformSecureLibNull/PlatformSecureLibNull.inf
!else
  AuthVariableLib|MdeModulePkg/Library/AuthVariableLibNull/AuthVariableLibNull.inf
!endif
#  VariableStorageSelectorLib|UefiPayloadPkg/Library/BaseVariableStorageSelectorLib/BaseVariableStorageSelectorLib.inf

[LibraryClasses.IA32.SEC]
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
  ReportStatusCodeLib|MdePkg/Library/BaseReportStatusCodeLibNull/BaseReportStatusCodeLibNull.inf

[LibraryClasses.IA32.PEI_CORE, LibraryClasses.IA32.PEIM]
  PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/PeiExtractGuidedSectionLib/PeiExtractGuidedSectionLib.inf
!if $(SOURCE_DEBUG_ENABLE)
  DebugAgentLib|SourceLevelDebugPkg/Library/DebugAgent/SecPeiDebugAgentLib.inf
!endif

[LibraryClasses.common.DXE_CORE]
 # DebugLib|MdeModulePkg/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  HobLib|MdePkg/Library/DxeCoreHobLib/DxeCoreHobLib.inf
  MemoryAllocationLib|MdeModulePkg/Library/DxeCoreMemoryAllocationLib/DxeCoreMemoryAllocationLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
!if $(SOURCE_DEBUG_ENABLE)
  DebugAgentLib|SourceLevelDebugPkg/Library/DebugAgent/DxeDebugAgentLib.inf
!endif
  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/DxeCpuExceptionHandlerLib.inf

[LibraryClasses.common.DXE_DRIVER]
 # DebugLib|MdeModulePkg/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/DxeCpuExceptionHandlerLib.inf
  MpInitLib|UefiCpuPkg/Library/MpInitLib/DxeMpInitLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
 # DebugLib|MdeModulePkg/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/RuntimeDxeReportStatusCodeLib/RuntimeDxeReportStatusCodeLib.inf
!if $(SECURE_BOOT_ENABLE) == TRUE
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/RuntimeCryptLib.inf
!endif
[LibraryClasses.common.UEFI_DRIVER,LibraryClasses.common.UEFI_APPLICATION]
#  DebugLib|MdeModulePkg/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf

[LibraryClasses.common.SMM_CORE]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  SmmServicesTableLib|MdeModulePkg/Library/PiSmmCoreSmmServicesTableLib/PiSmmCoreSmmServicesTableLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/SmmReportStatusCodeLib/SmmReportStatusCodeLib.inf
  MemoryAllocationLib|MdeModulePkg/Library/PiSmmCoreMemoryAllocationLib/PiSmmCoreMemoryAllocationLib.inf
  SmmCorePlatformHookLib|MdeModulePkg/Library/SmmCorePlatformHookLibNull/SmmCorePlatformHookLibNull.inf
  SmmMemLib|MdePkg/Library/SmmMemLib/SmmMemLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/SmmReportStatusCodeLib/SmmReportStatusCodeLib.inf
  MemoryAllocationLib|MdePkg/Library/SmmMemoryAllocationLib/SmmMemoryAllocationLib.inf
  SmmServicesTableLib|MdePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf
  SmmMemLib|MdePkg/Library/SmmMemLib/SmmMemLib.inf
#  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/SmmCryptLib.inf
  SmmCpuPlatformHookLib|UefiCpuPkg/Library/SmmCpuPlatformHookLibNull/SmmCpuPlatformHookLibNull.inf
  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/SmmCpuExceptionHandlerLib.inf
  SmmCpuFeaturesLib|UefiCpuPkg/Library/SmmCpuFeaturesLib/SmmCpuFeaturesLib.inf
#  DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
#  CseVariableStorageLib|UefiPayloadPkg/Library/BaseCseVariableStorageLib/BaseCseVariableStorageLib.inf
#  VariableNvmStorageLib|UefiPayloadPkg/Library/BaseVariableNvmStorageLib/BaseVariableNvmStorageLib.inf
#  SmmHeciMsgLib|UefiPayloadPkg/Library/SmmHeciMsgLib/SmmHeciMsgLib.inf
#  HeciMsgLib|UefiPayloadPkg/Library/HeciMsgLib/DxeSmmHeciMsgLib.inf
#  HeciInitLib|UefiPayloadPkg/Library/PeiDxeHeciInitLib/PeiDxeHeciInitLib.inf
#  Heci2PowerManagementLib|UefiPayloadPkg/Library/BaseHeci2PowerManagementNullLib/BaseHeci2PowerManagementNullLib.inf
#  CseVariableStorageSelectorLib|UefiPayloadPkg/Library/DxeSmmCseVariableStorageSelectorLib/SmmCseVariableStorageSelectorLib.inf
!if $(SECURE_BOOT_ENABLE) == TRUE
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/SmmCryptLib.inf
!endif
!if $(FTPM_ENABLE) == TRUE
  Tcg2PhysicalPresenceLib|SecurityPkg/Library/SmmTcg2PhysicalPresenceLib/SmmTcg2PhysicalPresenceLib.inf
!endif
################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform.
#
################################################################################
[PcdsFeatureFlag]
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseMemory|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdDxeIplSwitchToLongMode|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutGopSupport|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutUgaSupport|FALSE

[PcdsFixedAtBuild]
#bugbug Coreboot qemu  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x8000004F
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x10000
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxHardwareErrorVariableSize|0x8000
  gEfiMdeModulePkgTokenSpaceGuid.PcdVariableStoreSize|0x10000

  gEfiMdeModulePkgTokenSpaceGuid.PcdVpdBaseAddress|0x0
  gEfiMdeModulePkgTokenSpaceGuid.PcdUse1GPageTable|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdBootManagerMenuFile|{ 0x21, 0xaa, 0x2c, 0x46, 0x14, 0x76, 0x03, 0x45, 0x83, 0x6e, 0x8a, 0xb6, 0xf4, 0x66, 0x23, 0x31 }

  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress|$(PCIE_BASE)

  gIntelFsp2WrapperTokenSpaceGuid.PcdSkipFspApi|0x00010000
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiS3Enable|FALSE

!if $(SOURCE_DEBUG_ENABLE)
  gEfiSourceLevelDebugPkgTokenSpaceGuid.PcdDebugLoadImageMethod|0x2
!endif
!if $(FTPM_ENABLE) == TRUE
#ydwei set it to false to avoid reset at memory mapping difference when enable TPM
  gEfiMdeModulePkgTokenSpaceGuid.PcdResetOnMemoryTypeInformationChange|FALSE
!endif

!if $(SECURE_BOOT_ENABLE) == TRUE
  gEfiSecurityPkgTokenSpaceGuid.PcdUserPhysicalPresence|TRUE
!endif

!if $(SPECIAL_POOL) == TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdNullPointerDetectionPropertyMask|0x03
  gEfiMdeModulePkgTokenSpaceGuid.PcdHeapGuardPageType|0x7FFF
  gEfiMdeModulePkgTokenSpaceGuid.PcdHeapGuardPoolType|0x7FFF
  gEfiMdeModulePkgTokenSpaceGuid.PcdHeapGuardPropertyMask|0x0F
  gEfiMdeModulePkgTokenSpaceGuid.PcdCpuStackGuard|TRUE
  gUefiCpuPkgTokenSpaceGuid.PcdCpuSmmStaticPageTable|FALSE
!endif

[PcdsPatchableInModule.common]
  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x7
#bugbug coreboot qemu comment out below line
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x8000004F
!if $(SOURCE_DEBUG_ENABLE)
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x17
!else
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2F
!endif

  #
  # The following parameters are set by Library/PlatformHookLib
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialUseMmio|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x3f8
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialBaudRate|$(BAUD_RATE)
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterStride|1

  #
  # Enable these parameters to be set on the command line
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialClockRate|$(SERIAL_CLOCK_RATE)
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialLineControl|$(SERIAL_LINE_CONTROL)
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialUseHardwareFlowControl|$(SERIAL_HARDWARE_FLOW_CONTROL)
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialDetectCable|$(SERIAL_DETECT_CABLE)
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialFifoControl|$(SERIAL_FIFO_CONTROL)
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialExtendedTxFifoSize|$(SERIAL_EXTENDED_TX_FIFO_SIZE)

  gEfiMdeModulePkgTokenSpaceGuid.PcdPciDisableBusEnumeration|TRUE
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultBaudRate|$(UART_DEFAULT_BAUD_RATE)
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultDataBits|$(UART_DEFAULT_DATA_BITS)
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultParity|$(UART_DEFAULT_PARITY)
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultStopBits|$(UART_DEFAULT_STOP_BITS)
  gEfiMdePkgTokenSpaceGuid.PcdDefaultTerminalType|$(DEFAULT_TERMINAL_TYPE)
  gEfiMdeModulePkgTokenSpaceGuid.PcdPciSerialParameters|$(PCI_SERIAL_PARAMETERS)

  gUefiCpuPkgTokenSpaceGuid.PcdCpuMaxLogicalProcessorNumber|$(MAX_LOGICAL_PROCESSORS)
  gUefiCpuPkgTokenSpaceGuid.PcdCpuSmmStackSize|0x3000

  #
  # Set the proper Shell file GUID
  #
!if $(SHELL_TYPE) == FULL_BIN
  # c57ad6b7-0515-40a8-9d21-551652854e37
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdShellFile|{ 0xB7, 0xD6, 0x7A, 0xC5, 0x15, 0x05, 0xA8, 0x40, 0x9D, 0x21, 0x55, 0x16, 0x52, 0x85, 0x4E, 0x37 }
!else
  # 7C04A583-9E3E-4f1c-AD65-E05268D0B4D1
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdShellFile|{ 0x83, 0xA5, 0x04, 0x7C, 0x3E, 0x9E, 0x1c, 0x4f, 0xAD, 0x65, 0xE0, 0x52, 0x68, 0xD0, 0xB4, 0xD1 }
!endif

################################################################################
#
# Pcd Dynamic Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsDynamicDefault]
  gEfiMdeModulePkgTokenSpaceGuid.PcdEmuVariableNvStoreReserved|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableBase64|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingBase|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareBase|0
  gEfiMdePkgTokenSpaceGuid.PcdPlatformBootTimeOut|3
  gIntelFsp2WrapperTokenSpaceGuid.PcdFspsBaseAddress|0
  gUefiPayloadPkgTokenSpaceGuid.PcdFspHobList|0

  ## This PCD defines the video horizontal resolution.
  #  This PCD could be set to 0 then video resolution could be at highest resolution.
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoHorizontalResolution|0
  ## This PCD defines the video vertical resolution.
  #  This PCD could be set to 0 then video resolution could be at highest resolution.
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoVerticalResolution|0

  ## The PCD is used to specify the video horizontal resolution of text setup.
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoHorizontalResolution|0
  ## The PCD is used to specify the video vertical resolution of text setup.
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoVerticalResolution|0

  gEfiSecurityPkgTokenSpaceGuid.PcdTpmInstanceGuid |{0x66, 0x6f, 0xd6, 0x93, 0xda, 0x55, 0x03, 0x4f, 0x9b, 0x5f, 0x32, 0xcf, 0x9e, 0x54, 0x3b, 0x3a}

  # (BIT0 - SHA1. BIT1 - SHA256)
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2HashMask|0x00000003
  gEfiSecurityPkgTokenSpaceGuid.PcdTcg2HashAlgorithmBitmap|0x00000003


################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform.
#
################################################################################
[Components.IA32]
  #
  # SEC Core
  #
  UefiPayloadPkg/Drivers/SecCore/SecCore.inf

  #
  # PEI Core
  #
  MdeModulePkg/Core/Pei/PeiMain.inf

  #
  # PEIM
  #
  MdeModulePkg/Universal/PCD/Pei/Pcd.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }
  MdeModulePkg/Universal/ReportStatusCodeRouter/Pei/ReportStatusCodeRouterPei.inf
  MdeModulePkg/Universal/StatusCodeHandler/Pei/StatusCodeHandlerPei.inf
  UefiPayloadPkg/Drivers/UefiPayloadPei/UefiPayloadPei.inf
  #
  # Vtd support
  #
  IntelSiliconPkg/Feature/VTd/IntelVTdPmrPei/IntelVTdPmrPei.inf
  MdeModulePkg/Core/DxeIplPeim/DxeIpl.inf

[Components.X64]
  #
  # DXE Core
  #
  MdeModulePkg/Core/Dxe/DxeMain.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
      #
      # bugbug NOTE: has to use a NULL serial lib instance as our normal serial lib init uses 
      # dynamic PCD service which is not available at early DXE core
      #
      SerialPortLib|MdePkg/Library/BaseSerialPortLibNull/BaseSerialPortLibNull.inf
  }

  #
  # Components that produce the architectural protocols
  #
 MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf {
    <LibraryClasses>
    !if $(SECURE_BOOT_ENABLE) == TRUE
      NULL|SecurityPkg/Library/DxeImageVerificationLib/DxeImageVerificationLib.inf
    !endif
    !if $(FTPM_ENABLE) == TRUE
      NULL|SecurityPkg/Library/DxeTpm2MeasureBootLib/DxeTpm2MeasureBootLib.inf
    !endif
 }

  UefiCpuPkg/CpuDxe/CpuDxe.inf
  MdeModulePkg/Universal/BdsDxe/BdsDxe.inf
  MdeModulePkg/Application/UiApp/UiApp.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/DeviceManagerUiLib/DeviceManagerUiLib.inf
      NULL|MdeModulePkg/Library/BootManagerUiLib/BootManagerUiLib.inf
      NULL|MdeModulePkg/Library/BootMaintenanceManagerUiLib/BootMaintenanceManagerUiLib.inf
  }
!if $(USE_HPET_TIMER) == TRUE
  PcAtChipsetPkg/HpetTimerDxe/HpetTimerDxe.inf
!else
  PcAtChipsetPkg/8254TimerDxe/8254Timer.inf
!endif
  MdeModulePkg/Universal/Metronome/Metronome.inf
  MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf
  MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf
  MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  MdeModulePkg/Universal/MonotonicCounterRuntimeDxe/MonotonicCounterRuntimeDxe.inf
  MdeModulePkg/Universal/ResetSystemRuntimeDxe/ResetSystemRuntimeDxe.inf
  PcAtChipsetPkg/PcatRealTimeClockRuntimeDxe/PcatRealTimeClockRuntimeDxe.inf
!if $(EMU_VARIABLE) == TRUE
  MdeModulePkg/Universal/Variable/EmuRuntimeDxe/EmuVariableRuntimeDxe.inf
!else
#  UefiPayloadPkg/Drivers/VariableStorage/Dxe/CseVariableStorageSmmRuntimeDxe/CseVariableStorageSmmRuntimeDxe.inf
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableSmmRuntimeDxe.inf {
    <LibraryClasses>
      !if $(SECURE_BOOT_ENABLE) == FALSE
        TpmMeasurementLib|MdeModulePkg/Library/TpmMeasurementLibNull/TpmMeasurementLibNull.inf
      !endif
  }
#  UefiPayloadPkg/Drivers/Variable/RuntimeDxe/VariableSmm.inf{
#    <LibraryClasses>
#      !if $(SECURE_BOOT_ENABLE) == TRUE
#        PlatformSecureLib|SecurityPkg/Library/PlatformSecureLibNull/PlatformSecureLibNull.inf
#        AuthVariableLib|SecurityPkg/Library/AuthVariableLib/AuthVariableLib.inf
#      !else
#        AuthVariableLib|MdeModulePkg/Library/AuthVariableLibNull/AuthVariableLibNull.inf
#      !endif
#  }
!endif

  #
  # Following are the DXE drivers
  #
  MdeModulePkg/Universal/PCD/Dxe/Pcd.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }

  MdeModulePkg/Universal/ReportStatusCodeRouter/RuntimeDxe/ReportStatusCodeRouterRuntimeDxe.inf
  MdeModulePkg/Universal/StatusCodeHandler/RuntimeDxe/StatusCodeHandlerRuntimeDxe.inf
  UefiCpuPkg/CpuIo2Dxe/CpuIo2Dxe.inf
  MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf
  MdeModulePkg/Universal/MemoryTest/NullMemoryTestDxe/NullMemoryTestDxe.inf
  PcAtChipsetPkg/8259InterruptControllerDxe/8259.inf
  MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf
  MdeModulePkg/Universal/SetupBrowserDxe/SetupBrowserDxe.inf
  MdeModulePkg/Universal/DisplayEngineDxe/DisplayEngineDxe.inf

  UefiPayloadPkg/Drivers/UefiPayloadDxe/UefiPayloadDxe.inf

!if $(SECURE_BOOT_ENABLE) == TRUE
  SecurityPkg/VariableAuthenticated/SecureBootConfigDxe/SecureBootConfigDxe.inf
!endif
  #
  # SMBIOS Support
  #
  MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf

  #
  # ACPI Support
  #
  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf

  #
  # PCI Support
  #
  MdeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf
  MdeModulePkg/Bus/Pci/PciHostBridgeDxe/PciHostBridgeDxe.inf {
    <LibraryClasses>
      PciHostBridgeLib|UefiPayloadPkg/Library/PciHostBridgeLib/PciHostBridgeLib.inf
  }

!if $(SMM_ENABLE) == TRUE
  #
  # SMM
  #
  MdeModulePkg/Universal/ReportStatusCodeRouter/Smm/ReportStatusCodeRouterSmm.inf
  MdeModulePkg/Universal/StatusCodeHandler/Smm/StatusCodeHandlerSmm.inf
  MdeModulePkg/Core/PiSmmCore/PiSmmIpl.inf
  MdeModulePkg/Core/PiSmmCore/PiSmmCore.inf
  UefiCpuPkg/PiSmmCpuDxeSmm/PiSmmCpuDxeSmm.inf
  UefiCpuPkg/CpuIo2Smm/CpuIo2Smm.inf
  UefiPayloadPkg/Drivers/SwSmiDispatcher/Smm/SwSmiDispatcher.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/VarCheckUefiLib/VarCheckUefiLib.inf
      NULL|MdeModulePkg/Library/VarCheckHiiLib/VarCheckHiiLib.inf
      NULL|MdeModulePkg/Library/VarCheckPcdLib/VarCheckPcdLib.inf
  }
  UefiPayloadPkg/Drivers/SmmAccessDxe/SmmAccessDxe.inf
  UefiPayloadPkg/Drivers/SmmControlDxe/SmmControlDxe.inf
!endif

!if $(FTPM_ENABLE) == TRUE
    SecurityPkg/Tcg/MemoryOverwriteControl/TcgMor.inf
    SecurityPkg/Tcg/MemoryOverwriteRequestControlLock/TcgMorLockSmm.inf
#     SecurityPkg/Tcg/Tcg2Config/Tcg2ConfigDxe.inf
    SecurityPkg/Tcg/Tcg2Dxe/Tcg2Dxe.inf {
      <LibraryClasses>
        Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibRouter/Tpm2DeviceLibRouterDxe.inf
        NULL|UefiPayloadPkg/Library/Tpm2InstanceLib/Tpm2InstanceLib.inf
        HashLib|SecurityPkg/Library/HashLibBaseCryptoRouter/HashLibBaseCryptoRouterDxe.inf
        NULL|SecurityPkg/Library/HashInstanceLibSha1/HashInstanceLibSha1.inf
        NULL|SecurityPkg/Library/HashInstanceLibSha256/HashInstanceLibSha256.inf
#	HeciMsgLib|UefiPayloadPkg/Library/HeciMsgLib/DxeSmmHeciMsgLib.inf
#        HeciInitLib|UefiPayloadPkg/Library/PeiDxeHeciInitLib/PeiDxeHeciInitLib.inf
#	Heci2PowerManagementLib|UefiPayloadPkg/Library/BaseHeci2PowerManagementNullLib/BaseHeci2PowerManagementNullLib.inf
    }
!endif #FTPM_ENABLE

  #
  # SCSI/ATA/IDE/DISK Support
  #
  MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf
  FatPkg/EnhancedFatDxe/Fat.inf
  UefiPayloadPkg/Drivers/SataControllerDxe/SataControllerDxe.inf
  MdeModulePkg/Bus/Ata/AtaBusDxe/AtaBusDxe.inf
  MdeModulePkg/Bus/Ata/AtaAtapiPassThru/AtaAtapiPassThru.inf
  MdeModulePkg/Bus/Scsi/ScsiBusDxe/ScsiBusDxe.inf
  MdeModulePkg/Bus/Scsi/ScsiDiskDxe/ScsiDiskDxe.inf

  #
  # SD/eMMC Support
  #
  MdeModulePkg/Bus/Pci/SdMmcPciHcDxe/SdMmcPciHcDxe.inf
  MdeModulePkg/Bus/Sd/EmmcDxe/EmmcDxe.inf
  MdeModulePkg/Bus/Sd/SdDxe/SdDxe.inf

  #
  # Usb Support
  #
  MdeModulePkg/Bus/Pci/UhciDxe/UhciDxe.inf
  MdeModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf
  MdeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf
  MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  MdeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
  MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf

  #
  # OHCI support
  #
  #QuarkSocPkg/QuarkSouthCluster/Usb/Ohci/Dxe/OhciDxe.inf

  #
  # ISA Support
  #
  MdeModulePkg/Universal/SerialDxe/SerialDxe.inf
  PcAtChipsetPkg/IsaAcpiDxe/IsaAcpi.inf
  IntelFrameworkModulePkg/Bus/Isa/IsaBusDxe/IsaBusDxe.inf
  IntelFrameworkModulePkg/Bus/Isa/Ps2KeyboardDxe/Ps2keyboardDxe.inf

  #
  # Console Support
  #
  MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  MdeModulePkg/Universal/Console/TerminalDxe/TerminalDxe.inf

  #
  # Framebuffer Gop
  #
  UefiPayloadPkg/Drivers/FbGop/FbGop.inf

  #
  # FSP Notify
  #
  IntelFsp2WrapperPkg/FspWrapperNotifyDxe/FspWrapperNotifyDxe.inf
  
  #
  # Vtd support
  #
  IntelSiliconPkg/Feature/VTd/IntelVTdDxe/IntelVTdDxe.inf
  
  #------------------------------
  #  Build the Custom Boot APP
  #------------------------------  
  UefiPayloadPkg/Application/CustomBoot.inf
  
  #------------------------------
  #  Build the shell
  #------------------------------

!if $(SHELL_TYPE) == BUILD_SHELL

[PcdsFixedAtBuild]
  ## This flag is used to control initialization of the shell library
  #  This should be FALSE for compiling the shell application itself only.
  gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE

  #
  # Shell Lib
  #
[LibraryClasses]
  BcfgCommandLib|ShellPkg/Library/UefiShellBcfgCommandLib/UefiShellBcfgCommandLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf

[Components.X64]
  ShellPkg/Application/Shell/Shell.inf {

    #------------------------------
    #  Basic commands
    #------------------------------

    <LibraryClasses>
      NULL|ShellPkg/Library/UefiShellLevel1CommandsLib/UefiShellLevel1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel2CommandsLib/UefiShellLevel2CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel3CommandsLib/UefiShellLevel3CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDriver1CommandsLib/UefiShellDriver1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellInstall1CommandsLib/UefiShellInstall1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.inf

    #------------------------------
    #  Networking commands
    #------------------------------

    <LibraryClasses>
      NULL|ShellPkg/Library/UefiShellNetwork1CommandsLib/UefiShellNetwork1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellTftpCommandLib/UefiShellTftpCommandLib.inf

    #------------------------------
    #  Performance command
    #------------------------------

    <LibraryClasses>
      NULL|ShellPkg/Library/UefiDpLib/UefiDpLib.inf

    #------------------------------
    #  Support libraries
    #------------------------------

    <LibraryClasses>
      DebugLib|MdePkg/Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
      DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
      FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
      HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf
      NetLib|MdeModulePkg/Library/DxeNetLib/DxeNetLib.inf
      PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
      ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf
      ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
      ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
      SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf
  }

!endif
