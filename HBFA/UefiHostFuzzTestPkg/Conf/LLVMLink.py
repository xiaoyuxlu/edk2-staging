## @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

import os
import subprocess
import sys
from optparse import OptionParser

# python version
python_version = sys.version_info[0]

def GetObjFile(TargetPath):
    ObjFileList = []
    static_library_files = open(os.path.join(TargetPath, r'static_library_files.lst'), 'r')
    static_library_files_list = static_library_files.readlines()
    static_library_files.close()

    for file in static_library_files_list:
        object_files = open(os.path.join(os.path.dirname(file.replace('.lib\n','.lib')), r'object_files.lst'), 'r')
        object_files_list = object_files.readlines()
        object_files.close()
        for objfile in object_files_list:
            ObjFileList.append(objfile.replace('.obj\n', '.obj'))

    return ObjFileList

def GenerateCommand(Command, Flags, InputFileList, OutputFile):
    Template = "<Command> -o <OutputFile> <Flags> <InputFileList>"
    InputFile = ' '.join(InputFileList)
    CommandLine = Template.replace("<Command>", Command).replace("<Flags>", Flags).replace("<InputFileList>", InputFile).replace("<OutputFile>", OutputFile)
    print(CommandLine)
    return CommandLine

def CallCommand(CommandLine):
    Cm = subprocess.Popen(CommandLine,
                          stdin=subprocess.PIPE,
                          stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE,
                          shell=True)
    msg = Cm.communicate()
    for m in msg:
        print(m if python_version == 2 else m.decode())

if __name__ == '__main__':
    # # # Opt Parser
    parse = OptionParser()
    parse.add_option("-o", dest="output", metavar=" ", help="output file name", default=None)
    parse.add_option("-d", dest="targetpath", metavar=" ", help="target path", default=None)
    parse.add_option("-t", dest="tool", metavar=" ", help="the tool you use", default=None)
    options, args = parse.parse_args()
    if options.tool:
        if options.targetpath:
            if options.output:
                CallCommand(GenerateCommand(options.tool, "", GetObjFile(options.targetpath), options.output))
            else:
                raise Exception("Please input -o output file name")
        else:
            raise Exception("Plesase input -d target file path")
    else:
        raise Exception("Please input -t tool you use.")