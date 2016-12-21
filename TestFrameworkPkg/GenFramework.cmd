@REM @file
@REM Windows batch file used to create installer in build output directory
@REM
@REM Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
@REM This program and the accompanying materials
@REM are licensed and made available under the terms and conditions of the BSD License
@REM which accompanies this distribution.  The full text of the license may be found at
@REM http://opensource.org/licenses/bsd-license.php
@REM
@REM THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
@REM WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
@REM

@echo off

::**********************************************************************
:: Parse command line arguments
::**********************************************************************
set ARCHLIST=
set CLEAN=""
:ArgLoop
if /i "%~1"=="" (goto Done)
if /i "%~1"=="clean" (
    set CLEAN=%~1
    shift
    goto ArgLoop
)
if /i "%~2"=="" (goto Done)
if /i "%~1"=="-a" (
    set ARCHLIST=%ARCHLIST% %~2
    shift
    shift
    goto ArgLoop
)
if /i "%~1"=="-b" (
    set TARGET=%~2
    shift
    shift
    goto ArgLoop
)
if /i "%~1"=="-t" (
    set TOOLCHAIN=%~2
    shift
    shift
    goto ArgLoop
)
shift
goto ArgLoop
:Done


@set TEST_FRAMEWORK_PKG_PATH=%WORKSPACE%\TestFrameworkPkg
@if defined PACKAGES_PATH (
  @for %%i IN (%PACKAGES_PATH%) DO (
    @if exist %%~fi\TestFrameworkPkg (
      @set TEST_FRAMEWORK_PKG_PATH=%%~fi\TestFrameworkPkg
      @goto ProcessArchList
    )
  )
)

:ProcessArchList
for %%G in (%ARCHLIST%) do (
  set ARCH=%%G
  call :CopyFiles
)

goto Finished

:CopyFiles
set Framework=%WORKSPACE%\Build\SctPackage\%ARCH%
set BuildOutput=%WORKSPACE%\Build\TestFrameworkPkg\%TARGET%_%TOOLCHAIN%\%ARCH%
if not %CLEAN%=="" (
  if exist %Framework%\..  rmdir /s/q %Framework%\..
  exit /b
)

rem *********************************************
rem Create target directories
rem *********************************************

if not exist %Framework% mkdir %Framework%
for %%G in (Data, Dependency, Support, Test, Sequence, Report, Proxy) do (
  if not exist %Framework%\%%G mkdir %Framework%\%%G
)

::*****************************************************
:: Copy the SCT framework and the related application
::*****************************************************

copy %BuildOutput%\InstallSct.efi       %Framework%          > NUL
copy %BuildOutput%\StallForKey.efi      %Framework%          > NUL

copy %BuildOutput%\SCT.efi              %Framework%          > NUL

copy %BuildOutput%\StandardTest.efi     %Framework%\Support  > NUL
copy %BuildOutput%\TestProfile.efi      %Framework%\Support  > NUL
copy %BuildOutput%\TestRecovery.efi     %Framework%\Support  > NUL
copy %BuildOutput%\TestLogging.efi      %Framework%\Support  > NUL


::*********************************************
:: Copy the SCT configuration data
::*********************************************

copy %TEST_FRAMEWORK_PKG_PATH%\Scripts\SctStartup.nsh  %Framework%\..    
copy %TEST_FRAMEWORK_PKG_PATH%\Data\Category.ini       %Framework%\Data  > NUL
copy %TEST_FRAMEWORK_PKG_PATH%\Data\GuidFile.txt       %Framework%\Data  > NUL
exit /b

:Finished
