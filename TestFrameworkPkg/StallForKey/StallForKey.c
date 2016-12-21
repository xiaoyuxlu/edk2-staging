/** @file
  Waiting for a key input.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Protocol/ShellParameters.h>

#define DEFAULT_WAIT_TIME         10        // 10 seconds


UINTN  Argc;
CHAR16 **Argv;

/**

  This function parse application ARG.

  @return Status
**/
EFI_STATUS
GetArg (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_SHELL_PARAMETERS_PROTOCOL *ShellParameters;

  Status = gBS->OpenProtocol(
                  gImageHandle,
                  &gEfiShellParametersProtocolGuid,
                  (VOID **)&ShellParameters,
                  gImageHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Argc = ShellParameters->Argc;
  Argv = ShellParameters->Argv;

  gBS->CloseProtocol(
         gImageHandle,
         &gEfiShellParametersProtocolGuid,
         gImageHandle,
         NULL
         );

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
StallForKey (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS      Status;
  UINTN           Seconds;
  EFI_EVENT       TimerEvent;
  EFI_EVENT       WaitList[2];
  UINTN           WaitIndex;
  EFI_INPUT_KEY   Key;

  GetArg ();

  Seconds = DEFAULT_WAIT_TIME;
  if (Argc > 1) {
    Seconds = StrDecimalToUintn (Argv[1]);
  }

  //
  // Create timer event
  //
  Status = gBS->CreateEvent (
                  EVT_TIMER,
                  0,
                  NULL,
                  NULL,
                  &TimerEvent
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Create event. %a:%d:%r\n", __FILE__, __LINE__, Status));
    return Status;
  }

  //
  // Set up the timer
  //
  Status = gBS->SetTimer (
                  TimerEvent,
                  TimerPeriodic,
                  10000000                   // 1 second
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Set timer. %a:%d:%r\n", __FILE__, __LINE__, Status));
    gBS->CloseEvent (TimerEvent);
    return Status;
  }

  //
  // Set up waiting list
  //
  WaitList[0] = gST->ConIn->WaitForKey;
  WaitList[1] = TimerEvent;

  //
  // Do it until timeout
  //
  while (Seconds != 0) {
    //
    // Print the prompt information (The last blank char is used to clean up
    // the last string. Don't remove it.)
    //
    Print (L"Press any key within %d seconds \r", Seconds);

    //
    // Wait for timeout or key input
    //
    Status = gBS->WaitForEvent (
                   2,
                   WaitList,
                   &WaitIndex
                   );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Wait for event. %a:%d:%r\n", __FILE__, __LINE__, Status));
      break;
    }

    //
    // Check the triggered event
    //
    if (WaitIndex == 0) {
      //
      // Key input event is triggered
      //
      Key.ScanCode = SCAN_NULL;
      Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
      if (!EFI_ERROR (Status)) {
        break;
      }

      DEBUG ((DEBUG_ERROR, "Read key stroke. %a:%d:%r\n", __FILE__, __LINE__, Status));
    } else {
      //
      // Timer event is triggered
      //
      Seconds--;
    }
  }
  Print (L"\n");

  //
  // Close the timer event
  //
  gBS->CloseEvent (TimerEvent);

  //
  // Done
  //
  if (Seconds == 0) {
    return EFI_TIMEOUT;
  } else {
    return EFI_SUCCESS;
  }
}
