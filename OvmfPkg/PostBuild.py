## @file
# OVMF post build script.
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
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
