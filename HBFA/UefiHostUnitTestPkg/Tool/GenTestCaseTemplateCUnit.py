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
#include <Basic.h>
#include <Automated.h>

#include <Uefi.h>
#include <Library/${HeaderFileName}>

""")

gCIncludeString = TemplateString("""\
#include "${ModuleName}Test.h"

""")

gHIncludeString = """\

typedef struct {
  const char  *Name;
  CU_TestFunc TestFunc;
} TEST_DESC;

typedef struct {
  const char        *Name;
  CU_InitializeFunc InitFunc;
  CU_CleanupFunc    CleanFunc;
  int               *TestNum;
  TEST_DESC         *TestDesc;
} SUITE_DESC;

"""

gCCaseMainBeginString = """\

SUITE_DESC  mSuiteDesc[] = {
"""

gCCaseMainString = TemplateString("""\
  {"test of ${CaseName}()", InitSuite${CaseName}, CleanSuite${CaseName}, &m${CaseName}TestNum, m${CaseName}Test },
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
  int     SuiteIndex;
  int     TestIndex;

  if (CUE_SUCCESS != CU_initialize_registry()) {
    return CU_get_error();
  }

  for (SuiteIndex = 0; SuiteIndex < ARRAY_SIZE(mSuiteDesc); SuiteIndex++) {
    CU_pSuite Suite = NULL;
    Suite = CU_add_suite(mSuiteDesc[SuiteIndex].Name, mSuiteDesc[SuiteIndex].InitFunc, mSuiteDesc[SuiteIndex].CleanFunc);
    if (Suite == NULL) {
      CU_cleanup_registry();
      return CU_get_error();
    }
    for (TestIndex = 0; TestIndex < *mSuiteDesc[SuiteIndex].TestNum; TestIndex++) {
      if (NULL == CU_add_test(Suite, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->Name, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->TestFunc)) { 
        CU_cleanup_registry();
        return CU_get_error();
      }
    }
  }

#ifdef CUNIT_TEST_XML_MODE
  CU_set_output_filename("${ModuleName}TestSuites");
  CU_list_tests_to_file();
  CU_automated_run_tests();
#else
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
#endif

  CU_cleanup_registry();
  return CU_get_error();
}
""")

gCCaseMainHString = TemplateString("""\
int InitSuite${CaseName}(void);
int CleanSuite${CaseName}(void);
TEST_DESC m${CaseName}Test[];
int m${CaseName}TestNum;

""")

gCCaseString = TemplateString("""\
int InitSuite${CaseName}(void)
{
  return 0;
}

int CleanSuite${CaseName}(void)
{
  return 0;
}

void ${CaseName}TestFunc1 (void)
{
}

void ${CaseName}TestConf1 (void)
{
}

TEST_DESC m${CaseName}Test[] = {
  { "${CaseName}TestFunc1(): ", ${CaseName}TestFunc1 },
  { "${CaseName}TestConf1(): ", ${CaseName}TestConf1 }
};

int m${CaseName}TestNum = ARRAY_SIZE(m${CaseName}Test);

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
    #print("\tGenTestCaseTemplate.py \home\XXXPkg\Include\Library\YYYLib.h XXXPkg\Library\ZZZLib .\CUnit\XXXTestCasePkg")

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
