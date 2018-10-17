## @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#
# This program and the accompanying materials are licensed and made available 
# under the terms and conditions of the BSD License which accompanies this 
# distribution. The full text of the license may be found at 
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN 'AS IS' BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
import os, sys, argparse, subprocess, shutil, multiprocessing

def prep_env():
  os.environ['EDK_TOOLS_PATH'] = os.path.abspath('BaseTools')
  os.environ['BASE_TOOLS_PATH'] = os.path.abspath('BaseTools')
  os.environ['PYTHONPATH'] = os.path.join(os.environ['BASE_TOOLS_PATH'], 'Source/Python')
  if not os.path.exists(os.path.abspath('Conf')):
    os.makedirs(os.path.abspath('Conf'))
  os.environ['CONF_PATH'] = os.path.abspath('Conf')
  for name in ['target', 'tools_def', 'build_rule']:
    txt_file = os.path.abspath('Conf/%s.txt' % name)
    if not os.path.exists(txt_file):
      shutil.copy('BaseTools/Conf/%s.template' % name, 'Conf/%s.txt' % name)
  if os.name == 'posix':
    toolchain = 'GCC49'
    gcc_ver = subprocess.Popen(['gcc', '-dumpversion'], stdout=subprocess.PIPE)
    gcc_ver, err = subprocess.Popen(['sed', 's/\\..*//'], stdin=gcc_ver.stdout, stdout=subprocess.PIPE).communicate()
    if int(gcc_ver) > 4: toolchain = 'GCC5'
    if not os.path.exists('BaseTools/Source/C/bin/GenFfs'):
      if subprocess.call(['make', '-C', 'BaseTools']):
        print('Build BaseTools failed!')
        sys.exit(1)
    os.environ['PATH'] = os.environ['PATH'] + ':' + os.path.abspath('BaseTools/BinWrappers/PosixLike')
  elif os.name == 'nt':
    toolchain = ''
    vs_ver_list = {('2015', 'VS140COMNTOOLS'),
                   ('2013', 'VS120COMNTOOLS'),
                   ('2012', 'VS110COMNTOOLS'),
                   ('2010', 'VS100COMNTOOLS'),
                   ('2008', 'VS90COMNTOOLS'),
                   ('2005', 'VS80COMNTOOLS')}
    for vs_ver, vs_tool in vs_ver_list:
      if vs_tool in os.environ:
        toolchain = 'VS%s%s' % (vs_ver, 'x86')
        toolchainprefix = 'VS%s_PREFIX' % (vs_ver)
        os.environ[toolchainprefix] = os.path.join(os.environ[vs_tool], '../../')
        break
    if not toolchain:
      print 'Could not find supported Visual Studio version !'
      sys.exit(1)
    os.environ['PATH'] = os.environ['PATH'] + ';' + os.path.abspath('BaseTools/Bin/Win32')
    os.environ['TOOL_CHAIN'] = toolchain
    build_exe_path = os.path.join(os.environ['WORKSPACE'], 'BaseTools', 'Bin', 'Win32', 'build.exe')
    if not os.path.exists(build_exe_path):
      print 'Could not find pre-built BaseTools binaries, try to rebuild BaseTools ...'
      if 'PYTHON_HOME' not in os.environ:
          os.environ['PYTHON_HOME'] = 'C:\\Python27'
      if subprocess.call([os.path.abspath('BaseTools/toolsetup.bat'), 'rebuild']):
        print 'Build BaseTools failed!'
    os.environ['PATH'] = os.environ['PATH'] + ';' + os.path.abspath('BaseTools/BinWrappers/WindowsLike')
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
    ret = subprocess.call(['python', 'TranslateConfig.py', '-b', platform, '-a', architectrue])
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
                           '0x0000, SecCore:__ModuleEntryPoint, @Payload Entry point',
                           '0x0004, 0x600000                  , @Payload execution base'])
    if ret:
      print('patching failed')
      exit(1)

if __name__ == '__main__':

  os.environ['WORKSPACE'] = os.path.abspath(os.path.join(os.getcwd(),'../../edk2'))
  os.environ['PACKAGES_PATH'] = '%s;%s/..' % (os.environ['WORKSPACE'], os.getcwd())
  os.chdir(os.environ['WORKSPACE'])

  parser = argparse.ArgumentParser(prog='python %s' % sys.argv[0])
  parser.add_argument('p', help='platform', choices=['MinnowBoard3', 'Qemu'])
  parser.add_argument('a', help='architecture', choices=['IA32', 'X64'])
  parser.add_argument('t', help='target', choices=['RELEASE', 'DEBUG'])
  parser.add_argument('-n', help='thread number for building', type=int, default=multiprocessing.cpu_count())
  parser.add_argument('-c', help='clean', action='store_true')
  args = parser.parse_args()

  if len(sys.argv) == 1:
    parser.print_help(sys.stderr)
    sys.exit(1)

  if args.c:
    print('Removing Build and Conf directories ...')
    if os.path.exists('Build'): shutil.rmtree('Build')
    if os.path.exists('Conf'): shutil.rmtree('Conf')

  build(args.p, args.a, args.t, args.n)
  os.chdir('../UEFIPayload/UefiPayloadPkg')

