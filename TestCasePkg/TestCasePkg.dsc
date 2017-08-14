## @file
# Package build file for the TestCasePkg
#
# Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
#
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution. The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#
##

[Defines]
  PLATFORM_NAME                  = TestCasePkg
  PLATFORM_GUID                  = d6217f9e-d2d7-11e6-bf11-402cf4d70a24
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/TestCasePkg
  SUPPORTED_ARCHITECTURES        = IA32|IPF|X64|EBC|ARM
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  POSTBUILD                      = TestCasePkg/GenFramework.cmd

  #
  # Platform On/Off features are defined here
  #
  DEFINE LOGGING              = FALSE
  DEFINE SOURCE_DEBUG_ENABLE  = FALSE
  DEFINE NT32                 = FALSE

  !if $(TARGET) == "DEBUG"
    DEFINE LOGGING             = TRUE
    DEFINE SOURCE_DEBUG_ENABLE = TRUE
  !endif

[LibraryClasses]
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf
  UefiScsiLib|MdePkg/Library/UefiScsiLib/UefiScsiLib.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
!if $(LOGGING)
  DebugLib|MdePkg/Library/UefiDebugLibStdErr/UefiDebugLibStdErr.inf
!else
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
!endif
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  PostCodeLib|MdePkg/Library/BasePostCodeLibPort80/BasePostCodeLibPort80.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  EfiTestLib|TestFrameworkPkg/Library/EfiTestLib/EfiTestLib.inf

[PcdsFixedAtBuild]
!if $(LOGGING)
  !if $(SOURCE_DEBUG_ENABLE)
    #
    # Enabled ASSERT(), DEBUG(), and DEBUG_CODE() and configure ASSERT() to
    # generate a breakpoint.
    #
    gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x17
  !else
    #
    # Enabled ASSERT(), DEBUG(), and DEBUG_CODE() and configure ASSERT() to
    # generate a deadloop.
    #
    gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x27
  !endif
!else
  #
  # Disable ASSERT(), DEBUG(), DEBUG_CODE(), and DEBUG_CLEAR_MEMORY()
  #
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x0
!endif

[PcdsPatchableInModule]
!if $(LOGGING)
  #
  # Enable DEBUG() messages of type DEBUG_ERROR, DEBUG_VERBOSE, DEBUG_INFO,
  # DEBUG_WARN, and DEBUG_INIT
  #
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80400043
!endif

[Components]
  TestCasePkg/Timer/ArchTimerBBTest.inf

[BuildOptions]
!if $(TARGET) == "DEBUG"
  #
  # Generate mixed C/ASM files for debug builds
  #
  MSFT:*_*_*_CC_FLAGS = /FAsc
!endif

!if $(NT32)
  #
  # If -D NT32 is set on command line to build, then build all components to be
  # compatible with NT32 environment debuggers.
  #
  DEBUG_*_*_DLINK_FLAGS   = /EXPORT:InitializeDriver=$(IMAGE_ENTRY_POINT) /BASE:0x10000 /ALIGN:4096 /FILEALIGN:4096 /SUBSYSTEM:CONSOLE
  NOOPT_*_*_DLINK_FLAGS   = /EXPORT:InitializeDriver=$(IMAGE_ENTRY_POINT) /BASE:0x10000 /ALIGN:4096 /FILEALIGN:4096 /SUBSYSTEM:CONSOLE
  RELEASE_*_*_DLINK_FLAGS = /ALIGN:4096 /FILEALIGN:4096
!endif
