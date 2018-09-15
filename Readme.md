# UEFI Payload Project

**UEFI Payload** (**UefiPayloadPkg**) aims to be an upgrade to [CorebootModulePkg](https://github.com/tianocore/edk2/tree/master/CorebootModulePkg) and [CorebootPayloadPkg](https://github.com/tianocore/edk2/tree/master/CorebootPayloadPkg)

UEFI Payload has some new features:
* Supporting [Slim Bootloader](https://github.com/slimbootloader/slimbootloader) in addition to [Coreboot](https://www.coreboot.org/)
* Source level configuration using .ini format
* User Extension using simple "C" codes
* Platform support library for adding platform specific codes

Features are to be further developed / enriched.

## ** NOTE **
The platform supporting codes in `CustomizationSample/Boards` are for illustration purpose only. These codes do **NOT** aim at being complete or being of product quality.

## How-Tos

### To get UEFI Payload and construct a work tree:

1. `git clone https://github.com/tianocore/edk2-staging.git -b UefiPayload`
2. `git clone --recurse-submodules https://github.com/tianocore/edk2.git`
3. Rename `edk2-staging` to `UEFIPayload` (otherwise there is some path handling error during build which is yet to be root-caused)

### To build UEFI Payload:

1. Setup a build host 
  - In addition to typical EDKII build tools, please install [PyYAML](https://pyyaml.org/)
  - If you want to integrate with [Slim Bootloader](https://github.com/slimbootloader/slimbootloader), refer to [Slim Bootloader Host Setup](https://slimbootloader.github.io/getting-started/build-host-setup.html)
2. In folder `UEFIPayload/UefiPayloadPkg`, run `BuildPayload.sh` (Linux) or `BuildPayload.bat` (Windows)
3. Refer to `CorebootIntergration.txt` or `SlimBootloaderIntegration.txt` for instructions on integration with [Coreboot](https://www.coreboot.org/) or [Slim Bootloader](https://github.com/slimbootloader/slimbootloader)

### To run UEFI Payload on [Qemu](https://www.qemu.org/):
1. Copy the resulting firmware after integration with [Coreboot](https://www.coreboot.org/) or [Slim Bootloader](https://github.com/slimbootloader/slimbootloader) (e.g., `firmware.bin`) to Qemu folder
2. Run qemu optionally with an OS image. 
   For example, 
  `"c:\Program Files\qemu\qemu-system-x86_64.exe" -machine q35 -pflash firmware.bin  -hda ubuntu-17.10.1-desktop-amd64.iso -m size=4096`
  - Running Ubuntu on Qemu may be very slow. In order to see more Linux boot logs, you may change the kernel boot parameters, e.g. replace `quiet` with `loglevel=7 console=ttyS0,115200n81`)

## Description of Features

### Slim Bootloader support
* Added parsing logic for Slim Bootloader parameters which are in HOB format

### .ini format configuration
* Board specific configuration samples are specified in `CustomizationSamples/Boards/*/Setup/*/*.ini`
* These settings are translated into `UefiPayloadPkgIa32X64.dsc` file (and Slim Bootloader's `*.dlt` file)
  * Translation rules are specified in `CustomizationSamples/Boards/*/Setup/Rules`
  * Translation is done by `TranslateConfig.py`

### User Extension using simple "C" codes
* User Extension code samples are in `CustomizationSample/Boards/*/SourceCodes/UserExtension/Custom.c`
* User Extension run as the first boot option
* User Extension supporting library API is defined at `Include/Library/UserExtensionLib.h and Include/Library/UserExtensionApi`

### Platform support library
* Platform support library samples are in `CustomizationSamples/Boards/*/SourceCodes/PlatformLib`
* Platform support library API is defined at `Include/Library/PlatformLib.h`

## Resources

* [TianoCore](http://www.tianocore.org)
* [EDK II](https://github.com/tianocore/tianocore.github.io/wiki/EDK-II)
* [Getting Started with EDK II](https://github.com/tianocore/tianocore.github.io/wiki/Getting-Started-with-EDK-II)
* [Mailing Lists](https://github.com/tianocore/tianocore.github.io/wiki/Mailing-Lists)
* [TianoCore Bugzilla](https://bugzilla.tianocore.org)
* [How To Contribute](https://github.com/tianocore/tianocore.github.io/wiki/How-To-Contribute)
