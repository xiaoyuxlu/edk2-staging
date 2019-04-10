# Platform Runtime Mechanism Case Study

**Platform Runtime Mechanism (PRM)** is an architecture for ACPI codes to call into UEFI BIOS’s Runtime Services at OS runtime. Traditionally, ACPI codes call into BIOS through the invocation of Software SMIs. PRM provides an alternative for ACPI codes to invoke UEFI Runtime Services, which runs in OS kernel space, without invoking SMM.

This package contains proof-of-concept codes that implement the ideas of PRM.

Detailed description of the package can be found in PRM_Proof_Of_Concept.pdf

## ** IMPORTANT NOTE **
The codes in this package are for proof-of-concept only. The codes do not represent a formal design, nor do the codes aim at product quality, or being used in products. For example, the codes interpret a parameter of the GetVariable() service as a function pointer, and do not consider the case that the GetVariable() service may be called from user mode with invalid or even malicious parameters. This is a security issue that this proof-of-concept implementation does not address.

## Resources

* [PRM introduction](https://www.opencompute.org/events/past-summits) (Please search for “UEFI Implementation Intel® Xeon Based OCP Platform” in the webpage, which lists pointers to video and presentation that introduce the concepts of PRM and SMM reduction.)
* [PRM Case Study](https://www.opencompute.org/events/past-summits) (Please search for “Case Study: Alternatives for SMM usage in Intel Platforms” in the webpage, which lists pointers to video and presentation that introduce a case study for SMM reduction.)
* [TianoCore](http://www.tianocore.org)
* [EDK II](https://github.com/tianocore/tianocore.github.io/wiki/EDK-II)
* [UEFI Forum](https://uefi.org/)