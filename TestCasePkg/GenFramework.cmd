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

for %%G in (%ARCHLIST%) do (
  set ARCH=%%G
  call :CopyFiles
)

goto Finished

:CopyFiles
set Framework=%WORKSPACE%\Build\SctPackage\%ARCH%
set BuildOutput=%WORKSPACE%\Build\TestCasePkg\%TARGET%_%TOOLCHAIN%\%ARCH%

if not %CLEAN%=="" (
  if exist %Framework%\Test rmdir /s/q %Framework%\Test
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
:: Copy all test cases
::*****************************************************

copy %BuildOutput%\*.efi       %Framework%\Test          > NUL
exit /b

:Finished



