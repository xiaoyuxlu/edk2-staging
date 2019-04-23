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

#include <Uefi.h>
#include <UnitTestTypes.h>
#include <Library/UnitTestLib.h>
#include <Library/UnitTestAssertLib.h>

#include <Library/${HeaderFileName}>

""")

gCIncludeString = TemplateString("""\
#include "${ModuleName}Test.h"

""")

gHIncludeString = """\

#define UNIT_TEST_NAME        L"${ModuleName} Unit Test"
#define UNIT_TEST_VERSION     L"0.1"

typedef struct {
  CHAR16               *Description;
  CHAR16               *ClassName;
  UNIT_TEST_FUNCTION   Func;
  UNIT_TEST_PREREQ     PreReq;
  UNIT_TEST_CLEANUP    CleanUp;
  UNIT_TEST_CONTEXT    Context;
} TEST_DESC;

typedef struct {
  CHAR16                     *Title;
  CHAR16                     *Package;
  UNIT_TEST_SUITE_SETUP      Sup;
  UNIT_TEST_SUITE_TEARDOWN   Tdn;
  UINTN                      *TestNum;
  TEST_DESC                  *TestDesc;
} SUITE_DESC;

"""

gCCaseMainBeginString = """\

SUITE_DESC  mSuiteDesc[] = {
"""

gCCaseMainString = TemplateString("""\
  {L"test of ${CaseName}()", L"${CaseName}.Basic", NULL, NULL, &m${CaseName}TestNum, m${CaseName}Test },
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
  UINTN                     TestIndex;
  EFI_STATUS                Status;
  UNIT_TEST_FRAMEWORK       *Fw = NULL;
  CHAR16                    ShortName[100];

  ShortName[0] = L'\0';

  UnicodeSPrint (&ShortName[0], sizeof(ShortName), L"%a", gEfiCallerBaseName); 
  DEBUG((DEBUG_INFO, "%s v%s\n", UNIT_TEST_NAME, UNIT_TEST_VERSION));

  Status = InitUnitTestFramework (&Fw, UNIT_TEST_NAME, ShortName, UNIT_TEST_VERSION);
  if (EFI_ERROR(Status)) {
    goto EXIT;
  }

  for (SuiteIndex = 0; SuiteIndex < ARRAY_SIZE(mSuiteDesc); SuiteIndex++) {
    UNIT_TEST_SUITE *Suite = NULL;
    Status = CreateUnitTestSuite (&Suite, Fw, mSuiteDesc[SuiteIndex].Title, mSuiteDesc[SuiteIndex].Package, mSuiteDesc[SuiteIndex].Sup, mSuiteDesc[SuiteIndex].Tdn);
    if (EFI_ERROR (Status)) {
      Status = EFI_OUT_OF_RESOURCES;
      goto EXIT;
    }
    for (TestIndex = 0; TestIndex < *mSuiteDesc[SuiteIndex].TestNum; TestIndex++) {
      AddTestCase (Suite, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->Description, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->ClassName, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->Func, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->PreReq, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->CleanUp, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->Context);
    }
  }

  Status = RunAllTestSuites (Fw);

EXIT:
  if (Fw != NULL) {
    FreeUnitTestFramework (Fw);
  }

  return Status;
}
""")

gCCaseMainHString = TemplateString("""\
TEST_DESC m${CaseName}Test[];
UINTN m${CaseName}TestNum;

""")

gCCaseString = TemplateString("""\

UNIT_TEST_STATUS
EFIAPI
${CaseName}TestFunc1 (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  return UNIT_TEST_PASSED;
}

UNIT_TEST_STATUS
EFIAPI
${CaseName}TestConf1 (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  return UNIT_TEST_PASSED;
}

TEST_DESC m${CaseName}Test[] = {
  { "${CaseName}TestFunc1(): ", L"${CaseName}.Basic.Func", ${CaseName}TestFunc1, NULL, NULL, NULL },
  { "${CaseName}TestConf1(): ", L"${CaseName}.Basic.Conf", ${CaseName}TestConf1, NULL, NULL, NULL }
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
    #print("\tGenTestCaseTemplate.py \home\XXXPkg\Include\Library\YYYLib.h XXXPkg\Library\ZZZLib .\Host\XXXCasePkg")

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
