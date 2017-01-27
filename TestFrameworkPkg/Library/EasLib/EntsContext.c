/** @file
  Driver to publish the Test Profile Library Protocol.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>
#include <Library/MemoryAllocationLib.h>

#include <Library/EntsLib.h>

#define MAX_FILENAME_LEN                   128
#define MAX_RECORD_LEN                     512

typedef struct _EFI_PASSIVE_TEST_CONTEXT_RECORD {
  CHAR8                                            *Key;
  UINTN                                            Size;
  VOID                                             *Value;
  struct _EFI_PASSIVE_TEST_CONTEXT_RECORD          *Next;
}EFI_PASSIVE_TEST_CONTEXT_RECORD;

typedef struct _EFI_PASSIVE_TEST_CONTEXT {
  EFI_FILE_HANDLE                                  DirHandle;
  EFI_FILE_HANDLE                                  FileHandle;
  CHAR16                                           FileName[MAX_FILENAME_LEN];
  IN UINT64                                        Attributes;
}EFI_PASSIVE_TEST_CONTEXT;

/*-------------------------------------------------------------------------------*/

#define ENTS_CONTEXT_DEBUG

#ifdef ENTS_CONTEXT_DEBUG
STATIC
VOID
_BuffPrint(
  IN CHAR8                                *Buffer,
  IN UINTN                                Size
  )
{
  UINTN                                   Index;

  for(Index = 0; Index < Size; Index++) {
    if ((Index & 0xFF) == 0) {
      EntsPrint(L"\n\t");
    }
    EntsPrint(L"%c", Buffer[Index]);
  }
  EntsPrint(L"\n");
}

STATIC
VOID
_RecordPrint(
  IN EFI_PASSIVE_TEST_CONTEXT_RECORD      *Record
  )
{
  EntsPrint(L"Key:[%a] ValueSize: [%d] Value:", Record->Key, Record->Size);
  _BuffPrint((CHAR8 *)Record->Value, Record->Size);
}


VOID
_RecordListPrint(
  IN EFI_PASSIVE_TEST_CONTEXT_RECORD      *RecordList
  )
{
  EFI_PASSIVE_TEST_CONTEXT_RECORD         *Record;

  if(RecordList == NULL) {
    EntsPrint(L"RecordList is empty\n");
    return ;
  }

  for(Record = RecordList; Record != NULL; Record = Record->Next) {
    _RecordPrint(Record);
  }
}

#define BuffPrint(Buffer, Size)           _BuffPrint((Buffer), (Size))
#define RecordPrint(Record)               _RecordPrint((Record))
#define RecordListPrint(RecordList)       _RecordListPrint((RecordList))
#else
#define BuffPrint(Buffer, Size)
#define RecordPrint(Record)
#define RecordListPrint(RecordList)
#endif

/*-------------------------------------------------------------------------------------*/
STATIC
EFI_STATUS
ContextReopen (
  IN EFI_PASSIVE_TEST_CONTEXT                      *Context
  );

STATIC
EFI_STATUS
ContextOpen (
  IN EFI_DEVICE_PATH_PROTOCOL                      *DevicePath,
  IN CHAR16                                        *FileName,
  IN UINT64                                        Attributes,
  OUT EFI_PASSIVE_TEST_CONTEXT                     **Context
  );

STATIC
EFI_STATUS
DelRecord (
  IN EFI_PASSIVE_TEST_CONTEXT                      *Context,
  IN CHAR8                                         *Key
  );

STATIC
EFI_STATUS
ParseRecordLine (
  IN CHAR8                                 *LineBuf,
  IN OUT EFI_PASSIVE_TEST_CONTEXT_RECORD   *Record
  )
{
  CHAR8                                    *TmpStr;
  CHAR8                                    *KeyBuf;
  VOID                                     *ValueBuf;
  UINTN                                    ValueSize;

  TmpStr  = AsciiStrStr(LineBuf, "|");
  if (TmpStr == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *TmpStr = '\0';
  KeyBuf = (CHAR8 *)AllocateZeroPool((TmpStr - LineBuf + 1));
  if (KeyBuf == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  AsciiStrCpy (KeyBuf, LineBuf);
  TmpStr++;
  CopyMem(&ValueSize, TmpStr, sizeof(UINTN));
  ValueBuf = (VOID *)AllocateZeroPool (ValueSize);
  if (ValueBuf == NULL) {
    FreePool (KeyBuf);
    return EFI_OUT_OF_RESOURCES;
  }
  TmpStr += sizeof(UINTN) + 1;
  CopyMem(ValueBuf, TmpStr, ValueSize);

  Record->Key   = KeyBuf;
  Record->Size  = ValueSize;
  Record->Value = ValueBuf;
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ReadRecordsFromFile (
  IN EFI_PASSIVE_TEST_CONTEXT              *Context,
  OUT EFI_PASSIVE_TEST_CONTEXT_RECORD      **RecordListHead
  )
{
  EFI_STATUS                               Status;
  EFI_FILE_HANDLE                          FileHandle;
  CHAR8                                    Buffer[MAX_RECORD_LEN];
  UINTN                                    BufSize;
  EFI_PASSIVE_TEST_CONTEXT_RECORD          *Record;

  if (RecordListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *RecordListHead = NULL;
  FileHandle       = Context->FileHandle;

  //
  // Read all the records from the file to initialize the record list
  //
  BufSize = MAX_RECORD_LEN;
  Status = FileHandle->Read (FileHandle, &BufSize, Buffer);
  if (EFI_ERROR(Status)) {
    EFI_ENTS_DEBUG((EFI_ENTS_D_ERROR, L"Read file error - %r", Status));
    goto Error1;
  }
  while (BufSize != 0) {
    Record = (EFI_PASSIVE_TEST_CONTEXT_RECORD *)AllocateZeroPool(sizeof(EFI_PASSIVE_TEST_CONTEXT_RECORD));
    if (Record == NULL) {
      goto Error1;
    }

    //
    // Process a line and insert the record into record list
    //
    Status = ParseRecordLine(Buffer, Record);
    if (EFI_ERROR(Status)) {
      FreePool(Record);
      EFI_ENTS_DEBUG((EFI_ENTS_D_ERROR, L"Parse the record line fail - %r", Status));
      goto Error1;
    }

    Record->Next = *RecordListHead;
    *RecordListHead = Record;

#if 0
    for (BufPos = 0; BufPos < BufSize; BufPos++) {
      if (Buffer[BufPos] == '\n') {
        //
        // Make the LineBuf as a string (end with '\0')
        //
        LineBuf[LinePos++] = '\0';

        Record = (EFI_PASSIVE_TEST_CONTEXT_RECORD *)AllocateZeroPool(sizeof(EFI_PASSIVE_TEST_CONTEXT_RECORD));
        if (Record == NULL) {
          goto Error1;
        }
        //
        // Process a line and insert the record into record list
        //
        Status  = ParseRecordLine(LineBuf, LinePos,  Record);
        if (EFI_ERROR(Status)) {
          FreePool(Record);
          EFI_ENTS_DEBUG((EFI_ENTS_D_ERROR, L"Parse the record line %s fail - %r", LineBuf, Status));
          goto Error1;
        }
        Record->Next = *RecordListHead;
    *RecordListHead = Record;

        LinePos = 0;
        continue;
      }
      LineBuf[LinePos++] = Buffer[BufPos];
    }
#endif
    //
    // Read a new buffer
    //
    BufSize = MAX_RECORD_LEN;
    Status = FileHandle->Read (FileHandle, &BufSize, Buffer);
    if (EFI_ERROR(Status)) {
      EFI_ENTS_DEBUG((EFI_ENTS_D_ERROR, L"Read file error - %r", Status));
      goto Error1;
    }
  }

  return EFI_SUCCESS;
Error1:
  //
  // Free the record list and return
  //
  while(*RecordListHead != NULL) {
    Record = *RecordListHead;
  *RecordListHead = (*RecordListHead)->Next;
    FreePool(Record->Key);
  FreePool(Record->Value);
  FreePool(Record);
  }
  return Status;
}

STATIC
EFI_STATUS
WriteRecordsToFile(
  IN EFI_PASSIVE_TEST_CONTEXT              *Context,
  IN EFI_PASSIVE_TEST_CONTEXT_RECORD       *RecordListHead
  )
{
  EFI_STATUS                               Status;
  EFI_FILE_HANDLE                          FileHandle;
  EFI_PASSIVE_TEST_CONTEXT_RECORD          *Record;
  CHAR8                                    Buffer[MAX_RECORD_LEN];
  UINTN                                    BufSize;
  UINTN                                    Index;

  //
  // Delete the old file and create a new one to throw away old records
  //
  Status = ContextReopen(Context);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (RecordListHead == NULL) {
    return EFI_SUCCESS;
  }

  FileHandle = Context->FileHandle;

  for (Record = RecordListHead; Record != NULL; Record = Record->Next) {
    AsciiStrCpy(Buffer, Record->Key);
    Index = AsciiStrLen(Record->Key);
    Buffer[Index++] = '|';
    CopyMem(Buffer + Index, &Record->Size, sizeof(UINTN));
    Index += sizeof(UINTN);
  Buffer[Index++] = '|';
  CopyMem(Buffer + Index, Record->Value, MAX_RECORD_LEN - Index);
    BufSize = MAX_RECORD_LEN;
    Status = FileHandle->Write(FileHandle, &BufSize, Buffer);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

  return FileHandle->Flush(FileHandle);
}

STATIC
EFI_STATUS
SetRecord (
  IN EFI_PASSIVE_TEST_CONTEXT               *Context,
  IN CHAR8                                  *Key,
  IN UINTN                                  RecordSize,
  IN VOID                                   *RecordValue
  )
{
  EFI_STATUS                                Status;
  EFI_PASSIVE_TEST_CONTEXT_RECORD           *RecordListHead;
  EFI_PASSIVE_TEST_CONTEXT_RECORD           *Record;
  CHAR8                                     *KeyBuf;
  VOID                                      *ValueBuf;

  Status = ReadRecordsFromFile(Context, &RecordListHead);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  for (Record = RecordListHead; Record != NULL; Record = Record->Next) {
    if (AsciiStrCmp(Key, Record->Key) == 0) {
      break;
    }
  }

  //
  // Create a new record if does not found otherwise replease the
  // value with new one and then inset the record list
  //
  if (Record == NULL) {
    Record = (EFI_PASSIVE_TEST_CONTEXT_RECORD *)AllocateZeroPool(sizeof(EFI_PASSIVE_TEST_CONTEXT_RECORD));
    if (Record == NULL ) {
      Status = EFI_OUT_OF_RESOURCES;
      goto FreeAndReturn;
    }
    KeyBuf = (CHAR8 *)AllocateZeroPool (AsciiStrLen(Key) + 1);
    if (KeyBuf == NULL) {
      FreePool(Record);
      Status = EFI_OUT_OF_RESOURCES;
      goto FreeAndReturn;
    }
    ValueBuf = (CHAR8 *)AllocateZeroPool (RecordSize);
    if (ValueBuf == NULL) {
      FreePool(Record);
      FreePool(KeyBuf);
      Status = EFI_OUT_OF_RESOURCES;
      goto FreeAndReturn;
    }
    AsciiStrCpy(KeyBuf, Key);
  CopyMem(ValueBuf, RecordValue, RecordSize);
    Record->Key    = KeyBuf;
    Record->Size   = RecordSize;
  Record->Value  = ValueBuf;
    Record->Next   = RecordListHead;
    RecordListHead = Record;
  } else {
    ValueBuf = (VOID *)AllocateZeroPool (RecordSize);
    if (ValueBuf == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto FreeAndReturn;
    }
    FreePool(Record->Value);
  CopyMem(ValueBuf, RecordValue, RecordSize);
  Record->Value = ValueBuf;
  Record->Size  = RecordSize;
  }

  //
  // Write all the records to file
  //
  Status = WriteRecordsToFile(Context, RecordListHead);
  if (EFI_ERROR(Status)) {
    EFI_ENTS_DEBUG((EFI_ENTS_D_ERROR, L"WriteRecordsToFile fail - %r", Status));
  }
FreeAndReturn:
  //
  // Free the record list and return
  //
  while(RecordListHead != NULL) {
    Record = RecordListHead;
  RecordListHead = RecordListHead->Next;
    FreePool(Record->Key);
  FreePool(Record->Value);
  FreePool(Record);
  }
  return Status;
}

STATIC
EFI_STATUS
GetRecord (
  IN EFI_PASSIVE_TEST_CONTEXT               *Context,
  IN CHAR8                                  *Key,
  IN UINTN                                  *BufSize,
  OUT VOID                                  *RecordBuf
  )
{
  EFI_STATUS                                Status;
  EFI_PASSIVE_TEST_CONTEXT_RECORD           *RecordListHead;
  EFI_PASSIVE_TEST_CONTEXT_RECORD           *Record;

  Status = ReadRecordsFromFile(Context, &RecordListHead);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Search the record with key in the record line of the contextcontainer. Notes:
  // the records in the list are same as the ones in file
  //
  for (Record = RecordListHead; Record != NULL; Record = Record->Next) {
    if (AsciiStrCmp(Record->Key, Key) == 0) {
      break;
    }
  }

  if (Record == NULL) {
    Status = EFI_NOT_FOUND;
    goto FreeAndReturn;
  }

  if (*BufSize > Record->Size) {
    *BufSize = Record->Size;
  }
  CopyMem(RecordBuf, Record->Value, *BufSize);
  Status = EFI_SUCCESS;

FreeAndReturn:
  //
  // Free the record list and return
  //
  while(RecordListHead != NULL) {
    Record = RecordListHead;
  RecordListHead = RecordListHead->Next;
    FreePool(Record->Key);
  FreePool(Record->Value);
  FreePool(Record);
  }
  return Status;
}

STATIC
EFI_STATUS
DelRecord (
  IN EFI_PASSIVE_TEST_CONTEXT                      *Context,
  IN CHAR8                                         *Key
  )
{
  EFI_STATUS                                       Status;
  EFI_PASSIVE_TEST_CONTEXT_RECORD                  *RecordListHead;
  EFI_PASSIVE_TEST_CONTEXT_RECORD                  *Record;
  EFI_PASSIVE_TEST_CONTEXT_RECORD                  *Tmp;

  Status = ReadRecordsFromFile(Context, &RecordListHead);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (RecordListHead == NULL) {
    return EFI_NOT_FOUND;
  } else {
    if (AsciiStrCmp(Key, RecordListHead->Key) == 0) {
      Record = RecordListHead;
    RecordListHead = RecordListHead->Next;
    } else {
      for (Record = RecordListHead; Record->Next != NULL; Record = Record->Next) {
        if (AsciiStrCmp(Key, Record->Next->Key) == 0) {
          break;
        }
      }
      if(Record->Next == NULL) {
        Status = EFI_NOT_FOUND;
        goto FreeAndReturn;
      } else {
        Tmp = Record->Next;
        Record->Next = Record->Next->Next;
        FreePool(Tmp->Key);
        FreePool(Tmp->Value);
        FreePool(Tmp);
      }
    }
  }

  //
  // Write the remain records to file
  //
  Status = WriteRecordsToFile(Context, RecordListHead);
  if (EFI_ERROR(Status)) {
    EFI_ENTS_DEBUG((EFI_ENTS_D_WARNING, L"WriteRecordsToFile fail - %r", Status));
  }
FreeAndReturn:
  //
  // Free the record list and return
  //
  while(RecordListHead != NULL) {
    Record = RecordListHead;
  RecordListHead = RecordListHead->Next;
    FreePool(Record->Key);
  FreePool(Record->Value);
  FreePool(Record);
  }
  return Status;
}

STATIC
EFI_STATUS
ContextReopen (
  IN EFI_PASSIVE_TEST_CONTEXT                      *Context
  )
{
  EFI_STATUS                                       Status;

  Status = Context->FileHandle->Close(Context->FileHandle);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  Status = Context->DirHandle->Open (
                                 Context->DirHandle,
                                 &Context->FileHandle,
                                 Context->FileName,
                                 Context->Attributes,
                                 0
                                 );
  if (EFI_ERROR(Status)) {
    Context->FileHandle = NULL;
    return Status;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ContextOpen (
  IN EFI_DEVICE_PATH_PROTOCOL                      *DevicePath,
  IN CHAR16                                        *FileName,
  IN UINT64                                        Attributes,
  OUT EFI_PASSIVE_TEST_CONTEXT                     **Context
  )
{
  EFI_STATUS                                       Status;
  EFI_HANDLE                                       DeviceHandle;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL                  *Vol;
  EFI_FILE_HANDLE                                  RootDir;
  EFI_FILE_HANDLE                                  FileHandle;

  //
  // Check the parameter
  //
  if (Context == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Context = (EFI_PASSIVE_TEST_CONTEXT *)AllocateZeroPool(sizeof(EFI_PASSIVE_TEST_CONTEXT));
  if (*Context == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Open the file if not exist create a new
  //
  Status = gntBS->LocateDevicePath (
                    &gEfiSimpleFileSystemProtocolGuid,
                    &DevicePath,
                    &DeviceHandle
                    );
  if (EFI_ERROR(Status)) {
    goto ContextOpenError;
  }

  Status = gntBS->HandleProtocol (
                    DeviceHandle,
                    &gEfiSimpleFileSystemProtocolGuid,
                    (VOID *)&Vol
                    );
  if (EFI_ERROR(Status)) {
    goto ContextOpenError;
  }

  Status = Vol->OpenVolume (Vol, &RootDir);
  if (EFI_ERROR (Status)) {
    goto ContextOpenError;
  }

  Status = RootDir->Open (
                      RootDir,
                      &FileHandle,
                      FileName,
                      Attributes,
                      0
                      );
  if (EFI_ERROR(Status)) {
    RootDir->Close(RootDir);
    goto ContextOpenError;
  }

  (*Context)->DirHandle    = RootDir;
  (*Context)->FileHandle   = FileHandle;
  StrCpy((*Context)->FileName, FileName);
  (*Context)->Attributes   = Attributes;

  return EFI_SUCCESS;
ContextOpenError:
  FreePool(*Context);
  return Status;
}

STATIC
VOID
ContextClose (
  IN EFI_PASSIVE_TEST_CONTEXT                      *Context
  )
{
  if (Context == NULL) {
    return ;
  }

  if (Context->FileHandle != NULL) {
    Context->FileHandle->Close(Context->FileHandle);
  }

  if (Context->DirHandle != NULL) {
    Context->DirHandle->Close(Context->DirHandle);
  }

  FreePool(Context);

  return ;
}

EFI_STATUS
PassiveTestContextCreate (
  IN EFI_DEVICE_PATH_PROTOCOL                      *DevicePath,
  IN CHAR16                                        *FileName
  )
{
  EFI_STATUS                                       Status;
  EFI_HANDLE                                       DeviceHandle;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL                  *Vol;
  EFI_FILE_HANDLE                                  FileHandle;
  EFI_FILE_HANDLE                                  RootDir;

  //
  // Check the parameter
  //
  if ((DevicePath == NULL) || (FileName == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  if (StrLen(FileName) > MAX_FILENAME_LEN) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Open the file if not exist create a new
  //
  Status = gntBS->LocateDevicePath (
                    &gEfiSimpleFileSystemProtocolGuid,
                    &DevicePath,
                    &DeviceHandle
                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gntBS->HandleProtocol (
                    DeviceHandle,
                    &gEfiSimpleFileSystemProtocolGuid,
                    (VOID *)&Vol
                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = Vol->OpenVolume (Vol, &RootDir);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Delete it first if the file exist
  //
  Status = RootDir->Open (
                      RootDir,
                      &FileHandle,
                      FileName,
                      EFI_FILE_MODE_WRITE|EFI_FILE_MODE_READ,
                      0
                      );
  if (!EFI_ERROR(Status)) {
    FileHandle->Delete(FileHandle);
  }

  //
  // Create a new file
  //
  Status = RootDir->Open (
                      RootDir,
                      &FileHandle,
                      FileName,
                      EFI_FILE_MODE_CREATE|EFI_FILE_MODE_WRITE|EFI_FILE_MODE_READ,
                      0
                      );
  if (EFI_ERROR(Status)) {
    RootDir->Close(RootDir);
    return Status;
  }

  //
  // Close the file and return
  //
  FileHandle->Close (FileHandle);
  RootDir->Close (RootDir);
  return EFI_SUCCESS;
}

EFI_STATUS
PassiveTestContextDelete (
  IN EFI_DEVICE_PATH_PROTOCOL                      *DevicePath,
  IN CHAR16                                        *FileName
  )
{
  EFI_STATUS                                       Status;
  EFI_HANDLE                                       DeviceHandle;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL                  *Vol;
  EFI_FILE_HANDLE                                  FileHandle;
  EFI_FILE_HANDLE                                  RootDir;

  //
  // Check the parameter
  //
  if ((DevicePath == NULL) || (FileName == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  if (StrLen(FileName) > MAX_FILENAME_LEN) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Open the file if not exist create a new
  //
  Status = gntBS->LocateDevicePath (
                    &gEfiSimpleFileSystemProtocolGuid,
                    &DevicePath,
                    &DeviceHandle
                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gntBS->HandleProtocol (
                    DeviceHandle,
                    &gEfiSimpleFileSystemProtocolGuid,
                    (VOID *)&Vol
                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = Vol->OpenVolume (Vol, &RootDir);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = RootDir->Open (
                      RootDir,
                      &FileHandle,
                      FileName,
                      EFI_FILE_MODE_WRITE|EFI_FILE_MODE_READ,
                      0
                      );
  if (EFI_ERROR(Status)) {
    RootDir->Close(RootDir);
    return Status;
  }

  FileHandle->Delete(FileHandle);
  RootDir->Close(RootDir);
  return EFI_SUCCESS;
}

EFI_STATUS
SetContextRecord (
  IN EFI_DEVICE_PATH_PROTOCOL                  *DevicePath,
  IN CHAR16                                    *FileName,
  IN CHAR16                                    *Key,
  IN UINTN                                     Size,
  IN VOID                                      *Value
  )
{
  EFI_STATUS                                   Status;
  EFI_PASSIVE_TEST_CONTEXT                     *Context;
  CHAR8                                        AsciiKey[MAX_RECORD_LEN];

  //
  // Check the pararmeters.
  //
  if ((Key == NULL) || (DevicePath == NULL) || (FileName == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  if ((Size != 0) && (Value == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  if (StrLen(FileName) > MAX_FILENAME_LEN) {
    return EFI_INVALID_PARAMETER;
  }

  Unicode2Ascii(AsciiKey, Key);
  if (AsciiStrLen(AsciiKey) + Size + 1 > MAX_RECORD_LEN) {
    return EFI_INVALID_PARAMETER;
  }

  if (AsciiStrStr(AsciiKey, "=") != NULL) {
    EFI_ENTS_DEBUG((EFI_ENTS_D_ERROR, L"The KEY of record can not contain ="));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Open the file
  //
  Status = ContextOpen (
             DevicePath,
             FileName,
             EFI_FILE_MODE_WRITE|EFI_FILE_MODE_READ,
             &Context
             );
  if (EFI_ERROR(Status)) {
    EFI_ENTS_DEBUG((EFI_ENTS_D_WARNING, L"SetContextRecord: ContextOpen %s Fail - %r", FileName, Status));
    return Status;
  }

  EFI_ENTS_DEBUG((EFI_ENTS_D_TRACE, L"SetContextRecord: ContextOpen %s Success", FileName));

  //
  // Record string.
  //
  if (Size != 0) {
    EFI_ENTS_DEBUG((EFI_ENTS_D_TRACE, L"Do SetRecord"));
    Status = SetRecord(
               Context,
               AsciiKey,
               Size,
               Value
               );
    if(EFI_ERROR(Status)) {
      EFI_ENTS_DEBUG((EFI_ENTS_D_ERROR, L"SetContextRecord: SetRecord Key:%s Value:%s - %r", Key, Value, Status));
    }
  } else {
    EFI_ENTS_DEBUG((EFI_ENTS_D_TRACE, L"Do DelRecord"));
    Status = DelRecord(
               Context,
               AsciiKey
               );
    if(EFI_ERROR(Status)) {
      EFI_ENTS_DEBUG((EFI_ENTS_D_WARNING, L"SetContextRecord: DelRecord Key:%s - %r", Key, Status));
    }
  }

  ContextClose(Context);
  return Status;
}

EFI_STATUS
GetContextRecord (
  IN EFI_DEVICE_PATH_PROTOCOL                  *DevicePath,
  IN CHAR16                                    *FileName,
  IN CHAR16                                    *Key,
  IN UINTN                                     *BufSize,
  OUT VOID                                     *RecordBuf
  )
{
  EFI_STATUS                                   Status;
  EFI_PASSIVE_TEST_CONTEXT                     *Context;
  CHAR8                                        AsciiKey[MAX_RECORD_LEN];

  if ((DevicePath == NULL) || (FileName == NULL) ||
      (Key == NULL) || (RecordBuf == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  if (StrLen(FileName) > MAX_FILENAME_LEN) {
    return EFI_INVALID_PARAMETER;
  }

  Unicode2Ascii(AsciiKey, Key);

  //
  // Open the file
  //
  Status = ContextOpen (
             DevicePath,
             FileName,
             EFI_FILE_MODE_READ,
             &Context
             );
  if (EFI_ERROR(Status)) {
    EFI_ENTS_DEBUG((EFI_ENTS_D_WARNING, L"GetContextRecord: ContextOpen %s Fail - %r", FileName, Status));
    return Status;
  }

  Status = GetRecord(
             Context,
             AsciiKey,
             BufSize,
             RecordBuf
             );
  if (EFI_ERROR(Status)) {
    EFI_ENTS_DEBUG((EFI_ENTS_D_WARNING, L"GetContextRecord: GetRecord Key:%s - %r", Key, Status));
    ContextClose(Context);
  return Status;
  }

  ContextClose(Context);
  return EFI_SUCCESS;
}
