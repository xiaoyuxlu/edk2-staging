## @ GenTestCaseTemplate.py
#
# Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent

**/

""")

gHIncludePreString = TemplateString("""\
#ifndef _${ModuleName}_TEST_H_
#define _${ModuleName}_TEST_H_

""")

gHIncludePostString = """\

#endif

"""

gCHIncludeString = TemplateString("""\
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <Uefi.h>
#include <Library/${HeaderFileName}>

""")

gCIncludeString = TemplateString("""\
#include "${ModuleName}Test.h"

""")

gHIncludeString = """\

typedef struct {
  char8                      *GroupName;
  CMFixtureFunction          GroupSetup;
  CMFixtureFunction          GroupTeardown;
  UINTN                      *TestNum;
  struct CMUnitTest          *TestDesc;
} SUITE_DESC;

"""

gCCaseMainBeginString = """\

SUITE_DESC  mSuiteDesc[] = {
"""

gCCaseMainString = TemplateString("""\
  {"test of ${CaseName}()", NULL, NULL, &m${CaseName}TestNum, m${CaseName}Test },
""")

gCCaseMainEndString = """\
};
"""

gCCaseMainRestString = TemplateString("""\

/**
  The main() function for setting up and running the tests.

  @retval CUE_SUCCESS on successful running
  @retval Other CUnit error code on failure.
**/
int main()
{
  UINTN                     SuiteIndex;

  if (PcdGet8 (HostUnitTestMode) == 1) {
    cmocka_set_message_output (CM_OUTPUT_XML);
  }

  for (SuiteIndex = 0; SuiteIndex < ARRAY_SIZE(mSuiteDesc); SuiteIndex++) {
    _cmocka_run_group_tests (mSuiteDesc[SuiteIndex]->GroupName, mSuiteDesc[SuiteIndex]->TestDesc, mSuiteDesc[SuiteIndex]->NumTests, mSuiteDesc[SuiteIndex]->GroupSetup, mSuiteDesc[SuiteIndex]->GroupTeardown);
  }

  return 0;
}
""")

gCCaseMainHString = TemplateString("""\
TEST_DESC m${CaseName}Test[];
UINTN m${CaseName}TestNum;

""")

gCCaseString = TemplateString("""\

void ${CaseName}TestFunc1 (void **state)
{
}

void ${CaseName}TestConf1 (void **state)
{
}

struct CMUnitTest m${CaseName}Test[] = {
  { "${CaseName}TestFunc1(): ", ${CaseName}TestFunc1, NULL, NULL, NULL },
  { "${CaseName}TestConf1(): ", ${CaseName}TestConf1, NULL, NULL, NULL }
};

UINTN m${CaseName}TestNum = ARRAY_SIZE(m${CaseName}Test);

""")


class TestCaseTemplate:
    def __init__(self):
        self.FunctionList = []
        self.ModuleLocation = ""
        self.TestPkgPath = ""
        self.ModuleName = ""
        self.HeaderFileName = ""
        self.ModulePkgName = ""
        self.TestPkgName = ""

    def CreateFuncList(self, libHFile):
        f = open(libHFile, 'r')
        lines = f.readlines ()
        foundEFIAPI = False
        for line in lines:
            line.strip()
            if cmp (line[:-1], "EFIAPI") == 0:
                foundEFIAPI = True
                #print('found EFIAPI')
                continue
            if foundEFIAPI :
                word = re.split('[ (]', line)
                self.FunctionList.append(word[0])
                foundEFIAPI = False
        #for func in self.FunctionList:
        #    print("func - \"" + func + "\"")
        f.close()
        word = re.split('[\\\\/]', libHFile)
        self.HeaderFileName = word[-1]

    def CreateModuleLocation(self, ModuleLocation):
        self.ModuleLocation = ModuleLocation
        word = re.split('[\\\\/]', self.ModuleLocation)
        self.ModuleName = word[-1]
        self.ModulePkgName = word[0]

    def CreateTestPkgPath(self, TestPkgPath):
        self.TestPkgPath = TestPkgPath
        word = re.split('[\\\\/]', TestPkgPath)
        self.TestPkgName = word[-1]

    def CreateCaseDir(self):
        caseDir= str(self.TestPkgPath) + "/" + str("TestCase") + "/" + str(self.ModuleLocation) + "/"
        if not os.path.exists(caseDir):
            os.makedirs(caseDir)

    def CreateCaseInf(self):
        caseInf= str(self.TestPkgPath) + "/" + str("TestCase") + "/" + str(self.ModuleLocation) + "/" + str(self.ModuleName) + "Test" + ".inf"
        if os.path.exists(caseInf):
            os.remove(caseInf)
        f = open(caseInf, 'w')
        infText = TemplateString()
        infText.Append(gInfHeaderString.Replace({'FileName':self.ModuleName}))
        infText.Append(gInfDefinesString.Replace({'FileName':self.ModuleName}))
        infText.Append(gInfSourcesString)
        infText.Append("  " + self.ModuleName + "Test.c\n")
        for list in self.FunctionList :
            infText.Append("  " + list + "Test.c\n")
        infText.Append("\n")
        infText.Append(gInfRestsString.Replace({'TestPkgName':self.TestPkgName, 'ModulePkgName':self.ModulePkgName}))
        f.write(str(infText))
        f.close()

    def CreateCaseMainC(self):
        caseMainC = str(self.TestPkgPath) + "/" + str("TestCase") + "/" + str(self.ModuleLocation) + "/" + str(self.ModuleName) + "Test" + ".c"
        if os.path.exists(caseMainC):
            os.remove(caseMainC)
        f = open(caseMainC, 'w')
        cText = TemplateString()
        cText.Append(gHeaderString.Replace({'FileName':self.ModuleName}))
        cText.Append(gCHIncludeString.Replace({'HeaderFileName':self.HeaderFileName}))
        cText.Append(gCIncludeString.Replace({'ModuleName':self.ModuleName}))
        cText.Append(gCCaseMainBeginString)
        for list in self.FunctionList :
            cText.Append(gCCaseMainString.Replace({'CaseName':list}))
        cText.Append(gCCaseMainEndString)
        cText.Append(gCCaseMainRestString.Replace({'ModuleName':self.ModuleName}))
        f.write(str(cText))
        f.close()

    def CreateCaseMainH(self):
        caseMainH = str(self.TestPkgPath) + "/" + str("TestCase") + "/" + str(self.ModuleLocation) + "/" + str(self.ModuleName) + "Test" + ".h"
        if os.path.exists(caseMainH):
            os.remove(caseMainH)
        f = open(caseMainH, 'w')
        hText = TemplateString()
        hText.Append(gHeaderString.Replace({'FileName':self.ModuleName}))
        hText.Append(gHIncludePreString.Replace({'ModuleName':self.ModuleName}))
        hText.Append(gCHIncludeString.Replace({'HeaderFileName':self.HeaderFileName}))
        hText.Append(gHIncludeString)
        for list in self.FunctionList :
            hText.Append(gCCaseMainHString.Replace({'CaseName':list}))
        hText.Append(gHIncludePostString)
        f.write(str(hText))
        f.close()

    def CreateCaseC(self):
        for list in self.FunctionList :
          caseC = str(self.TestPkgPath) + "/" + str("TestCase") + "/" + str(self.ModuleLocation) + "/" + list + "Test" + ".c"
          if os.path.exists(caseC):
              os.remove(caseC)
          f = open(caseC, 'w')
          cText = TemplateString()
          cText.Append(gHeaderString.Replace({'FileName':list}))
          cText.Append(gCHIncludeString.Replace({'HeaderFileName':self.HeaderFileName}))
          cText.Append(gCIncludeString.Replace({'ModuleName':self.ModuleName}))
          cText.Append(gCCaseString.Replace({'CaseName':list}))
          f.write(str(cText))
          f.close()

def usage():
    print("Usage:")
    print("\tGenTestCaseTemplate <HeadFilePath> <ModuleLocation> <TestPkgPath>")
    print("\t\t <HeadFilePath>    The path of the header file")
    print("\t\t <ModuleLocation>  The location of the module")
    print("\t\t <TestPkgPath>     The path of the TestPkg.")
    #print("Example:")
    #print("\tGenTestCaseTemplate.py \home\XXXPkg\Include\Library\YYYLib.h XXXPkg\Library\ZZZLib .\cmocka\XXXTestCasePkg")

def main():
    global TestCaseTemplate

    if len(sys.argv) < 4:
        usage()
        return 1

    testCaseTemplate = TestCaseTemplate()

    testCaseTemplate.CreateFuncList (sys.argv[1])
    testCaseTemplate.CreateModuleLocation (sys.argv[2])
    testCaseTemplate.CreateTestPkgPath (sys.argv[3])

    testCaseTemplate.CreateCaseDir()
    testCaseTemplate.CreateCaseInf()
    testCaseTemplate.CreateCaseMainC()
    testCaseTemplate.CreateCaseMainH()
    testCaseTemplate.CreateCaseC()

if __name__ == '__main__':
    sys.exit(main())
