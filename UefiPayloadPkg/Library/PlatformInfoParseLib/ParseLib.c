/** @file
  Platform Info Parse Library. Provides functions to parse information passed from the
  previous boot stage.

  Copyright (c) 2014 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/PlatformInfoParseLib.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/UefiTcgPlatform.h>
#include <IndustryStandard/Tpm20.h>
#include <Protocol/Tcg2Protocol.h>
#include "Coreboot.h"

VOID *mPayLoadHOBBase = NULL ;
#pragma pack (1)
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER Header;
  UINT32                      Flags;
  UINT64                      AddressOfControlArea;
  UINT32                      StartMethod;
  UINT8                       PlatformSpecificParameters[12];
  UINT32                      LAML;
  UINT64                      LASA;
} EFI_TPM2_ACPI_TABLE;

typedef struct _EFI_TCG_CLIENT_ACPI_TABLE {
  EFI_ACPI_DESCRIPTION_HEADER       Header;
  UINT16                            PlatformClass;
  UINT32                            LAML;
  UINT64                            LASA;
} EFI_TCG_CLIENT_ACPI_TABLE;

typedef struct {
  TPMI_ALG_HASH              HashAlgo;
  UINT16                     HashSize;
  UINT32                     HashMask;
} INTERNAL_HASH_INFO;

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER  Header;
  UINT32                       Entry;
} RSDT_TABLE;

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER  Header;
  UINT64                       Entry;
} XSDT_TABLE;

#pragma pack ()

STATIC INTERNAL_HASH_INFO mHashInfo[] = {
  { TPM_ALG_SHA1,          SHA1_DIGEST_SIZE,     HASH_ALG_SHA1 },
  { TPM_ALG_SHA256,        SHA256_DIGEST_SIZE,   HASH_ALG_SHA256 },
  { TPM_ALG_SM3_256,       SM3_256_DIGEST_SIZE,  HASH_ALG_SM3_256 },
  { TPM_ALG_SHA384,        SHA384_DIGEST_SIZE,   HASH_ALG_SHA384 },
  { TPM_ALG_SHA512,        SHA512_DIGEST_SIZE,   HASH_ALG_SHA512 },
};

/**
  Convert a packed value from cbuint64 to a UINT64 value.

  @param  val      The pointer to packed data.

  @return          the UNIT64 value after convertion.

**/
UINT64
cb_unpack64 (
  IN struct cbuint64 val
  )
{
  return LShiftU64 (val.hi, 32) | val.lo;
}


/**
  Returns the sum of all elements in a buffer of 16-bit values.  During
  calculation, the carry bits are also been added.

  @param  Buffer      The pointer to the buffer to carry out the sum operation.
  @param  Length      The size, in bytes, of Buffer.

  @return Sum         The sum of Buffer with carry bits included during additions.

**/
UINT16
CbCheckSum16 (
  IN UINT16   *Buffer,
  IN UINTN    Length
  )
{
  UINT32 Sum, TmpValue;
  UINTN  Idx;
  UINT8  *TmpPtr;

  Sum = 0;
  TmpPtr = (UINT8 *)Buffer;
  for(Idx = 0; Idx < Length; Idx++) {
    TmpValue  = TmpPtr[Idx];
    if (Idx % 2 == 1) {
      TmpValue <<= 8;
    }

    Sum += TmpValue;

    // Wrap
    if (Sum >= 0x10000) {
      Sum = (Sum + (Sum >> 16)) & 0xFFFF;
    }
  }

  return (UINT16)((~Sum) & 0xFFFF);
}


/**
  Find coreboot record with given Tag from the memory Start in 4096
  bytes range.

  @param  Start              The start memory to be searched in
  @param  Tag                The tag id to be found

  @retval NULL              The Tag is not found.
  @retval Others            The poiter to the record found.

**/
VOID *
EFIAPI
FindCbTag (
  IN  VOID     *Start,
  IN  UINT32   Tag
  )
{
  struct cb_header   *Header;
  struct cb_record   *Record;
  UINT8              *TmpPtr;
  UINT8              *TagPtr;
  UINTN              Idx;
  UINT16             CheckSum;

  Header = NULL;
  TmpPtr = (UINT8 *)Start;
  for (Idx = 0; Idx < 4096; Idx += 16, TmpPtr += 16) {
    Header = (struct cb_header *)TmpPtr;
    if (Header->signature == CB_HEADER_SIGNATURE) {
      break;
    }
  }

  if (Idx >= 4096) {
    return NULL;
  }

  if ((Header == NULL) || (Header->table_bytes == 0)) {
    return NULL;
  }

  //
  // Check the checksum of the coreboot table header
  //
  CheckSum = CbCheckSum16 ((UINT16 *)Header, sizeof (*Header));
  if (CheckSum != 0) {
    DEBUG ((EFI_D_ERROR, "Invalid coreboot table header checksum\n"));
    return NULL;
  }

  CheckSum = CbCheckSum16 ((UINT16 *)(TmpPtr + sizeof (*Header)), Header->table_bytes);
  if (CheckSum != Header->table_checksum) {
    DEBUG ((EFI_D_ERROR, "Incorrect checksum of all the coreboot table entries\n"));
    return NULL;
  }

  TagPtr = NULL;
  TmpPtr += Header->header_bytes;
  for (Idx = 0; Idx < Header->table_entries; Idx++) {
    Record = (struct cb_record *)TmpPtr;
    if (Record->tag == CB_TAG_FORWARD) {
      TmpPtr = (VOID *)(UINTN)((struct cb_forward *)(UINTN)Record)->forward;
      if (Tag == CB_TAG_FORWARD) {
        return TmpPtr;
      } else {
        return FindCbTag (TmpPtr, Tag);
      }
    }
    if (Record->tag == Tag) {
      TagPtr = TmpPtr;
      break;
    }
    TmpPtr += Record->size;
  }

  return TagPtr;
}


/**
  Find the given table with TableId from the given coreboot memory Root.

  @param  Root               The coreboot memory table to be searched in
  @param  TableId            Table id to be found
  @param  pMemTable          To save the base address of the memory table found
  @param  pMemTableSize      To save the size of memory table found

  @retval RETURN_SUCCESS            Successfully find out the memory table.
  @retval RETURN_INVALID_PARAMETER  Invalid input parameters.
  @retval RETURN_NOT_FOUND          Failed to find the memory table.

**/
RETURN_STATUS
EFIAPI
FindCbMemTable (
  IN  struct cbmem_root  *Root,
  IN  UINT32             TableId,
  OUT VOID               **pMemTable,
  OUT UINT32             *pMemTableSize
  )
{
  UINTN                Idx;
  BOOLEAN              IsImdEntry;
  struct cbmem_entry  *Entries;

  if ((Root == NULL) || (pMemTable == NULL)) {
    return RETURN_INVALID_PARAMETER;
  }
  //
  // Check if the entry is CBMEM or IMD
  // and handle them separately
  //
  Entries = Root->entries;
  if (Entries[0].magic == CBMEM_ENTRY_MAGIC) {
    IsImdEntry = FALSE;
  } else {
    Entries = (struct cbmem_entry *)((struct imd_root *)Root)->entries;
    if (Entries[0].magic == IMD_ENTRY_MAGIC) {
      IsImdEntry = TRUE;
    } else {
      return RETURN_NOT_FOUND;
    }
  }

  for (Idx = 0; Idx < Root->num_entries; Idx++) {
    if (Entries[Idx].id == TableId) {
      if (IsImdEntry) {
        *pMemTable = (VOID *) ((UINTN)Entries[Idx].start + (UINTN)Root);
      } else {
        *pMemTable = (VOID *) (UINTN)Entries[Idx].start;
      }
      if (pMemTableSize != NULL) {
        *pMemTableSize = Entries[Idx].size;
      }

      DEBUG ((EFI_D_INFO, "Find CbMemTable Id 0x%x, base %p, size 0x%x\n",
        TableId, *pMemTable, Entries[Idx].size));
      return RETURN_SUCCESS;
    }
  }

  return RETURN_NOT_FOUND;
}

/**
  Get the Hob list passed from the previous stage firmware

  @param  (VOID)

  @retval Pointer to the Hob list

**/
VOID *GetPayloadHobList (VOID)
{  
  if (mPayLoadHOBBase == NULL) {
    mPayLoadHOBBase = (VOID *)(UINTN)(*(UINT32 *)(PcdGet32(PcdPayloadStackTop) - sizeof(UINT32)));
  }
  return mPayLoadHOBBase;
}

/**
  Determine if Coreboot exists in the system

  @param  (VOID)

  @retval TRUE     Coreboot exists in the system
  @retval FALSE    Coreboot doesn't exist in the system

**/
BOOLEAN
EFIAPI
CorebootExists (
  VOID
  )
{
  struct cb_memory         *rec;

  //
  // Get the coreboot memory table
  //
  rec = (struct cb_memory *)FindCbTag (0, CB_TAG_MEMORY);
  if (rec == NULL) {
    rec = (struct cb_memory *)FindCbTag ((VOID *)(UINTN)PcdGet32 (PcdCbHeaderPointer), CB_TAG_MEMORY);
  }

  if (rec == NULL) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/**
  Acquire the memory information from the coreboot table in memory.

  @param  MemInfoCallback     The callback routine
  @param  pParam              Pointer to the callback routine parameter

  @retval RETURN_SUCCESS     Successfully find out the memory information.
  @retval RETURN_NOT_FOUND   Failed to find the memory information.

**/
RETURN_STATUS
EFIAPI
ParseMemoryInfoByCb (
  IN  CB_MEM_INFO_CALLBACK  MemInfoCallback,
  IN  VOID                  *pParam
  )
{
  struct cb_memory         *rec;
  struct cb_memory_range   *Range;
  UINT64                   Start;
  UINT64                   Size;
  UINTN                    Index;

  //
  // Get the coreboot memory table
  //
  rec = (struct cb_memory *)FindCbTag (0, CB_TAG_MEMORY);
  if (rec == NULL) {
    rec = (struct cb_memory *)FindCbTag ((VOID *)(UINTN)PcdGet32 (PcdCbHeaderPointer), CB_TAG_MEMORY);
  }

  if (rec == NULL) {
    return RETURN_NOT_FOUND;
  }

  for (Index = 0; Index < MEM_RANGE_COUNT(rec); Index++) {
    Range = MEM_RANGE_PTR(rec, Index);
    Start = cb_unpack64(Range->start);
    Size = cb_unpack64(Range->size);
    DEBUG ((EFI_D_INFO, "%d. %016lx - %016lx [%02x]\n",
            Index, Start, Start + Size - 1, Range->type));

    MemInfoCallback (Start, Size, Range->type, pParam);
  }

  return RETURN_SUCCESS;
}



/**
  Acquire the coreboot memory table with the given table id

  @param  TableId            Table id to be searched
  @param  pMemTable          Pointer to the base address of the memory table
  @param  pMemTableSize      Pointer to the size of the memory table

  @retval RETURN_SUCCESS     Successfully find out the memory table.
  @retval RETURN_INVALID_PARAMETER  Invalid input parameters.
  @retval RETURN_NOT_FOUND   Failed to find the memory table.

**/
STATIC
RETURN_STATUS
EFIAPI
ParseCbMemTable (
  IN  UINT32     TableId,
  OUT VOID       **pMemTable,
  OUT UINT32     *pMemTableSize
  )
{
  struct cb_memory         *rec;
  struct cb_memory_range   *Range;
  UINT64                   Start;
  UINT64                   Size;
  UINTN                    Index;

  if (pMemTable == NULL) {
    return RETURN_INVALID_PARAMETER;
  }
  *pMemTable = NULL;

  //
  // Get the coreboot memory table
  //
  rec = (struct cb_memory *)FindCbTag (0, CB_TAG_MEMORY);
  if (rec == NULL) {
    rec = (struct cb_memory *)FindCbTag ((VOID *)(UINTN)PcdGet32 (PcdCbHeaderPointer), CB_TAG_MEMORY);
  }

  if (rec == NULL) {
    return RETURN_NOT_FOUND;
  }

  for (Index = 0; Index < MEM_RANGE_COUNT(rec); Index++) {
    Range = MEM_RANGE_PTR(rec, Index);
    Start = cb_unpack64(Range->start);
    Size = cb_unpack64(Range->size);

    if ((Range->type == CB_MEM_TABLE) && (Start > 0x1000)) {
      if (FindCbMemTable ((struct  cbmem_root *)(UINTN)(Start + Size - DYN_CBMEM_ALIGN_SIZE), TableId, pMemTable, pMemTableSize) == RETURN_SUCCESS)
        return RETURN_SUCCESS;
    }
  }

  return RETURN_NOT_FOUND;
}


/**
  Acquire the acpi table from coreboot

  @param  pMemTable          Pointer to the base address of the memory table
  @param  pMemTableSize      Pointer to the size of the memory table

  @retval RETURN_SUCCESS     Successfully find out the memory table.
  @retval RETURN_INVALID_PARAMETER  Invalid input parameters.
  @retval RETURN_NOT_FOUND   Failed to find the memory table.

**/
RETURN_STATUS
EFIAPI
ParseAcpiTableByCb (
  OUT VOID       **pMemTable,
  OUT UINT32     *pMemTableSize
  )
{
  return ParseCbMemTable (SIGNATURE_32 ('I', 'P', 'C', 'A'), pMemTable, pMemTableSize);
}

/**
  Acquire the smbios table from coreboot

  @param  pMemTable          Pointer to the base address of the memory table
  @param  pMemTableSize      Pointer to the size of the memory table

  @retval RETURN_SUCCESS     Successfully find out the memory table.
  @retval RETURN_INVALID_PARAMETER  Invalid input parameters.
  @retval RETURN_NOT_FOUND   Failed to find the memory table.

**/
RETURN_STATUS
EFIAPI
ParseSmbiosTableByCb (
  OUT VOID       **pMemTable,
  OUT UINT32     *pMemTableSize
  )
{
  return ParseCbMemTable (SIGNATURE_32 ('T', 'B', 'M', 'S'), pMemTable, pMemTableSize);
}


/**
  Acquire the memory information from the HOBs provided by Slim Bootloader.

  @param  MemInfoCallback     The callback routine
  @param  pParam              Pointer to the callback routine parameter

  @retval RETURN_SUCCESS     Successfully find out the memory information.
  @retval RETURN_NOT_FOUND   Failed to find the memory information.

**/
RETURN_STATUS
EFIAPI
ParseMemoryInfoByHob (
  IN  SBL_MEM_INFO_CALLBACK  MemInfoCallback,
  IN  VOID                  *Param
  )
{  
  EFI_HOB_GUID_TYPE             *GuidHob;
  MEMROY_MAP_INFO               *MemoryMapInfo;
  UINTN                          Idx;
  
  GuidHob = GetNextGuidHob (&gLoaderMemoryMapInfoGuid, GetPayloadHobList());
  if (GuidHob == NULL) {      
    ASSERT (GuidHob);
    return RETURN_NOT_FOUND;
  } 
  
  MemoryMapInfo = (MEMROY_MAP_INFO *)GET_GUID_HOB_DATA(GuidHob);
  for (Idx = 0; Idx < MemoryMapInfo->Count; Idx++) {  
    MemInfoCallback (&MemoryMapInfo->Entry[Idx], Param);        
  }
  
  return RETURN_SUCCESS;
}



/**
  Acquire the acpi table from Slim Bootloader

  @param  pMemTable          Pointer to the base address of the memory table
  @param  pMemTableSize      Pointer to the size of the memory table

  @retval RETURN_SUCCESS     Successfully find out the memory table.
  @retval RETURN_INVALID_PARAMETER  Invalid input parameters.
  @retval RETURN_NOT_FOUND   Failed to find the memory table.

**/
RETURN_STATUS
EFIAPI
ParseAcpiTableByHob (
  OUT VOID       **pMemTable,
  OUT UINT32     *pMemTableSize
  )
{
  EFI_HOB_GUID_TYPE             *GuidHob;
  SYSTEM_TABLE_INFO             *PlatformTbl;
  
  GuidHob = GetNextGuidHob (&gUefiSystemTableInfoGuid, GetPayloadHobList());
  if (GuidHob) {
    PlatformTbl = (SYSTEM_TABLE_INFO *)GET_GUID_HOB_DATA(GuidHob);
    if (pMemTable) {
      *pMemTable = (VOID *)(UINTN)PlatformTbl->AcpiTableBase;
    }
    if (pMemTableSize) {
      *pMemTableSize = PlatformTbl->AcpiTableSize;  
    }
    return RETURN_SUCCESS;
  } else {
    ASSERT (GuidHob);
    return RETURN_NOT_FOUND;
  } 
}

/**
  Acquire the SMBIOS table from Slim Bootloader

  @param  pMemTable          Pointer to the base address of the memory table
  @param  pMemTableSize      Pointer to the size of the memory table

  @retval RETURN_SUCCESS     Successfully find out the memory table.
  @retval RETURN_INVALID_PARAMETER  Invalid input parameters.
  @retval RETURN_NOT_FOUND   Failed to find the memory table.

**/
RETURN_STATUS
EFIAPI
ParseSmbiosTableByHob (
  OUT VOID       **pMemTable,
  OUT UINT32     *pMemTableSize
  )
{
  EFI_HOB_GUID_TYPE             *GuidHob;
  SYSTEM_TABLE_INFO             *PlatformTbl;
  
  GuidHob = GetNextGuidHob (&gUefiSystemTableInfoGuid, GetPayloadHobList());
  if (GuidHob) {
    PlatformTbl = (SYSTEM_TABLE_INFO *)GET_GUID_HOB_DATA(GuidHob);
    if (pMemTable) {
      *pMemTable = (VOID *)(UINTN)PlatformTbl->SmbiosTableBase;
    }
    if (pMemTableSize) {
      *pMemTableSize = PlatformTbl->SmbiosTableSize;  
    }
    return RETURN_SUCCESS;
  } else {
    ASSERT (GuidHob);
    return RETURN_NOT_FOUND;
  } 
}

/**
  Acquire the SMBIOS table from Coreboot

  @param  pMemTable          Pointer to the base address of the memory table
  @param  pMemTableSize      Pointer to the size of the memory table

  @retval RETURN_SUCCESS     Successfully find out the memory table.
  @retval RETURN_INVALID_PARAMETER  Invalid input parameters.
  @retval RETURN_NOT_FOUND   Failed to find the memory table.

**/
RETURN_STATUS
EFIAPI
ParseFspInfoByHob (
  OUT LOADER_FSP_INFO *LdrFspInfo  
  )
{
  EFI_HOB_GUID_TYPE             *GuidHob;
  LOADER_FSP_INFO               *FspInfo;
  
  GuidHob = GetNextGuidHob (&gLoaderFspInfoGuid, GetPayloadHobList());
  if (GuidHob) {
    FspInfo = (LOADER_FSP_INFO *)GET_GUID_HOB_DATA(GuidHob);
    if (LdrFspInfo) {
      *LdrFspInfo = *FspInfo;
    }
    return RETURN_SUCCESS;
  } else {
    ASSERT (GuidHob);
    return RETURN_NOT_FOUND;
  } 
}

/**
  Find the required fadt information

  @param  Coreboot           Whether to retrieve from Coreboot (TRUE) or from Hobs (FALSE)
  @param  pPmCtrlReg         Pointer to the address of power management control register
  @param  pPmTimerReg        Pointer to the address of power management timer register
  @param  pResetReg          Pointer to the address of system reset register
  @param  pResetValue        Pointer to the value to be writen to the system reset register
  @param  pPmEvtReg          Pointer to the address of power management event register
  @param  pPmGpeEnReg        Pointer to the address of power management GPE enable register

  @retval RETURN_SUCCESS     Successfully find out all the required fadt information.
  @retval RETURN_NOT_FOUND   Failed to find the fadt table.

**/
STATIC
RETURN_STATUS
EFIAPI
ParseFadtInfoInternal (
  IN  BOOLEAN    Coreboot,
  OUT UINTN      *pPmCtrlReg,
  OUT UINTN      *pPmTimerReg,
  OUT UINTN      *pResetReg,
  OUT UINTN      *pResetValue,
  OUT UINTN      *pPmEvtReg,
  OUT UINTN      *pPmGpeEnReg
  )
{
  EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Rsdp;
  EFI_ACPI_DESCRIPTION_HEADER                   *Rsdt;
  UINT32                                        *Entry32;
  UINTN                                         Entry32Num;
  EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE     *Fadt;
  EFI_ACPI_DESCRIPTION_HEADER                   *Xsdt;
  UINT64                                        *Entry64;
  UINTN                                         Entry64Num;
  UINTN                                         Idx;
  RETURN_STATUS                                 Status;

  Rsdp = NULL;
  Status = RETURN_SUCCESS;

  if (Coreboot) {
    Status = ParseAcpiTableByCb ((VOID **)&Rsdp, NULL);
  } else {
    Status = ParseAcpiTableByHob ((VOID **)&Rsdp, NULL);
  }

  if (RETURN_ERROR(Status)) {
    return Status;
  }

  if (Rsdp == NULL) {
    return RETURN_NOT_FOUND;
  }

  DEBUG ((EFI_D_INFO, "Find Rsdp at %p\n", Rsdp));
  DEBUG ((EFI_D_INFO, "Find Rsdt 0x%x, Xsdt 0x%lx\n", Rsdp->RsdtAddress, Rsdp->XsdtAddress));

  //
  // Search Rsdt First
  //
  Rsdt     = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)(Rsdp->RsdtAddress);
  if (Rsdt != NULL) {
    Entry32  = (UINT32 *)(Rsdt + 1);
    Entry32Num = (Rsdt->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) >> 2;
    for (Idx = 0; Idx < Entry32Num; Idx++) {
      if (*(UINT32 *)(UINTN)(Entry32[Idx]) == EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE) {
        Fadt = (EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE *)(UINTN)(Entry32[Idx]);
        if (pPmCtrlReg != NULL) {
          *pPmCtrlReg = Fadt->Pm1aCntBlk;
        }
        DEBUG ((EFI_D_INFO, "PmCtrl Reg 0x%x\n", Fadt->Pm1aCntBlk));

        if (pPmTimerReg != NULL) {
          *pPmTimerReg = Fadt->PmTmrBlk;
        }
        DEBUG ((EFI_D_INFO, "PmTimer Reg 0x%x\n", Fadt->PmTmrBlk));

        if (pResetReg != NULL) {
          *pResetReg = (UINTN)Fadt->ResetReg.Address;
        }
        DEBUG ((EFI_D_INFO, "Reset Reg 0x%lx\n", Fadt->ResetReg.Address));

        if (pResetValue != NULL) {
          *pResetValue = Fadt->ResetValue;
        }
        DEBUG ((EFI_D_INFO, "Reset Value 0x%x\n", Fadt->ResetValue));

        if (pPmEvtReg != NULL) {
          *pPmEvtReg = Fadt->Pm1aEvtBlk;
          DEBUG ((EFI_D_INFO, "PmEvt Reg 0x%x\n", Fadt->Pm1aEvtBlk));
        }

        if (pPmGpeEnReg != NULL) {
          *pPmGpeEnReg = Fadt->Gpe0Blk + Fadt->Gpe0BlkLen / 2;
          DEBUG ((EFI_D_INFO, "PmGpeEn Reg 0x%x\n", *pPmGpeEnReg));
        }

        //
        // Verify values for proper operation
        //
        ASSERT(Fadt->Pm1aCntBlk != 0);
        ASSERT(Fadt->PmTmrBlk != 0);
        ASSERT(Fadt->ResetReg.Address != 0);
        ASSERT(Fadt->Pm1aEvtBlk != 0);
        ASSERT(Fadt->Gpe0Blk != 0);

        DEBUG_CODE_BEGIN ();
          BOOLEAN    SciEnabled;

          //
          // Check the consistency of SCI enabling
          //

          //
          // Get SCI_EN value
          //
          if (Fadt->Pm1CntLen == 4) {
            SciEnabled = (IoRead32 (Fadt->Pm1aCntBlk) & BIT0)? TRUE : FALSE;
          } else {
            //
            // if (Pm1CntLen == 2), use 16 bit IO read;
            // if (Pm1CntLen != 2 && Pm1CntLen != 4), use 16 bit IO read as a fallback
            //
            SciEnabled = (IoRead16 (Fadt->Pm1aCntBlk) & BIT0)? TRUE : FALSE;
          }

          if (!(Fadt->Flags & EFI_ACPI_5_0_HW_REDUCED_ACPI) &&
              (Fadt->SmiCmd == 0) &&
              !SciEnabled) {
            //
            // The ACPI enabling status is inconsistent: SCI is not enabled but ACPI
            // table does not provide a means to enable it through FADT->SmiCmd
            //
            DEBUG ((DEBUG_ERROR, "ERROR: The ACPI enabling status is inconsistent: SCI is not"
              " enabled but the ACPI table does not provide a means to enable it through FADT->SmiCmd."
              " This may cause issues in OS.\n"));
            ASSERT (FALSE);
          }
        DEBUG_CODE_END ();
        return RETURN_SUCCESS;
      }
    }
  }

  //
  // Search Xsdt Second
  //
  Xsdt     = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)(Rsdp->XsdtAddress);
  if (Xsdt != NULL) {
    Entry64  = (UINT64 *)(Xsdt + 1);
    Entry64Num = (Xsdt->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) >> 3;
    for (Idx = 0; Idx < Entry64Num; Idx++) {
      if (*(UINT32 *)(UINTN)(Entry64[Idx]) == EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE) {
        Fadt = (EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE *)(UINTN)(Entry64[Idx]);
        if (pPmCtrlReg)
          *pPmCtrlReg = Fadt->Pm1aCntBlk;
        DEBUG ((EFI_D_ERROR, "PmCtrl Reg 0x%x\n", Fadt->Pm1aCntBlk));

        if (pPmTimerReg)
          *pPmTimerReg = Fadt->PmTmrBlk;
        DEBUG ((EFI_D_ERROR, "PmTimer Reg 0x%x\n", Fadt->PmTmrBlk));

        if (pResetReg)
          *pResetReg = (UINTN)Fadt->ResetReg.Address;
        DEBUG ((EFI_D_ERROR, "Reset Reg 0x%lx\n", Fadt->ResetReg.Address));

        if (pResetValue)
          *pResetValue = Fadt->ResetValue;
        DEBUG ((EFI_D_ERROR, "Reset Value 0x%x\n", Fadt->ResetValue));

        if (pPmEvtReg != NULL) {
          *pPmEvtReg = Fadt->Pm1aEvtBlk;
           DEBUG ((EFI_D_INFO, "PmEvt Reg 0x%x\n", Fadt->Pm1aEvtBlk));
        }

        if (pPmGpeEnReg != NULL) {
          *pPmGpeEnReg = Fadt->Gpe0Blk + Fadt->Gpe0BlkLen / 2;
          DEBUG ((EFI_D_INFO, "PmGpeEn Reg 0x%x\n", *pPmGpeEnReg));
        }
        return RETURN_SUCCESS;
      }
    }
  }

  return RETURN_NOT_FOUND;
}

/**
  Find fadt information from Coreboot

  @param  pPmCtrlReg         Pointer to the address of power management control register
  @param  pPmTimerReg        Pointer to the address of power management timer register
  @param  pResetReg          Pointer to the address of system reset register
  @param  pResetValue        Pointer to the value to be writen to the system reset register
  @param  pPmEvtReg          Pointer to the address of power management event register
  @param  pPmGpeEnReg        Pointer to the address of power management GPE enable register

  @retval RETURN_SUCCESS     Successfully find out all the required fadt information.
  @retval RETURN_NOT_FOUND   Failed to find the fadt table.

**/
RETURN_STATUS
EFIAPI
ParseFadtInfoByCb (
  OUT UINTN      *pPmCtrlReg,
  OUT UINTN      *pPmTimerReg,
  OUT UINTN      *pResetReg,
  OUT UINTN      *pResetValue,
  OUT UINTN      *pPmEvtReg,
  OUT UINTN      *pPmGpeEnReg
  )
{
  return ParseFadtInfoInternal (TRUE, pPmCtrlReg, pPmTimerReg, pResetReg, pResetValue, pPmEvtReg, pPmGpeEnReg);
}

/**
  Find fadt information from Slim Bootloader

  @param  pPmCtrlReg         Pointer to the address of power management control register
  @param  pPmTimerReg        Pointer to the address of power management timer register
  @param  pResetReg          Pointer to the address of system reset register
  @param  pResetValue        Pointer to the value to be writen to the system reset register
  @param  pPmEvtReg          Pointer to the address of power management event register
  @param  pPmGpeEnReg        Pointer to the address of power management GPE enable register

  @retval RETURN_SUCCESS     Successfully find out all the required fadt information.
  @retval RETURN_NOT_FOUND   Failed to find the fadt table.

**/
RETURN_STATUS
EFIAPI
ParseFadtInfoByHob (
  OUT UINTN      *pPmCtrlReg,
  OUT UINTN      *pPmTimerReg,
  OUT UINTN      *pResetReg,
  OUT UINTN      *pResetValue,
  OUT UINTN      *pPmEvtReg,
  OUT UINTN      *pPmGpeEnReg
  )
{
  return ParseFadtInfoInternal (FALSE, pPmCtrlReg, pPmTimerReg, pResetReg, pResetValue, pPmEvtReg, pPmGpeEnReg);
}

/**
  Find the required TPM information

  @param  Coreboot           CoreBoot or Slim Bootloader
  @param  Version            TPM ACPI Version
  @param  pLAML              Pointer to the address of LAML

  @retval RETURN_SUCCESS     Successfully find out all the required fadt information.
  @retval RETURN_NOT_FOUND   Failed to find the TPM table.

**/
STATIC
RETURN_STATUS
EFIAPI
ParseTpmInfoInternal(
  IN  BOOLEAN    Coreboot,
  OUT UINTN      *Version,
  OUT UINTN      **pLAML,
  OUT UINTN      **pLASA,
  OUT UINT8      **pChecksum
)
{
  EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Rsdp;
  EFI_ACPI_DESCRIPTION_HEADER                   *Rsdt;
  UINT32                                        *Entry32;
  UINTN                                         Entry32Num;
  UINTN                                         Idx;
  RETURN_STATUS                                 Status;
  EFI_TPM2_ACPI_TABLE                           *Tpm2Table;
  EFI_TCG_CLIENT_ACPI_TABLE                     *Tpm12Table;

  Rsdp = NULL;
  Status = RETURN_SUCCESS;

  if (Coreboot) {
    Status = ParseAcpiTableByCb((VOID **)&Rsdp, NULL);
  }
  else {
    Status = ParseAcpiTableByHob((VOID **)&Rsdp, NULL);
  }

  if (RETURN_ERROR(Status)) {
    return Status;
  }

  if (Rsdp == NULL) {
    return RETURN_NOT_FOUND;
  }

  DEBUG((EFI_D_INFO, "Find Rsdp at %p\n", Rsdp));
  DEBUG((EFI_D_INFO, "Find Rsdt 0x%x, Xsdt 0x%lx\n", Rsdp->RsdtAddress, Rsdp->XsdtAddress));

  //
  // Search Rsdt First
  //
  Rsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)(Rsdp->RsdtAddress);
  if (Rsdt != NULL) {
    Entry32 = (UINT32 *)(Rsdt + 1);
    Entry32Num = (Rsdt->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) >> 2;
    for (Idx = 0; Idx < Entry32Num; Idx++) {
      if (*(UINT32 *)(UINTN)(Entry32[Idx]) == SIGNATURE_32('T', 'P', 'M', '2')) {
        Tpm2Table = (EFI_TPM2_ACPI_TABLE *)(UINTN)(Entry32[Idx]);
        DEBUG((EFI_D_INFO, "Find Tpm2Table at %p\n", Tpm2Table));
        DEBUG((EFI_D_INFO, "Length = %x\n", Tpm2Table->Header.Length));
        if (Tpm2Table->Header.Length == 76) {
          *Version = 0x2;
          *pChecksum = (UINT8*)((UINTN) &Tpm2Table->Header.Checksum);
          *pLAML = (UINTN*)((UINTN)&Tpm2Table->LAML);
          *pLASA = (UINTN*)((UINTN)&Tpm2Table->LASA);
          return RETURN_SUCCESS;
        }
      }
      else if(*(UINT32 *)(UINTN)(Entry32[Idx]) == SIGNATURE_32('T', 'C', 'P', 'A')) {
        Tpm12Table = (EFI_TCG_CLIENT_ACPI_TABLE *)(UINTN)(Entry32[Idx]);
        if (Tpm12Table->Header.Length == 50) {
          *Version = 0x12;
          *pChecksum = (UINT8*)((UINTN) &Tpm12Table->Header.Checksum);
          *pLAML = (UINTN*)((UINTN) &Tpm12Table->LAML);
          *pLASA = (UINTN*)((UINTN) &Tpm12Table->LASA);
          return RETURN_SUCCESS;
        }
      }
    }
  }

  return RETURN_NOT_FOUND;
}


/**
  Find the serial port information from Slim Bootloader

  @param  pRegBase           Pointer to the base address of serial port registers
  @param  pRegAccessType     Pointer to the access type of serial port registers
  @param  pRegWidth          Pointer to the register width in bytes
  @param  pBaudrate          Pointer to the serial port baudrate
  @param  pInputHertz        Pointer to the input clock frequency
  @param  pUartPciAddr       Pointer to the UART PCI bus, dev and func address

  @retval RETURN_SUCCESS     Successfully find the serial port information.
  @retval RETURN_NOT_FOUND   Failed to find the serial port information .

**/
RETURN_STATUS
EFIAPI
ParseSerialInfoByHob (
  OUT UINT32      *pRegBase,
  OUT UINT32      *pRegAccessType,
  OUT UINT32      *pRegWidth,
  OUT UINT32      *pBaudrate,
  OUT UINT32      *pInputHertz,
  OUT UINT32      *pUartPciAddr
  )
{
  EFI_HOB_GUID_TYPE             *GuidHob;
  SERIAL_PORT_INFO              *PldSerialInfo;
  
  GuidHob = GetNextGuidHob (&gUefiSerialPortInfoGuid, GetPayloadHobList());
  if (!GuidHob) {
    ASSERT (FALSE);
    return RETURN_NOT_FOUND;
  } 

  PldSerialInfo = (SERIAL_PORT_INFO *)GET_GUID_HOB_DATA(GuidHob);

  if (pRegBase != NULL) {
    *pRegBase = PldSerialInfo->BaseAddr;
  }

  if (pRegWidth != NULL) {
    *pRegWidth = PldSerialInfo->RegWidth;
  }

  if (pRegAccessType != NULL) {
    *pRegAccessType = PldSerialInfo->Type;
  }

  if (pBaudrate != NULL) {
    *pBaudrate = PldSerialInfo->Baud;
  }

  if (pInputHertz != NULL) {
    *pInputHertz = PldSerialInfo->InputHertz;
  }

  if (pUartPciAddr != NULL) {
    *pUartPciAddr = PldSerialInfo->UartPciAddr;
  }

  return RETURN_SUCCESS;
}


/**
  Find the video frame buffer information from Slim Bootloader

  @param  pFbInfo            Pointer to the FRAME_BUFFER_INFO structure

  @retval RETURN_SUCCESS     Successfully find the video frame buffer information.
  @retval RETURN_NOT_FOUND   Failed to find the video frame buffer information .

**/
RETURN_STATUS
EFIAPI
ParseFrameBufferInfoByHob (
  OUT FRAME_BUFFER_INFO       *pFbInfo
  )
{
  EFI_HOB_GUID_TYPE     *GuidHob;
  FRAME_BUFFER_INFO     *FbInfo;
  
  GuidHob = GetNextGuidHob (&gUefiFrameBufferInfoGuid, GetPayloadHobList());
  if (!GuidHob) {
    return RETURN_NOT_FOUND;
  } 

  FbInfo = (FRAME_BUFFER_INFO *)GET_GUID_HOB_DATA(GuidHob);
  
  if (pFbInfo) {
    *pFbInfo = *FbInfo;
  }
  
  return RETURN_SUCCESS;
}


/**
  Find the serial port information from Coreboot

  @param  pRegBase           Pointer to the base address of serial port registers
  @param  pRegAccessType     Pointer to the access type of serial port registers
  @param  pRegWidth          Pointer to the register width in bytes
  @param  pBaudrate          Pointer to the serial port baudrate
  @param  pInputHertz        Pointer to the input clock frequency
  @param  pUartPciAddr       Pointer to the UART PCI bus, dev and func address

  @retval RETURN_SUCCESS     Successfully find the serial port information.
  @retval RETURN_NOT_FOUND   Failed to find the serial port information .

**/
RETURN_STATUS
EFIAPI
ParseSerialInfoByCb (
  OUT UINT32      *pRegBase,
  OUT UINT32      *pRegAccessType,
  OUT UINT32      *pRegWidth,
  OUT UINT32      *pBaudrate,
  OUT UINT32      *pInputHertz,
  OUT UINT32      *pUartPciAddr
  )
{
  struct cb_serial    *CbSerial;

  CbSerial = FindCbTag (0, CB_TAG_SERIAL);
  if (CbSerial == NULL) {
    CbSerial = FindCbTag ((VOID *)(UINTN)PcdGet32 (PcdCbHeaderPointer), CB_TAG_SERIAL);
  }

  if (CbSerial == NULL) {
    return RETURN_NOT_FOUND;
  }

  if (pRegBase != NULL) {
    *pRegBase = CbSerial->baseaddr;
  }

  if (pRegWidth != NULL) {
    *pRegWidth = CbSerial->regwidth;
  }

  if (pRegAccessType != NULL) {
    *pRegAccessType = CbSerial->type;
  }

  if (pBaudrate != NULL) {
    *pBaudrate = CbSerial->baud;
  }

  if (pInputHertz != NULL) {
    *pInputHertz = CbSerial->input_hertz;
  }

  if (pUartPciAddr != NULL) {
    *pUartPciAddr = CbSerial->uart_pci_addr;
  }

  return RETURN_SUCCESS;
}

/**
  Search for the Coreboot table header

  @param  Level              Level of the search depth
  @param  HeaderPtr          Pointer to the pointer of coreboot table header

  @retval RETURN_SUCCESS     Successfully find the coreboot table header .
  @retval RETURN_NOT_FOUND   Failed to find the coreboot table header .

**/
RETURN_STATUS
EFIAPI
GetCbHeaderInDepth (
  IN  UINTN  Level,
  OUT VOID   **HeaderPtr
  )
{
  UINTN Index;
  VOID  *TempPtr;

  if (HeaderPtr == NULL) {
    return RETURN_NOT_FOUND;
  }

  TempPtr = NULL;
  for (Index = 0; Index < Level; Index++) {
    TempPtr = FindCbTag (TempPtr, CB_TAG_FORWARD);
    if (TempPtr == NULL) {
      break;
    }
  }

  if ((Index >= Level) && (TempPtr != NULL)) {
    *HeaderPtr = TempPtr;
    return RETURN_SUCCESS;
  }

  return RETURN_NOT_FOUND;
}

/**
  Find the video frame buffer information from Coreboot

  @param  pFbInfo            Pointer to the FRAME_BUFFER_INFO structure

  @retval RETURN_SUCCESS     Successfully find the video frame buffer information.
  @retval RETURN_NOT_FOUND   Failed to find the video frame buffer information .

**/
RETURN_STATUS
EFIAPI
ParseFrameBufferInfoByCb (
  OUT FRAME_BUFFER_INFO       *pFbInfo
  )
{
  struct cb_framebuffer       *CbFbRec;

  if (pFbInfo == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  CbFbRec = FindCbTag (0, CB_TAG_FRAMEBUFFER);
  if (CbFbRec == NULL) {
    CbFbRec = FindCbTag ((VOID *)(UINTN)PcdGet32 (PcdCbHeaderPointer), CB_TAG_FRAMEBUFFER);
  }

  if (CbFbRec == NULL) {
    return RETURN_NOT_FOUND;
  }

  DEBUG ((EFI_D_INFO, "Found coreboot video frame buffer information\n"));
  DEBUG ((EFI_D_INFO, "physical_address: 0x%lx\n", CbFbRec->physical_address));
  DEBUG ((EFI_D_INFO, "x_resolution: 0x%x\n", CbFbRec->x_resolution));
  DEBUG ((EFI_D_INFO, "y_resolution: 0x%x\n", CbFbRec->y_resolution));
  DEBUG ((EFI_D_INFO, "bits_per_pixel: 0x%x\n", CbFbRec->bits_per_pixel));
  DEBUG ((EFI_D_INFO, "bytes_per_line: 0x%x\n", CbFbRec->bytes_per_line));

  DEBUG ((EFI_D_INFO, "red_mask_size: 0x%x\n", CbFbRec->red_mask_size));
  DEBUG ((EFI_D_INFO, "red_mask_pos: 0x%x\n", CbFbRec->red_mask_pos));
  DEBUG ((EFI_D_INFO, "green_mask_size: 0x%x\n", CbFbRec->green_mask_size));
  DEBUG ((EFI_D_INFO, "green_mask_pos: 0x%x\n", CbFbRec->green_mask_pos));
  DEBUG ((EFI_D_INFO, "blue_mask_size: 0x%x\n", CbFbRec->blue_mask_size));
  DEBUG ((EFI_D_INFO, "blue_mask_pos: 0x%x\n", CbFbRec->blue_mask_pos));
  DEBUG ((EFI_D_INFO, "reserved_mask_size: 0x%x\n", CbFbRec->reserved_mask_size));
  DEBUG ((EFI_D_INFO, "reserved_mask_pos: 0x%x\n", CbFbRec->reserved_mask_pos));

  pFbInfo->LinearFrameBuffer    = CbFbRec->physical_address;
  pFbInfo->HorizontalResolution = CbFbRec->x_resolution;
  pFbInfo->VerticalResolution   = CbFbRec->y_resolution;
  pFbInfo->BitsPerPixel         = CbFbRec->bits_per_pixel;
  pFbInfo->BytesPerScanLine     = (UINT16)CbFbRec->bytes_per_line;
  pFbInfo->Red.Mask             = (1 << CbFbRec->red_mask_size) - 1;
  pFbInfo->Red.Position         = CbFbRec->red_mask_pos;
  pFbInfo->Green.Mask           = (1 << CbFbRec->green_mask_size) - 1;
  pFbInfo->Green.Position       = CbFbRec->green_mask_pos;
  pFbInfo->Blue.Mask            = (1 << CbFbRec->blue_mask_size) - 1;
  pFbInfo->Blue.Position        = CbFbRec->blue_mask_pos;
  pFbInfo->Reserved.Mask        = (1 << CbFbRec->reserved_mask_size) - 1;
  pFbInfo->Reserved.Position    = CbFbRec->reserved_mask_pos;

  return RETURN_SUCCESS;
}

/**
  Return size of digest.

  @param[in] HashAlgo  Hash algorithm

  @return size of digest
**/
UINT16
EFIAPI
ParseLibGetHashSizeFromAlgo(
  IN TPMI_ALG_HASH    HashAlgo
)
{
  UINTN  Index;

  for (Index = 0; Index < sizeof(mHashInfo) / sizeof(mHashInfo[0]); Index++) {
    if (mHashInfo[Index].HashAlgo == HashAlgo) {
      return mHashInfo[Index].HashSize;
    }
  }
  return 0;
}

/**
  This function returns size of TCG PCR event 2.

  @param[in]  TcgPcrEvent2     TCG PCR event 2 structure.

  @return size of TCG PCR event 2.
**/
UINTN
ParseLibGetPcrEvent2Size(
  IN TCG_PCR_EVENT2        *TcgPcrEvent2
)
{
  UINT32                    DigestIndex;
  UINT32                    DigestCount;
  TPMI_ALG_HASH             HashAlgo;
  UINT32                    DigestSize;
  UINT8                     *DigestBuffer;
  UINT32                    EventSize;
  UINT8                     *EventBuffer;

  DigestCount = TcgPcrEvent2->Digest.count;
  HashAlgo = TcgPcrEvent2->Digest.digests[0].hashAlg;
  DigestBuffer = (UINT8 *)&TcgPcrEvent2->Digest.digests[0].digest;
  for (DigestIndex = 0; DigestIndex < DigestCount; DigestIndex++) {
    DigestSize = ParseLibGetHashSizeFromAlgo(HashAlgo);
    //
    // Prepare next
    //
    CopyMem(&HashAlgo, DigestBuffer + DigestSize, sizeof(TPMI_ALG_HASH));
    DigestBuffer = DigestBuffer + DigestSize + sizeof(TPMI_ALG_HASH);
  }
  DigestBuffer = DigestBuffer - sizeof(TPMI_ALG_HASH);

  CopyMem(&EventSize, DigestBuffer, sizeof(TcgPcrEvent2->EventSize));
  EventBuffer = DigestBuffer + sizeof(TcgPcrEvent2->EventSize);

  return (UINTN)EventBuffer + EventSize - (UINTN)TcgPcrEvent2;
}

/**
  Add the TPM table from Slim Bootloader into HOB

  @param[in]  TpmVersion     TPM12 or TPM2.
  @param[in]  EventHdr       The TPM Event log address.
  @param[in]  DataLength     The TPM Event log size.

  @retval RETURN_SUCCESS     Successfully add the TPM table.

**/
RETURN_STATUS
EFIAPI
AddEventIntoHob(
  IN UINT8                       TpmVersion,
  IN VOID                        *EventHdr,
  IN UINTN                       DataLength
  ) 
{
  if (TpmVersion == EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2) {
    BuildGuidDataHob(
      &gTcgEventEntryHobGuid,
      EventHdr,
      DataLength
    );
  }
  else if (TpmVersion == EFI_TCG2_EVENT_LOG_FORMAT_TCG_2) {
    BuildGuidDataHob(
      &gTcgEvent2EntryHobGuid,
      EventHdr,
      DataLength
    );
  }
  return RETURN_SUCCESS;
}

/**
  Acquire the TPM table from Slim Bootloader

  @param  None

  @retval RETURN_SUCCESS     Successfully find out the TPM table.
  @retval RETURN_NOT_FOUND   Failed to find the TPM table.

**/
EFI_STATUS
EFIAPI
ParseTpmTable(
  BOOLEAN Coreboot,
  UINTN   **Lasa,
  UINT8   **Checksum
)
{
  UINTN                       EventLogLocation12 = 0;
  UINTN                       EventLogLocation2 = 0;
  UINTN                       LAML12 = 0;
  UINTN                       LAML2 = 0;
  TCG_PCR_EVENT_HDR           *EventHdr;
  TCG_PCR_EVENT2              *TcgPcrEvent2;

  EFI_HOB_GUID_TYPE             *GuidHob;
  TPM_INFO                      *PldTpmInfo;
  UINT32                        TPM_Ver;
  UINTN                         *pLAML = NULL;
  UINTN                         *pLASA = NULL;
  UINT8                         *pChecksum = NULL;
  RETURN_STATUS                 Status;

  if (Coreboot) {
    GuidHob = NULL;
  } else {
    GuidHob = GetNextGuidHob(&gUefiTpmInfoGuid, GetPayloadHobList());
  }
  if (!GuidHob) {
    Status = ParseTpmInfoInternal (Coreboot, (UINTN*)&TPM_Ver, &pLAML, &pLASA, &pChecksum);
    if (RETURN_ERROR(Status)) {
    }
    else {
      if (TPM_Ver == 0x2) {
        EventLogLocation2 = (UINTN) *pLASA;
        LAML2 = (UINTN) *pLAML;
      }
      else {
        EventLogLocation12 = (UINTN)*pLASA;
        LAML12 = (UINTN)*pLAML;
      }
    }
  } else {

    PldTpmInfo = (TPM_INFO *)GET_GUID_HOB_DATA(GuidHob);

    EventLogLocation12 = PldTpmInfo->TpmTable12Base;
    LAML12 = PldTpmInfo->TpmTable12LAML;
    EventLogLocation2 = PldTpmInfo->TpmTable2Base;
    LAML2 = PldTpmInfo->TpmTable2LAML;
  }
  
  EventHdr = (TCG_PCR_EVENT_HDR *)(UINTN)EventLogLocation12;
  if (EventHdr) {
    while ((UINTN)EventHdr < (EventLogLocation12 + LAML12 -1)) {
          if(*((UINT32*)EventHdr) == 0xFFFFFFFF) break;
      AddEventIntoHob(EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2, EventHdr, sizeof(TCG_PCR_EVENT_HDR) + EventHdr->EventSize);
      EventHdr = (TCG_PCR_EVENT_HDR *)((UINTN)EventHdr + sizeof(TCG_PCR_EVENT_HDR) + EventHdr->EventSize);
    }
  }
  
  TcgPcrEvent2 = (TCG_PCR_EVENT2*)(UINTN)EventLogLocation2;
  if (TcgPcrEvent2) {
    // bypass the first event log for spec ID
      TcgPcrEvent2 = (TCG_PCR_EVENT2 *)((UINTN)TcgPcrEvent2 + sizeof(TCG_PCR_EVENT_HDR) + ((TCG_PCR_EVENT_HDR *)TcgPcrEvent2)->EventSize);

    while ((UINTN)TcgPcrEvent2 < (EventLogLocation2 + LAML2 -1)) {
//to-do:         if(*((UINT32*)TcgPcrEvent2) == 0xFFFFFFFF)  break;
          if (TcgPcrEvent2->EventType == 0)  break;
      AddEventIntoHob(EFI_TCG2_EVENT_LOG_FORMAT_TCG_2, TcgPcrEvent2, ParseLibGetPcrEvent2Size(TcgPcrEvent2));
      TcgPcrEvent2 = (TCG_PCR_EVENT2 *)((UINTN)TcgPcrEvent2 + ParseLibGetPcrEvent2Size(TcgPcrEvent2));
    }
  }
  
  *Lasa = pLASA;
  *Checksum = pChecksum;

  return EFI_SUCCESS;
}
    