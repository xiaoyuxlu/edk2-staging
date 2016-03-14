## Introduction

This branch(edk2-host-test) is used for edk2 unit tests that can be running in OS environment. 
Owner is: Jiewen Yao <jiewen.yao@intel.com>


edk2-host-test are separated from [https://github.com/tianocore/edk2-staging/tree/HBFA](https://github.com/tianocore/edk2-staging/tree/HBFA). It contains a framework for running host based unit test in OS environment and all unit test cases. 

Here are some advantages for using it:

* Lightweight unit testing

  It can run directly in host os instead of running sct in uefi emulator. So it's lightweight and simple to use.

* Easy to debug (for unit tests)

  In past it was hard to do source code debug for UEFI unit testing because in UEFI environment gdb need to be configure appropriately. But now we run unit tests as OS application, so it is very easy to do source code debug same as normal applications.

* Easy for continuous integration.

  Running UEFI unit test as OS application is more easy for integration than UEFI SCT test.

## Suppored Features

* Running test cases in OS environment.
* Supported windows and linux environment.
* Use [cmocka](https://cmocka.org/) as unit testing framework.
* Generate code coverage report

## How to use edk2-host-test

Basic setup: 
0. ref: [https://github.com/tianocore/tianocore.github.io/wiki/Getting-Started-with-EDK-II](https://github.com/tianocore/tianocore.github.io/wiki/Getting-Started-with-EDK-II)
1. ```mkdir your-workspace``` and ```cd your-workspace``` 
1. ```git clone https://github.com/tianocore/edk2.git```
2. ```git clone -b edk2-host-test https://github.com/xiaoyuxlu/edk2-staging.git edk2-host-test```
3. if windows: ```set WORKSAPCE=your-workspace``` ```set PACKAGES_PATH=%WORKSPACE%\edk2-host-test;%WORKSPACE%\edk2``` if linux: ```export WORKSPACE=your-workspace``` ```export PACKAGES_PATH=$WORKSPACE/edk2-host-test:$WORKSPACE/edk2```
4. run edksetup.bat/edksetup.sh ```edk2/edksetup.bat``` ```source edk2/edksetup.sh```
5. ```build -p UefiHostUnitTestCasePkg/UefiHostUnitTestCasePkg.dsc -a X64 -t VS2017 -DUNIT_TEST_FRAMEWORK_MODE=CMOCKA``` or ```build -p UefiHostUnitTestCasePkg/UefiHostUnitTestCasePkg.dsc -a X64 -t GCC5 -DUNIT_TEST_FRAMEWORK_MODE=CMOCKA```

REF: [UefiHostUnitTestPkg/ReadMe-cmocka.txt](UefiHostUnitTestPkg/ReadMe-cmocka.txt)

## Timeline

| Time | Event | Related Module|
|---|---| --- |
| 2019 July | Initial Version | UefiHostTestPkg UefiHostUnitTestCasePkg UefiHostUnitTestPkg CmockaHostUnitTestPkg |

## Related Materials

* [HBFA](https://github.com/tianocore/edk2-staging/tree/HBFA)

