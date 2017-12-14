This branch is used to integrate open source UEFI UNDI drivers supporting Intel(R) server network adapters to EDK2.

Branch owner:
Pawe³ Or³owski < pawel.orlowski@intel.com >

## Feature Introduction
Drivers being introduced in this branch allow utilizing and controlling Intel(R) server network adapters through UNDI protocol.

#### Related modules
The following module is being introduced in this branch:
```
IntelUndiPkg - new package which will contain sources for all Intel(R) server network adapter drivers.
```

## Timeline
| Time | Event |
|:----:|:-----:|
| WW 36 2017 | Initial commit |
| ... | Cleaning known issues |
|...|...|

## Support scope
UEFI UNDI Drivers being introduced in this branch will support only basic UNDI functionality for Intel(R) server network adapters.
Branch will be updated with support for new devices after those devices are released to public.

Architecture: IA32, IPF, X64

## Known issues
| Issue | Status |
|:-----:|:------:|
| Remove EFI_VLAN_PROTOCOL support | Open |
| Change intrinsic function calls to library function calls within EDK | Open |
| Incorrect exit flow in InitChild process | Open |
| E1000Transmit function flow review for fragmented packets | Open |
| GigUndiPxeStructInit: fix double assignment of PXE_ROMID_MINORVER_31 | Open |
| GigUndiPxeUpdate: mActiveChildren bounds verification/review | Open |
| Add UNLOAD_IMAGE to INF instead of opening EFI_LOADED_IMAGE_PROTOCOL | Open |
| Correct comments within UEFI driver code (at least in GigUndiNotifyExitBs function) | Open |

## Related materials

UEFI Specification (UNDI) - http://www.uefi.org/specifications
