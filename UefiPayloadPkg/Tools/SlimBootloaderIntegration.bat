@echo off
REM @file
REM
REM 
REM
REM Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
REM
REM This program and the accompanying materials are licensed and made available 
REM under the terms and conditions of the BSD License which accompanies this 
REM distribution. The full text of the license may be found at 
REM http://opensource.org/licenses/bsd-license.php
REM
REM THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
REM WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
REM

if /I "%1" == "" goto Usage
if /I "%1" == "-h" goto Usage
if /I "%1" == "--help" goto Usage

call python -c "from SlimBootloaderIntegration import parse_arg; parse_arg()" %*
if %errorlevel% NEQ 0 goto Exit

set PACKAGES_PATH=
cd ..\WorkSpace\SlimBootloader

set arg=%*
echo %arg% | findstr " -c | -c$" > nul
if %errorlevel% EQU 0 ( 
  if exist Conf rmdir /S /Q Conf
  if exist Build rmdir /S /Q Build
  echo cleaning finished
  set arg=%arg:-c=%
)

call Edksetup.bat
python SlimBootloaderIntegration.py %arg%
cd ..\..\Tools
goto Exit

:Usage
  call python -c "from SlimBootloaderIntegration import print_usage; print_usage()" %*
  
:Exit