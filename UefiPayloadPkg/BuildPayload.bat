@REM @file
@REM
@REM Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
@REM
@REM This program and the accompanying materials are licensed and made available 
@REM under the terms and conditions of the BSD License which accompanies this 
@REM distribution. The full text of the license may be found at 
@REM http://opensource.org/licenses/bsd-license.php
@REM
@REM THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
@REM WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
@REM

@echo off
if /I "%1" == "" goto Usage
if /I "%1" == "-h" goto Usage
if /I "%1" == "--help" goto Usage

call python -c "from BuildPayload import parse_arg; parse_arg()" %*
if %errorlevel% NEQ 0 goto Exit

set PYTHON_HOME=C:\Python27
set WORKSPACE=%CD%\..\..\edk2
set PACKAGES_PATH=%WORKSPACE%;%CD%\..
cd %WORKSPACE%

set arg=%*
echo %arg% | findstr " -c | -c$" > nul
if %errorlevel% EQU 0 ( 
  if exist Conf rmdir /S /Q Conf
  if exist Build rmdir /S /Q Build
  echo cleaning finished
  set arg=%arg:-c=%
)

call edksetup.bat Rebuild
python ../UEFIPayload/UefiPayloadPkg/BuildPayload.py %arg%
cd ../UEFIPayload/UefiPayloadPkg
goto Exit

:Usage
  call python -c "from BuildPayload import print_usage; print_usage()" %*
  
:Exit