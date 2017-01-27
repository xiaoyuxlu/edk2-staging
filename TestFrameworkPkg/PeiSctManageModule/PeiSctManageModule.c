/** @file
  An EFI Application for management

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ShellLib.h>
#include <Library/ShellCommandLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/PeiSctResultHob.h>

//#include "PeiSctManageModule.h"

typedef struct SCT_ITEM_RECORD {
  INT8 ItemId;
  INT8 CheckNum;    //item need reboot if none zero
  EFI_GUID HobGuid; //mark the HOB we used to store check results during PEI phase.
}SCT_ITEM_RECORD;

EFI_STATUS
InitManageModule(VOID);

EFI_STATUS
SelectedItemAvailable (SCT_ITEM_RECORD*);

EFI_STATUS
RecordUserSelection(VOID);

EFI_STATUS
ConfigVar(VOID);

EFI_STATUS
DeleteVar(VOID);

EFI_STATUS
ParseHob(EFI_GUID*);

EFI_STATUS
ParseHobData (VOID*, SHELL_FILE_HANDLE, UINTN);

EFI_STATUS
GenerateReport(VOID);


VOID
BurnFd(EFI_HANDLE, CHAR16*);

SHELL_FILE_HANDLE
Fopen (
  CHAR16    *Name,
  UINT64    Mode
  );
EFI_STATUS
Fclose (
  SHELL_FILE_HANDLE   Handle
  );
UINTN
Fwrite (
  SHELL_FILE_HANDLE   Handle,
  UINT8             *Buffer,
  UINTN             BufferSize
  );
UINTN
Fread (
  SHELL_FILE_HANDLE   Handle,
  UINT8             *Buffer,
  UINTN             BufferSize
  );
EFI_STATUS
Fseek (
  SHELL_FILE_HANDLE   Handle,
  UINT64            Position
  );
EFI_STATUS
Flocate (
  SHELL_FILE_HANDLE   Handle,
  UINT64            *Position
  );
EFI_STATUS
Fremove (CHAR16*);



#define NO_ITEM_ID -1
#define ITEM_NUM_MAX 32
#define INPUT_LEN_MAX 256
#define REPORT_BUFFER_SIZE 256

#define INIT_FILE  L"Config.ini"
#define CHECK_LIST_FILE  L"SCT.item"
#define REPORT_FILE  L"SCT.report"

CHAR16 BurnUtility[INPUT_LEN_MAX];
CHAR16 Option[INPUT_LEN_MAX];

INT8 RebootNeed[ITEM_NUM_MAX + 1];

#define EFI_HOB_LIST_GUID \
  { \
    0x7739f24c, 0x93d7, 0x11d4, 0x9a, 0x3a, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d \
  }

STATIC CHAR16 FdLocation[ITEM_NUM_MAX + 1][INPUT_LEN_MAX];



/**
  Removes (trims) specified leading and trailing characters from a string.

  @param[in, out]  Str  Pointer to the null-terminated string to be trimmed. On return,
                        Str will hold the trimmed string.

  @param[in]      CharC Character will be trimmed from str.
**/
VOID
StrTrim (
  IN OUT CHAR16   *Str,
  IN     CHAR16   CharC
  )
{
  CHAR16  *Pointer1;
  CHAR16  *Pointer2;

  if (*Str == 0) {
    return;
  }

  //
  // Trim off the leading and trailing characters c
  //
  for (Pointer1 = Str; (*Pointer1 != 0) && (*Pointer1 == CharC); Pointer1++) {
    ;
  }

  Pointer2 = Str;
  if (Pointer2 == Pointer1) {
    while (*Pointer1 != 0) {
      Pointer2++;
      Pointer1++;
    }
  } else {
    while (*Pointer1 != 0) {
    *Pointer2 = *Pointer1;
    Pointer1++;
    Pointer2++;
    }
    *Pointer2 = 0;
  }


  for (Pointer1 = Str + StrLen(Str) - 1; Pointer1 >= Str && *Pointer1 == CharC; Pointer1--) {
    ;
  }
  if  (Pointer1 !=  Str + StrLen(Str) - 1) {
    *(Pointer1 + 1) = 0;
  }
}


EFI_STATUS
EFIAPI
ManageModuleEntryPoint (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
/*++

Routine Description:

  EFI App for managing SCT Peims.

Arguments:

  ImageHandle   - Image's handle.

  SystemTable   - System table.

Returns:

  EFI_STATUS

--*/
{
  EFI_STATUS Status = EFI_SUCCESS;
  SCT_ITEM_RECORD ItemRecord;
  BOOLEAN NeedReset = TRUE;

  ConfigVar();

  Status = InitManageModule();
  if (EFI_ERROR(Status)) {
    Print(L"Init Fail!\n");
    goto end;
  }

  Status = RecordUserSelection();
  if (EFI_ALREADY_STARTED == Status) {
    Status = GenerateReport();
    if (EFI_ERROR(Status)) {
    //deal with report generation error here
      }
  }

  Status = SelectedItemAvailable(&ItemRecord);
  if (EFI_NOT_FOUND == Status) {
      NeedReset = FALSE;
  }

  if (NeedReset) {
    BurnFd(ImageHandle, FdLocation[ItemRecord.ItemId]);
    gST->RuntimeServices->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  }

end:
  Print(L"\n\n");
  DeleteVar();
  Status = Fremove(CHECK_LIST_FILE);

  return Status;
}

EFI_STATUS
SelectedItemAvailable (
  SCT_ITEM_RECORD * ItemRecord
  )
{
  SHELL_FILE_HANDLE ConfigFileHandle = NULL;
  UINT64 Pos = 0;
  INTN Size = 0;
  SCT_ITEM_RECORD TempRecord;

  SetMem(&TempRecord, sizeof(SCT_ITEM_RECORD), 0);

  ConfigFileHandle = Fopen(CHECK_LIST_FILE, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE);
   if (NULL == ConfigFileHandle) {
    Print (L"CANNOT OPEN CHECK LIST FILE !\n");
    return EFI_ABORTED;
  }

  Fseek(ConfigFileHandle, 0);
  while ((Size = Fread(ConfigFileHandle, (UINT8 *)&TempRecord, sizeof (SCT_ITEM_RECORD))) == sizeof (SCT_ITEM_RECORD)) {
    if (TempRecord.ItemId != NO_ITEM_ID) {
      CopyMem (ItemRecord, &TempRecord, sizeof(SCT_ITEM_RECORD));
      TempRecord.ItemId = NO_ITEM_ID;
      Flocate (ConfigFileHandle, &Pos);
      Fseek (ConfigFileHandle, Pos-Size);
      Fwrite(ConfigFileHandle, (UINT8 *)&TempRecord, Size);
      Fclose(ConfigFileHandle);
      return EFI_SUCCESS;
    }

    else {
      if (TempRecord.CheckNum > 0) {
        TempRecord.CheckNum--;
        Flocate (ConfigFileHandle, &Pos);
        Fseek (ConfigFileHandle, Pos-Size);
        Fwrite(ConfigFileHandle, (UINT8 *)&TempRecord, Size);
        Fclose(ConfigFileHandle);
        gST->RuntimeServices->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
      }
    }
  }

  Fclose(ConfigFileHandle);

  return EFI_NOT_FOUND;
}

EFI_STATUS
RecordUserSelection (
  VOID
  )
{
  UINTN ItemId = 1;
  EFI_STATUS Status = EFI_SUCCESS;
  SCT_ITEM_RECORD SelectedItem;
  SHELL_FILE_HANDLE ConfigFileHandle = NULL;
  INTN WriteSize = 0;

  //
  //Config file has been created
  //
  ConfigFileHandle = Fopen(CHECK_LIST_FILE, EFI_FILE_MODE_READ);
  if (ConfigFileHandle != NULL) {
    Fclose(ConfigFileHandle);
    return EFI_ALREADY_STARTED;
  }

  ConfigFileHandle = Fopen(CHECK_LIST_FILE, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE);
  if (NULL == ConfigFileHandle) {
    Print(L"\nCheck List File Creation Fail!\n");
      return EFI_NOT_STARTED;
    }

  Fseek(ConfigFileHandle, 0);

  while (FdLocation[ItemId][0] != 0) {

    SetMem(&SelectedItem, sizeof(SCT_ITEM_RECORD), 0);

    SelectedItem.ItemId = (INT8)ItemId;
    if (RebootNeed[SelectedItem.ItemId] == 1) {
        SelectedItem.CheckNum = 1;
    }

    CopyMem (&SelectedItem.HobGuid, &gPeiSctResultHobGuid, sizeof(EFI_GUID));
    //record files to burn into check list file.
    WriteSize = Fwrite(ConfigFileHandle, (UINT8 *)&SelectedItem, sizeof (SelectedItem));
    if (WriteSize != sizeof (SelectedItem)) {
      Status = EFI_ABORTED;
      break;
    }

    ItemId++;
  }

  if (ConfigFileHandle != NULL) {
    Fclose(ConfigFileHandle);
  }
  return Status;

}

EFI_STATUS
ConfigVar (
  VOID
  )
{
  //
  //set config var for pei var service ppi function test.
  //
  INT8 VarData = 0;
  INT8 VarData1 = -1;
  UINTN VarLen = 1;
  EFI_STATUS Status;

  /*if Var exists, no set and reboot needed, clear this var and return to see Var Ppi behavior*/
  Status = gST->RuntimeServices->GetVariable (L"TEST_VAR_1", &gPeiSctVendorGuid, NULL, &VarLen, &VarData1);
  if (!EFI_ERROR(Status)) {
    return Status;
  }

    Status = gST->RuntimeServices->SetVariable (L"TEST_VAR_1", &gPeiSctVendorGuid, EFI_VARIABLE_RUNTIME_ACCESS |EFI_VARIABLE_BOOTSERVICE_ACCESS |EFI_VARIABLE_NON_VOLATILE, sizeof (INT8), &VarData);
    if (EFI_ERROR(Status)) {
      Print (L"\nSetVar Error : %d\n", Status);
    }

  return Status;
}

EFI_STATUS
DeleteVar (
  VOID
  )
{
  INT8 VarData1 = -1;
  EFI_STATUS Status = gST->RuntimeServices->SetVariable (L"TEST_VAR_1", &gPeiSctVendorGuid, 0, 0, &VarData1);
  if (EFI_ERROR(Status)) {
    Print (L"\nDelVar Error : %d\n", Status);
  }

  return Status;
}

EFI_STATUS
SctGuidToStr (
  IN EFI_GUID                     *Guid,
  OUT CHAR16                      *Buffer
  )
/*++

Routine Description:

  Convert a GUID to a string.

--*/
{
  UINT32  Index;
  UINT32  BufferIndex;
  UINT32  DataIndex;
  UINT32  Len;

  if ((Guid == NULL) || (Buffer == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  BufferIndex = 0;

  //
  // Convert a GUID to a string
  //
  Len = sizeof(UINT32)*2;
  for (Index = 0; Index < Len; Index++) {
    Buffer[BufferIndex]  = 0;
    Buffer[BufferIndex] = (CHAR16)((Guid->Data1 & (0xf0000000>>(4 * Index)))>>(4 * (Len - Index - 1)));
    if (Buffer[BufferIndex] < 0x0A) {
       Buffer[BufferIndex] += (CHAR16)(L'0');
    } else {
       Buffer[BufferIndex] = (CHAR16)((L'A') + (Buffer[BufferIndex] - 0x0A));
    }
    BufferIndex ++;
  }

  Buffer[BufferIndex++] = L'-';

  //
  // Convert Guid->Data2
  //
  Len = sizeof(UINT16)*2;
  for (Index = 0; Index < Len; Index++) {
    Buffer[BufferIndex]  = 0;
    Buffer[BufferIndex] = (CHAR16)((Guid->Data2 & (0xf000>>(4 * Index)))>>(4 * (Len - Index - 1)));
    if (Buffer[BufferIndex] < 0x0A) {
       Buffer[BufferIndex] += (CHAR16)(L'0');
    } else {
       Buffer[BufferIndex] = (CHAR16)((L'A') + (Buffer[BufferIndex] - 0x0A));
    }
    BufferIndex ++;
  }

  Buffer[BufferIndex++] = L'-';

  //
  // Convert Guid->Data3
  //
  Len = sizeof(UINT16)*2;
  for (Index = 0; Index < Len; Index++) {
    Buffer[BufferIndex]  = 0;
    Buffer[BufferIndex] = (CHAR16)((Guid->Data3 & (0xf000>>(4 * Index)))>>(4 * (Len - Index - 1)));
    if (Buffer[BufferIndex] < 0x0A) {
       Buffer[BufferIndex] += (CHAR16)(L'0');
    } else {
       Buffer[BufferIndex] = (CHAR16)((L'A') + (Buffer[BufferIndex] - 0x0A));
    }
    BufferIndex ++;
  }

  Buffer[BufferIndex++] = L'-';

  //
  // Convert Guid->Data4[x]
  //
  Len = sizeof(UINT8)*2;
  for (DataIndex = 0; DataIndex < 8; DataIndex++) {
     for (Index = 0; Index < Len; Index++) {
        Buffer[BufferIndex]  = 0;
        Buffer[BufferIndex] = (CHAR16)((Guid->Data4[DataIndex] & (0xf0>>(4 * Index)))>>(4 * (Len - Index - 1)));
        if (Buffer[BufferIndex] < 0x0A) {
           Buffer[BufferIndex] += (CHAR16)(L'0');
        } else {
       Buffer[BufferIndex] = (CHAR16)((L'A') + (Buffer[BufferIndex] - 0x0A));
        }
        BufferIndex ++;
     }

     if (DataIndex == 1) {
       Buffer[BufferIndex++] = L'-';
     }
  }

  Buffer[BufferIndex] = 0;

  return EFI_SUCCESS;
}


EFI_STATUS
ParseHob (
  EFI_GUID* ResultGuid
  )
{
  EFI_STATUS Status;
  VOID* HobList = NULL;
  VOID* Hob = NULL;
  UINTN BufferSize;
  CHAR16* ReportBuffer = NULL;
  EFI_TIME TimeBuffer;
  VOID* ResultBuffer;
  SHELL_FILE_HANDLE ReportFileHandle = NULL;

  ReportFileHandle = Fopen(REPORT_FILE, EFI_FILE_MODE_WRITE);
  if (NULL == ReportFileHandle) {
    Print (L"CANNOT OPEN REPORT FILE!\n");
    return EFI_ABORTED;
  }
  Fseek(ReportFileHandle, (UINT64)-1);

  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  Status = gST->BootServices->AllocatePool (EfiBootServicesData, REPORT_BUFFER_SIZE, (VOID **)&ReportBuffer);
  if (EFI_ERROR (Status)) {
    gST->BootServices->FreePool ((VOID*)ReportBuffer);
    return Status;
  }

  SetMem(ReportBuffer, REPORT_BUFFER_SIZE, 0);
  //CurrentBufferPos = ReportBuffer;
  gST->RuntimeServices->GetTime(&TimeBuffer, NULL);
  UnicodeSPrint (ReportBuffer, REPORT_BUFFER_SIZE, L"    \n%d-%d-%d, %d:%d:%d\n", TimeBuffer.Year, TimeBuffer.Month, TimeBuffer.Day, TimeBuffer.Hour, TimeBuffer.Minute, TimeBuffer.Second);
  //CurrentBufferPos += StrLen(CurrentBufferPos);
  Fwrite(ReportFileHandle, (UINT8*)ReportBuffer, StrLen(ReportBuffer) * sizeof (CHAR16));

  Hob = HobList;
  Hob = GetNextGuidHob (ResultGuid, Hob);
  while (Hob != NULL) {
    ResultBuffer = GET_GUID_HOB_DATA (Hob);
    BufferSize = GET_GUID_HOB_DATA_SIZE (Hob);

    ParseHobData (ResultBuffer, ReportFileHandle, BufferSize);

    Hob = GetNextGuidHob (ResultGuid, Hob);
  }

  if (ReportFileHandle != NULL) {
    Fclose(ReportFileHandle);
  }
  gST->BootServices->FreePool ((VOID*)ReportBuffer);
  return EFI_SUCCESS;
}

EFI_STATUS
ParseHobData (
  VOID             *ResultData,
  SHELL_FILE_HANDLE  ReportFileHandle,
  UINTN            BufferSize
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN len = 0;
  INT8* HobData = (INT8*)ResultData;
  CHAR16 ReportBuffer[REPORT_BUFFER_SIZE];
  CHAR16* Buffer = NULL;

  while (BufferSize >= sizeof(PEI_SCT_RESULT_HOB)) {
    Buffer = ReportBuffer;
    SetMem(Buffer, REPORT_BUFFER_SIZE* sizeof (CHAR16), 0);
    SctGuidToStr (&((PEI_SCT_RESULT_HOB*)HobData)->AssertionGuid, Buffer);
    Buffer += StrLen(Buffer);

    len = UnicodeSPrint(Buffer, REPORT_BUFFER_SIZE, L" data:0x%x ", ((PEI_SCT_RESULT_HOB*)HobData)->ProcedureNumber);
    Buffer += len;

    if (((PEI_SCT_RESULT_HOB*)HobData)->Result == 1) {
      len = UnicodeSPrint(Buffer, REPORT_BUFFER_SIZE, L" PASS\n");
      Buffer += len;
    }
    else {
      len = UnicodeSPrint(Buffer, REPORT_BUFFER_SIZE, L" FAIL!\n");
      Buffer += len;
    }

    Fwrite(ReportFileHandle, (UINT8*)ReportBuffer, StrLen(ReportBuffer) * sizeof (CHAR16));
    HobData += sizeof(PEI_SCT_RESULT_HOB);
    BufferSize -= sizeof(PEI_SCT_RESULT_HOB);
  }

  return Status;
}

EFI_STATUS
GenerateReport (
  VOID
  )
{
  EFI_STATUS Status;
  SHELL_FILE_HANDLE ConfigFileHandle = NULL;
  SCT_ITEM_RECORD ItemRecord;
  INTN Size = 0;

  ConfigFileHandle = Fopen (CHECK_LIST_FILE, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE);
  if (NULL == ConfigFileHandle) {
    Print (L"CANNOT OPEN CONFIGURATION FILE!\n");
    return EFI_ABORTED;
  }
  //
  //look through check list file to find user selection and see whether reboot needed.
  //
  Fseek (ConfigFileHandle, 0);
  while ((Size = Fread(ConfigFileHandle, (UINT8 *)&ItemRecord, sizeof (SCT_ITEM_RECORD))) == sizeof (SCT_ITEM_RECORD)) {

    if ((NO_ITEM_ID == ItemRecord.ItemId) && (ItemRecord.CheckNum > 0) ) {
      Fclose(ConfigFileHandle);
      return EFI_SUCCESS;
    }

  }

  if (ConfigFileHandle != NULL) {
    Fclose(ConfigFileHandle);
      }

      /*normal process*/

  Status = ParseHob(&gPeiSctResultHobGuid);

  return Status;
}

VOID
BurnFd (
  EFI_HANDLE ImageHandle,
  CHAR16* FdPath
  )
{
  EFI_STATUS  ExecuteStatus;
  VOID* CmdLine = NULL;
  SHELL_FILE_HANDLE BurnHandle = NULL;

  if (NULL == FdPath) {
    Print(L"NO FD TO BURN!");
    return;
  }

  BurnHandle = Fopen (BurnUtility, EFI_FILE_MODE_READ);
  if (NULL == BurnHandle) {
    Print(L"CAN'T RUN BURN FD UTILITY!\n");
    return;
  }
  Fclose(BurnHandle);

  CmdLine = AllocatePool((StrLen(BurnUtility)  + StrLen(Option) + StrLen(FdPath) + 2) * sizeof (CHAR16));
  StrCpy ((CHAR16*)CmdLine, BurnUtility);
  StrCat ((CHAR16*)CmdLine, L" ");
  StrCat ((CHAR16*)CmdLine, Option);
  StrCat ((CHAR16*)CmdLine, L" ");
  StrCat ((CHAR16*)CmdLine, FdPath);


  Print (L"\n\n");
  Print (L"Burning %s\n\n", FdPath);

  ShellExecute (
    &ImageHandle,
    CmdLine,
    FALSE,
    NULL,
    &ExecuteStatus
    );

  FreePool (CmdLine);

}


EFI_STATUS
InitManageModule (
  VOID
  )
{
  CHAR16* FileBuffer = NULL;
  CHAR16* tmp = 0;
  CHAR16* QuoteBegin = 0;
  CHAR16* QuoteEnd = 0;
  SHELL_FILE_HANDLE InitConfig = NULL;
  UINTN LenRead = 0;
  UINTN Offset = 0;
  INT8 i = 1;
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN FileSize = 0;

  SetMem(FdLocation, sizeof FdLocation, 0);
  SetMem(BurnUtility, sizeof BurnUtility, 0);
  SetMem(Option, sizeof Option, 0);
  SetMem(RebootNeed, sizeof RebootNeed, 0);

  InitConfig = Fopen(INIT_FILE, EFI_FILE_MODE_READ);
  if (NULL == InitConfig) {
    goto end;
  }

  FileSize = ITEM_NUM_MAX*INPUT_LEN_MAX*2;
  if (FileSize == 0) {
    Fclose(InitConfig);
    goto end;
  }
  Status = gST->BootServices->AllocatePool (EfiBootServicesData, FileSize, (VOID **)&FileBuffer);
  if (EFI_ERROR (Status)) {
    goto end;
  }
  SetMem(FileBuffer, FileSize, 0);
  Fseek(InitConfig, 0);

  /*fetch the whole contents of InitConfig file to FileBuffer, assume the file not too large*/
  LenRead = Fread(InitConfig, (UINT8 *) FileBuffer, FileSize);
  if (LenRead > FileSize) {
    goto end;
  }
  Fclose(InitConfig);

  StrTrim (FileBuffer, ' ');

  /*PICK OUT FLASH UPDATE TOOL*/
  tmp = StrStr(FileBuffer, L"[BurnTool]");
  if (tmp == NULL) {
    goto end;
  }
  tmp = StrStr (tmp + StrLen(L"BurnTool]"), L"\"");
  if (tmp == NULL) {
    goto end;
  }
  tmp++;

  StrnCpy (BurnUtility, tmp, StrStr (tmp, L"\"") - tmp);

  /*PICK OUT THE OPTION FOR THIS TOOL*/
  tmp = StrStr(FileBuffer, L"[Option]");
  if (tmp == NULL) {
    goto end;
  }

  tmp = StrStr (tmp + StrLen(L"Option]"), L"\"");
  if (tmp == NULL) {
    goto end;
  }
  tmp++;

  StrnCpy(Option, tmp, StrStr (tmp, L"\"") - tmp);

  /*PICK OUT THE ARRAY OF FD NAMES FOR FLASH UPDATE*/
  tmp = StrStr(FileBuffer, L"[FdName]");
  if (tmp == NULL) {
    goto end;
  }

  tmp = tmp + StrLen(L"FdName]");
  while(i <= ITEM_NUM_MAX) {

    QuoteBegin = StrStr (tmp, L"\"");
    if (NULL == QuoteBegin) {
      break;
    }
    QuoteEnd = StrStr (QuoteBegin + 1, L"\"");
    if (QuoteEnd - QuoteBegin <= 0) {
      break;
    }

    if (QuoteEnd - QuoteBegin > INPUT_LEN_MAX) {
      Status = EFI_NOT_STARTED;
      Print (L"FD Number %d path too long\n", i);
      break;
    }
    StrnCpy(FdLocation[i], QuoteBegin + 1, QuoteEnd - QuoteBegin -1);
    tmp = QuoteEnd + 1;
    LenRead = StrStr(tmp, L"reboot") - tmp;
    if (LenRead > 0) {
      Offset = StrStr (tmp, L"\"") - tmp;
      if (LenRead < Offset || Offset < 0) {
        RebootNeed[i] = 1;
      }
    }
    i++;
  }

  if (i == ITEM_NUM_MAX + 1) {
    Status = EFI_NOT_STARTED;
    Print (L"Too many FDs. The maximum number is %d", ITEM_NUM_MAX);
  }

  if (FileBuffer != NULL) {
    gST->BootServices->FreePool ((VOID*)FileBuffer);
  }

  return Status;

end:
  Status = EFI_NOT_STARTED;
  if (FileBuffer != NULL) {
    gST->BootServices->FreePool ((VOID*)FileBuffer);
  }

  return Status;

}

SHELL_FILE_HANDLE
Fopen (
  CHAR16    *Name,
  UINT64    Mode
  )
{
  EFI_STATUS         Status;
  SHELL_FILE_HANDLE  ShellFileHandle;

  //
  // Get the device path of file system
  //
  Status = ShellOpenFileByName (Name, &ShellFileHandle, Mode, 0);
  if (EFI_ERROR(Status) || ShellFileHandle == NULL) {
    return NULL;
  }
  return ShellFileHandle;
}

EFI_STATUS
Fremove (
  CHAR16    *Name
  )
{
  EFI_STATUS         Status;
  SHELL_FILE_HANDLE  ShellFileHandle;

  Status = ShellOpenFileByName (Name, &ShellFileHandle, EFI_FILE_MODE_READ, 0);
  if (EFI_ERROR(Status) || ShellFileHandle == NULL) {
    return EFI_NOT_FOUND;
  }
  Status = ShellDeleteFile (&ShellFileHandle);
  return Status;
}

EFI_STATUS
Fclose (
  SHELL_FILE_HANDLE   Handle
  )
{
  return ShellCloseFile (&Handle);
}

UINTN
Fwrite (
  SHELL_FILE_HANDLE   Handle,
  UINT8             *Buffer,
  UINTN             BufferSize
  )
{
  ShellWriteFile (Handle, &BufferSize, Buffer);
  return BufferSize;
}

UINTN
Fread (
  SHELL_FILE_HANDLE   Handle,
  UINT8             *Buffer,
  UINTN             BufferSize
  )
{
  return ShellReadFile (Handle, &BufferSize, Buffer);
}

EFI_STATUS
Fseek (
  SHELL_FILE_HANDLE   Handle,
  UINT64            Position
  )
{
  return ShellSetFilePosition (Handle, Position);
}

EFI_STATUS
Flocate (
  SHELL_FILE_HANDLE   Handle,
  UINT64            *Position
  )
{
  return ShellGetFilePosition (Handle, Position);
}
