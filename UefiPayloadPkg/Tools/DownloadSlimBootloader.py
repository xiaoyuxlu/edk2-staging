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
import os, sys, shutil, subprocess

workPath = os.getcwd()

def gitDownload(path, Url, name):
    if not os.path.exists(path):
        os.makedirs(path)
    os.chdir(path)
    if os.path.exists(os.path.join(name, '.git')):
        os.chdir(name)
        subprocess.call(['git', 'pull'])
    else:
        if os.path.exists(name):
            shutil.rmtree(name)
        subprocess.call(['git', 'clone', '--recursive', Url, name])
    os.chdir(workPath)

if __name__ == '__main__':
    #download slimboot
    print('Downloading slim bootloader ...')
    gitDownload('../WorkSpace', 'https://github.com/slimbootloader/slimbootloader', 'SlimBootloader')
    PayloadBins = '../WorkSpace/SlimBootloader/PayloadPkg/PayloadBins'
    os.makedirs(PayloadBins)
    shutil.copy('SlimBootloaderIntegration.py', '../WorkSpace/SlimBootloader')

