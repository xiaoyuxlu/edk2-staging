## @file
# Startup script for EFI SCT automatic running
#
# Copyright (c) 2005 - 2017, Intel Corporation. All rights reserved.<BR>
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

#
# NOTE: The file system name is hard coded since I don't know how to get the
# file system name in the script.
#

#
# Startup script for the EFI SCT Harness
#

echo -off

for %i in 0 1 2 3 4 5 6 7 8 9 A B C D E F
  if exist FS%i:\Sct\IA32 then
    FS%i:
    cd Sct\IA32
    goto Found
  endif

  if exist FS%i:\Sct\X64 then
    FS%i:
    cd Sct\X64
    goto Found
  endif
endfor
goto Done

:Found
if exist .\Sct.efi then
    stallforkey.efi
    if %lasterror% == 0 then
      goto Done
    endif

    sct -c

    goto Done
  endif

:Done
