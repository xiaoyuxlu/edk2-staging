# **EDK II Tests**

edk2-staging branch for a test harness, test case SDK, and test cases for the
edk2 repository and platform firmware that is based on the edk2 repository.

## **edk2-stagaing branch owners**
* Michael Kinney <michael.d.kinney@intel.com>

## **Features**
* Test harness that runs from the UEFI Shell
* Implemented as EDK II packages
* Supports black box tests, white box tests, and functional tests
* Test case categories and test cases can be extended
* Logs all results to files
* Provides report generation
* Compatible with binary releases of the PI SCTs
* Compatible with binary releases of the UEFI SCTs
* Supports a user interface (run ```Sct.efi -u```)

## **Known Limitations**
* Requires latest version of UEFI Shell that must be built from sources
  + Not compatible with pre-built binaries of UEFI Shell
* For UEFI SCTs, the EbcBBTest.efi and PxeBaseCodeBBTest.efi files must be removed

## **Quality Criteria for edk2 trunk**
* Support all CPU architectures
* Support all compilers
* Follow EDK II Coding Style
* Compatible with all platforms in edk2/master

## **Additional work items**
* Update to take advantage of latest edk2 features/libraries.
* Update for all supported CPU types
* Update for all supported compilers
* Review initial test harness features and determine
  what features should be dropped and what new features
  should be added.
* Determine where the test harness, test case SDK, and
  test cases should live once the initial functional and
  quality criteria are met.  Could be packages in the
  edk2 repo or packages in a new edk2-test repo.  Other
  options???
* Resolve compatibility issues with binary releases of the
  PI SCTs and UEFI SCTs.
* Update test harness to support PEI tests
* Update test harness to support Runtime tests
* Update test harness to support SMM tests
* Optimize performance of the test harness and tests.

## **Windows Build Instructions**

### Pre-requisites

* GIT client: Available from https://git-scm.com/downloads
* Microsoft Visual Studio.
  - Visual Studio 2015 recommended and is used in the examples below.

Create a new directory for an EDK II WORKSPACE.

The code block below shows the GIT clone operations required to pull the edk2
repository, and the edk2-test branch from the edk2-staging repository.

Next it sets environment variables that must be set before running
```edksetup.bat```. Since content is being pulled from multiple repositories,
the EDK II [Multiple Workspace](
https://github.com/tianocore/tianocore.github.io/wiki/Multiple_Workspace)
feature is used.

Next, the ```edksetup.bat``` file is run to complete the initialization of an
EDK II build environment.  Two example build commands are shown.  The first one
in ```TestFrameworkPkg/TestFrameworkPkg.dsc``` builds a test harness and creates
an installer in the build output directory.  The second one in
```TestCasePkg/TestCasePkg.dsc``` builds a sample test case using a test library
from the ```TestFrameworkPkg``` and adds the test case to the installer in the
build output directory.

```cmd
git clone https://github.com/tianocore/edk2.git
git clone https://github.com/tianocore/edk2-staging.git --brach edk2-test

set WORKSPACE=%CD%
set EDK_TOOLS_PATH=%WORKSPACE%\edk2\BaseTools
set PACKAGES_PATH=%WORKSPACE%\edk2;%WORKSPACE%\edk2-staging
set EDK_TOOLS_BIN=%WORKSPACE%\BaseTools\BinWrappers\WindowsLike
path=%path%;%WORKSPACE%\edk2\BaseTools\Bin\Win32

cd edk2
edkSetup.bat

build -a IA32 -a X64 -t VS2015x86 -p TestFrameworkPkg/TestFrameworkPkg.dsc
build -a IA32 -a X64 -t VS2015x86 -p TestCasePkg/TestCasePkg.dsc
```

## **Linux Build Instructions**

## **Installation Instructions**

* Copy the Build/SctPackage directory to media for the target platform
* Boot target platform to UEFI Shell built from latest sources
* CD to the SctPackage directory on media
* CD to IA32 or X64 depending on current CPU mode
* Run InstallSct.efi and follow instructions

## **Executing Test Harness**

* CD to SCT directrory where test harness was installed
* CD to IA32 or X64 depending on current CPU mode
* Run ```Sct.efi -u``` for user interface
* Use menus to select test cases to run
* Some test cases reboot the platform.  These should continue automatically
  using the Startup.nsh file installed by the installer.
