# Overview

The sections below show the build commands for QEMU for IA32, X64, and IA32/X64.
Both `DEBUG` and `RELEASE` profiles are supported, and there are options to
provide capsule update progress indicators on a Graphics Output console
(`GRAPHICS`), a Simple Text Output console (`TEXT`), or using beep codes on an
AdLib/Sound Blaster controller (`BEEP`) that is emulated by QEMU.

After a build is completed, post build scripts copy `CapsuleApp.efi` and the
`*.cap` files are in the OVMF build output directory in a `Capsules/TestCert`
 directory.

## Generated Capsules

Each build generates multiple capsules for the Red, Green, Blue, and E1000
devices with different Version values and one update to the LowestSupportedVersion
value to support testing of anti-rollback below LSV.

| Device       | GUID                                 | Version    | LSV        | Version String |
| ------------ | ------------------------------------ | ---------- | ---------- | -------------- |
| Red          | 72E2945A-00DA-448E-9AA7-075AD840F9D4 | 0x00000010 | 0x00000000 | 0.0.0.16       |
| Red          | 72E2945A-00DA-448E-9AA7-075AD840F9D4 | 0x00000011 | 0x00000000 | 0.0.0.17       |
| Red          | 72E2945A-00DA-448E-9AA7-075AD840F9D4 | 0x00000012 | 0x00000000 | 0.0.0.18       |
| Green        | 79179BFD-704D-4C90-9E02-0AB8D968C18A | 0x00000020 | 0x00000020 | 0.0.0.32       |
| Green        | 79179BFD-704D-4C90-9E02-0AB8D968C18A | 0x00000021 | 0x00000020 | 0.0.0.33       |
| Green        | 79179BFD-704D-4C90-9E02-0AB8D968C18A | 0x00000022 | 0x00000020 | 0.0.0.34       |
| Blue         | 149DA854-7D19-4FAA-A91E-862EA1324BE6 | 0x00000010 | 0x00000000 | 0.0.0.16       |
| Blue         | 149DA854-7D19-4FAA-A91E-862EA1324BE6 | 0x00000011 | 0x00000000 | 0.0.0.17       |
| Blue         | 149DA854-7D19-4FAA-A91E-862EA1324BE6 | 0x00000012 | 0x00000012 | 0.0.0.18       |
| Blue         | 149DA854-7D19-4FAA-A91E-862EA1324BE6 | 0x00000013 | 0x00000012 | 0.0.0.19       |
| Blue         | 149DA854-7D19-4FAA-A91E-862EA1324BE6 | 0x00000014 | 0x00000012 | 0.0.0.20       |
| E1000Payload | AF2FDE1E-234B-11E9-9356-54E1AD3BF134 | 0x00000010 | 0x00000000 | 0.0.0.16       |
| E1000Payload | AF2FDE1E-234B-11E9-9356-54E1AD3BF134 | 0x00000011 | 0x00000000 | 0.0.0.17       |
| E1000Payload | AF2FDE1E-234B-11E9-9356-54E1AD3BF134 | 0x00000012 | 0x00000012 | 0.0.0.18       |
| E1000Payload | AF2FDE1E-234B-11E9-9356-54E1AD3BF134 | 0x00000013 | 0x00000012 | 0.0.0.19       |
| E1000Payload | AF2FDE1E-234B-11E9-9356-54E1AD3BF134 | 0x00000014 | 0x00000012 | 0.0.0.20       |

## Testing using QEMU

Use the Build and Run Commands provided below to build a specific OVMF target
and run the OVMF firmware image on QEMU.  Boot to the UEFI Shell.  The
`Capsules` directory in the OVMF build output directory is mounted as `FS0:`.
`CapsuleApp.efi` and the test capsules are in the `TestCert` directory.  The
example below submits multiple test capsules.

```
Shell> fs0:
FS0:> cd TestCert
FS0:\TestCert\> CapsuleApp.efi Red.0.0.0.16.cap Green.0.0.0.32.cap Blue.0.0.0.16.cap E1000Payload.0.0.0.16.cap
```

The system will reboot and display the progress bar for the Red, Green, and Blue
sample device firmware update.  It will then display a yellow progress bar four
times for the four E1000 PCI devices that are present.

# Prerequisites

The following are the prerequisite for Windows and Linux to support the Build
and Run Commands.

* OpenSSL command line utilities installed and in PATH
* QEMU installed and in PATH

## Windows Specific Prerequisites

* TeraTerm terminal emulator installed and in PATH

## Linux Specific Prerequisites

* putty terminal emulator installed and in PATH

# OVMF IA32 DEBUG Build and Run Commands
```
build -a IA32 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32.dsc -b DEBUG -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=GRAPHICS
build -a IA32 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32.dsc -b DEBUG -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=TEXT
build -a IA32 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32.dsc -b DEBUG -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=BEEP
```

```
build -a IA32 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32.dsc -b DEBUG -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=GRAPHICS run
build -a IA32 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32.dsc -b DEBUG -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=TEXT run
build -a IA32 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32.dsc -b DEBUG -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=BEEP run
```

# OVMF IA32 RELEASE Build and Run Commands
```
build -a IA32 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32.dsc -b RELEASE -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=GRAPHICS
build -a IA32 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32.dsc -b RELEASE -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=TEXT
build -a IA32 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32.dsc -b RELEASE -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=BEEP
```

```
build -a IA32 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32.dsc -b RELEASE -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=GRAPHICS run
build -a IA32 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32.dsc -b RELEASE -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=TEXT run
build -a IA32 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32.dsc -b RELEASE -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=BEEP run
```

# OVMF X64 DEBUG Build and Run Commands
```
build -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgX64.dsc -b DEBUG -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=GRAPHICS
build -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgX64.dsc -b DEBUG -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=TEXT
build -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgX64.dsc -b DEBUG -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=BEEP
```

```
build -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgX64.dsc -b DEBUG -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=GRAPHICS run
build -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgX64.dsc -b DEBUG -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=TEXT run
build -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgX64.dsc -b DEBUG -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=BEEP run
```

# OVMF X64 RELEASE Build and Run Commands
```
build -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgX64.dsc -b RELEASE -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=GRAPHICS
build -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgX64.dsc -b RELEASE -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=TEXT
build -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgX64.dsc -b RELEASE -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=BEEP
```

```
build -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgX64.dsc -b RELEASE -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=GRAPHICS run
build -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgX64.dsc -b RELEASE -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=TEXT run
build -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgX64.dsc -b RELEASE -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=BEEP run
```

# OVMF IA32/X64 DEBUG Build and Run Commands
```
build -a IA32 -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32X64.dsc -b DEBUG -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=GRAPHICS
build -a IA32 -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32X64.dsc -b DEBUG -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=TEXT
build -a IA32 -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32X64.dsc -b DEBUG -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=BEEP
```

```
build -a IA32 -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32X64.dsc -b DEBUG -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=GRAPHICS run
build -a IA32 -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32X64.dsc -b DEBUG -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=TEXT run
build -a IA32 -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32X64.dsc -b DEBUG -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=BEEP run
```

# OVMF IA32/X64 RELEASE Build and Run Commands
```
build -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32X64.dsc -b RELEASE -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=GRAPHICS
build -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32X64.dsc -b RELEASE -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=TEXT
build -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32X64.dsc -b RELEASE -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=BEEP
```

```
build -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32X64.dsc -b RELEASE -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=GRAPHICS run
build -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32X64.dsc -b RELEASE -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=TEXT run
build -a X64 -n 5 -t VS2015x86 -p OvmfPkg\OvmfPkgIa32X64.dsc -b RELEASE -D DEBUG_ON_SERIAL_PORT -D CAPSULE_PROGRESS=BEEP run
```
