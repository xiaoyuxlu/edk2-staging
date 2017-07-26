## @file
# Create EDK II Test Framework installer in build output directory
#
# Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#

'''
GenFramework
'''

import os
import sys
import argparse
import glob
import shutil

#
# Globals for help information
#
__prog__        = 'GenFramework'
__version__     = '%s Version %s' % (__prog__, '1.0')
__copyright__   = 'Copyright (c) 2017, Intel Corporation. All rights reserved.'
__description__ = 'Create EDK II Test Framework installer in build output directory.\n'

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

def RelativePath(target):
  return os.path.relpath (target, gWorkspace)

def NormalizePath(target):
  if isinstance(target, tuple):
    return os.path.normpath (os.path.join (*target))
  else:
    return os.path.normpath (target)

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

if __name__ == '__main__':
  #
  # Create command line argument parser object
  #
  parser = argparse.ArgumentParser (
                      prog = __prog__,
                      version = __version__,
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
  # Start
  #
  Log ('Start')

  #
  # Get WORKSPACE environment variable
  #
  try:
    gWorkspace = os.environ['WORKSPACE']
  except:
    Error ('WORKSPACE environment variable not set')

  #
  # Find test framework package directory in WORKSPACE or PACKAGES_PATH
  #
  PathList = [gWorkspace]
  try:
    PathList += os.environ['PACKAGES_PATH'].split(os.pathsep)
  except:
    pass
  for Path in PathList:
    TestFrameworkPkgPath = NormalizePath((Path, 'TestFrameworkPkg'))
    if os.path.exists(TestFrameworkPkgPath):
      break
  if not os.path.exists(TestFrameworkPkgPath):
    Error ('TestFrameworkPkg directory not found in WORKSPACE or PACKAGES_PATH')

  #
  # Process build target
  #
  InstallerPath = NormalizePath((gWorkspace, 'Build', 'SctPackage'))
  if gArgs.BuildType == 'clean':
    if gArgs.Package:
      #
      # Remove test cases for each package and CPU architecture
      #
      for Arch in gArgs.Arch:
        Arch = Arch[0]
        for Package in gArgs.Package:
          Package = Package[0]
          Log ('Remove test cases from %s from test framework installer in %s' % (Package, InstallerPath))
          RemoveDirectory ((InstallerPath, Arch, 'Test', Package))
    else:
      #
      # Remove the installer
      #
      Log ('Remove test framework installer in %s' % (InstallerPath))
      RemoveDirectory (InstallerPath)
  else:
    if gArgs.Package:
      #
      # Copy test cases for each package and CPU architcture
      #
      for Package in gArgs.Package:
        Package = Package[0]
        Log ('Add test cases from %s to test framework installer in %s' % (Package, InstallerPath))
        for Arch in gArgs.Arch:
          Arch = Arch[0]
          BuildOutput = NormalizePath((
                          gWorkspace,
                          'Build',
                          Package,
                          gArgs.BuildTarget + '_' + gArgs.ToolChain,
                          Arch
                          ))
          if os.path.exists(BuildOutput):
            CreateDirectory ((InstallerPath, Arch, 'Test', Package))
            Copy ((BuildOutput, '*.efi'), (InstallerPath, Arch, 'Test', Package))
    else:
      #
      # Copy test framework files for each CPU architcture
      #
      Log ('Create test framework installer in %s' % (InstallerPath))
      CreateDirectory ((InstallerPath))
      Copy ((TestFrameworkPkgPath, 'Scripts', 'SctStartup.nsh'), (InstallerPath))
      for Arch in gArgs.Arch:
        Arch = Arch[0]
        for Directory in ['Data', 'Dependency', 'Support', 'Test', 'Sequence', 'Report', 'Proxy']:
          CreateDirectory ((InstallerPath, Arch, Directory))

        BuildOutput = NormalizePath((
                        gWorkspace,
                        'Build',
                        'TestFrameworkPkg',
                        gArgs.BuildTarget + '_' + gArgs.ToolChain,
                        Arch
                        ))
        if os.path.exists(BuildOutput):
          for File in ['InstallSct.efi', 'StallForKey.efi', 'Sct.efi']:
            Copy ((BuildOutput, File), (InstallerPath, Arch))
          for File in ['StandardTest.efi', 'TestProfile.efi', 'TestRecovery.efi', 'TestLogging.efi']:
            Copy ((BuildOutput, File), (InstallerPath, Arch, 'Support'))

        Copy ((TestFrameworkPkgPath, 'Data/Category.ini'), (InstallerPath, Arch, 'Data'))
        Copy ((TestFrameworkPkgPath, 'Data/GuidFile.txt'), (InstallerPath, Arch, 'Data'))
        Copy ((TestFrameworkPkgPath, 'Data/GuidFile.txt'), (InstallerPath, Arch, 'Data'))

  #
  # Done
  #
  Log ('Done')
