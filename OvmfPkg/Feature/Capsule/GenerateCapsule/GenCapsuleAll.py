## @file
# Generate capsules for OVMF
#   openssl must be install and in path
#   In Linux systems, guestmount and guestunmount must be installed and in path
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

'''
GenCapsuleAll
'''

import os
import sys
import argparse
import subprocess
import glob
import shutil
import struct

#
# Globals for help information
#
__prog__        = 'GenCapsuleAll'
__copyright__   = 'Copyright (c) 2019, Intel Corporation. All rights reserved.'
__description__ = 'Generate OVMF capsules.\n'

#
# Globals
#
gWorkspace = ''
gBaseToolsPath = ''
gArgs      = None

def LogAlways(Message):
    sys.stdout.write (__prog__ + ': ' + Message + '\n')

def Log(Message):
    if not gArgs.Verbose:
        return
    sys.stdout.write (__prog__ + ': ' + Message + '\n')

def Error(Message, ExitValue=1):
    sys.stderr.write (__prog__ + ': ERROR: ' + Message + '\n')
    sys.exit (ExitValue)

def RelativePath(target):
    return os.path.relpath (target, gWorkspace)

def NormalizePath(target):
    if isinstance(target, tuple):
        return os.path.normpath (os.path.join (*target))
    else:
        return os.path.normpath (target)

def RemoveFile(target):
    target = NormalizePath(target)
    if not target or target == os.pathsep:
        Error ('RemoveFile() invalid target')
    if os.path.exists(target):
        os.remove (target)
        Log ('remove %s' % (RelativePath (target)))

def RemoveDirectory(target):
    target = NormalizePath(target)
    if not target or target == os.pathsep:
        Error ('RemoveDirectory() invalid target')
    if os.path.exists(target):
        Log ('rmdir %s' % (RelativePath (target)))
        shutil.rmtree(target)

def CreateDirectory(target):
    target = NormalizePath(target)
    if not os.path.exists(target):
        Log ('mkdir %s' % (RelativePath (target)))
        os.makedirs (target)

def Copy(src, dst):
    src = NormalizePath(src)
    dst = NormalizePath(dst)
    for File in glob.glob(src):
        Log ('copy %s -> %s' % (RelativePath (File), RelativePath (dst)))
        shutil.copy (File, dst)


GenerateCapsuleCommand = '''
GenerateCapsule
--encode
--guid {FMP_CAPSULE_GUID}
--fw-version {FMP_CAPSULE_VERSION}
--lsv {FMP_CAPSULE_LSV}
--capflag PersistAcrossReset
--capflag InitiateReset
--signer-private-cert={BASE_TOOLS_PATH}/Source/Python/Pkcs7Sign/TestCert.pem
--other-public-cert={BASE_TOOLS_PATH}/Source/Python/Pkcs7Sign/TestSub.pub.pem
--trusted-public-cert={BASE_TOOLS_PATH}/Source/Python/Pkcs7Sign/TestRoot.pub.pem
-o {FMP_CAPSULE_FILE}
{FMP_CAPSULE_PAYLOAD}
'''

def GenCapsuleSampleDevice (Payload, Guid, Version, Lsv, CapsulesPath):
    LogAlways ('Generate Capsule: {0} {1:08x} {2:08x} {3}'.format (Guid, Version, Lsv, Payload))

    VersionString = '.'.join([str(ord(x)) for x in struct.pack('>I', Version).decode()])

    BinaryPayload = Payload.encode() + bytearray(0x18 - len (Payload.encode()))
    BinaryPayload = BinaryPayload + struct.pack('<I', Version)
    BinaryPayload = BinaryPayload + struct.pack('<I', Lsv)

    f = open (NormalizePath ((CapsulesPath, 'Payload.bin')), 'wb')
    f.write(BinaryPayload)
    f.close()

    FmpCapsuleFile = NormalizePath ((CapsulesPath, 'TestCert', Payload + '.' + VersionString + '.cap'))
    Command = GenerateCapsuleCommand.format (
                FMP_CAPSULE_GUID    = Guid,
                FMP_CAPSULE_VERSION = Version,
                FMP_CAPSULE_LSV     = Lsv,
                BASE_TOOLS_PATH     = gBaseToolsPath,
                FMP_CAPSULE_FILE    = FmpCapsuleFile,
                FMP_CAPSULE_PAYLOAD = NormalizePath ((CapsulesPath, 'Payload.bin'))
                )
    Command = ' '.join(Command.splitlines()).strip()
    if gArgs.Verbose:
        Command = Command + ' -v'

    Log (Command)

    Process = subprocess.Popen(Command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    ProcessOutput = Process.communicate()

    RemoveFile ((CapsulesPath, 'Payload.bin'))

    if Process.returncode == 0:
        Log (ProcessOutput[0].decode())
    else:
        LogAlways (Command)
        LogAlways (ProcessOutput[0].decode())
        Error ('GenerateCapsule returned an error')

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
    # Get WORKSPACE environment variable
    #
    try:
        gWorkspace = os.environ['WORKSPACE']
    except:
        Error ('WORKSPACE environment variable not set')

    #
    # Get PACKAGES_PATH and generate prioritized list of paths
    #
    PathList = [gWorkspace]
    try:
        PathList += os.environ['PACKAGES_PATH'].split(os.pathsep)
    except:
        pass

    #
    # Deteermine full path to BaseTools
    #
    for Path in PathList:
        if os.path.exists (os.path.join (Path, 'BaseTools')):
            gBaseToolsPath = os.path.join (Path, 'BaseTools')
            break

    #
    # Parse OUTPUT_DIRECTORY from DSC file
    #
    for Path in PathList:
        if os.path.exists (os.path.join (Path, gArgs.PlatformFile)):
            Dsc = open (os.path.join (Path, gArgs.PlatformFile), 'r').readlines()
            break
    for Line in Dsc:
        if Line.strip().startswith('OUTPUT_DIRECTORY'):
            OutputDirectory = Line.strip().split('=')[1].strip()
            break

    #
    # Determine full paths to EDK II build directory, EDK II build output
    # directory and the CPU arch of the UEFI phase.
    #
    CommandDir = os.path.dirname(sys.argv[0])
    EdkiiBuildDir = os.path.join (gWorkspace, OutputDirectory)
    EdkiiBuildOutput = os.path.join (EdkiiBuildDir, gArgs.BuildTarget + '_' + gArgs.ToolChain)
    UefiArch = gArgs.Arch[0][0]
    if len (gArgs.Arch) > 1:
        if ['X64'] in gArgs.Arch:
            UefiArch = 'X64'

    CapsulesPath = NormalizePath((EdkiiBuildDir, 'Capsules'))

    #
    # Create output directories
    #
    try:
        RemoveDirectory ((CapsulesPath))
        CreateDirectory ((CapsulesPath))
    except:
        pass
    try:
        CreateDirectory ((CapsulesPath, 'TestCert'))
    except:
        pass

    #
    #  Copy CapsuleApp, virtual hard drive, and powershell scripts into output directories
    #
    Copy ((EdkiiBuildOutput, UefiArch, 'CapsuleApp.efi'), (CapsulesPath, 'TestCert'))

    #
    # Generate capsules for the Red Sample Device
    #
    GenCapsuleSampleDevice('Red','72E2945A-00DA-448E-9AA7-075AD840F9D4',0x00000010,0x00000000, CapsulesPath)
    GenCapsuleSampleDevice('Red','72E2945A-00DA-448E-9AA7-075AD840F9D4',0x00000011,0x00000000, CapsulesPath)
    GenCapsuleSampleDevice('Red','72E2945A-00DA-448E-9AA7-075AD840F9D4',0x00000012,0x00000000, CapsulesPath)

    #
    # Generate capsules for the Green Sample Device
    #
    GenCapsuleSampleDevice('Green','79179BFD-704D-4C90-9E02-0AB8D968C18A',0x00000020,0x00000020, CapsulesPath)
    GenCapsuleSampleDevice('Green','79179BFD-704D-4C90-9E02-0AB8D968C18A',0x00000021,0x00000020, CapsulesPath)
    GenCapsuleSampleDevice('Green','79179BFD-704D-4C90-9E02-0AB8D968C18A',0x00000022,0x00000020, CapsulesPath)

    #
    # Generate capsules for the Blue Sample Device
    #
    GenCapsuleSampleDevice('Blue','149DA854-7D19-4FAA-A91E-862EA1324BE6',0x00000010,0x00000000, CapsulesPath)
    GenCapsuleSampleDevice('Blue','149DA854-7D19-4FAA-A91E-862EA1324BE6',0x00000011,0x00000000, CapsulesPath)
    GenCapsuleSampleDevice('Blue','149DA854-7D19-4FAA-A91E-862EA1324BE6',0x00000012,0x00000012, CapsulesPath)
    GenCapsuleSampleDevice('Blue','149DA854-7D19-4FAA-A91E-862EA1324BE6',0x00000013,0x00000012, CapsulesPath)
    GenCapsuleSampleDevice('Blue','149DA854-7D19-4FAA-A91E-862EA1324BE6',0x00000014,0x00000012, CapsulesPath)

    #
    # Generate capsules for the E1000 PCI Adapter
    #
    GenCapsuleSampleDevice('E1000Payload','AF2FDE1E-234B-11E9-9356-54E1AD3BF134',0x00000010,0x00000000, CapsulesPath)
    GenCapsuleSampleDevice('E1000Payload','AF2FDE1E-234B-11E9-9356-54E1AD3BF134',0x00000011,0x00000000, CapsulesPath)
    GenCapsuleSampleDevice('E1000Payload','AF2FDE1E-234B-11E9-9356-54E1AD3BF134',0x00000012,0x00000012, CapsulesPath)
    GenCapsuleSampleDevice('E1000Payload','AF2FDE1E-234B-11E9-9356-54E1AD3BF134',0x00000013,0x00000012, CapsulesPath)
    GenCapsuleSampleDevice('E1000Payload','AF2FDE1E-234B-11E9-9356-54E1AD3BF134',0x00000014,0x00000012, CapsulesPath)
