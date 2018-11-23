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

def get_toolchain():
  if os.name == 'posix':
    toolchain = 'GCC49'
    gcc_ver = subprocess.Popen(['gcc', '-dumpversion'], stdout=subprocess.PIPE)
    (gcc_ver, err) = subprocess.Popen(['sed', 's/\\..*//'], stdin=gcc_ver.stdout, stdout=subprocess.PIPE).communicate()
    if int(gcc_ver) > 4: toolchain = 'GCC5'
  elif os.name == 'nt':
    toolchain = ''
    os.environ['PATH'] = os.environ['PATH'] + ';' + os.path.abspath('BaseTools/Bin/Win32')
    vs_ver_list = [
      ('2015', 'VS140COMNTOOLS'),
      ('2013', 'VS120COMNTOOLS'),
      ('2012', 'VS110COMNTOOLS'),
      ('2010', 'VS100COMNTOOLS'),
      ('2008', 'VS90COMNTOOLS'),
      ('2005', 'VS80COMNTOOLS')
    ]
    for vs_ver, vs_tool in vs_ver_list:
      if vs_tool in os.environ:
        toolchain='VS%s%s' % (vs_ver, 'x86')
        toolchainprefix = 'VS%s_PREFIX' % (vs_ver)
        os.environ[toolchainprefix] = os.path.join(os.environ[vs_tool], '../../')
        break
    if not toolchain:
      print 'Could not find supported Visual Studio version !'
      sys.exit(1)
  else:
    print('Unsupported operating system !')
    sys.exit(1)
  return toolchain

def build(platform, architectrue, target):
  payload = '../../../../edk2/Build/UefiPayloadPkg%s/%s_%s/FV/UEFIPAYLOAD.fd' % (architectrue, target, get_toolchain())
  print ('***** %s\n' % payload)
  if os.path.exists(payload):
    shutil.copy(payload, 'PayloadPkg/PayloadBins/UefiPld.fd')
  else:
    print('corresponding payload binary not found, please build payload first')
    exit(1)
  os.chdir('../../Tools')
  ret = subprocess.call(['python', 'TranslateConfig.py', '-b', platform, '-a', architectrue])
  os.chdir('../WorkSpace/SlimBootloader')
  if ret:
    print('translating configuration failed!')
    sys.exit(1)
  print('start building Slim Bootloader ...')
  cmd = 'python BuildLoader.py build -p OsLoader.efi:LLDR:Lz4;UefiPld.fd:UEFI:Lzma %s %s' % \
    ('apl' if platform == 'ApolloLake' else 'qemu', '' if target == 'DEBUG' else '-r')
  ret = subprocess.call(cmd.split())
  if ret:
    print('building Slim Bootloader failed')
    exit(1)
  if platform == 'ApolloLake':
    with open('board.info') as bd:
      Board = bd.read().strip()
    if Board:
      Board = ' -p ' + Board
    cmd = 'python Platform/ApollolakeBoardPkg/Script/StitchLoader.py -i base.bin -s Outputs/apl/Stitch_Components.zip -o APL/Output/APL_BX_SPI_IFWI.bin%s' % Board 
    ret = subprocess.call(cmd.split())
    if ret:
      print('stitching failed')
      exit(1)
    shutil.copy('APL/Output/APL_BX_SPI_IFWI.bin', '../../firmware.bin')
  else:
    shutil.copy('Outputs/qemu/SlimBootloader.bin', '../../firmware.bin')

if __name__ == '__main__':
  os.chdir('../WorkSpace/SlimBootloader')
  parser = argparse.ArgumentParser(prog='python %s' % sys.argv[0])
  parser.add_argument('p', help='platform', choices=['ApolloLake', 'Qemu'])
  parser.add_argument('a', help='architecture', choices=['IA32', 'X64'])
  parser.add_argument('t', help='payload\'s target', choices=['RELEASE', 'DEBUG'])
  parser.add_argument('-c', help='clean', action='store_true')
  args = parser.parse_args()

  if len(sys.argv) == 1:
    parser.print_help(sys.stderr)
    sys.exit(1)

  if args.c:
    print('Removing Build and Conf directories ...')
    if os.path.exists('Build'): shutil.rmtree('Build')
    if os.path.exists('Conf'): shutil.rmtree('Conf')

  build(args.p, args.a, args.t)
  os.chdir('../../Tools')

