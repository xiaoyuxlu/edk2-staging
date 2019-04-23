How to run CUnit for UEFI code.
=========================
1) install CUnit-2.1-3
Part A: Install CUnit in Linux
1.1 download CUnit-2.1-3.tar.bz2 (https://sourceforge.net/projects/cunit/) and unzip it ($tar jxvf CUnit-2.1-3.tar.bz2)
1.2 build
  $ cd CUnit-2.1-3
  $ autoscan  
  $ mv configure.in configure.ac  
  $ aclocal  
  $ autoheader  
  $ libtoolize --automake --copy --debug --force
  $ automake --add-missing  
  $ autoconf  
  $ CFLAGS=-m64 ./configure
  $ make
  $ mv CUnit/Sources/.libs CUnit/Sources/.libs64
  $ CFLAGS=-m32 ./configure
  $ make
  $ sudo sh -c "echo '<...>/CUnit/Sources/.libs' >> /etc/ld.so.conf"
  $ sudo sh -c "echo '<...>/CUnit/Sources/.libs64' >> /etc/ld.so.conf"
  $ sudo ldconfig 
  the lib for i386 is at ./CUnit/Sources/.libs/libcunit.a, /CUnit/Sources/.libs/libcunit.so
  the lib for x86_64 is at ./CUnit/Sources/.libs64/libcunit.a, /CUnit/Sources/.libs64/libcunit.so

1.3 export CUNIT_INC_PATH=<...>/CUnit/Headers
    export CUNIT_LIB_PATH=<...>/CUnit/Sources/.libs
    export CUNIT_LIB_PATH_64=<...>/CUnit/Sources/.libs64
    export CUNIT_LIB_NAME=cunit
    export CUNIT_LIB_NAME_64=cunit

Part B: Install CUnit in Windows
1.1 download CUnit-2.1-3 MSVC from https://github.com/lelegard/CUnit-msvc
1.2 cd CUnit-msvc\VC14 and build the solution.
    or open visual studio dev command prompt, type:
    devenv CUnit.sln /Build "[Debug|Release]-[StaticLib|DLL]|[Win32|x64]" /Project CUnit

    for example:
    devenv CUnit.sln /Build "Debug-StaticLib|Win32" /Project CUnit

    static lib can be found at:
    CUnit-msvc\VC14\[Debug|Release]-[StaticLib|DLL]-[Win32|x64]\CUnit.lib

    dynamic lib can be found at:
    CUnit-msvc\VC14\[Debug|Release]-[StaticLib|DLL]-[Win32|x64]\CUnit[.dll|_dll.lib]

1.3 set CUNIT_INC_PATH=<...>\CUnit-msvc\CUnit\Headers
    set CUNIT_LIB_PATH=<...>\CUnit-msvc\VC14\[Debug|Release]-[StaticLib|DLL]-Win32
    set CUNIT_LIB_PATH_64=<...>\CUnit-msvc\VC14\[Debug|Release]-[StaticLib|DLL]-x64
    set CUNIT_LIB_NAME=CUnit[_dll].lib
    set CUNIT_LIB_NAME_64=CUnit[_dll].lib

1.4 add env path, only for dynamic lib build.
    Add %CUNIT_LIB_PATH% to %PATH%, for IA32 run.
    Add %CUNIT_LIB_PATH_64% to %PATH%, for X64 run.

2) run sample code (Basic Mode)
Part A: Build in Linux (can only build the same architecture as host)
    build -p UefiHostUnitTestCasePkg/UefiHostUnitTestCasePkg.dsc -a X64 -t GCC5 -DUNIT_TEST_FRAMEWORK_MODE=CUNIT --disable-include-path-check
    <...>/Build/UefiHostUnitTestCasePkg/DEBUG_GCC5/X64/TestBaseSafeIntLib

Part B: Build in Windows
    build -p UefiHostUnitTestCasePkg\UefiHostUnitTestCasePkg.dsc -a X64 -t VS2015x86 -DUNIT_TEST_FRAMEWORK_MODE=CUNIT --disable-include-path-check
    <...>\Build\UefiHostUnitTestCasePkg\DEBUG_VS2015x86\X64\TestBaseSafeIntLib.exe  

You may see below. Have fun
==============================
     CUnit - A unit testing framework for C - Version 2.1-3 
     http://cunit.sourceforge.net/ 




Suite: Suite_1 
  Test: test of SafeUint64Add() ...passed
  Test: test of SafeUint64Mult() ...passed


Run Summary:    Type  Total    Ran Passed Failed Inactive 
              suites      1      1    n/a      0        0 
               tests      2      2      2      0        0 
             asserts      6      6      6      0      n/a 


Elapsed time =    0.000 seconds 
==============================

3) run sample code (XML report)
Part A: Build in Linux
    build -p UefiHostUnitTestCasePkg/UefiHostUnitTestCasePkg.dsc -a X64 -t GCC5 -DUNIT_TEST_FRAMEWORK_MODE=CUNIT -DUNIT_TEST_XML_MODE --disable-include-path-check
    <...>/Build/UefiHostUnitTestCasePkg/DEBUG_GCC5/X64/TestBaseSafeIntLib

Part B: Build in Windows
    build -p UefiHostUnitTestCasePkg\UefiHostUnitTestCasePkg.dsc -a X64 -t VS2015x86 -DUNIT_TEST_FRAMEWORK_MODE=CUNIT -DUNIT_TEST_XML_MODE --disable-include-path-check
    <...>\Build\UefiHostUnitTestPkg\DEBUG_VS2015x86\X64\TestBaseSafeIntLib.exe

3.1 Open XML report.
The executable will generate "SafeIntTest-Listing.xml" (test case list) and "SafeIntTest-Results.xml" (test result).
The user need copy CUnit-2.1-3/Share/CUnit-List.dtd, CUnit-List.xsl, CUnit-Run.dtd, CUnit-Run.xsl to the same directory.
Then the user may use web brower to open the 2 xml file.

4) get code coverage
See HBFA/UefiHostUnitTestPkg/ReadMe-Coverage.txt


