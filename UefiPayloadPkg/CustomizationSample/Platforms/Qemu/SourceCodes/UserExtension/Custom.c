/** @file
  This is the file for user extension codes. (A sample is provided here)

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "UserExtensionLib.h"

//
// The USB Controller PCI address for BXT
//
#define PCI_BUS_NUMBER_ICH                0x00  ///< ICH is on PCI Bus 0.
#define PCI_DEVICE_NUMBER_ICH_LPC           31  ///< ICH is Device 31.
#define PCI_FUNCTION_NUMBER_ICH_LPC          0  ///< ICH is Function 0.

#define MAX_STRING_LENGTH                  100
#define MAX_FILE_PATH_LENGTH               260

//
// The global definitions
//
UINT32  EventCount;
UINT8   NotifyContext[]={0x5,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}; 

// to-do: use macro defined in ConsoleInOut.h instead?

EFI_GRAPHICS_OUTPUT_BLT_PIXEL        mGraphicsEfiColors[16] = {
  //
  // B    G    R   reserved
  //
  { 0x00, 0x00, 0x00, 0x00 },  // BLACK
  { 0x98, 0x00, 0x00, 0x00 },  // LIGHTBLUE
  { 0x00, 0x98, 0x00, 0x00 },  // LIGHGREEN
  { 0x98, 0x98, 0x00, 0x00 },  // LIGHCYAN
  { 0x00, 0x00, 0x98, 0x00 },  // LIGHRED
  { 0x98, 0x00, 0x98, 0x00 },  // MAGENTA
  { 0x00, 0x98, 0x98, 0x00 },  // BROWN
  { 0x98, 0x98, 0x98, 0x00 },  // LIGHTGRAY
  { 0x30, 0x30, 0x30, 0x00 },  // DARKGRAY - BRIGHT BLACK
  { 0xff, 0x00, 0x00, 0x00 },  // BLUE
  { 0x00, 0xff, 0x00, 0x00 },  // LIME
  { 0xff, 0xff, 0x00, 0x00 },  // CYAN
  { 0x00, 0x00, 0xff, 0x00 },  // RED
  { 0xff, 0x00, 0xff, 0x00 },  // FUCHSIA
  { 0x00, 0xff, 0xff, 0x00 },  // YELLOW
  { 0xff, 0xff, 0xff, 0x00 }   // WHITE
};

/** 
  This is customer timer event Function which runs at time events triggered. 
  
  @param Event          EFI Event created by CreateTimerEvent.
  @param Context        The Pointer of input Parameters for NotifyFunc.
                        It will be passed by CreateTimerEvent.
                        
  @return               No.

**/
VOID
EFIAPI
NotifyFunc (
  IN VOID    *Event,
  IN VOID    *Context
  )
{
  if (*((UINT8*)Context) == 1) {
    Print(L"%s\n", L"---------------TEST FOR TIMER EVENT_1-------------------\n");
    DEBUG((EFI_D_INFO, " ============ TEST FOR TIMER EVENT_1 ===========\n"));
    EventCount++;
  } else if (*((UINT8*)Context) == 5){
    Print(L"%s\n", L"---------------TEST FOR TIMER EVENT_5-------------------\n");
    Print(L"Repeat time = %x\n", EventCount);
    DEBUG((EFI_D_INFO, " ============ TEST FOR TIMER EVENT_5 ===========\n"));
    EventCount++;
  } else {
    Print(L"%s\n", L"---------------TEST FOR TIMER EVENT-------------------\n");
    DEBUG((EFI_D_INFO, " ============ TEST FOR TIMER EVENT ===========\n"));   
    EventCount++;
  }
  if (EventCount >= *((UINT8*)Context)) {
    CancelTimerEvent(Event);
    EventCount = 0;
  }
}

/** 
  Sample codes that interact with user to perform various tasks 

  @param IN        None
  @return          None

**/
VOID
UserInteractions (
  VOID
  )
{
  VOID                          *BltData;
  UINTN                         BltSize;
  UINT8                         *ImageData;
  UINTN                         ImageSize;
  UINTN                         Height;
  UINTN                         Width;
  UINTN                         *FileBuffer;
  EFI_STATUS                    Status;
  CHAR16                        writeBuffer[] = L"Test Progress Bar - wait seconds";  
  CHAR16                        *GetLineString;
  CHAR16                        *MediaPartition = NULL;
  CHAR16                        *FileString = NULL;
  CHAR16                        FileString_Save[MAX_FILE_PATH_LENGTH];
  CHAR16                        *FolderString;
  CHAR16                        FolderString_Save[MAX_FILE_PATH_LENGTH];
  UINT64                        BufferSize = 0;
  UINT64                        ParBufferSize = 0;
  UINTN                         i;
  EFI_GUID                      FileID_0 = Custom_File_0;
  EFI_GUID                      FileID_2 = Custom_File_2;
  UINTN                         *ParBuffer;
  UINTN                         ParPathNum;
  UINT32                        VenderInfo;
  EFI_INPUT_KEY                 InputKey;
  
  EFI_EVENT                     TimerEvent;
  UINT64                        TriggerTime = 6000000;//0.02ms
  EFI_TIMER_DELAY               Type = TimerPeriodic;
  
  DEBUG((EFI_D_INFO, " ============ UserInteractions start ===========\n"));
  ClearScreen();

  Print(L"%s\n\n", L"==========================UserInteractions TEST========================\n");
  
  //
  // hardware register access
  //
  Print(L"%s\n\n", L"Do you want to show USB PCI controller ID? This test is for hardware registers access. \n\r'yes' to run, other key to bypass\n");
  GetLineString = GetNextLine(MAX_STRING_LENGTH);
  Print(L"Your input is: %s\n\n\n", GetLineString);
  if(StrCmp (GetLineString, L"") == 0) {
    FreePool(GetLineString);
    return;
  }
  if((StrCmp (GetLineString, L"yes") == 0) || (StrCmp (GetLineString, L"YES") == 0) ) {
    VenderInfo = PciRead32(PCI_LIB_ADDRESS(PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_LPC, PCI_FUNCTION_NUMBER_ICH_LPC, 0));
    Print(L"USB controller VenderInfo = 0x%x\n", VenderInfo);
    Print(L"\n\n%s\n", L"Press Esc/Enter Key to continue......\n");
    Status = ReadKeyStrokes(&InputKey);
    ASSERT_EFI_ERROR(Status);
  }
  ClearScreen();
  FreePool(GetLineString);

  //
  // file system access
  //
  Print(L"%s\n\n", L"Do you want to browse a specific directory? This test is for media file system access. \n\r'yes' to run, other key to bypass\n");
  GetLineString = GetNextLine(MAX_STRING_LENGTH);
  Print(L"Your input is: %s\n\n\n", GetLineString);
  if((StrCmp (GetLineString, L"yes") == 0) || (StrCmp (GetLineString, L"YES") == 0) ){ 
    Print(L"Select the disk volume (example: 0,1...): \n\n");
    FreePool(GetLineString);

    ParBuffer = GetAllPartitionPaths(&ParBufferSize);
    if (ParBuffer == NULL) {
       Print(L"%s\n", L"No partition found");
    } else {
      for (i = 0; i < ParBufferSize / sizeof(UINTN); i++) {
        Print(L"%d --- %s\n", i, (CHAR16*) (ParBuffer[i]));
      }
      GetLineString = GetNextLine(MAX_STRING_LENGTH);

      Print(L"Your input is: %s\n", GetLineString);
      ParPathNum = StrDecimalToUintn(GetLineString);
      FreePool(GetLineString);
      if (ParPathNum < ParBufferSize / sizeof(UINTN)) {
        MediaPartition = (CHAR16*)(ParBuffer[ParPathNum]);
        Print(L"Your selected partition is: %s\n", MediaPartition);
        Print(L"Enter the Directory Path(example- \\test\\): \n\n");
        GetLineString = GetNextLine(MAX_STRING_LENGTH);
        Print(L"Your input is: %s\n\n", GetLineString);
        Print(L"Showing the contents of the directory: \n");
        FileBuffer = ReadDirectoryFromDevice(MediaPartition, GetLineString, &BufferSize);
        FreePool(GetLineString);
        if (FileBuffer != NULL) {
          for (i = 0; i < BufferSize / sizeof(UINTN); i++) {
            Print(L"%s\n", ((EFI_FILE_INFO*)*(FileBuffer + i))->FileName);
            FreePool ((VOID*)*(FileBuffer + i));
          }
          FreePool (FileBuffer);
        }
      } else {
        Print(L"You entered a wrong device path. Bypassing the test\n\n");
      }
      for (i = 0; i < ParBufferSize / sizeof(UINTN); i++) {
        FreePool((CHAR16*)(ParBuffer[i]));
      }
      FreePool(ParBuffer);
    }
    Print(L"\n\n%s\n", L"Press Esc/Enter Key to continue......\n");
    Status = ReadKeyStrokes(&InputKey);
    ASSERT_EFI_ERROR(Status);
  } else { 
    FreePool(GetLineString);
  }
  ClearScreen();

  //
  // flash file access and graphical display
  //
  Print(L"%s\n\n", L"Do you want to show Logo? This test is for flash access, file access, and bitmap display.  \n\r'yes' to run, other key to bypass\n");
  GetLineString = GetNextLine(MAX_STRING_LENGTH);
  Print(L"Your input is: %s\n\n\n", GetLineString);
  if((StrCmp (GetLineString, L"yes") == 0) || (StrCmp (GetLineString, L"YES") == 0) ) {
    FreePool(GetLineString);
    DEBUG((EFI_D_INFO, " FileID_0 = 0x%llX\n", FileID_0));
    Status = GetFileFromFlash(&FileID_0,(VOID **)&ImageData, &ImageSize);
    if (EFI_ERROR(Status)) {
      Print(L"Cannot get file from flash, exiting ... \n\n");
      return;
    }
    Print(L"Select the disk volume (example: 0,1...): \n\n");

    ParBuffer = GetAllPartitionPaths(&ParBufferSize);
    if (ParBuffer != NULL) {
      for (i = 0; i < ParBufferSize / sizeof(UINTN); i++) {
        Print(L"%d --- %s\n", i, (CHAR16*)(ParBuffer[i]));
      }
      GetLineString = GetNextLine(MAX_STRING_LENGTH);

      Print(L"Your input is: %s\n", GetLineString);
      ParPathNum = StrDecimalToUintn(GetLineString);
      FreePool(GetLineString);
      if (ParPathNum < ParBufferSize / sizeof(UINTN)) {
        MediaPartition = (CHAR16*)(ParBuffer[ParPathNum]);
        Print(L"Your selected partition is: %s\n", MediaPartition);
    
        Print(L"Enter the Directory Path(example- \\test\\): \n\n");
        FolderString = GetNextLine(MAX_STRING_LENGTH);
        Print(L"Your input is: %s\n\n", FolderString);
        StrCpyS(FolderString_Save, sizeof(FolderString_Save) / sizeof(CHAR16), FolderString);
        Print(L"Enter the File name(example- Logo.bmp): \n");
        FileString = GetNextLine(MAX_STRING_LENGTH);
        Print(L"Your input is: %s\n\n", FileString);
        StrCpyS(FileString_Save, sizeof(FileString_Save) / sizeof(CHAR16), FolderString);
        StrCatS(FileString_Save, (sizeof(FileString_Save) + sizeof(FileString)) / sizeof(CHAR16) - 1, FileString);
        Status = CreateDirectory(MediaPartition, FolderString_Save);
        if (!EFI_ERROR(Status)) {
          Print(L"Directory is created: %s\n\n", FolderString_Save);
        }  else {
          Print(L"Fail to create Directory: %s\n\n", FolderString_Save);
        }
        Print(L"%s\n", L"Press Esc/Enter Key to continue......\n");
        Status = ReadKeyStrokes(&InputKey);
        ASSERT_EFI_ERROR(Status);
        Print(L"Creating file: %s\n", FileString_Save);
        Status = WriteToDevice(MediaPartition, FileString_Save, (UINT64)ImageSize, (VOID*)ImageData, 0x0);
        if (!EFI_ERROR(Status)) {
          Print(L"Logo file is saved as: %s\n\n", FileString_Save);
          Print(L"%s\n", L"Press Esc/Enter Key to continue......\n");
          Status = ReadKeyStrokes(&InputKey);
          ASSERT_EFI_ERROR(Status);
          BufferSize = 0;
          FileBuffer = ReadFromDevice(MediaPartition, FileString_Save, 0x0, &BufferSize);
          BltData = NULL;
          Status = ConvertBmpToGopBlt(
            FileBuffer,
            ImageSize,
            (VOID **)&BltData,
            &BltSize,
            &Height,
            &Width
          );
          if (!EFI_ERROR(Status)) {
            ClearScreen();
            Status = ShowBltLogo(BltData, Height, Width);
          }
        }  else {
          Print(L"Fail to save logo file as: %s\n\n", FileString);
        }
        for (i = 0; i < ParBufferSize / sizeof(UINTN); i++) {
          FreePool((CHAR16*)(ParBuffer[i]));
        }
        FreePool(ParBuffer);
      }  else {
        Print(L"You entered a wrong device path. The test is bypassed\n\n");
      }
      
    }
    Print(L"%s\n", L"Press Esc/Enter Key to continue......\n");
    Status = ReadKeyStrokes(&InputKey);
    ASSERT_EFI_ERROR(Status);
  } else {
    FreePool(GetLineString);
  }
  ClearScreen();

  Print(L"%s\n\n", L"Do you want to delete logo directory? \n\r'yes' to run, other key to bypass\n");
  GetLineString = GetNextLine(MAX_STRING_LENGTH);
  Print(L"Your input is: %s\n\n\n", GetLineString);
  if((StrCmp (GetLineString, L"yes") == 0) || (StrCmp (GetLineString, L"YES") == 0) ){
    FreePool(GetLineString);
    Status = DeleteFileOnDevice(MediaPartition, FileString_Save);
    if (!EFI_ERROR (Status)) {
      Status = DeleteFileOnDevice(MediaPartition, FolderString_Save);
    }
    if (!EFI_ERROR (Status)) {
      Print(L"%s\n\n\n", L"The directories are deleted\n");
    } else {
      Print(L"%s\n\n\n", L"Can't delete the directories\n");
    }
    Print(L"%s\n", L"Press Esc/Enter Key to continue......\n");
    Status = ReadKeyStrokes(&InputKey);
    ASSERT_EFI_ERROR(Status);
  } else {
    FreePool(GetLineString);
  }
  ClearScreen();

  //
  // Flash file access
  //
  Print(L"%s\n\n", L"Do you want to show default image content? this test is for Flash file read. \n\r'yes' to run, other key to bypass\n");
  GetLineString = GetNextLine(MAX_STRING_LENGTH);
  Print(L"your input is: %s\n\n\n", GetLineString);
  if((StrCmp (GetLineString, L"yes") == 0) || (StrCmp (GetLineString, L"YES") == 0) ){
    FreePool(GetLineString);
    DEBUG((EFI_D_INFO, " FileID_2 = 0x%llX\n", FileID_2));
    Status = GetFileFromFlash(&FileID_2,(VOID **)&ImageData, &ImageSize);
    if (EFI_ERROR(Status)) {
      Print(L"Cannot get file from flash, exiting ... \n\n");
      return;
    }
    Print(L"FileID_2 image content: %a\n\n\n", ImageData);
    Print(L"%s\n", L"Press Esc/Enter Key to continue......\n");
    Status = ReadKeyStrokes(&InputKey);
    ASSERT_EFI_ERROR(Status);
  } else {
    FreePool(GetLineString);
  }
  ClearScreen();

  //
  // Timer event
  //
  Print(L"%s\n\n", L"Do you want to create a timer event? this test is for timer event and show progress bar.  \n\r'yes' to run, other key to bypass\n");
  GetLineString = GetNextLine(MAX_STRING_LENGTH);
  Print(L"Your input is: %s\n\n\n", GetLineString);
  if ((StrCmp(GetLineString, L"yes") == 0) || (StrCmp(GetLineString, L"YES") == 0)) {
    FreePool(GetLineString);
    EventCount = 0;
    Status = CreateTimerEvent(Type, TriggerTime, NotifyFunc, (VOID*)NotifyContext, &TimerEvent);
    for (i = 1; i < 100; i++) {
      Status = ShowProgress(mGraphicsEfiColors[7], mGraphicsEfiColors[9], writeBuffer, mGraphicsEfiColors[14], i, i - 1);
      MicroSecondDelay(40000);
    }
    Print(L"\n\n%s\n", L"Press Esc/Enter Key to continue......\n");
    Status = ReadKeyStrokes(&InputKey);
    ASSERT_EFI_ERROR(Status);
  } else {
    FreePool(GetLineString);
  }
  ClearScreen();

  DEBUG((EFI_D_INFO, " ============ UserInteractions end ===========\n"));  
}

/** 
  This is the main entry of user extension codes run at the boot time 

**/
VOID
UserExtension (
  VOID
  )
{

  EFI_STATUS      Status;
  UINT16          OptionalData[] = {L'-',L'd',L'e',L'l',L'a',L'y',L'\0',L'3',L'\0'};
  UINT32          OptionalDataSize = 18;  
  CHAR16          *MediaPartition;
  UINTN           *ParBuffer;
  UINTN           ParPathNum;
  UINTN           i;
  CHAR16          *GetLineString;
  UINT64          ParBufferSize = 0;
  EFI_INPUT_KEY   InputKey;

  DEBUG((EFI_D_INFO, " ============ User Extension Test start ===========\n"));
  ClearScreen();

  Print(L"%s\n\n", L" ============ User Extension Sample ============\n\n\r Press 'ESC' to bypass, 'Yes' to continue ...\n");
  GetLineString = GetNextLine(MAX_STRING_LENGTH);
  if(StrCmp (GetLineString, L"") == 0) {
    FreePool(GetLineString);
    return;
  }

  //
  // Call user interaction sample codes
  //
  UserInteractions ();

  //
  // Attempt a boot from user selected media
  //
  DEBUG((EFI_D_INFO, " ============ CustomerHookReadyToBoot start ===========\n"));  
  
  Print(L"%s\n\n", L"Do you want to run custom image to boot? \n\r'yes' for run, other key bypass\n");
  GetLineString = GetNextLine(MAX_STRING_LENGTH);
  Print(L"your input is: %s\n\n\n", GetLineString);
  if ((StrCmp(GetLineString, L"yes") == 0) || (StrCmp(GetLineString, L"YES") == 0)) {
    Print(L"Select the disk volume (example: 0,1...): \n\n");
    FreePool(GetLineString);
    ParBuffer = GetAllPartitionPaths(&ParBufferSize);
    if (ParBuffer == NULL) {
      Print(L"%s\n", L"No partition found");
      return;
    }
    for (i = 0; i < ParBufferSize / sizeof(UINTN); i++) {
      Print(L"%d --- %s\n", i, (CHAR16*)(ParBuffer[i]));
    }
    GetLineString = GetNextLine(MAX_STRING_LENGTH);

    Print(L"your input is: %s\n", GetLineString);
    ParPathNum = StrDecimalToUintn(GetLineString);
    FreePool(GetLineString);
    if (ParPathNum < ParBufferSize / sizeof(UINTN)) {
      MediaPartition = (CHAR16*)(ParBuffer[ParPathNum]);
      Print(L"Your selected Partition is: %s\n", MediaPartition);

      Status = RunFromEfiPeFile(MediaPartition, L"\\EFI\\BOOT\\BOOTX64.EFI", OptionalData, OptionalDataSize);
      DEBUG((DEBUG_INFO, "0x%x\n", Status));
      Print(L"%s\n", L"Press Esc/Enter Key to continue......\n");
      Status = ReadKeyStrokes(&InputKey);
    } else {
      Print(L"You enter the wrong device path. Bypassing the test \n\n");
      Print(L"%s\n", L"Press Esc/Enter Key to continue......\n");
      Status = ReadKeyStrokes(&InputKey);
      ASSERT_EFI_ERROR(Status);
    }
    for (i = 0; i < ParBufferSize / sizeof(UINTN); i++) {
      FreePool((CHAR16*)(ParBuffer[i]));
    }
    FreePool(ParBuffer);

  } else {
    FreePool(GetLineString);
  }
  ClearScreen();
  Print(L"%s\n", L"CustomerHookReadyToBoot End, Continue POST......\n");
  DEBUG((EFI_D_INFO, " ============ CustomerHookReadyToBoot end ===========\n"));

}
