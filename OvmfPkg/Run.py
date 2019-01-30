## @file
# Run OVMF in QEMU
#   Assumes QEMU is installed and in path
#   For Linux, assumes the putty terminal emulator installed and in path
#   For Windows, assumes ttermpro is install and in the path
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

'''
Run
'''

import os
import sys
import argparse
import subprocess
import time

#
# Globals for help information
#
__prog__        = 'Run'
__copyright__   = 'Copyright (c) 2019, Intel Corporation. All rights reserved.'
__description__ = 'Run OVMF in QEMU.\n'

#
# Globals
#
gWorkspace = ''
gArgs      = None

def Log(Message):
    if not gArgs.Verbose:
        return
    sys.stdout.write (__prog__ + ': ' + Message + '\n')

def Error(Message, ExitValue=1):
    sys.stderr.write (__prog__ + ': ERROR: ' + Message + '\n')
    sys.exit (ExitValue)

QEMU_COMMAND = '''qemu-system-x86_64
-machine q35,smm=on,accel=tcg
-cpu Nehalem
-global ICH9-LPC.disable_s3=1
-drive if=pflash,format=raw,unit=0,file=$EDKII_BUILD_OUTPUT/FV/OVMF_CODE.fd,readonly=on
-drive if=pflash,format=raw,unit=1,file=$EDKII_BUILD_OUTPUT/FV/OVMF_VARS.fd
-monitor tcp:127.0.0.1:20717,server
-serial tcp:127.0.0.1:20716,server
-soundhw adlib
-net none
-drive file=fat:rw:$EDKII_BUILD_DIR/Capsules,format=raw
-device e1000,romfile="$EDKII_BUILD_OUTPUT/$UEFI_ARCH/E1000Fmp.rom"
-device e1000,romfile="$EDKII_BUILD_OUTPUT/$UEFI_ARCH/E1000Fmp.rom"
-device e1000,romfile="$EDKII_BUILD_OUTPUT/$UEFI_ARCH/E1000Fmp.rom"
-device e1000,romfile="$EDKII_BUILD_OUTPUT/$UEFI_ARCH/E1000Fmp.rom"
'''

if __name__ == '__main__':
    #
    # Create command line argument parser object
    #
    parser = argparse.ArgumentParser (
                        prog = __prog__,
                        description = __description__ + __copyright__,
                        conflict_handler = 'resolve'
                        )
    parser.add_argument (
             '-a', '--arch', dest = 'Arch', nargs = '+', action = 'append',
             required = True,
             help = '''ARCHS is one of list: IA32, X64, IPF, ARM, AARCH64 or EBC,
                       which overrides target.txt's TARGET_ARCH definition. To
                       specify more archs, please repeat this option.'''
             )
    parser.add_argument (
             '-t', '--tagname', dest = 'ToolChain', required = True,
             help = '''Using the Tool Chain Tagname to build the platform,
                       overriding target.txt's TOOL_CHAIN_TAG definition.'''
             )
    parser.add_argument (
             '-p', '--platform', dest = 'PlatformFile', required = True,
             help = '''Build the platform specified by the DSC file name argument,
                       overriding target.txt's ACTIVE_PLATFORM definition.'''
             )
    parser.add_argument (
             '-b', '--buildtarget', dest = 'BuildTarget', required = True,
             help = '''Using the TARGET to build the platform, overriding
                       target.txt's TARGET definition.'''
             )
    parser.add_argument (
             '--conf=', dest = 'ConfDirectory', required = True,
             help = '''Specify the customized Conf directory.'''
             )
    parser.add_argument (
             '-D', '--define', dest = 'Define', nargs='*', action = 'append',
             help = '''Macro: "Name [= Value]".'''
             )
    parser.add_argument (
             '-v', '--verbose', dest = 'Verbose', action = 'store_true',
             help = '''Turn on verbose output with informational messages printed'''
             )
    parser.add_argument (
             '--package', dest = 'Package', nargs = '*', action = 'append',
             help = '''The directory name of a package of tests to copy'''
             )

    #
    # Parse command line arguments
    #
    gArgs, remaining = parser.parse_known_args()
    gArgs.BuildType = 'all'
    for BuildType in ['all', 'fds', 'genc', 'genmake', 'clean', 'cleanall', 'modules', 'libraries', 'run']:
        if BuildType in remaining:
            gArgs.BuildType = BuildType
            remaining.remove(BuildType)
            break
    gArgs.Remaining = ' '.join(remaining)

    #
    # This script only supports a BuildType of 'run'
    #
    if gArgs.BuildType != 'run':
      sys.exit (1)

    #
    # Get WORKSPACE environment variable
    #
    try:
        gWorkspace = os.environ['WORKSPACE']
    except:
        Error ('WORKSPACE environment variable not set')

    PathList = [gWorkspace]
    try:
        PathList += os.environ['PACKAGES_PATH'].split(os.pathsep)
    except:
        pass

    for Path in PathList:
        if os.path.exists (os.path.join (Path, gArgs.PlatformFile)):
            Dsc = open (os.path.join (Path, gArgs.PlatformFile), 'r').readlines()
            break
    for Line in Dsc:
        if Line.strip().startswith('OUTPUT_DIRECTORY'):
            OutputDirectory = Line.strip().split('=')[1].strip()
            break

    EdkiiBuildDir = os.path.join (gWorkspace, OutputDirectory)
    EdkiiBuildOutput = os.path.join (EdkiiBuildDir, gArgs.BuildTarget + '_' + gArgs.ToolChain)
    UefiArch = gArgs.Arch[0][0]
    if len (gArgs.Arch) > 1:
        if ['X64'] in gArgs.Arch:
            UefiArch = 'X64'

    QemuCommand = QEMU_COMMAND
    QemuCommand = QemuCommand.replace('$EDKII_BUILD_DIR', EdkiiBuildDir)
    QemuCommand = QemuCommand.replace('$EDKII_BUILD_OUTPUT', EdkiiBuildOutput)
    QemuCommand = QemuCommand.replace('$UEFI_ARCH', UefiArch)
    QemuCommand = ' '.join(QemuCommand.splitlines())

    Log (QemuCommand)

    if sys.platform == "win32":
        subprocess.Popen(QemuCommand, shell=True)
        time.sleep(0.5)
        subprocess.Popen('ttermpro localhost:20717 /nossh /W="QEMU Monitor"', shell=True)
        time.sleep(0.1)
        subprocess.Popen('ttermpro localhost:20716 /nossh /W="UEFI Console"', shell=True)
    else:
        subprocess.Popen(QemuCommand + ' &', shell=True)
        time.sleep(0.5)
        Process = subprocess.Popen('putty telnet:localhost:20717 &', shell=True)
        time.sleep(0.1)
        Process = subprocess.Popen('putty telnet:localhost:20716 &', shell=True)

    Log ('Done')
