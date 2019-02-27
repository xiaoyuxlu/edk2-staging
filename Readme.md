# FmpDevicePkg Multiple Controllers

This branch is used to evaluate methods to update the FmpDevicePkg to support
drivers that manage multiple controllers and also provide a firmware update
capability to each managed controller.  The primary use case is the update of
a PCI Option ROM when multiple adapters are present.  This branch addresses
the following TianoCore Bugzilla issues:

https://bugzilla.tianocore.org/show_bug.cgi?id=1525

Branch owner(s):

Michael D Kinney <michael.d.kinney@intel.com>

Multiple Controller Examples:

* [OvmfPkg E1000 Example](OvmfPkg/README_CAPSULES.md)

# EDK II Project

A modern, feature-rich, cross-platform firmware development environment
for the UEFI and PI specifications from www.uefi.org.

Contributions to the EDK II open source project are covered by the
[TianoCore Contribution Agreement 1.1](Contributions.txt)

The majority of the content in the EDK II open source project uses a
[BSD 2-Clause License](License.txt).  The EDK II open source project contains
the following components that are covered by additional licenses:
* [AppPkg/Applications/Python/Python-2.7.2/Tools/pybench](AppPkg/Applications/Python/Python-2.7.2/Tools/pybench/LICENSE)
* [AppPkg/Applications/Python/Python-2.7.2](AppPkg/Applications/Python/Python-2.7.2/LICENSE)
* [AppPkg/Applications/Python/Python-2.7.10](AppPkg/Applications/Python/Python-2.7.10/LICENSE)
* [BaseTools/Source/C/BrotliCompress](BaseTools/Source/C/BrotliCompress/LICENSE)
* [MdeModulePkg/Library/BrotliCustomDecompressLib](MdeModulePkg/Library/BrotliCustomDecompressLib/LICENSE)
* [OvmfPkg](OvmfPkg/License.txt)
* [CryptoPkg/Library/OpensslLib/openssl](CryptoPkg/Library/OpensslLib/openssl/LICENSE)

The EDK II Project is composed of packages.  The maintainers for each package
are listed in [Maintainers.txt](Maintainers.txt).

# Resources
* [TianoCore](http://www.tianocore.org)
* [EDK II](https://github.com/tianocore/tianocore.github.io/wiki/EDK-II)
* [Getting Started with EDK II](https://github.com/tianocore/tianocore.github.io/wiki/Getting-Started-with-EDK-II)
* [Mailing Lists](https://github.com/tianocore/tianocore.github.io/wiki/Mailing-Lists)
* [TianoCore Bugzilla](https://bugzilla.tianocore.org)
* [How To Contribute](https://github.com/tianocore/tianocore.github.io/wiki/How-To-Contribute)
* [Release Planning](https://github.com/tianocore/tianocore.github.io/wiki/EDK-II-Release-Planning)
* [UDK2017](https://github.com/tianocore/edk2/releases/tag/vUDK2017)
* [UDK2018](https://github.com/tianocore/edk2/releases/tag/vUDK2018)
* [edk2-stable201811](https://github.com/tianocore/edk2/releases/tag/edk2-stable201811)
