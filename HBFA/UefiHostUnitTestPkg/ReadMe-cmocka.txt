How to run cmocka for UEFI code.
=========================
0) download CMake (http://www.cmake.org/)

1) install cmocka-1.1.5
Part A: Install cmocka in linux
1.1 download cmocka-1.1.5.tar.xz (https://cmocka.org/) and unzip it ($tar xJvf cmocka-1.1.5.tar.xz)
1.2 build
  $ cd cmocka-1.1.5
  $ mkdir build
  $ cd build
  $ export CFLAGS=-m32
  $ export CXXFLAGS=-m32
  $ cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Debug -D WITH_STATIC_LIB=ON ..
  $ make
  $ sudo sh -c "echo '<...>/cmocka-1.1.5/build/src' >> /etc/ld.so.conf"
  $ sudo ldconfig 
  $ cd ..
  $ mkdir build64
  $ cd build64
  $ export CFLAGS=-m64
  $ export CXXFLAGS=-m64
  $ cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Debug -D WITH_STATIC_LIB=ON ..
  $ make
  $ sudo sh -c "echo '<...>/cmocka-1.1.5/build64/src' >> /etc/ld.so.conf"
  $ sudo ldconfig 

  the lib is at ./src/libcmocka-static.a, ./src/libcmocka.so

1.3 export CMOCKA_INC_PATH=<...>/cmocka-1.1.5/include
    export CMOCKA_LIB_PATH=<...>/cmocka-1.1.5/build/src
    export CMOCKA_LIB_PATH_64=<...>/cmocka-1.1.5/build64/src
    export CMOCKA_LIB_NAME=cmocka[-static]
    export CMOCKA_LIB_NAME_64=cmocka[-static]

Part B: Install cmocka in windows
1.1 download cmocka-1.1.5 from https://cmocka.org/, and unzip cmocka-1.1.5.tar.xz
1.2 open visual studio dev command prompt, to go 
    cd <...>\cmocka-1.1.5\
    mkdir build
    cd build
    cmake -G "Visual Studio 14 2015" -D WITH_STATIC_LIB=ON ..

    mkdir build64
    cd build64
    cmake -G "Visual Studio 14 2015 Win64" -D WITH_STATIC_LIB=ON ..

1.3 load solution at cmocka.sln, and build the solution.
    or type:
    devenv cmocka.sln /Build [Debug|Release] /Project cmocka[-static]

    static lib can be found at:
    cmocka-1.1.5\[build|build64]\src\[Debug|Release]\cmocka-static.lib

    dynamic lib can be found at: 
    cmocka-1.1.5\[build|build64]\src\[Debug|Release]\cmocka.[lib|dll]

1.4 set CMOCKA_INC_PATH=<...>\cmocka-1.1.5\include
    set CMOCKA_LIB_PATH=<...>\cmocka-1.1.5\build\src\[Debug|Release]
    set CMOCKA_LIB_PATH_64=<...>\cmocka-1.1.5\build64\src\[Debug|Release]
    set CMOCKA_LIB_NAME=cmocka[-static].lib
    set CMOCKA_LIB_NAME_64=cmocka[-static].lib

1.5 add env path, only for dynamic lib build
    Add %CMOCKA_LIB_PATH% to %PATH%, for IA32 run.
    Add %CMOCKA_LIB_PATH_64% to %PATH%, for X64 run.

2) run sample code (Basic Mode)
Part A: Build in Linux
    build -p UefiHostUnitTestCasePkg/UefiHostUnitTestCasePkg.dsc -a X64 -t GCC5 -DUNIT_TEST_FRAMEWORK_MODE=CMOCKA --disable-include-path-check
    <...>/Build/UefiHostUnitTestCasePkg/DEBUG_GCC5/X64/TestBaseSafeIntLib

Part B: Build in Windows
    build -p UefiHostUnitTestCasePkg\UefiHostUnitTestCasePkg.dsc -a X64 -t VS2015x86 -DUNIT_TEST_FRAMEWORK_MODE=CMOCKA --disable-include-path-check
    <...>\Build\UefiHostUnitTestCasePkg\DEBUG_VS2015x86\X64\TestBaseSafeIntLib.exe  

You may see below. Have fun
==============================
Int Safe Lib Unit Test Application v0.1
[==========] Running 71 test(s).
[ RUN      ] Test SafeInt8ToUint8
[       OK ] Test SafeInt8ToUint8
[ RUN      ] Test SafeInt8ToUint16
[       OK ] Test SafeInt8ToUint16
[ RUN      ] Test SafeInt8ToUint32
[       OK ] Test SafeInt8ToUint32
[ RUN      ] Test SafeInt8ToUintn
[       OK ] Test SafeInt8ToUintn
[ RUN      ] Test SafeInt8ToUint64
[       OK ] Test SafeInt8ToUint64
...
[==========] 71 test(s) run.
[  PASSED  ] 67 test(s).
[  FAILED  ] 4 test(s), listed below:
[  FAILED  ] Test SafeInt16ToChar8
[  FAILED  ] Test SafeInt32ToChar8
[  FAILED  ] Test SafeIntnToChar8
[  FAILED  ] Test SafeInt64ToChar8
[==========] Running 10 test(s).
...
[ RUN      ] Test SafeInt64Mult
[       OK ] Test SafeInt64Mult
[==========] 10 test(s) run.
[  PASSED  ] 10 test(s).
==============================

3) run sample code (XML report)
  set environment variable: CMOCKA_XML_FILE=<test.xml>

  Build in Linux
    build -p UefiHostUnitTestCasePkg/UefiHostUnitTestCasePkg.dsc -a X64 -t GCC5 -DUNIT_TEST_FRAMEWORK_MODE=CMOCKA -DUNIT_TEST_XML_MODE --disable-include-path-check
    export CMOCKA_XML_FILE=TestBaseSafeIntLib.X64.xml
    ./Build/UefiHostUnitTestCasePkg/DEBUG_GCC5/X64/TestBaseSafeIntLib

  Build in Windows
    build -p UefiHostUnitTestCasePkg/UefiHostUnitTestCasePkg.dsc -a X64 -t VS2015x86 -DUNIT_TEST_FRAMEWORK_MODE=CMOCKA -DUNIT_TEST_XML_MODE --disable-include-path-check
    set CMOCKA_XML_FILE=TestBaseSafeIntLib.X64.xml
    <...>\Build\UefiHostUnitTestCasePkg\DEBUG_VS2015x86\X64\TestBaseSafeIntLib.exe

  report is <test.xml>

4) get code coverage
See HBFA/UefiHostUnitTestPkg/ReadMe-Coverage.txt


