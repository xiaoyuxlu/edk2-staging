## @file
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

import os
import shutil

try:
    import ConfigParser as configparser
except Exception as e:
    import configparser as configparser


class Common(object):
    def __init__(self, path):
        self.conf = configparser.ConfigParser()
        self.conf_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'Env.conf')
        self.conf.read(self.conf_path)
        self.OutputPath = self.conf.get('afl', 'OutputPath').strip()
        self.test_case_path = path
        self.case = os.path.basename(self.test_case_path).split('.inf')[0].strip()
        self.workspace = os.environ['WORKSPACE']

    def copy_seeds(self, original_seed_paths):
        if os.path.exists(os.path.join(self.workspace, "SEEDS", "%s_seeds") % self.case):
            shutil.rmtree(os.path.join(self.workspace, "SEEDS", "%s_seeds") % self.case, ignore_errors=True)
        os.makedirs(os.path.join(self.workspace, "SEEDS", "%s_seeds") % self.case)
        for path in original_seed_paths:
            shutil.copyfile(path.strip(),
                            os.path.join(self.workspace, "SEEDS", "%s_seeds" % self.case, os.path.basename(path)))

    def _get_exec_time(self):
        content = open(os.path.join(self.OutputPath, 'afl_%s' % self.case, 'plot_data'), 'r')
        lines = content.readlines()
        content.close()
        if len(lines) > 1:
            line1 = lines[1]
            line_last = lines[-1]
            time_start = int(line1.split(',')[0])

            time_end = int(line_last.split(',')[0])
            execution_time = (time_end - time_start)
            day = int(execution_time / (24 * 3600))
            hours = int(execution_time % (24 * 3600) / 3600)
            mins = int(execution_time % (24 * 3600) % 3600 / 60)
            seconds = int(execution_time % (24 * 3600) % 3600 % 60)
            return day, hours, mins, seconds
        else:
            return 0, 0, 0, 0

    def get_crash_hang(self):
        try:
            day, hours, mins, seconds = self._get_exec_time()
            execution_time = "%s days,%s hrs,%s min,%s sec" % (day, hours, mins, seconds)
            hangs = len(os.listdir(os.path.join(self.OutputPath, 'afl_%s' % self.case, 'hangs')))
            crashes_num = len(os.listdir(os.path.join(self.OutputPath, 'afl_%s' % self.case, 'crashes')))
            if crashes_num == 0:
                crashes = 0
            elif crashes_num > 0:
                crashes = crashes_num - 1
            seeds = len(os.listdir(os.path.join(self.OutputPath, 'afl_%s' % self.case, 'queue'))) - 1
            return seeds, crashes, hangs, execution_time
        except Exception as e:
            print('get crash hang Error:%s' % e)
            return 0, 0, 0, 0
