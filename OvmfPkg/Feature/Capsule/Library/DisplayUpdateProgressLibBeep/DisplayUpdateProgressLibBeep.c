/** @file
  Provides services to display completion progress of a firmware update on an
  AdLib/Sound Blaster sound card.

  Copyright (c) 2016, Microsoft Corporation. All rights reserved.<BR>
  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

  Programming the AdLib/Sound Blaster FM Music Chips, Version 2.0,
  Feb 24, 1992, http://www.oplx.com/opl2/docs/adlib_sb.txt

  Programmer's Guide to Yamaha YMF 262/OPL3 FM Music Synthesizer, HTML Version
  1.12 Last Updated Nov-23-2000, http://www.fit.vutbr.cz/~arnost/opl/opl3.html

**/

#include <PiDxe.h>

#include <Library/DisplayUpdateProgressLib.h>

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>

///
/// AdLib/Sound Blaster Address/Data I/O Port addresses
///
#define ADLIB_ADDRESS_PORT  0x388
#define ADLIB_DATA_PORT     0x389

///
/// Frequency of AdLib/SOund Blaster clock in Hz
///
#define ADLIB_CLOCK_HZ  49716

///
/// Control Style.  Set to 100 so it is reset on first call.
///
UINTN  mPreviousProgress = 100;

///
/// Table to convert progress completion to sound frequency
///
UINTN mFrequencyTable[][2] = {
  { 523,  90}, // C4
  { 494,  80}, // B4
  { 440,  70}, // A4
  { 391,  60}, // G4
  { 349,  50}, // F4
  { 329,  40}, // E4
  { 293,  30}, // D4
  { 261,   0}  // C3
};

UINTN
ProgressToFreqency (
  UINTN  Progress
  )
{
  UINTN  Index;

  for (Index = 0; Index < 8; Index++) {
    if (Progress >= mFrequencyTable[Index][1]) {
      return mFrequencyTable[Index][0];
    }
  }
  return mFrequencyTable[7][0];
}

VOID
AdLibWriteRegister (
  UINT8  Register,
  UINT8  Value
  )
{
  IoWrite8 (ADLIB_ADDRESS_PORT, Register);
  IoWrite8 (ADLIB_DATA_PORT,    Value);
}

VOID
AdLibPlayNote (
  UINTN  Frequency,
  UINTN  DurationMilliseconds
  )
{
  UINTN  F;

  //
  // Compute 10-bit Register value F
  //
  F = (Frequency << 16) / ADLIB_CLOCK_HZ;

  //
  // Initialize registers
  //
  AdLibWriteRegister (0x20, 0x01);
  AdLibWriteRegister (0x40, 0x10);
  AdLibWriteRegister (0x60, 0xF0);
  AdLibWriteRegister (0x80, 0x77);
  AdLibWriteRegister (0x23, 0x01);
  AdLibWriteRegister (0x43, 0x00);
  AdLibWriteRegister (0x63, 0xF0);
  AdLibWriteRegister (0x83, 0x77);

  //
  // Set F value for Frequency and enable output
  //
  AdLibWriteRegister (0xA0, F & 0xFF);
  AdLibWriteRegister (0xB0, 0x30 | ((F >> 8) & 0x03));

  //
  // Wait DurationMilliseconds
  //
  MicroSecondDelay (DurationMilliseconds * 1000);

  //
  // Disable output
  //
  AdLibWriteRegister (0xB0, 0x10 | ((F >> 8) & 0x03));
}

/**
  Function indicates the current completion progress of a firmware update.
  Platform may override with its own specific function.

  @param[in] Completion  A value between 0 and 100 indicating the current
                         completion progress of a firmware update.  This
                         value must the the same or higher than previous
                         calls to this service.  The first call of 0 or a
                         value of 0 after reaching a value of 100 resets
                         the progress indicator to 0.
  @param[in] Color       Color of the progress indicator.  Only used when
                         Completion is 0 to set the color of the progress
                         indicator.  If Color is NULL, then the default color
                         is used.

  @retval EFI_SUCCESS            Progress displayed successfully.
  @retval EFI_INVALID_PARAMETER  Completion is not in range 0..100.
  @retval EFI_INVALID_PARAMETER  Completion is less than Completion value from
                                 a previous call to this service.
  @retval EFI_NOT_READY          The device used to indicate progress is not
                                 available.
**/
EFI_STATUS
EFIAPI
DisplayUpdateProgress (
  IN UINTN                                Completion,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL_UNION  *Color       OPTIONAL
  )
{
  //
  // Check range
  //
  if (Completion > 100) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check to see if this Completion percentage has already been displayed
  //
  if (Completion == mPreviousProgress) {
    return EFI_SUCCESS;
  }

  //
  // Do special init on first call of each progress session
  //
  if (mPreviousProgress == 100) {
    //
    // Clear previous
    //
    mPreviousProgress = 0;
  }

  //
  // Can not update progress bar if Completion is less than previous
  //
  if (Completion < mPreviousProgress) {
    DEBUG ((DEBUG_WARN, "WARNING: Completion (%d) should not be less than Previous (%d)!!!\n", Completion, mPreviousProgress));
    return EFI_INVALID_PARAMETER;
  }

  if (ProgressToFreqency (Completion) > ProgressToFreqency (mPreviousProgress)) {
    //
    // Play note for 20ms
    //
    AdLibPlayNote (ProgressToFreqency (Completion), 20);
  }

  mPreviousProgress = Completion;

  return EFI_SUCCESS;
}
