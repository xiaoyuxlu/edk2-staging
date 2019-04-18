
# Introduction

This branch is used to develop the **Capsule-On-Disk** feature.
The branch owner: Chao Zhang < [chao.b.zhang@intel.com](mailto:chao.b.zhang@intel.com) >, Wei Xu < [wei6.xu@intel.com](mailto:wei6.xu@intel.com) >

# Feature Summary

Traditionally capsule image is delivered to BIOS in persistent memory across system reset, but not all platforms support or function well across memory persistent reset. To solve this problem, **Capsule-On-Disk** delivers capsule images through EFI system partition on peripheral storage device. For security reasons, Design is composed of 2 solutions. 
- **Solution A)** - Load the image out of TCB and rely on Capsule-In-RAM to deliver Capsule-On-Disk. 
- **Solution B)** - Relocate capsule image outside TCB. And leverage existing storage stack in PEI to load all capsule on disk images. Solution B) has bigger TCB but can work without Capsule-In-RAM support

>User can test this feature with **CapsuleApp** in **MdeModulePkg**. It has been updated to support Capsule on Disk since **2019 Q1 stable release**.

Brief working flow of  **Capsule-On-Disk**:
```
1. Store capsule images into \EFI\Capsules\ folder on EFI system partition.
2. Set EFI_OS_INDICATIONS_FILE_CAPSULE_DELIVERY_SUPPORTED flag in L"OsIndications".
3. Reboot system.
4. Get all capsule images from \EFI\Capsules\ after TCB, relocated them to root direcotry of a platform-specific NV storage device with BlockIo protocol.
5. Reboot system.
6. Load capsule imaages from the root direcotry in TCB, and build CV hobs
```

## Related Modules

The following modules are related to **Capsule-On-Disk**.
```
MdeModulePkg\Library\DxeCapsuleLibFmp\DxeCapsuleLib.inf
MdeModulePkg\Universal\CapsuleOnDiskLoadPei\CapsuleOnDiskLoadPei.inf
```

## How to enable Capsule-On-Disk

### Add following PCD and Library in DSC:
```
[PcdsFixedAtBuild]
gEfiMdeModulePkgTokenSpaceGuid.PcdCapsuleOnDiskSupport|TRUE

[PcdsDynamicDefault]
gEfiMdeModulePkgTokenSpaceGuid.PcdRecoveryFileName|L"RECOVERY.Cap"
gEfiMdeModulePkgTokenSpaceGuid.PcdCoDRelocationFileName|L"Test123.tmp"

[LibraryClasses]
CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibFmp/DxeCapsuleLib.inf

[Components]
MdeModulePkg/Universal/CapsuleOnDiskLoadPei/CapsuleOnDiskLoadPei.inf
```

### Add CapsuleOnDiskLoadPei.inf in FDF:
```
INF MdeModulePkg/Universal/CapsuleOnDiskLoadPei/CapsuleOnDiskLoadPei.inf
```

### Add platform code to enable Capsule-On-Disk:

Following code snippet is for reference.
- If normal boot path, check CapsuleOnDisk flag and relocate capsules in platform BDS.
```
  switch (BootMode) {
  case BOOT_ASSUMING_NO_CONFIGURATION_CHANGES:
  case BOOT_WITH_MINIMAL_CONFIGURATION:
    if (CoDCheckCapsuleOnDiskFlag()) {
        CoDClearCapsuleOnDiskFlag();
        CoDRelocateCapsule(20);
        gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
    }
  }
```

- Base on COD_RELOCATION_INFO_VAR_NAME variable, determine BootMode and install gEfiPeiBootInCapsuleOnDiskModePpiGuid PPI.
```
BOOLEAN
IsCapsuleOnDisk (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINTN                           Size;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  BOOLEAN                         CodRelocInfo;

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariableServices
             );
  if (EFI_ERROR(Status)) {
    return FALSE;
  }

  DataSize = sizeof (BOOLEAN);
  Status = VariableServices->GetVariable (
                               PPIVariableServices,
                               COD_RELOCATION_INFO_VAR_NAME,
                               &gEfiCapsuleVendorGuid,
                               NULL,
                               &DataSize,
                               (VOID *) &CapsuleRelocInfo
                               );
  if (EFI_ERROR(Status) || DataSize != sizeof(BOOLEAN) || CapsuleRelocInfo != TRUE) {
    return FALSE;
  }

  return TRUE;
}

EFI_PEI_PPI_DESCRIPTOR  mPpiListCapsuleOnDiskMode = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiBootInCapsuleOnDiskModePpiGuid,
  NULL
};

UpdateBootMode (
  IN EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_BOOT_MODE        BootMode;

  PeiServicesGetBootMode (&BootMode);
  if (IsCapsuleOnDisk()) {
    BootMode = BOOT_ON_FLASH_UPDATE;
    Status = (**PeiServices).InstallPpi ((CONST EFI_PEI_SERVICES **)PeiServices, &mPpiListCapsuleOnDiskMode);
  }
  PeiServicesSetBootMode (BootMode);
}
```

### Test Capsule-On-Disk with CapsuleApp:

Boot UEFI shell, run command: ```CapsuleApp.efi test.cap -OD```

# Promote to edk2 Trunk
 
If a subset feature or a bug fix in this staging branch could meet below requirement, it could be promoted to edk2 trunk and removed from this staging branch:

- Meet all edk2 required quality criteria.
- Ready for product integration.

# Time Line

|Time| Event |
|---|---|
|2019 Q2| Exit Staging|


# Related Materials

UEFI Specification - http://uefi.org/specifications