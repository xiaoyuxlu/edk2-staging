# # @file
# This file is a demo script to automate shell operations and HII environment
#
# INTEL CONFIDENTIAL
# Copyright (c) 2017-2018, Intel Corporation. All rights reserved.<BR>
#
# The source code contained or described herein and all documents
# related to the source code ("Material") are owned by Intel Corporation
# or its suppliers or licensors. Title to the Material remains with Intel
# Corporation or its suppliers and licensors. The Material contains trade
# secrets and proprietary and confidential information of Intel or its
# suppliers and licensors. The Material is protected by worldwide copyright
# and trade secret laws and treaty provisions. No part of the Material may be
# used, copied, reproduced, modified, published, uploaded, posted, transmitted,
# distributed, or disclosed in any way without Intel's prior express written
# permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or
# delivery of the Materials, either expressly, by implication, inducement,
# estoppel or otherwise. Any license under such intellectual property
# rights must be express and approved by Intel in writing.
#

#Sample case for shell basic command test and HII environment operation

import sys
import ure
import mptf

def run(log_path):
    obj = mptf.mptf(log_path)
    obj.Input('cls' + mptf.ENTER)
    obj.SetTickTock(100)

    ########   edit   ######## 
    obj.Input('edit' + mptf.ENTER)
    obj.FuncKey(mptf.ENTER, 4)
    result1 = obj.WaitUntil('UEFI EDIT')
    if result1 == True:
        obj.Pass('EDIT opened')
    else:
        obj.Fail('Fail to EDIT')
    obj.Input("Hello MicroPython Test Framework for UEFI",1)
    obj.Input(".........", 1000)
    obj.FuncKey(mptf.F3)
    obj.Input("N")

    obj.Input('cls' + mptf.ENTER)
    obj.Input('fs0:' + mptf.ENTER)
    obj.SetTickTock(200)
    ########   ls   ########
    obj.Info('Shell Command \'ls\': ',True)
    obj.Input('ls')
    obj.FuncKey(mptf.ENTER)
    if obj.WaitUntil('Dir', 10):
        obj.Pass('List USB Driver PASS',True)
    else:
        obj.Fail('List USB Driver PASS FAIL',True)

    ########  echo  ########
    obj.Info('Shell Command \'echo\': ',True)
    obj.Input('cls' + mptf.ENTER)
    obj.Input('echo hello world' + mptf.ENTER)
    if obj.Verify('hello world', None):
        obj.Pass('Print Hello World PASS',True)
    else:
        obj.Fail('Print Hello World FAIL',True)

    ########  mkdir  ########
    obj.Info('Shell Command \'mkdir\': ',True)
    obj.Input('cls' + mptf.ENTER)
    obj.Input('mkdir demo1' + mptf.ENTER)
    obj.Input('ls')
    obj.FuncKey(mptf.ENTER)
    if obj.Verify('demo1', None):
        obj.Pass('mkdir PASS',True)
    else:
        obj.Fail('mkdir FAIL',True)

    ########  rm  ########
    obj.Input('cls' + mptf.ENTER)
    obj.Input('rm demo1' + mptf.ENTER)
    if obj.Verify('Delete successful', None):
        obj.Pass('rm PASS',True)
    else:
        obj.Fail('rm FAIL',True)
    obj.Input('ls')
    obj.FuncKey(mptf.ENTER)

    ########  exit  ########
    obj.Info('Shell Command \'exit\': ',True)
    obj.Input('exit' + mptf.ENTER)
    obj.Info('Shell Command \'exit\': ',True)

    ########  HII environment change boot order  ########  
    result = obj.SelectOption('Boot Maintenance Manager',mptf.LIGHTGRAY + mptf.BACKBLACK)
    obj.Debug ('Boot Maintenance Manager result = ' + str(result))
    if result:
        obj.FuncKey(mptf.ENTER)
        obj.Pass('Find Boot Maintenance Manager PASS',True)
    else:
        obj.Fail('Find Boot Maintenance Manager FAIL',True)
    boot_result = obj.SelectOption('Boot Options', mptf.LIGHTGRAY + mptf.BACKBLACK)
    obj.Debug ('Boot Options result = ' + str(boot_result))
    if boot_result:
        obj.FuncKey(mptf.ENTER)
        obj.Pass('Find Boot Options PASS',True)
    else:
        obj.Fail('Find Boot Options FAIL',True)
    bootorder_result = obj.SelectOption('Change Boot Order', mptf.LIGHTGRAY + mptf.BACKBLACK)
    obj.Debug ('Change Boot Order result = ' + str(bootorder_result))
    if bootorder_result:
        obj.FuncKey(mptf.ENTER)
        obj.FuncKey(mptf.ENTER)
        obj.Pass('Find Change Boot Order PASS',True)
    else:
        obj.Fail('Find Change Boot Order FAIL',True)
    obj.SetTickTock(500)
    obj.FuncKey(mptf.DOWN)
    obj.Input('+')
    obj.FuncKey(mptf.ENTER)
    obj.FuncKey(mptf.ENTER)
    obj.FuncKey(mptf.DOWN)
    obj.Input('+')
    obj.FuncKey(mptf.ENTER)
    obj.FuncKey(mptf.ESC)
    obj.FuncKey(mptf.ESC)
    obj.FuncKey(mptf.ESC)
    result2 = obj.SelectOption('Continue',mptf.LIGHTGRAY + mptf.BACKBLACK)
    obj.Debug ('Continue = ' + str(result2))
    if result2:
        obj.FuncKey(mptf.ENTER)
    
    shell_result = obj.WaitUntil('Shell> ', 10)
    if shell_result:
        obj.Input('echo The sample case is end' + mptf.ENTER)
    obj.Info('The sample case is end.', True)
    obj.Close()

