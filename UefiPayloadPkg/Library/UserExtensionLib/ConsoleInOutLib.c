/** @file
  This file includes Console in/out service functions  

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/UserExtensionApi/ConsoleInOut.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextOut.h>

/**
  Implements function to clear screen.

  @param  None  

  @retval None

**/
VOID
ClearScreen(
  VOID
  )
{

  gST->ConOut->ClearScreen(gST->ConOut);
  
}

/**
  Implements function to wait and read key stroke by user.

  @param  Key                 A pointer to a buffer that is filled in with the
                              keystroke information for the key that was sent
                              from terminal.

  @retval EFI_SUCCESS         The keystroke information is returned successfully.
  @retval EFI_NOT_READY       There is no keystroke data available.
  @retval EFI_DEVICE_ERROR    The dependent serial device encounters error.

**/
EFI_STATUS
ReadKeyStrokes(
  OUT  EFI_INPUT_KEY           *Key
)
{
  EFI_STATUS  Status;
  UINTN       Index;

  while (TRUE) {
    Status = gST->ConIn->ReadKeyStroke(gST->ConIn, Key);
    if (!EFI_ERROR(Status)) {
      break;
    }

    if (Status != EFI_NOT_READY) {
      continue;
    }

    gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &Index);
  }
  return Status;
}

/**
  Implements Reading next KeyStrokes and return key info.

  @param  Key                 A pointer to a buffer that is filled in with the
                              keystroke information for the key that was sent
                              from terminal.

  @retval return a pointer to Key info

**/
EFI_INPUT_KEY*
GetNextKey(
  VOID
  )
{
  EFI_STATUS       Status;
  EFI_INPUT_KEY*   InputKey;
  
  InputKey = AllocatePool(sizeof(EFI_INPUT_KEY));
  ASSERT(InputKey);	

  Status = ReadKeyStrokes(InputKey);
  ASSERT_EFI_ERROR(Status);
	
  return  InputKey;
}

/**
  Implements function to wait and read strings from key stroke.

  @param  MaximumLenth     the maxium lenth of strings input   

  @retval Return a pointer to read strings from console in. Caller is responsible for freeing the
          bufffer

**/
CHAR16*
GetNextLine(
  UINTN MaximumLenth
  )
{
  CHAR16                           *StringPtr;
  CHAR16                           *TempString;
  CHAR16                           KeyPad[2];
  EFI_INPUT_KEY                    Key;
  EFI_STATUS       Status;
  UINTN            Index;

  StringPtr = AllocatePool((MaximumLenth+1) * sizeof (CHAR16));
  ASSERT(StringPtr);
  
  TempString = AllocatePool((MaximumLenth+1) * sizeof (CHAR16));
  ASSERT(TempString);		

  SetMem(StringPtr, (int)((MaximumLenth+1) * sizeof (CHAR16)), 0x00);
  SetMem(TempString, (int)((MaximumLenth+1) * sizeof (CHAR16)), 0x00);
  do{		
    Status = ReadKeyStrokes(&Key);
    ASSERT_EFI_ERROR(Status);

    switch (Key.UnicodeChar) {
      case CHAR_NULL:
        switch (Key.ScanCode) {
          case SCAN_LEFT:
            break;

          case SCAN_RIGHT:
            break;

          case SCAN_ESC:
            goto Done;

          default:
            break;
        }

        break;

      case CHAR_CARRIAGE_RETURN:
        Print(L"\n");
        goto Done;
        break;

      case CHAR_BACKSPACE:
        if (StringPtr[0] != CHAR_NULL) {
          for (Index = 0; StringPtr[Index] != CHAR_NULL; Index++) {
            TempString[Index] = StringPtr[Index];
          }
          //
          // Effectively truncate string by 1 character
          //
          TempString[Index - 1] = CHAR_NULL;
          StrCpyS(StringPtr, MaximumLenth + 1, TempString);
          KeyPad[0] = Key.UnicodeChar;
          KeyPad[1] = CHAR_NULL;	
          Print(L"%s", KeyPad);			
        }
			
      default:
          //
          // If it is the beginning of the string, don't worry about checking maximum limits
          //
          if ((StringPtr[0] == CHAR_NULL) && (Key.UnicodeChar != CHAR_BACKSPACE)) {
            Status = StrnCpyS(StringPtr, MaximumLenth, &Key.UnicodeChar, 1);
            ASSERT_EFI_ERROR(Status);
			
            Status = StrnCpyS(TempString, MaximumLenth, &Key.UnicodeChar, 1);
            ASSERT_EFI_ERROR(Status);
			
            KeyPad[0] = Key.UnicodeChar;
            KeyPad[1] = CHAR_NULL;				
          }	
          else if (((StrLen(StringPtr)) < MaximumLenth) && (Key.UnicodeChar != CHAR_BACKSPACE)) {
            KeyPad[0] = Key.UnicodeChar;
            KeyPad[1] = CHAR_NULL;
            StrCatS(StringPtr, MaximumLenth, KeyPad);
            StrCatS(TempString, MaximumLenth, KeyPad);
          }
			
          if ((Key.UnicodeChar != CHAR_BACKSPACE))
            Print(L"%s", KeyPad);

          break;
      }
  } while (TRUE);
	
Done:
  FreePool (TempString);

  return StringPtr;
    
}





