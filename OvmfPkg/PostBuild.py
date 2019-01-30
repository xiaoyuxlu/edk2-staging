## @file
# OVMF post build script.
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#

'''
PostBuild
'''

import os
import sys
import subprocess

if __name__ == '__main__':
    CommandDir = os.path.dirname (sys.argv[0])
    if 'run' in sys.argv[1:]:
        subprocess.call (['python', os.path.join(CommandDir, 'Run.py')] + sys.argv[1:])
    else:
        subprocess.call (['python', os.path.join (CommandDir, 'Feature/Capsule/GenerateCapsule/GenCapsuleAll.py')] + sys.argv[1:])
