# # @file
# This file is used to schedule scripts execution sequence based on sequence.json
# under Config folder
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

# Add module in Lib to system path
import sys
import ure
import ujson
import uos
import utime

def initPath():
    global config_test_suite_path
    global config_test_suites_path
    global log_path_abs
    global log_path_relative
    global test_case_path
    log_path_relative = ""
    if len(sys.path) >= 1:
        first_path = sys.path[0]
        pattern = ure.compile("\\\\")
        items = pattern.split(first_path)
        if len(items) >= 1:
            lib_path = items[0] + "\\" + "MpyTest" + "\\" + "Lib"
            config_test_suite_path = items[0] + "\\" + "MpyTest" + "\\" + "Config" + "\\" + "Test_Suite.json"
            config_test_suites_path = items[0] + "\\" + "MpyTest" + "\\" + "Config" + "\\" + "Test_Suites.json"
            log_path_abs = items[0] + "\\" + "MpyTest" + "\\" + "Log" + "\\"
            test_case_path = items[0] + "\\" + "MpyTest" + "\\" + "Scripts"
            global drive
            drive = items[0]
            sys.path.append(lib_path)
            sys.path.append(test_case_path)
        else:
            print ('Warning! Please put current test script into the Lib folder')
            return False
    else:
        print ('Warning! Please put current test script into the Lib folder')

import mptf

def parseArgs():
    CASE_TOKEN = "-c"
    SUITE_TOKEN = "-s"
    SUITES_TOKEN = "-ss"
    tokens = [CASE_TOKEN, SUITE_TOKEN, SUITES_TOKEN]
    argc = 0
    for element in sys.argv:
        if element != "":
            argc = argc + 1
    INVALID_ARGUMENT = "Invalid argument. Input 'mptf.nsh -h' for usage info"
    case = ""
    suite = ""
    suites = ""

    previous_case_token = False
    previous_suite_token = False
    previous_suites_token = False

    for i in range(3, argc):
        if sys.argv[i] == CASE_TOKEN:
            previous_case_token = True
            continue
        if previous_case_token == True:
            if sys.argv[i] in tokens:
                print (INVALID_ARGUMENT)
                return False
            else:
                case = sys.argv[i]
                suite = ""
                suites = ""
                previous_case_token = False
                continue

        if sys.argv[i] == SUITE_TOKEN:
            previous_suite_token = True
            continue
        if previous_suite_token == True:
            if sys.argv[i] in tokens:
                print (INVALID_ARGUMENT)
                return False
            else:
                suite = sys.argv[i]
                case = ""
                suites = ""
                previous_suite_token = False
                continue

        if sys.argv[i] == SUITES_TOKEN:
            previous_suites_token = True
            continue
        if previous_suites_token == True:
            if sys.argv[i] in tokens:
                print (INVALID_ARGUMENT)
                return False
            else:
                suites = sys.argv[i]
                case = ""
                suite = ""
                previous_suites_token = False
                continue

    if previous_case_token == True:
        print (INVALID_ARGUMENT)
        return False

    if previous_suite_token == True:
        f = open(config_test_suite_path)
        data = ujson.load(f)
        execution_item = {}
        print ("Available Test Suite are: ")
        print ("==========================")
        for test_suite in data['test_suite']:
            print(test_suite['name'])
        print ("==========================")
        return False

    if previous_suites_token == True:
        f = open(config_test_suites_path)
        data = ujson.load(f)
        execution_item = {}
        print ("Available Test Suites are: ")
        print ("==========================")
        for test_suites in data['test_suites']:
            print(test_suites['name'])
        print ("==========================")
        return False
    if suites == "" and case == "" and suite == "":
        print (INVALID_ARGUMENT)
        return False

    print ("Test Case is: " + case)
    print ("Test Suite is: " + suite)
    print ("Test Suites is: " + suites)

    if case != "":
        time_stamp = get_stamp()
        suites_folder = "SUITES__" + "CASE" + "__" + time_stamp
        uos.mkdir(log_path_abs + suites_folder)
        suite_folder = "SUITE__" + "CASE" + "__" + time_stamp
        uos.mkdir(log_path_abs + suites_folder + "\\" + suite_folder)
        iteration_folder = "NO0"
        uos.mkdir(log_path_abs + suites_folder + "\\" + suite_folder + "\\" + iteration_folder)
        schedule_case(case, suites_folder + "\\" + suite_folder + "\\" + iteration_folder)

    if suite != "":
        time_stamp = get_stamp()
        suites_folder = "SUITES__" + "SUITE" + "__" + time_stamp
        uos.mkdir(log_path_abs + suites_folder)
        suite_folder = "SUITE__" + suite + "__" + time_stamp
        uos.mkdir(log_path_abs + suites_folder + "\\" + suite_folder)
        schedule_suite(suite, suites_folder + "\\" + suite_folder)

    if suites != "":
        suites_folder = "SUITES__" + suites + "__" + get_stamp()
        uos.mkdir(log_path_abs + suites_folder)
        schedule_suites(suites, suites_folder)

def schedule_case(case_name, relative_path):
    print ("The test case " + case_name + " is scheduled...")
    if case_name.endswith(".py"):
        case_name = case_name[:-3]
    script_module = __import__(case_name)
    if relative_path == "":
        relative_path = "CASE__" + case_name
    else:
        relative_path = relative_path + "\\" + case_name
    print ("The relative path is: " + relative_path)
    script_module.run(relative_path)

def schedule_suite(suite_name, relative_path):
    f = open(config_test_suite_path)
    data = ujson.load(f)
    execution_item = {}
    for test_suite in data['test_suite']:
        if test_suite['name'] == suite_name:
            execution_item = test_suite

    ordered_script_list = []
    print(execution_item)

    execution_item['sequence'].sort(key=lambda x: x['order'])

    for script_item in execution_item['sequence']:
        if script_item['order'] < 0:
            execution_item['sequence'].remove(script_item)

    '''
    for script_item in execution_item['sequence']:
        print (script_item['name'])
    '''

    cases = execution_item['sequence']

    for i in range(0, execution_item['run_times']):
        iteration_folder = log_path_abs + relative_path + "\\" + "NO" + str(i)
        uos.mkdir(iteration_folder)
        for case in cases:
            schedule_case(case['name'], relative_path + "\\" + "NO" + str(i))

def schedule_suites(suites_name, relative_path):
    f = open(config_test_suites_path)
    data = ujson.load(f)
    execution_item = {}

    for test_suites in data['test_suites']:
        if test_suites['name'] == suites_name:
            execution_item = test_suites

    ordered_suite_list = []
    print(execution_item)

    execution_item['test_suite'].sort(key=lambda x: x['order'])

    for suite_item in execution_item['test_suite']:
        if suite_item['order'] < 0:
            execution_item['test_suite'].remove(suite_item)

    '''
    for suite_item in execution_item['test_suite']:
        print (suite_item['name'])
    '''

    for suite in execution_item['test_suite']:
        suite_folder = "SUITE__" + suite['name'] + "__" + get_stamp()
        uos.mkdir(log_path_abs + relative_path + "\\" + suite_folder)
        schedule_suite(suite['name'], relative_path + "\\" + suite_folder)


def get_stamp():
    localtime = utime.localtime()
    year = "{0:04d}".format(localtime[0])
    month = "{0:02d}".format(localtime[1])
    day = "{0:02d}".format(localtime[2])
    hour = "{0:02d}".format(localtime[3])
    minute = "{0:02d}".format(localtime[4])
    second = "{0:02d}".format(localtime[5])
    time_in_format = year + "_" + month + "_" + day + "__" + hour + "_" + minute + "_" + second
    return time_in_format

if __name__ == "__main__":
    initPath()
    parseArgs()
