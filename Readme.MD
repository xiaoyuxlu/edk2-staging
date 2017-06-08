This branch is used to optimize BaseTools build performance. It bases on edk2 repo UDK2017 branch.

The branch owner:
Gao, Liming <liming.gao@intel.com>
Zhu, Yonghong <yonghong.zhu@intel.com>

## Feature Introduction
BaseTools supports more and more features. They will take more build time. To reduce build overhead, some points have been 
identified to be optimized. POC code will be added in this branch for evaluation.
1) Enable the multiple thread in GenFds phase.
   POC code has been added to moves GenSec and GenFfs step into Module Makefile. When build module, 
   its ffs will be generated. GenFds phase just GenFv image and combine GenFv into FD image. 
   POC has been verified in OvmfPkgIa32X64 in VS2015 and GCC5 tool chain. 
   In Windows7 VS2015, OvmfPkgIa32X64 GenFds build time can be reduced from 28s to 6s.
   In Ubuntu 14.04 GCC5, OvmfPkgIa32X64 GenFds build time can be reduced from 6s to 4s.
2) Support to merge multiple drivers into one. It should save the link time. But, it doesn't save much in the multiple build. 
   Besides, this feature can save the image size when the image is not compressed, such as PEI images.
   POC code has been added. One example in SamplePkg\SamplePkg.dsc is added to show how to combine more than drivers into single one.
   When more than one drivers are combined, their depex will be AND together. Platform developer can use APRIORI list to 
   describe the combined driver and make it be dispatched correctly.
3) Reduce the extra copy actions in build process.
4) Analyze cProfile data and enhance the parser logic. https://bugzilla.tianocore.org/show_bug.cgi?id=42

## Timeline
Target for 2017 Q3

### NOTES
Most changes in this branch are BaseTools. To apply them, user needs to run BaseTools 
from sources. In Linux, BaseTools run from source. In Windows, BaseTools can run from source. And, Reconfig option is also 
required to apply new config settings in BaseTools\Conf. The step is like below:
1. enter into edk2
2. set PYTHON_HOME=C:\Python27
3. type edksetup.bat --nt32 Reconfig
4. nmake -f BaseTools\Makefile
5. type build command to build platform.
