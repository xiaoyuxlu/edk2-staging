LLVM/CLANG8 formal release http://releases.llvm.org/download.html#8.0.0
It can be downloaded and installed in Windows/Linux/Mac OS.

CLANG8ELF tool chain is added to generate ELF image, and convert to PE/COFF.
On Windows OS, set CLANG_HOST_BIN=n, set CLANG8_BIN=LLVM installed directory
CLANG_HOST_BIN is used CLANG_HOST_PREFIX. Prefix n is for nmake.
For example:
  set CLANG_HOST_BIN=n
  set CLANG8_BIN=C:\Program Files\LLVM\bin\
On Linux/Mac, export CLANG8_BIN=LLVM installed directory, CLANG_HOST_BIN is 
not required, because there is no prefix for make.
For example:
  export CLANG8_BIN=/home/clang8/bin/

This tool chain can be used to compile the firmware code. On windows OS,
Visual Studio is still required to compile BaseTools C tools and 
provide nmake.exe for makefile. On Linux/Mac OS, gcc is used to compile 
BaseTools C tools. make is used for makefile.

This tool chain is verified on OVMF Ia32, X64 and Ia32X64 to boot Shell.
This tool chain is verified in Windows/Linux and Mac OS.

Update the image size table for OvmfIa32x64 (Bytes). 
GCC and CLANG enables LTO, VS2015 enables GL. 

OvmfIa32X64 (Bytes)    GCC5      VS2015x86  CLANG8ELF  CLANG8PE
PEIFV (IA32)           0x2ff28   0x2dfe8    0x2a5a8     0x57028
DXEFV (X64)            0x418528  0x429650   0x3ba6f8    0x502900
FVCOMPACT(Compress)    0x1372e8  0x1204d8   0x1177f0    0x116110
