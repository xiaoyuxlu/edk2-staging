/** @file
  This PEIM processes platform info handed from previous stage firmware.

  Copyright (c) 2014 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include "UefiPayloadPei.h"

#define LEGACY_8259_MASK_REGISTER_MASTER  0x21
#define LEGACY_8259_MASK_REGISTER_SLAVE   0xA1

extern VOID *mPayLoadHOBBase;

EFI_MEMORY_TYPE_INFORMATION mDefaultMemoryTypeInformation[] = {
  { EfiACPIReclaimMemory,   FixedPcdGet32 (PcdMemoryTypeEfiACPIReclaimMemory) },
  { EfiACPIMemoryNVS,       FixedPcdGet32 (PcdMemoryTypeEfiACPIMemoryNVS) },
  { EfiReservedMemoryType,  FixedPcdGet32 (PcdMemoryTypeEfiReservedMemoryType) },
  { EfiRuntimeServicesData, FixedPcdGet32 (PcdMemoryTypeEfiRuntimeServicesData) },
  { EfiRuntimeServicesCode, FixedPcdGet32 (PcdMemoryTypeEfiRuntimeServicesCode) },
  { EfiMaxMemoryType,       0     }
};

EFI_PEI_PPI_DESCRIPTOR   mPpiBootMode[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiMasterBootModePpiGuid,
    NULL
  }
};

/**
  Create memory mapped IO resource hob.

  @param  MmioBase    Base address of the memory mapped io range
  @param  MmioSize    Length of the memory mapped io range

**/
VOID
BuildMemoryMappedIoRangeHob (
  EFI_PHYSICAL_ADDRESS        MmioBase,
  UINT64                      MmioSize
  )
{
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT    |
    EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
    EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_TESTED),
    MmioBase,
    MmioSize
    );

  BuildMemoryAllocationHob (
    MmioBase,
    MmioSize,
    EfiMemoryMappedIO
    );
}

/**
  Check the integrity of firmware volume header

  @param[in]  FwVolHeader   A pointer to a firmware volume header

  @retval     TRUE          The firmware volume is consistent
  @retval     FALSE         The firmware volume has corrupted.

**/
STATIC
BOOLEAN
IsFvHeaderValid (
  IN EFI_FIRMWARE_VOLUME_HEADER    *FwVolHeader
  )
{
  UINT16 Checksum;

  // Skip nv storage fv
  if (CompareMem (&FwVolHeader->FileSystemGuid, &gEfiFirmwareFileSystem2Guid, sizeof(EFI_GUID)) != 0 ) {
    return FALSE;
  }

  if ( (FwVolHeader->Revision != EFI_FVH_REVISION)   ||
     (FwVolHeader->Signature != EFI_FVH_SIGNATURE) ||
     (FwVolHeader->FvLength == ((UINTN) -1))       ||
     ((FwVolHeader->HeaderLength & 0x01 ) !=0) )  {
    return FALSE;
  }

  Checksum = CalculateCheckSum16 ((UINT16 *) FwVolHeader, FwVolHeader->HeaderLength);
  if (Checksum != 0) {
    DEBUG (( DEBUG_ERROR,
              "ERROR - Invalid Firmware Volume Header Checksum, change 0x%04x to 0x%04x\r\n",
              FwVolHeader->Checksum,
              (UINT16)( Checksum + FwVolHeader->Checksum )));
    return FALSE;
  }

  return TRUE;
}

/**
  Install FvInfo PPI and create fv hobs for remaining fvs

**/
VOID
PayloadPeiReportRemainingFvs (
  VOID
  )
{
  UINT8*  TempPtr;
  UINT8*  EndPtr;

  TempPtr = (UINT8* )(UINTN) PcdGet32 (PcdPayloadFdMemBase);
  EndPtr = (UINT8* )(UINTN) (PcdGet32 (PcdPayloadFdMemBase) + PcdGet32 (PcdPayloadFdMemSize));

  for (;TempPtr < EndPtr;) {
    if (IsFvHeaderValid ((EFI_FIRMWARE_VOLUME_HEADER* )TempPtr)) {
      if (TempPtr != (UINT8* )(UINTN) PcdGet32 (PcdPayloadFdMemBase))  {
        // Skip the PEI FV
        DEBUG((EFI_D_ERROR, "Found one valid fv : 0x%lx.\n", TempPtr, ((EFI_FIRMWARE_VOLUME_HEADER* )TempPtr)->FvLength));

        PeiServicesInstallFvInfoPpi (
          NULL,
          (VOID *) (UINTN) TempPtr,
          (UINT32) (UINTN) ((EFI_FIRMWARE_VOLUME_HEADER* )TempPtr)->FvLength,
          NULL,
          NULL
          );
        BuildFvHob ((EFI_PHYSICAL_ADDRESS)(UINTN) TempPtr, ((EFI_FIRMWARE_VOLUME_HEADER* )TempPtr)->FvLength);
      }
    }
    TempPtr += ((EFI_FIRMWARE_VOLUME_HEADER* )TempPtr)->FvLength;
  }
}

/**
  Based on memory base, size and type, build resource descriptor HOB.

  @param  Base    Memory base address.
  @param  Size    Memory size.
  @param  Type    Memory type.
  @param  Param   A pointer to PAYLOAD_MEM_INFO.

  @retval EFI_SUCCESS if it completed successfully.
**/
EFI_STATUS
CorebootMemInfoCallback (
  UINT64                  Base,
  UINT64                  Size,
  UINT32                  Type,
  VOID                    *Param
  )
{
  PAYLOAD_MEM_INFO        *MemInfo;
  UINTN                   Attribue;

  Attribue = EFI_RESOURCE_ATTRIBUTE_PRESENT |
             EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
             EFI_RESOURCE_ATTRIBUTE_TESTED |
             EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
             EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
             EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
             EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE;

  if ((Base  < 0x100000) && ((Base + Size) > 0x100000)) {
         Size -= (0x100000 - Base);
         Base  = 0x100000;
  }

  MemInfo = (PAYLOAD_MEM_INFO *)Param;
  if (Base >= 0x100000) {
    if (Type == CB_MEM_RAM) {
      if (Base < 0x100000000ULL) {
        if (Size >= (UINT64)MemInfo->UsableLowMemMinSize) {
          if (MemInfo->UsableLowMemTop < (UINT32)(Base + Size)) {
            MemInfo->UsableLowMemTop = (UINT32)(Base + Size);
          }
        }
      } else {
        Attribue &= ~EFI_RESOURCE_ATTRIBUTE_TESTED;
      }
      BuildResourceDescriptorHob (
        EFI_RESOURCE_SYSTEM_MEMORY,
        Attribue,
        (EFI_PHYSICAL_ADDRESS)Base,
        Size
        );
      DEBUG ((EFI_D_INFO, "System Memory Base = 0x%lX, Size = 0x%lX\n",
      (UINT64)Base, (UINT64)Size));  

    } else if (Type == CB_MEM_TABLE) {
      BuildResourceDescriptorHob (
        EFI_RESOURCE_MEMORY_RESERVED,
        Attribue,
        (EFI_PHYSICAL_ADDRESS)Base,
        Size
        );
      DEBUG ((EFI_D_INFO, "Reserved (from CB_MEM_TABLE) Memory Base  = 0x%lX, Size = 0x%lX\n",
      (UINT64)Base, (UINT64)Size));  

      MemInfo->SystemLowMemTop = ((UINT32)(Base + Size) + 0x0FFFFFFF) & 0xF0000000;
    } else if (Type == CB_MEM_RESERVED) {
      if ((MemInfo->SystemLowMemTop == 0) || (Base < MemInfo->SystemLowMemTop)) {
        BuildResourceDescriptorHob (
          EFI_RESOURCE_MEMORY_RESERVED,
          Attribue,
          (EFI_PHYSICAL_ADDRESS)Base,
          Size
          ); 
        DEBUG ((EFI_D_INFO, "Reserved Memory Base  = 0x%lX, Size = 0x%lX\n",
        (UINT64)Base, (UINT64)Size));  
      }
    }
  }
  
  return EFI_SUCCESS;
}

/**
  Based on memory base, size and type, build resource descripter HOB.

  @param  Base    Memory base address.
  @param  Size    Memory size.
  @param  Type    Memory type.
  @param  Param   A pointer to PAYLOAD_MEM_INFO.

  @retval EFI_SUCCESS if it completed successfully.
**/
EFI_STATUS
SblMemInfoCallback (
  IN MEMROY_MAP_ENTRY             *MemoryMapEntry,
  IN VOID                         *Param
  )
{
  PAYLOAD_MEM_INFO        *MemInfo;
  UINTN                   Attribue;
  EFI_PHYSICAL_ADDRESS    Base;
  UINT64                  Size;
  UINT32                  SystemLowMemTop;
  
  Attribue = EFI_RESOURCE_ATTRIBUTE_PRESENT |
             EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
             EFI_RESOURCE_ATTRIBUTE_TESTED |
             EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
             EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
             EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
             EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE;

  MemInfo = (PAYLOAD_MEM_INFO *)Param;
  Base    = MemoryMapEntry->Base;
  Size    = MemoryMapEntry->Size;
  
  if (!Size) {
    return EFI_SUCCESS;
  }

  if (Base >= 0x100000) {
    if (MemoryMapEntry->Type == 1) {
      //
      // System RAM
      //
      if (Base < 0x100000000ULL) {
        if (Size >= (UINT64)MemInfo->UsableLowMemMinSize) {
          if (MemInfo->UsableLowMemTop < (UINT32)(Base + Size)) {
            MemInfo->UsableLowMemTop = (UINT32)(Base + Size);
          }
        }
      } else {
        Attribue &= ~EFI_RESOURCE_ATTRIBUTE_TESTED;
      }
      BuildResourceDescriptorHob (
        EFI_RESOURCE_SYSTEM_MEMORY,
        Attribue,
        (EFI_PHYSICAL_ADDRESS)Base,
        Size
        );    
      DEBUG ((EFI_D_INFO, "System Memory Base = 0x%lX, Size = 0x%lX\n",
      (UINT64)Base, (UINT64)Size));  
    } else if (MemoryMapEntry->Type == 2) {
      //
      // Memory Type Reserved
      //
      BuildResourceDescriptorHob (
          EFI_RESOURCE_MEMORY_RESERVED,
          Attribue,
          (EFI_PHYSICAL_ADDRESS)Base,
          Size
          ); 
      DEBUG ((EFI_D_INFO, "Reserved Memory Base = 0x%lX, Size = 0x%lX\n",
      (UINT64)Base, (UINT64)Size));  
      if (Base < 0x100000000ULL) {
        SystemLowMemTop = ((UINT32)(Base + Size) + 0x0FFFFFFF) & 0xF0000000;
        if (SystemLowMemTop > MemInfo->SystemLowMemTop) {
          MemInfo->SystemLowMemTop = SystemLowMemTop;
        }      
      }
    } else if (MemoryMapEntry->Type == 3) {
      //
      // ACPI Relcaim memory
      //
      BuildMemoryAllocationHob (
        (UINT64)(UINTN)Base,
        Size,
        EfiACPIReclaimMemory
      );

      DEBUG ((EFI_D_INFO, "ACPI Reclaim Memory Base = 0x%lX, Size = 0x%lX\n",
        (UINT64)Base, (UINT64)Size));  
      if (Base < 0x100000000ULL) {
        SystemLowMemTop = ((UINT32)(Base + Size) + 0x0FFFFFFF) & 0xF0000000;
        if (SystemLowMemTop > MemInfo->SystemLowMemTop) {
          MemInfo->SystemLowMemTop = SystemLowMemTop;
        }      
      }
    } else if (MemoryMapEntry->Type == 4) {
      //
      // Acpi NVS memory
      //
      BuildMemoryAllocationHob (
        (UINT64)(UINTN)Base,
        Size,
        EfiACPIMemoryNVS
      );

      DEBUG ((EFI_D_INFO, "Acpi NVS Memory Base = 0x%lX, Size = 0x%lX\n",
        (UINT64)Base, (UINT64)Size));  
      if (Base < 0x100000000ULL) {
        SystemLowMemTop = ((UINT32)(Base + Size) + 0x0FFFFFFF) & 0xF0000000;
        if (SystemLowMemTop > MemInfo->SystemLowMemTop) {
          MemInfo->SystemLowMemTop = SystemLowMemTop;
        }      
      }
    } else {
      DEBUG ((EFI_D_INFO, "Other Memory Base = 0x%lX, Size = 0x%lX\n",
      (UINT64)Base, (UINT64)Size));  
    }
  }
      
  return EFI_SUCCESS;
}

/**
  This is the entrypoint of PEIM

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCCESS if it completed successfully.
**/
EFI_STATUS
EFIAPI
PayloadPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS           Status;
  UINT64               LowMemorySize;
  UINT64               PeiMemSize = SIZE_64MB;
  EFI_PHYSICAL_ADDRESS PeiMemBase = 0;
  UINT32               RegEax;
  UINT8                PhysicalAddressBits;
  VOID*                pCbHeader;
  VOID*                pAcpiTable;
  UINT32               AcpiTableSize;
  VOID*                pSmbiosTable;
  UINT32               SmbiosTableSize;
  SYSTEM_TABLE_INFO*   pSystemTableInfo;
  FRAME_BUFFER_INFO    FbInfo;
  FRAME_BUFFER_INFO*   pFbInfo;
  ACPI_BOARD_INFO*     pAcpiBoardInfo;
  UINTN                PmCtrlRegBase, PmTimerRegBase, ResetRegAddress, ResetValue;
  UINTN                PmEvtBase;
  UINTN                PmGpeEnBase;
  PAYLOAD_MEM_INFO     MemInfo;
  LOADER_FSP_INFO      LdrFspInfo;
  BOOLEAN              CorebootFound;
  UINTN                *TpmLasa;
  UINT8                *TpmChecksum;

  if (CorebootExists ()) {
    DEBUG ((EFI_D_INFO, "Coreboot exists!\n"));
  } else {
    DEBUG ((EFI_D_INFO, "Coreboot does NOT exist!\n"));
  }
  
  //
  // Report lower 640KB of RAM. Attribute EFI_RESOURCE_ATTRIBUTE_TESTED  
  // is intentionally omitted to prevent erasing of the coreboot header  
  // record before it is processed by ParseMemoryInfo.
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_SYSTEM_MEMORY,
    (
    EFI_RESOURCE_ATTRIBUTE_PRESENT |
    EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
    EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
    ),
    (EFI_PHYSICAL_ADDRESS)(0),
    (UINT64)(0xA0000)
    );


  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_RESERVED,
    (
    EFI_RESOURCE_ATTRIBUTE_PRESENT |
    EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
    EFI_RESOURCE_ATTRIBUTE_TESTED |
    EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
    ),
    (EFI_PHYSICAL_ADDRESS)(0xA0000),
    (UINT64)(0x60000)
    );

  ZeroMem (&MemInfo, sizeof(MemInfo));
  MemInfo.UsableLowMemMinSize = (UINT32)PeiMemSize;

  Status = ParseMemoryInfoByCb (CorebootMemInfoCallback, (VOID *)&MemInfo);
  if (Status == EFI_SUCCESS) {
    //
    // Found Coreboot
    //
    CorebootFound = TRUE;
  } else {
    //
    // Didn't find Coreboot, fall back to Slim Bootloader
    //	
    CorebootFound = FALSE;
    Status = ParseMemoryInfoByHob ((SBL_MEM_INFO_CALLBACK)SblMemInfoCallback, (VOID *)&MemInfo);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

  DEBUG ((EFI_D_INFO, "Payload StackTop = 0x%X\n", PcdGet32(PcdPayloadStackTop)));
  DEBUG ((EFI_D_INFO, "mPayLoadHOBBase = 0x%X\n", mPayLoadHOBBase));
  
  LowMemorySize = MemInfo.UsableLowMemTop;
  DEBUG ((EFI_D_INFO, "Low memory 0x%lx\n", LowMemorySize));
  DEBUG ((EFI_D_INFO, "SystemLowMemTop 0x%x\n", MemInfo.SystemLowMemTop));
  ASSERT (MemInfo.UsableLowMemTop > 0);
  ASSERT (MemInfo.SystemLowMemTop > 0);

  //
  // Should be 64k aligned
  //
  if (CorebootFound) {
    PeiMemBase = (LowMemorySize - PeiMemSize) & (~(BASE_64KB - 1));
  } else {
    PeiMemBase = (LowMemorySize - PeiMemSize) & (~(BASE_64KB - 1));
  }

  DEBUG((EFI_D_ERROR, "PeiMemBase: 0x%lx.\n", PeiMemBase));
  DEBUG((EFI_D_ERROR, "PeiMemSize: 0x%lx.\n", PeiMemSize));

  Status = PeiServicesInstallPeiMemory (
         PeiMemBase,
         PeiMemSize
         );
  ASSERT_EFI_ERROR (Status);

  //
  // Set cache on the physical memory
  //
  MtrrSetMemoryAttribute (BASE_1MB, LowMemorySize - BASE_1MB, CacheWriteBack);
  MtrrSetMemoryAttribute (0, 0xA0000, CacheWriteBack);

  //
  // Create Memory Type Information HOB
  //
  BuildGuidDataHob (
    &gEfiMemoryTypeInformationGuid,
    mDefaultMemoryTypeInformation,
    sizeof(mDefaultMemoryTypeInformation)
    );

  //
  // Create Fv hob
  //
  PayloadPeiReportRemainingFvs ();

  BuildMemoryAllocationHob (
    PcdGet32 (PcdPayloadFdMemBase),
    PcdGet32 (PcdPayloadFdMemSize),
    EfiBootServicesData
    );

  //
  // Build CPU memory space and IO space hob
  //
  AsmCpuid (0x80000000, &RegEax, NULL, NULL, NULL);
  if (RegEax >= 0x80000008) {
    AsmCpuid (0x80000008, &RegEax, NULL, NULL, NULL);
    PhysicalAddressBits = (UINT8) RegEax;
  } else {
    PhysicalAddressBits  = 36;
  }
  //
  // Create a CPU hand-off information
  //
  BuildCpuHob (PhysicalAddressBits, 16);

  //
  // Report Local APIC range
  //
  BuildMemoryMappedIoRangeHob (0xFEC80000, SIZE_512KB);

  //
  // Boot mode
  //
  Status = PeiServicesSetBootMode (BOOT_WITH_FULL_CONFIGURATION);
  ASSERT_EFI_ERROR (Status);

  Status = PeiServicesInstallPpi (mPpiBootMode);
  ASSERT_EFI_ERROR (Status);

  Status = PcdSet32S (PcdCbHeaderPointer, 0);
  ASSERT_EFI_ERROR (Status);
  if (CorebootFound) {
    //
    // Set pcd to save the upper coreboot header in case the dxecore will
    // erase 0~4k memory
    //
    pCbHeader = NULL;
    if ((GetCbHeaderInDepth (1, &pCbHeader) == RETURN_SUCCESS)
      && ((UINTN)pCbHeader > BASE_4KB)) {
      DEBUG((EFI_D_ERROR, "Actual Coreboot header: %p.\n", pCbHeader));
      Status = PcdSet32S (PcdCbHeaderPointer, (UINT32)(UINTN)pCbHeader);
      ASSERT_EFI_ERROR (Status);
    }
  }

  //
  // Create guid hob for system tables like acpi table and smbios table
  //
  pAcpiTable = NULL;
  AcpiTableSize = 0;
  pSmbiosTable = NULL;
  SmbiosTableSize = 0;
  if (CorebootFound) {
    Status = ParseAcpiTableByCb (&pAcpiTable, &AcpiTableSize);
  } else {
    Status = ParseAcpiTableByHob (&pAcpiTable, &AcpiTableSize);
  }
  if (EFI_ERROR (Status)) {
    //
    // ACPI table is mandatory
    //
    DEBUG ((EFI_D_ERROR, "Failed to find the required ACPI table\n"));
    ASSERT (FALSE);
  }
  DEBUG ((EFI_D_INFO, "AcpiTable Base = 0x%lX, AcpiTableSize = 0x%lX\n", 
    (UINT64)(UINTN)pAcpiTable, (UINT64)AcpiTableSize ));

  if (CorebootFound) {
    ParseSmbiosTableByCb (&pSmbiosTable, &SmbiosTableSize);
  } else {
    ParseSmbiosTableByHob (&pSmbiosTable, &SmbiosTableSize);
  }

  TpmLasa = 0;
  TpmChecksum = 0;
  if (CorebootFound) {
	  Status = ParseTpmTable(TRUE, &TpmLasa, &TpmChecksum);
  }
  else {
	  Status = ParseTpmTable(FALSE, &TpmLasa, &TpmChecksum);
  }

  pSystemTableInfo = NULL;
  pSystemTableInfo = BuildGuidHob (&gUefiSystemTableInfoGuid, sizeof (SYSTEM_TABLE_INFO));
  ASSERT (pSystemTableInfo != NULL);
  pSystemTableInfo->AcpiTableBase = (UINT64) (UINTN)pAcpiTable;
  pSystemTableInfo->AcpiTableSize = AcpiTableSize;
  pSystemTableInfo->SmbiosTableBase = (UINT64) (UINTN)pSmbiosTable;
  pSystemTableInfo->SmbiosTableSize = SmbiosTableSize;
  DEBUG ((EFI_D_INFO, "Detected Acpi Table at 0x%lx, length 0x%x\n", pSystemTableInfo->AcpiTableBase, pSystemTableInfo->AcpiTableSize));
  DEBUG ((EFI_D_INFO, "Detected Smbios Table at 0x%lx, length 0x%x\n", pSystemTableInfo->SmbiosTableBase, pSystemTableInfo->SmbiosTableSize));
  DEBUG ((EFI_D_INFO, "Create system table info guid hob\n"));

  //
  // Create guid hob for acpi board information
  //
  if (CorebootFound) {
    Status = ParseFadtInfoByCb (&PmCtrlRegBase, &PmTimerRegBase, &ResetRegAddress, &ResetValue, &PmEvtBase, &PmGpeEnBase);
  } else {
    Status = ParseFadtInfoByHob (&PmCtrlRegBase, &PmTimerRegBase, &ResetRegAddress, &ResetValue, &PmEvtBase, &PmGpeEnBase);
  }
  ASSERT_EFI_ERROR (Status);
  pAcpiBoardInfo = NULL;
  pAcpiBoardInfo = BuildGuidHob (&gUefiAcpiBoardInfoGuid, sizeof (ACPI_BOARD_INFO));
  ASSERT (pAcpiBoardInfo != NULL);
  pAcpiBoardInfo->PmCtrlRegBase = (UINT64)PmCtrlRegBase;
  pAcpiBoardInfo->PmTimerRegBase = (UINT64)PmTimerRegBase;
  pAcpiBoardInfo->ResetRegAddress = (UINT64)ResetRegAddress;
  pAcpiBoardInfo->ResetValue = (UINT8)ResetValue;
  pAcpiBoardInfo->PmEvtBase = (UINT64)PmEvtBase;
  pAcpiBoardInfo->PmGpeEnBase = (UINT64)PmGpeEnBase;
  pAcpiBoardInfo->TpmLasa = (UINT64) ((UINTN)TpmLasa);
  pAcpiBoardInfo->TpmChecksum = (UINT64) ((UINTN)TpmChecksum);
  DEBUG ((EFI_D_INFO, "Created acpi board info guid hob\n"));

  //
  // Create guid hob for frame buffer information
  //
  ZeroMem (&FbInfo, sizeof (FRAME_BUFFER_INFO));
  if (CorebootFound) {
    Status = ParseFrameBufferInfoByCb (&FbInfo);
  } else {
    Status = ParseFrameBufferInfoByHob (&FbInfo);
  }
  if (!EFI_ERROR (Status)) {
    pFbInfo = BuildGuidHob (&gUefiFrameBufferInfoGuid, sizeof (FRAME_BUFFER_INFO));
    ASSERT (pSystemTableInfo != NULL);
    CopyMem (pFbInfo, &FbInfo, sizeof (FRAME_BUFFER_INFO));
    DEBUG ((EFI_D_INFO, "Created frame buffer info guid hob\n"));
  }

  if (!CorebootFound) {
    //
    // Update FSPs base
    //
    Status = ParseFspInfoByHob (&LdrFspInfo);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Error when parsing FSP info, Status = %r\n", Status));
      return Status;
    }
    PcdSet32S (PcdFspsBaseAddress, LdrFspInfo.FspsBase);
    PcdSet32S (PcdFspHobList, (UINT32)LdrFspInfo.FspHobList);    
  }

  //
  // Mask off all legacy 8259 interrupt sources
  //
  IoWrite8 (LEGACY_8259_MASK_REGISTER_MASTER, 0xFF);
  IoWrite8 (LEGACY_8259_MASK_REGISTER_SLAVE,  0xFF);

  return EFI_SUCCESS;
}

