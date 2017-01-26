## @file
# Package build file for the TestFrameworkPkg
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
  PLATFORM_NAME                  = TestFrameworkPkg
  PLATFORM_GUID                  = 32570f8f-c6dc-11e6-aa42-402cf4d70a24
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/TestFrameworkPkg
  SUPPORTED_ARCHITECTURES        = IA32|IPF|X64|EBC|ARM
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  POSTBUILD                      = TestFrameworkPkg/GenFramework.cmd

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
  DebugLib|MdePkg/Library/UefiDebugLibStdErr/UefiDebugLibStdErr.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  PostCodeLib|MdePkg/Library/BasePostCodeLibPort80/BasePostCodeLibPort80.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf

  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
  HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf

  EntsLib|TestFrameworkPkg/Library/EasLib/EntsLib.inf
  EfiTestLib|TestFrameworkPkg/Library/EfiTestLib/EfiTestLib.inf

[Components]
  TestFrameworkPkg/Library/EasLib/EntsLib.inf
  TestFrameworkPkg/Library/EfiTestLib/EfiTestLib.inf

  TestFrameworkPkg/Eftp/Eftp.inf
  TestFrameworkPkg/MonitorServices/SerialMonitor/SerialMonitor.inf
  TestFrameworkPkg/MonitorServices/ManagedNetworkMonitor/ManagedNetworkMonitor.inf
  TestFrameworkPkg/MonitorServices/IP4NetworkMonitor/IP4NetworkMonitor.inf

  TestFrameworkPkg/Support/StandardTest/StandardTest.inf
  TestFrameworkPkg/Support/TestLogging/TestLogging.inf
  TestFrameworkPkg/Support/TestProfile/TestProfile.inf
  TestFrameworkPkg/Support/TestRecovery/TestRecovery.inf

  TestFrameworkPkg/InstallSct/InstallSct.inf
  TestFrameworkPkg/StallForKey/StallForKey.inf
  TestFrameworkPkg/Sct/Sct.inf {
    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdUefiLibMaxPrintBufferSize|8000
  }
  TestFrameworkPkg/PeiSctManageModule/PeiSctManageModule.inf

[BuildOptions]
  MSFT:*_*_*_CC_FLAGS = /FAsc
