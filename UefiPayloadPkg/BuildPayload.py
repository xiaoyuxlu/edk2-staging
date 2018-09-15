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
import os, sys, argparse, subprocess, shutil, multiprocessing

name = 'BuildPayload'
if os.name == 'posix': name = 'BuildPayload.sh'
if os.name == 'nt': name = 'BuildPayload.bat'
parser = argparse.ArgumentParser(prog=name)
parser.add_argument('p', help='platform', choices=['MinnowBoard3', 'Qemu'])
parser.add_argument('a', help='architecture', choices=['IA32', 'X64'])
parser.add_argument('t', help='target', choices=['RELEASE', 'DEBUG'])
parser.add_argument('-n', help='thread number for building', type=int, default=multiprocessing.cpu_count())
parser.add_argument('-c', help='clean', action='store_true')

def parse_arg():
    return parser.parse_args()
    
def print_usage():
    parser.print_help(sys.stderr)

def prep_env():
  if os.name == 'posix':
    toolchain = 'GCC49'
    gcc_ver = subprocess.Popen(['gcc', '-dumpversion'], stdout=subprocess.PIPE)
    (gcc_ver, err) = subprocess.Popen(['sed', 's/\\..*//'], stdin=gcc_ver.stdout, stdout=subprocess.PIPE).communicate()
    if int(gcc_ver) > 4: toolchain = 'GCC5'
    os.environ['WORKSPACE'] = os.path.abspath('.')
    workspace = os.environ['WORKSPACE']
    os.environ['PATH'] = os.environ['PATH'] + ':' + os.path.join(workspace, 'BaseTools/BinWrappers/PosixLike')
    os.environ['EDK_TOOLS_PATH'] = os.path.join(workspace, 'BaseTools')
    if not os.path.exists(os.path.join(workspace, 'Conf')):
      os.makedirs(os.path.join(workspace, 'Conf'))
    for name in ['target', 'tools_def', 'build_rule']:
      txt_file = os.path.join(workspace, 'Conf/%s.txt' % name)
      if not os.path.exists(txt_file):
        shutil.copy (
          os.path.join(workspace, 'BaseTools/Conf/%s.template' % name),
          os.path.join(workspace, 'Conf/%s.txt' % name))
    ret = 0
    if os.name == 'posix':
      genffs_exe_path = os.path.join(workspace, 'BaseTools', 'Source', 'C', 'bin', 'GenFfs')
      genffs_exist = os.path.exists(genffs_exe_path)
      if not genffs_exist:
        ret = subprocess.call(['make', '-C', 'BaseTools'])
    if ret:
      print('Build BaseTools failed, please check required build environment and utilities !')
      sys.exit(1)
  elif os.name == 'nt':
    if 'WORKSPACE' not in os.environ:
      print 'Please run edkSetup.bat first !'
      sys.exit(1)
    toolchain = ''
    vs_ver = ['2015', '2013', '2012', '2010', '2008']
    for each in vs_ver:
      vs_test = 'VS%s_PREFIX' % (each)
      if vs_test in os.environ:
        toolchain='VS%s%s' % (each, 'x86' if '(x86)' in os.environ[vs_test] else '')
        break
    if not toolchain:
      print 'Could not find supported Visual Studio version !'
      sys.exit(1)
  else:
    print('Unsupported operating system !')
    sys.exit(1)

  return toolchain

def copytree(src, dst):
    for item in os.listdir(src):
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if os.path.isdir(s):
            if os.path.exists(d):
                copytree(s, d)
            else:
                shutil.copytree(s, d)
                print('copied %s to %s' % (s, d))
        else:
            shutil.copy(s, d)
            print('copied %s to %s' % (s, d))

def build(platform, architectrue, target, threadnum):
    ret = 0
    toolchain = prep_env()
    print('start building payload ...')
    UserExtension = '../UEFIPayload/UefiPayloadPkg/CustomizationSample/Boards/%s/SourceCodes/UserExtension' % platform
    if os.path.exists(os.path.join(UserExtension, 'Custom.c')):
        copytree(UserExtension, '../UEFIPayload/UefiPayloadPkg/Library/UserExtensionLib')

    PlatformLib = '../UEFIPayload/UefiPayloadPkg/CustomizationSample/Boards/%s/SourceCodes/PlatformLib' % platform
    if os.path.exists(PlatformLib):
        copytree(PlatformLib, '../UEFIPayload/UefiPayloadPkg/Library/CustomPlatformLib')

    Inc = '../UEFIPayload/UefiPayloadPkg/CustomizationSample/Inc'
    if os.path.exists(Inc):
        copytree(Inc, '../UEFIPayload/UefiPayloadPkg/Include/Library')
    os.chdir('../UEFIPayload/UefiPayloadPkg/Tools')
    ret = subprocess.call(['python', 'TranslateConfig.py', '-b', platform])
    os.chdir(os.getenv('WORKSPACE'))
    if ret:
        print('translating configuration failed!')
        sys.exit(1)
    Macro = 'IA32'
    Arch = '-a IA32'
    if architectrue == 'X64':
        Macro = 'IA32X64'
        Arch = '-a IA32 -a X64'
    platformfile = os.path.join(os.getenv('WORKSPACE'), '../UEFIPayload/UefiPayloadPkg', 'UefiPayloadPkg%s.dsc' % Macro)
    tool = 'build' if os.name == 'posix' else 'build.bat'
    cmd = '%s -p %s -b %s -D BD_ARCH=%s -t %s -n %d %s' % (tool, platformfile, target, Macro, toolchain, threadnum, Arch)
    ret = subprocess.call(cmd.split())
    if ret:
        print('building payload failed')
        exit(1)
    payload = 'Build/UefiPayloadPkg%s/%s_%s/FV' % (architectrue, target, toolchain)
    ret = subprocess.call(['python', '../UEFIPayload/UefiPayloadPkg/Tools/PatchFv.py', payload, 'PEIFV:UEFIPAYLOAD',
                           "0x0000, SecCore:__ModuleEntryPoint, @Payload Entry point",
                           "0x0004, 0x600000                  , @Payload execution base"])
    if ret:
        print('patching failed')
        exit(1)

if __name__ == '__main__':

    args = parse_arg()

    if len(sys.argv) == 1:
      parser.print_help(sys.stderr)
      sys.exit(1)

    if args.c:
        print('Removing Build and Conf directories ...')
        if os.path.exists('Build'): shutil.rmtree('Build')
        if os.path.exists('Conf'): shutil.rmtree('Conf')

    build(args.p, args.a, args.t, args.n)


