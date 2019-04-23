## @file UefiHostCryptoPkg.dsc
# 
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  PLATFORM_NAME                  = UefiHostCryptoPkg
  PLATFORM_GUID                  = 4946AC25-0A6B-4F43-AF4C-3B017A4D8828
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/UefiHostCryptoPkg
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

  DEFINE OPENSSL_FLAGS           = -DL_ENDIAN -DOPENSSL_SMALL_FOOTPRINT -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -DNO_SYSLOG

[LibraryClasses]
  BaseLib|UefiHostTestPkg/Library/BaseLibHost/BaseLibHost.inf
  CacheMaintenanceLib|UefiHostTestPkg/Library/BaseCacheMaintenanceLibHost/BaseCacheMaintenanceLibHost.inf
  BaseMemoryLib|UefiHostTestPkg/Library/BaseMemoryLibHost/BaseMemoryLibHost.inf
  MemoryAllocationLib|UefiHostTestPkg/Library/MemoryAllocationLibHost/MemoryAllocationLibHost.inf
  DebugLib|UefiHostTestPkg/Library/DebugLibHost/DebugLibHost.inf
  UefiBootServicesTableLib|UefiHostTestPkg/Library/UefiBootServicesTableLibHost/UefiBootServicesTableLibHost.inf
  HobLib|UefiHostTestPkg/Library/HobLibHost/HobLibHost.inf
  SmmMemLib|UefiHostTestPkg/Library/SmmMemLibHost/SmmMemLibHost.inf
  DevicePathLib|UefiHostTestPkg/Library/UefiDevicePathLibHost/UefiDevicePathLibHost.inf
  DxeServicesTableLib|UefiHostTestPkg/Library/DxeServicesTableLibHost/DxeServicesTableLibHost.inf
  UefiRuntimeServicesTableLib|UefiHostTestPkg/Library/UefiRuntimeServicesTableLibHost/UefiRuntimeServicesTableLibHost.inf
  SmmServicesTableLib|UefiHostTestPkg/Library/SmmServicesTableLibHost/SmmServicesTableLibHost.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  PeiServicesTablePointerLib|UefiHostTestPkg/Library/PeiServicesTablePointerLibHost/PeiServicesTablePointerLibHost.inf

  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  ReportStatusCodeLib|MdePkg/Library/BaseReportStatusCodeLibNull/BaseReportStatusCodeLibNull.inf

  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf

  CpuLib|UefiHostTestPkg/Library/BaseCpuLibHost/BaseCpuLibHost.inf

  BaseCryptLib|UefiHostCryptoPkg/Library/BaseCryptLibHost/BaseCryptLibHost.inf
  OpensslLib|UefiHostCryptoPkg/Library/OpensslLib/OpensslLib.inf

[LibraryClasses.common.USER_DEFINED]

[Components]
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


!include UefiHostTestPkg/UefiHostTestBuildOption.dsc
