## @file UefiHostUnitTestCasePkg.dsc
# 
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  PLATFORM_NAME                  = UefiHostUnitTestCasePkg
  PLATFORM_GUID                  = 85CFAA0E-7F82-440A-AB48-45C6D2C14689
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/UefiHostUnitTestCasePkg
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = UefiHostUnitTestCasePkg/UefiHostUnitTestCasePkg.fdf

  DEFINE OPENSSL_FLAGS           = -DL_ENDIAN -DOPENSSL_SMALL_FOOTPRINT -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -DNO_SYSLOG
  DEFINE OPENSSL_TEST_ENABLE = FALSE

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
  MmServicesTableLib|UefiHostTestPkg/Library/SmmServicesTableLibHost/SmmServicesTableLibHost.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  PeiServicesTablePointerLib|UefiHostTestPkg/Library/PeiServicesTablePointerLibHost/PeiServicesTablePointerLibHost.inf
  UefiDriverEntryPoint|UefiHostTestPkg/Library/UefiDriverEntryPointHost/UefiDriverEntryPointHost.inf
  PeimEntryPoint|UefiHostTestPkg/Library/PeimEntryPointHost/PeimEntryPointHost.inf

  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  ReportStatusCodeLib|MdePkg/Library/BaseReportStatusCodeLibNull/BaseReportStatusCodeLibNull.inf
  SerialPortLib|MdePkg/Library/BaseSerialPortLibNull/BaseSerialPortLibNull.inf
  SafeIntLib|MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
  RngLib|MdePkg/Library/BaseRngLib/BaseRngLib.inf
  TimerLib|UefiHostTestPkg/Library/BaseTimerLibHost/BaseTimerLibHost.inf

  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf

  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
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
  
  FatPeiLib|FatPkg/FatPei/FatPei.inf

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
  CmockaLib|CmockaHostUnitTestPkg/Library/CmockaLib/CmockaLib.inf
!endif

!if $(OPENSSL_TEST_ENABLE)
  BaseCryptLib|UefiHostCryptoPkg/Library/BaseCryptLibHost/BaseCryptLibHost.inf
  OpensslLib|UefiHostCryptoPkg/Library/OpensslLib/OpensslLib.inf
!endif

[LibraryClasses.common.USER_DEFINED]


[Components]

!if $(OPENSSL_TEST_ENABLE)
  UefiHostCryptoPkg/Library/OpensslLib/OpensslLib.inf {
  <BuildOptions>
    #
    # Disables the following Visual Studio compiler warnings brought by openssl source,
    # so we do not break the build with /WX option:
    #   C4090: 'function' : different 'const' qualifiers
    #   C4244: conversion from type1 to type2, possible loss of data
    #   C4245: conversion from type1 to type2, signed/unsigned mismatch
    #   C4267: conversion from size_t to type, possible loss of data
    #   C4306: 'identifier' : conversion from 'type1' to 'type2' of greater size
    #   C4389: 'operator' : signed/unsigned mismatch (xxxx)
    #   C4702: unreachable code
    #   C4706: assignment within conditional expression
    #   C4819: The file contains a character that cannot be represented in the current code page
    #
    MSFT:*_*_IA32_CC_FLAGS   = -U_WIN32 -U_WIN64 -U_MSC_VER $(OPENSSL_FLAGS) /wd4090 /wd4244 /wd4245 /wd4267 /wd4389 /wd4702 /wd4706 /wd4819
    MSFT:*_*_X64_CC_FLAGS    = -U_WIN32 -U_WIN64 -U_MSC_VER $(OPENSSL_FLAGS) /wd4090 /wd4244 /wd4245 /wd4267 /wd4306 /wd4389 /wd4702 /wd4706 /wd4819

    INTEL:*_*_IA32_CC_FLAGS  = -U_WIN32 -U_WIN64 -U_MSC_VER -U__ICC $(OPENSSL_FLAGS) /w
    INTEL:*_*_X64_CC_FLAGS   = -U_WIN32 -U_WIN64 -U_MSC_VER -U__ICC $(OPENSSL_FLAGS) /w

    #
    # Suppress the following build warnings in openssl so we don't break the build with -Werror
    #   -Werror=maybe-uninitialized: there exist some other paths for which the variable is not initialized.
    #   -Werror=format: Check calls to printf and scanf, etc., to make sure that the arguments supplied have
    #                   types appropriate to the format string specified.
    #
    GCC:*_*_IA32_CC_FLAGS    = -U_WIN32 -U_WIN64 $(OPENSSL_FLAGS) -Wno-error=maybe-uninitialized
    GCC:*_*_X64_CC_FLAGS     = -U_WIN32 -U_WIN64 $(OPENSSL_FLAGS) -Wno-error=maybe-uninitialized -Wno-error=format -Wno-format -DNO_MSABI_VA_FUNCS
    GCC:*_*_ARM_CC_FLAGS     = $(OPENSSL_FLAGS) -Wno-error=maybe-uninitialized
    GCC:*_*_AARCH64_CC_FLAGS = $(OPENSSL_FLAGS) -Wno-error=maybe-uninitialized -Wno-format

    # suppress the following warnings in openssl so we don't break the build with warnings-as-errors:
    # 1295: Deprecated declaration <entity> - give arg types
    #  550: <entity> was set but never used
    # 1293: assignment in condition
    #  111: statement is unreachable (invariably "break;" after "return X;" in case statement)
    #   68: integer conversion resulted in a change of sign ("if (Status == -1)")
    #  177: <entity> was declared but never referenced
    #  223: function <entity> declared implicitly
    #  144: a value of type <type> cannot be used to initialize an entity of type <type>
    #  513: a value of type <type> cannot be assigned to an entity of type <type>
    #  188: enumerated type mixed with another type (i.e. passing an integer as an enum without a cast)
    # 1296: Extended constant initialiser used
    #  128: loop is not reachable - may be emitted inappropriately if code follows a conditional return
    #       from the function that evaluates to true at compile time
    #  546: transfer of control bypasses initialization - may be emitted inappropriately if the uninitialized
    #       variable is never referenced after the jump
    #    1: ignore "#1-D: last line of file ends without a newline"
    # 3017: <entity> may be used before being set (NOTE: This was fixed in OpenSSL 1.1 HEAD with
    #       commit d9b8b89bec4480de3a10bdaf9425db371c19145b, and can be dropped then.)
    RVCT:*_*_ARM_CC_FLAGS     = $(OPENSSL_FLAGS) --library_interface=aeabi_clib99 --diag_suppress=1296,1295,550,1293,111,68,177,223,144,513,188,128,546,1,3017 -JCryptoPkg/Include
    XCODE:*_*_IA32_CC_FLAGS   = -mmmx -msse -U_WIN32 -U_WIN64 $(OPENSSL_FLAGS) -w
    XCODE:*_*_X64_CC_FLAGS    = -mmmx -msse -U_WIN32 -U_WIN64 $(OPENSSL_FLAGS) -w

    #
    # AARCH64 uses strict alignment and avoids SIMD registers for code that may execute
    # with the MMU off. This involves SEC, PEI_CORE and PEIM modules as well as BASE
    # libraries, given that they may be included into such modules.
    # This library, even though of the BASE type, is never used in such cases, and
    # avoiding the SIMD register file (which is shared with the FPU) prevents the
    # compiler from successfully building some of the OpenSSL source files that
    # use floating point types, so clear the flags here.
    #
    GCC:*_*_AARCH64_CC_XIPFLAGS ==
  }
!endif

[Components]
!if $(UNIT_TEST_FRAMEWORK_MODE) == CMOCKA
  CmockaHostUnitTestPkg/Library/CmockaLib/CmockaLib.inf {
  <BuildOptions>
    MSFT:*_*_*_CC_FLAGS      ==  /c /D _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1 /D _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1 /D _CRT_NONSTDC_NO_WARNINGS=1 /D _CRT_SECURE_NO_WARNINGS=1 -DHAVE_VSNPRINTF -DHAVE_SNPRINTF
  
    GCC:*_*_IA32_CC_FLAGS    == -m32 -O0 -g -fprofile-arcs -ftest-coverage -std=gnu99 -Wpedantic -Wall -Wshadow -Wmissing-prototypes -Wcast-align -Werror=address -Wstrict-prototypes -Werror=strict-prototypes -Wwrite-strings -Werror=write-strings -Werror-implicit-function-declaration -Wpointer-arith -Werror=pointer-arith -Wdeclaration-after-statement -Werror=declaration-after-statement -Wreturn-type -Werror=return-type -Wuninitialized -Werror=uninitialized -Werror=strict-overflow -Wstrict-overflow=2 -Wno-format-zero-length -Wmissing-field-initializers -Wformat-security -Werror=format-security -fno-common -Wformat -fno-common -fstack-protector-strong -DHAVE_SIGNAL_H
    GCC:*_*_X64_CC_FLAGS     == -m64 -O0 -g -fprofile-arcs -ftest-coverage -std=gnu99 -Wpedantic -Wall -Wshadow -Wmissing-prototypes -Wcast-align -Werror=address -Wstrict-prototypes -Werror=strict-prototypes -Wwrite-strings -Werror=write-strings -Werror-implicit-function-declaration -Wpointer-arith -Werror=pointer-arith -Wdeclaration-after-statement -Werror=declaration-after-statement -Wreturn-type -Werror=return-type -Wuninitialized -Werror=uninitialized -Werror=strict-overflow -Wstrict-overflow=2 -Wno-format-zero-length -Wmissing-field-initializers -Wformat-security -Werror=format-security -fno-common -Wformat -fno-common -fstack-protector-strong -DHAVE_SIGNAL_H
  }
!endif

  UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/TestPeiGpt.inf {
  <LibraryClasses>
    NULL|UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/Override/FatPei.inf
  }
  UefiHostUnitTestCasePkg/TestCase/MdePkg/Library/BaseSafeIntLib/TestBaseSafeIntLib.inf
  UefiHostUnitTestCasePkg/TestCase/MdePkg/Library/UefiBootServicesTableLib/TestUefiBootServicesTableLib.inf
  UefiHostUnitTestCasePkg/TestCase/MdePkg/Library/UefiRuntimeServicesTableLib/TestUefiRuntimeServicesTableLib.inf
  UefiHostUnitTestCasePkg/TestCase/MdePkg/Library/DxeServicesTableLib/TestDxeServicesTableLib.inf
  UefiHostUnitTestCasePkg/TestCase/MdePkg/Library/SmmServicesTableLib/TestSmmServicesTableLib.inf
  UefiHostUnitTestCasePkg/TestCase/MdePkg/Library/PeiServicesLib/TestPeiServicesLib.inf
  UefiHostUnitTestCasePkg/TestCase/MdePkg/Library/MemoryAllocationLib/TestMemoryAllocationLib.inf
  UefiHostUnitTestCasePkg/TestCase/MdePkg/Library/PcdLib/TestPcdLibStatic.inf {
  <LibraryClasses>
    PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  <PcdsPatchableInModule>
    gTestCasePkgTokenSpaceGuid.PcdTestPatchableInModuleVoidStrAsc|"StrAsc"|VOID*|12
    gTestCasePkgTokenSpaceGuid.PcdTestPatchableInModuleVoidStrUni|L"StrUni"|VOID*|28
    gTestCasePkgTokenSpaceGuid.PcdTestPatchableInModuleVoidBufAll0|{0x0, 0x0, 0x0}|VOID*|5
    gTestCasePkgTokenSpaceGuid.PcdTestPatchableInModuleVoidBufAll1|{0xFF, 0xFF, 0xFF}|VOID*|5
  }
  UefiHostUnitTestCasePkg/TestCase/MdePkg/Library/PcdLib/TestPcdLibDynamic.inf {
  <LibraryClasses>
    PcdLib|UefiHostTestPkg/Library/BasePcdLibHost/BasePcdLibHost.inf
  }

  UefiHostUnitTestCasePkg/TestCase/MdeModulePkg/Bus/Pci/PciBusDxe/TestPciBusDxe.inf {
  <LibraryClasses>
    NULL|MdeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf
  }

  UefiHostUnitTestCasePkg/TestCase/MdeModulePkg/Universal/RegularExpressionDxe/Override/RegularExpressionDxe.inf {
  <BuildOptions>
    # Enable STDARG for variable arguments
    *_*_*_CC_FLAGS = -DHAVE_STDARG_H -Dsprintf_s=SPRINTF_S

    # Override MSFT build option to remove /Oi and /GL
    MSFT:*_*_*_CC_FLAGS          = /GL-
    INTEL:*_*_*_CC_FLAGS         = /Oi-

    # Oniguruma: potentially uninitialized local variable used
    MSFT:*_*_*_CC_FLAGS = /wd4701 /wd4703

    # Oniguruma: intrinsic function not declared
    MSFT:*_*_*_CC_FLAGS = /wd4164

    # Oniguruma: old style declaration in st.c
    MSFT:*_*_*_CC_FLAGS = /wd4131

    # Oniguruma: 'type cast' : truncation from 'OnigUChar *' to 'unsigned int'
    MSFT:*_*_*_CC_FLAGS = /wd4305 /wd4306

    # Oniguruma: nameless union declared in regparse.h
    MSFT:*_*_*_CC_FLAGS = /wd4201

    # Oniguruma: 'type cast' : "int" to "OnigUChar", function pointer to "void *"
    MSFT:*_*_*_CC_FLAGS = /wd4244 /wd4054

    # Oniguruma: previous local declaration
    MSFT:*_*_*_CC_FLAGS = /wd4456

    # Oniguruma: signed and unsigned mismatch/cast
    MSFT:*_*_*_CC_FLAGS = /wd4018 /wd4245 /wd4389

    # Oniguruma: tag_end in parse_callout_of_name
    GCC:*_*_*_CC_FLAGS = -Wno-error=maybe-uninitialized

    # Not add -Wno-error=maybe-uninitialized option for XCODE
    # XCODE doesn't know this option
    XCODE:*_*_*_CC_FLAGS =
  }
  UefiHostUnitTestCasePkg/TestCase/MdeModulePkg/Universal/RegularExpressionDxe/TestRegularExpressionDxe.inf {
  <LibraryClasses>
    NULL|UefiHostUnitTestCasePkg/TestCase/MdeModulePkg/Universal/RegularExpressionDxe/Override/RegularExpressionDxe.inf
  }

  UefiHostUnitTestCasePkg/TestCase/UefiCpuPkg/Library/MtrrLib/TestMtrrLib.inf {
  <LibraryClasses>
    NULL|UefiCpuPkg/Library/MtrrLib/MtrrLib.inf
  }

  UefiHostUnitTestCasePkg/TestCase/SecurityPkg/RandomNumberGenerator/RngDxe/TestRngDxe.inf {
  <LibraryClasses>
    NULL|UefiHostTestPkg/Library/BaseLibNullCpuid/BaseLibNullCpuid.inf
    NULL|SecurityPkg/RandomNumberGenerator/RngDxe/RngDxe.inf
  }

!if $(OPENSSL_TEST_ENABLE)
  UefiHostUnitTestCasePkg/TestCase/SecurityPkg/Library/DxeImageVerificationLib/TestDxeImageVerificationLib.inf {
  <LibraryClasses>
    NULL|SecurityPkg/Library/DxeImageVerificationLib/DxeImageVerificationLib.inf
  }
  UefiHostUnitTestCasePkg/TestCase/CryptoPkg/Library/BaseCryptLib/TestBaseCryptLib.inf
!endif

[Components.IA32]
  # BUGBUG: Need change SmmInitPageTable() to return UINTN.
  # BUGBUG: Need enable allocate <4G memory in X64 build.
  UefiHostUnitTestCasePkg/TestCase/UefiCpuPkg/PiSmmCpuDxeSmm/TestSmmCpuMemoryManagement.inf {
  <LibraryClasses>
    NULL|UefiHostUnitTestCasePkg/TestCase/UefiCpuPkg/PiSmmCpuDxeSmm/Override/PiSmmCpuDxeSmm.inf
    NULL|UefiHostTestPkg/Library/BaseLibNullCpuid/BaseLibNullCpuid.inf
    NULL|UefiHostTestPkg/Library/BaseLibNullMsr/BaseLibNullMsr.inf
    MtrrLib|UefiCpuPkg/Library/MtrrLib/MtrrLib.inf
    IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
    PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
    PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
    LocalApicLib|UefiCpuPkg/Library/BaseXApicX2ApicLib/BaseXApicX2ApicLib.inf
    UefiCpuLib|UefiCpuPkg/Library/BaseUefiCpuLib/BaseUefiCpuLib.inf
    SmmCpuPlatformHookLib|UefiCpuPkg/Library/SmmCpuPlatformHookLibNull/SmmCpuPlatformHookLibNull.inf
    CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/SmmCpuExceptionHandlerLib.inf
    SmmCpuFeaturesLib|UefiCpuPkg/Library/SmmCpuFeaturesLib/SmmCpuFeaturesLib.inf
  }

!include UefiHostUnitTestPkg/UefiHostUnitTestBuildOption.dsc
