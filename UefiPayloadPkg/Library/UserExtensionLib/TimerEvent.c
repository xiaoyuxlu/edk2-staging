/** @file
  This file implements timer event service functions  

  Copyright (c) 2007 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>

/**
  This function creates a new timer event and returns it in the location referenced by Event.
  There are two types of events:
  TimerPeriodic - The event is to be signaled periodically at TriggerTime intervals from the current time.
  TimerRelative - The event is to be signaled in TriggerTime 100ns units.

  @param Type              The type of two events: TimerPeriodic or TimerRelative.
  @param TriggerTime       The number of 100ns units until the timer expires.
  @param NotifyFunc        The Function pointer of Event. this function should be this specific type:
                             typedef
                             VOID
                             (EFIAPI *EFI_EVENT_NOTIFY)(
                                IN  EFI_EVENT                Event,
                                IN  VOID                     *Context
                                );  
  @param NotifyContext     The Pointer of input Parameters for NotifyFunc.  
  @param TimerEventPtr     The Timer event created.  
                                            
  @return 
  EFI_SUCCESS              The event has been set to be signaled at the requested time.
  EFI_INVALID_PARAMETER    Event or Type is not valid.
  EFI_OUT_OF_RESOURCES     The event could not be allocated.

**/
EFI_STATUS
EFIAPI
CreateTimerEvent(
  IN  UINT32           Type,
  IN  UINT64           TriggerTime,
  IN  EFI_EVENT_NOTIFY NotifyFunc,
  IN  VOID             *NotifyContext,
  OUT EFI_EVENT        *TimerEventPtr
  )
{
  EFI_STATUS       Status;
  EFI_EVENT        TimerEvent;

  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  NotifyFunc,
                  NotifyContext,
                  &TimerEvent
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->SetTimer (TimerEvent, Type, TriggerTime);
  if (!EFI_ERROR (Status)) {
    *TimerEventPtr = TimerEvent;
  }
  
  return Status;

}

/**
  This function cancel and close the exist timer event.

  @param TimerEvent       The exist timer event.
                                            
  @return 
  EFI_SUCCESS              The event has been closed.
  EFI_INVALID_PARAMETER    Event or Type is not valid.

**/
EFI_STATUS
EFIAPI
CancelTimerEvent(
  IN EFI_EVENT        TimerEvent
  )
{
  EFI_STATUS          Status;
  
  Status = gBS->SetTimer (TimerEvent, TimerCancel, 0);
  if (!EFI_ERROR (Status)) {
    Status = gBS->CloseEvent (TimerEvent);
  }
  return Status;
}





