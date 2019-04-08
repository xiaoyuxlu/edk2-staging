# TCP/IP network stack with multiprocessing

This branch is used to contribute a faster, multi-core alternative to an existing TCP/IP network stack.

Branch owner:
Maciej Rabeda < maciej.rabeda@intel.com >

## Feature introduction

Proposed network stack utilizes non-boostrap processors (non-BSP) in order to speed up overall network processing and offload the core already busy with UEFI. In order to address lack of thread safety in current network stack design, lwIP open-source TCP/IP implementation was used instead.

Changes made to UDK2018 stable implementation allowed to execute PXE boot to WDS server in UEFI environment at average boot image download rate of ~1.1 Gb/s.

```
Advisory: this implementation is not fully compliant with UEFI specification. This project is currently treated as a proof-of-concept and will be subject to change. Code is provided as-is with no quality assurance. Any and all suggestions or/and constructive criticism are most welcome.
```
