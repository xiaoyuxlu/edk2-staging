## @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#
# This program and the accompanying materials are licensed and made available 
# under the terms and conditions of the BSD License which accompanies this 
# distribution. The full text of the license may be found at 
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#!/bin/bash

if [ "$1" == "" ] || [ "$1" == "-h" ]; then
  python -c "from BuildPayload import print_usage; print_usage()" $*
  exit
fi

export WORKSPACE=$PWD/../../edk2
export PACKAGES_PATH=$WORKSPACE:$PWD/..
cd $WORKSPACE

python ../UEFIPayload/UefiPayloadPkg/BuildPayload.py $*

cd ../UEFIPayload/UefiPayloadPkg
