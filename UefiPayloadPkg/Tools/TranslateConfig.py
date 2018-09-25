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

import sys
import os
import stat
import getopt
import signal
import subprocess
import shutil
import glob
import re
import json
import yaml
import ConfigParser

RULES_FILE = "Rules.json"
CONFIG_FILE_BASE = r"../CustomizationSample/Boards/"
SLIM_BOOT_DSC_FILE_BASE = r"../WorkSpace/SlimBootloader/Platform/"
UEFI_PAYLOAD_DSC_FILE_BASE = r".."
Arch = "IA32X64"

def load_rules_from_json(file):
    fd = open(file)
    rules = yaml.safe_load(fd)
    fd.close()
    return rules


def load_config_from_ini(files):
    config = ConfigParser.RawConfigParser()
    config.read(files)
    return config


def toInt(s):
    if s.find("{") > -1:
        nums = re.findall('0x[a-fA-F0-9][a-fA-F0-9]', s)
        num = ''.join([i.replace('0x','') for i in nums])
        return int(num,16)
    elif s.find("x") > -1:
        return int(s, 16)
    else:
        return int(s)


def get_dest_value(DestValue, SecMatches, ValueMatches, ValueRange):
    #
    # Compute Dest Value
    #
    Value = ""
    for ValueComponent in DestValue:
    #    print ("Value Component = %s" % ValueComponent)
        Match = re.match(r"^\$\([S|V]([0-9]+)\)", ValueComponent)
        if Match:
            GroupIdx = Match.group(1)
            if ValueComponent[2] == 'S':
                #
                # This is section matched value
                #
                MatchComponent = SecMatches[int(GroupIdx) - 1]
            else:
                #
                # This is source value matched value
                #
                print (ValueMatches)
                print (GroupIdx)
                MatchComponent = ValueMatches[int(GroupIdx) - 1]
        else:
            #
            # This is a normal component
            #
            MatchComponent = ValueComponent
        #
        # Search for a mapping for the MatchComponent
        #
        print ("Resolved ValueComponent = %s" % MatchComponent)

        Value = Value + MatchComponent
        if re.match('TRUE|FALSE', Value, re.IGNORECASE):
            Value = Value.upper()

        #
        # See if there is a mapping for the Value
        #
        if ValueRange:
            if ValueRange.find("-") > -1:
                ValueRange = re.split("\s*-\s*", ValueRange)
                if toInt(Value) < toInt(ValueRange[0]) or toInt(Value) > toInt(ValueRange[1]):
                    raise Exception('invalide value %s' % Value)
            elif ValueRange.find(":") > -1:
                map = {}
                ValueRange = re.split("\s*,\s*", ValueRange.lower())
                try:
                    for v in ValueRange:
                        map[toInt(v.split(":")[0])] = v.split(":")[1]
                    Value = toInt(Value)
                except:
                    for v in ValueRange:
                        map[re.sub('\s+', ' ', v.split(":")[0])] = v.split(":")[1]
                    Value = re.sub('\s+', ' ', Value.lower())
                if Value in map.keys():
                    #Value = map[Value]
                    pass
                else:
                    print(Value)
                    print(map)
                    raise Exception('invalide value %s' % Value)
            else:
                print(Value)
                print(ValueRange)
                if Value not in re.split("\s*,\s*", ValueRange):
                    raise Exception('invalide value %s' % Value)

    return Value


def translate(item, rule, SecMatches, ValueMatches, SlimBootDscFileBase):
    #
    # SlimBootDscFileBase is platform dependent, so receive it as a parameter
    #

    # for SecGroup in SecMatches:
        # print ("  Sec group: %s" % SecGroup)
    # for ValueGroup in ValueMatches:
        # print ("  Value group: %s" % ValueGroup)

    if rule["DestFile"] == "UefiPayloadPkgIA32X64.dsc":
        if Arch == 'IA32':
            rule["DestFile"] = "UefiPayloadPkgIA32.dsc"
        DestPath = UEFI_PAYLOAD_DSC_FILE_BASE
    else:
        DestPath = SlimBootDscFileBase

    #
    # Open target file
    #
    target_file = os.path.join(DestPath, rule["DestFile"])
    #print target_file
    if not os.path.exists(target_file):
        return  
    fd = open(target_file)
    Lines = fd.readlines()
    fd.close()
    OutFd = open(os.path.join(DestPath, rule["DestFile"] + "_o"), "w")
    #
    # Compute Match Regex and Substitude Regex from rule["DestItem"]
    #
    DestMatchRegex = ""
    DestSubstRegex = ""
    DestSubstString = ""
    SubstIdx = 0
    DestValueLast = 0
    DestValueFirst = 0
    SubstGroupBegin = True
    for DestComponent in rule["DestItem"]:
    #    print ("DestComponent = %s" % DestComponent)
        SubstComponent = ""
        Match = re.match(r"^\$\([S|V]([0-9]+)\)", DestComponent)
        if Match:
            GroupIdx = Match.group(1)
            if DestComponent[2] == 'S':
                #
                # This is section matched value
                #
                MatchComponent = SecMatches[int(GroupIdx) - 1]
                if SubstGroupBegin:
                    SubstComponent = "(?P<" + str(SubstIdx) + ">"
                    SubstGroupBegin = False
                SubstComponent = SubstComponent + SecMatches[int(GroupIdx) - 1]
            else:
                #
                # This is source value matched value
                #
                MatchComponent = ValueMatches[int(GroupIdx) - 1]
                if SubstGroupBegin:
                    SubstComponent = "(?P<" + str(SubstIdx) + ">"
                    SubstGroupBegin = False
                SubstComponent = SubstComponent + ValueMatches[int(GroupIdx) - 1]
        else:
            Match = re.match(r"\$\(D(.*)\)", DestComponent)
            if Match:
                #
                # This is DestValue
                #
                DestValueLast = 1
                MatchComponent = Match.group(1)  # a regex
                if DestSubstRegex != "":  # not at the line beginning
                    SubstComponent = SubstComponent + ")"
                    SubstIdx = SubstIdx + 1
                else:
                    DestValueFirst = 1
                SubstComponent = SubstComponent + Match.group(1)
                SubstGroupBegin = True
            else:
                #
                # This is a normal component
                #
                DestValueLast = 0
                MatchComponent = DestComponent
                if SubstGroupBegin:
                    SubstComponent = "(?P<" + "a" + str(SubstIdx + 1) + ">"
                    SubstGroupBegin = False
                SubstComponent = SubstComponent + DestComponent
        DestMatchRegex = DestMatchRegex + MatchComponent
        DestSubstRegex = DestSubstRegex + SubstComponent

    if DestValueLast == 0:
        DestSubstRegex = DestSubstRegex + ")"  # append the trailing ")" if the dest item ends with non Dest Value

    print ("DestMatchRegex is: %s" % DestMatchRegex)
    print ("DestSubstRegex is: %s" % DestSubstRegex)

    #
    # Process target file lines, find matches, and update according to rule
    #
    Matched = False
    while len(Lines):

        OutLine = Lines[0]
        Line = Lines.pop(0).strip()
        #      print ("Line to match: %s" % Line)

        #
        # Match the pattern against the current line
        #
        Match = re.match(DestMatchRegex, Line)
        if Match:
            Matched = True
            print ("MATCHES")
            #
            # Matches, substitute the target line with dest value
            #
            DestValue = get_dest_value(rule["DestValue"], SecMatches, ValueMatches, rule["SourceValueRange"])
            p = re.compile(DestSubstRegex)
            if SubstIdx == 1:
                if DestValueFirst:
                    DestSubstString = "%s\g<a1>"
                else:
                    DestSubstString = "\g<a1>%s"
            elif SubstIdx == 2:
                DestSubstString = "\g<a1>%s\g<a2>"
            else:
                print ("error: NO dest value in dest item")
                sys.exit(1)
            OutLine = p.sub(DestSubstString % DestValue, OutLine)

        OutFd.write(OutLine)

    #
    # Compose a new line if no matched line found in the file
    #
    if not Matched:
        #
        # Remove escape symbol. BUGBUG: is this complete?
        #
        AddedLine = str.replace(DestMatchRegex, "\.", ".")
        AddedLine = str.replace(AddedLine, "\|", "|")
        AddedLine = str.replace(AddedLine, "\s*", " ")

        DestValue = get_dest_value(rule["DestValue"], SecMatches, ValueMatches, rule["SourceValueRange"])
        p = re.compile(DestSubstRegex)
        if SubstIdx == 1:
            if DestValueFirst:
                DestSubstString = "\n%s\g<a1>"
            else:
                DestSubstString = "\n\g<a1>%s"
        elif SubstIdx == 2:
            DestSubstString = "\n\g<a1>%s\g<a2>"
        else:
            print ("error: NO dest value in dest item")
            sys.exit(1)
        AddedLine = p.sub(DestSubstString % DestValue, AddedLine)
        print ("AddedLine is: %s" % AddedLine)
        OutFd.write(AddedLine)

    OutFd.close()
    os.remove(os.path.join(DestPath, rule["DestFile"]))
    os.rename(os.path.join(DestPath, rule["DestFile"] + "_o"), os.path.join(DestPath, rule["DestFile"]))

    return 0


def main(args=sys.argv[1:]):
    #ConfigFileList = ['SlimbootConfigSample.ini','UefipayloadConfigSample.ini']
    ConfigFileList = []
    rc = 0
    global Arch
    #
    # get the -b BoardName option
    #
    try:
        opts, args = getopt.getopt(sys.argv[1:], "b:a:")
    except getopt.GetoptError as err:
        # print help information and exit:
        print "Command line option error\n"
        sys.exit(2)
    for o, a in opts:
        if o in ("-b"):
            BoardName = a
            ConfigFileBase = CONFIG_FILE_BASE + BoardName + "/Setup"
            RulesFile = ConfigFileBase + "/Rules/" + RULES_FILE
            if BoardName == "Qemu":
                SlimBootDscFileBase = SLIM_BOOT_DSC_FILE_BASE + "QemuBoardPkg/CfgData"
            else:
                SlimBootDscFileBase = SLIM_BOOT_DSC_FILE_BASE + "ApollolakeBoardPkg/CfgData"
            print "ConfigFileBase = %s\n" % ConfigFileBase
            print "RulesFile = %s\n" % RulesFile
            print "SlimBootDscFileBase = %s\n" % SlimBootDscFileBase
        elif o in ("-a"):
            Arch = a
        else:
            assert False, "unhandled option"

    #
    # Read rules
    #
    rules = load_rules_from_json(RulesFile)

    #
    # Load all configurations

    for i in os.listdir(ConfigFileBase):
        if i == "SourceCodes":
            continue
        ItemPath = os.path.join(ConfigFileBase, i)
        if os.path.isdir(ItemPath):
            ConfigPath = os.path.join(ItemPath, "Setup.ini")
            if os.path.exists(ConfigPath):
                ConfigFileList.append(ConfigPath)
    config = load_config_from_ini(ConfigFileList)

    #
    # Print out rules
    #
    #print ("\nRule list:\n===========")
    # index = 0
    # for i in rules['Rules']:
        # Section = i['SourceSection']
        # print "Rule %d on: %s" % (index, Section)
        # index = index + 1

    #print ("\nItem list:\n============")
    #
    # Traverse configurations
    #
    for i in config.sections():
    #    print ""
    #    print "[%s]" % i
        for j in config.items(i):
    #        print "%s = %s" % (j[0], j[1])
            #
            # check if <section name, key name> matches a rule
            #
            found = False
            for k in rules['Rules']:
                Section = k['SourceSection']
                Key = k['SourceKey']
                Value = k['SourceValue']
                SecMatch = re.match(Section, i)
                if SecMatch:
                    # KeyMatch = re.match(Key, j[0])
                    if Key == j[0]:
                        #
                        # found a rule that matches both section and key
                        #
                        found = True
               #         print ("  matches")
                        #
                        # get the matched value groups
                        #
                        ValueMatch = re.match(Value, j[1])
                        if not ValueMatch:
                            print ("error: value is not valid: rule: %s, item: %s" % (Value, j[1]))
                            sys.exit(1)
                        break
            if not found:
                print ("  no match")
                #
                # not found
                #
                continue
            #
            # found matching rule, translate
            # No need to pass KeyMatch groups?
            #
            translate(j, k, SecMatch.groups(), ValueMatch.groups(), SlimBootDscFileBase)

    #    GenCfgData = UpdateSlimBootloaderCfg.CGenCfgData()
    #    if GenCfgData.ParseDscFile(DSC_FILE, 'SiliconTest1', "0x33") != 0:
    #      print "ERROR: %s !" % GenCfgData.Error
    #      return 5

    sys.exit(rc)


if __name__ == '__main__':
    sys.exit(main())
