## @file UefiHostUnitTestCasePkg.dsc
# 
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  PLATFORM_NAME                  = UefiHostUnitTestPkg
  PLATFORM_GUID                  = 3074D854-4F09-4E4E-BB6B-DE0D7282AC40
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/UefiHostUnitTestPkg
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

  DEFINE UNIT_TEST_XML_MODE = FALSE

  # Valid option: HOST, CUNIT, CMOCKA
  DEFINE UNIT_TEST_FRAMEWORK_MODE = CUNIT

[PcdsFixedAtBuild]
!if $(UNIT_TEST_XML_MODE)
  gUefiHostUnitTestPkgTokenSpaceGuid.HostUnitTestMode|0x1
!endif

[LibraryClasses]
  BaseLib|UefiHostTestPkg/Library/BaseLibHost/BaseLibHost.inf
  CacheMaintenanceLib|UefiHostTestPkg/Library/BaseCacheMaintenanceLibHost/BaseCacheMaintenanceLibHost.inf
  BaseMemoryLib|UefiHostTestPkg/Library/BaseMemoryLibHost/BaseMemoryLibHost.inf
  MemoryAllocationLib|UefiHostTestPkg/Library/MemoryAllocationLibHost/MemoryAllocationLibHost.inf
  DebugLib|UefiHostTestPkg/Library/DebugLibHost/DebugLibHost.inf
  UefiBootServicesTableLib|UefiHostTestPkg/Library/UefiBootServicesTableLibHost/UefiBootServicesTableLibHost.inf
  HobLib|UefiHostTestPkg/Library/HobLibHost/HobLibHost.inf
  DevicePathLib|UefiHostTestPkg/Library/UefiDevicePathLibHost/UefiDevicePathLibHost.inf
  DxeServicesTableLib|UefiHostTestPkg/Library/DxeServicesTableLibHost/DxeServicesTableLibHost.inf
  UefiRuntimeServicesTableLib|UefiHostTestPkg/Library/UefiRuntimeServicesTableLibHost/UefiRuntimeServicesTableLibHost.inf
  SmmServicesTableLib|UefiHostTestPkg/Library/SmmServicesTableLibHost/SmmServicesTableLibHost.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  PeiServicesTablePointerLib|UefiHostTestPkg/Library/PeiServicesTablePointerLibHost/PeiServicesTablePointerLibHost.inf

  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  ReportStatusCodeLib|MdePkg/Library/BaseReportStatusCodeLibNull/BaseReportStatusCodeLibNull.inf
  SafeIntLib|MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf

  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf

  SecurityManagementLib|MdeModulePkg/Library/DxeSecurityManagementLib/DxeSecurityManagementLib.inf
  TpmMeasurementLib|SecurityPkg/Library/DxeTpmMeasurementLib/DxeTpmMeasurementLib.inf
  PeCoffLib|MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
  PeCoffExtraActionLib|MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf

  CpuLib|UefiHostTestPkg/Library/BaseCpuLibHost/BaseCpuLibHost.inf

  PciHostBridgeStubLib|UefiHostUnitTestCasePkg/TestStub/PciHostBridgeStubLib/PciHostBridgeStubLib.inf
  PciSegmentLib|UefiHostUnitTestCasePkg/TestStub/PciSegmentStubLib/PciSegmentStubLib.inf
  PciSegmentStubLib|UefiHostUnitTestCasePkg/TestStub/PciSegmentStubLib/PciSegmentStubLib.inf
  MtrrStubLib|UefiHostUnitTestCasePkg/TestStub/MtrrStubLib/MtrrStubLib.inf

  OsServiceLib|UefiHostTestPkg/Library/OsServiceLibHost/OsServiceLibHost.inf

!if $(UNIT_TEST_FRAMEWORK_MODE) == HOST
  UnitTestAssertLib|UnitTestPkg/Library/UnitTestAssertLib/UnitTestAssertLib.inf
  UnitTestLogLib|UnitTestPkg/Library/UnitTestLogLib/UnitTestLogLib.inf
  UnitTestLib|UefiHostUnitTestPkg/Library/UnitTestLibHost/UnitTestLibHost.inf
  UnitTestBootLib|UefiHostUnitTestPkg/Library/UnitTestBootLibHost/UnitTestBootLibHost.inf
  UnitTestPersistenceLib|UefiHostUnitTestPkg/Library/UnitTestPersistenceLibHost/UnitTestPersistenceLibHost.inf
  UnitTestResultReportLib|UnitTestPkg/Library/UnitTestResultReportLibDebug/UnitTestResultReportLibDebug.inf
!if $(UNIT_TEST_XML_MODE)
  XmlTreeLib|XmlSupportPkg/Library/XmlTreeLib/XmlTreeLib.inf
  UnitTestResultReportLib|UefiHostUnitTestPkg/Library/UnitTestResultReportLibJUnitFormatHost/UnitTestResultReportLibJUnitFormatHost.inf
!endif
!endif

!if $(UNIT_TEST_FRAMEWORK_MODE) == CUNIT
  UnitTestLib|UefiHostUnitTestPkg/Library/UnitTestLibCUnit/UnitTestLibCUnit.inf
  UnitTestAssertLib|UefiHostUnitTestPkg/Library/UnitTestAssertLibCUnit/UnitTestAssertLibCUnit.inf
!endif

!if $(UNIT_TEST_FRAMEWORK_MODE) == CMOCKA
  UnitTestLib|UefiHostUnitTestPkg/Library/UnitTestLibcmocka/UnitTestLibcmocka.inf
  UnitTestAssertLib|UefiHostUnitTestPkg/Library/UnitTestAssertLibcmocka/UnitTestAssertLibcmocka.inf
!endif

[LibraryClasses.common.USER_DEFINED]

[Components]
  UefiHostUnitTestPkg/Sample/SampleUnitTestHost/SampleUnitTestHost.inf

!if $(UNIT_TEST_FRAMEWORK_MODE) == CUNIT
  UefiHostUnitTestPkg/Sample/SampleUnitTestCUnit/SampleUnitTestCUnit.inf
!endif

!if $(UNIT_TEST_FRAMEWORK_MODE) == CMOCKA
  UefiHostUnitTestPkg/Sample/SampleUnitTestcmocka/SampleUnitTestcmocka.inf
!endif

!include UefiHostUnitTestPkg/UefiHostUnitTestBuildOption.dsc
