/** @file
  This is the include file for Console in/out API. 

  Copyright (c) 2007 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __Console_InOut_LIB__
#define __Console_InOut_LIB__

typedef struct {
  UINT8 Blue;
  UINT8 Green;
  UINT8 Red;
  UINT8 Reserved;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL;

//
// Globle Graphics Colors definition
//
//to-do: use macro instead?
extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL        mGraphicsEfiColors[16];

/** 
  Prints a formatted Unicode string to the console output device specified by 
  ConOut defined in the EFI_SYSTEM_TABLE.

  This function prints a formatted Unicode string to the console output device 
  specified by ConOut in EFI_SYSTEM_TABLE and returns the number of Unicode 
  characters that printed to ConOut.  If the length of the formatted Unicode 
  string is greater than PcdUefiLibMaxPrintBufferSize, then only the first 
  PcdUefiLibMaxPrintBufferSize characters are sent to ConOut.
  If Format is NULL, then ASSERT().
  If Format is not aligned on a 16-bit boundary, then ASSERT().
  If gST->ConOut is NULL, then ASSERT().

  @param Format   A null-terminated Unicode format string.
  @param ...      The variable argument list whose contents are accessed based 
                  on the format string specified by Format.
  
  @return Number of Unicode characters printed to ConOut.

**/
UINTN
EFIAPI
Print (
  IN CONST CHAR16  *Format,
  ...
  );

/** 
  Prints a formatted ASCII string to the console output device specified by 
  ConOut defined in the EFI_SYSTEM_TABLE.

  This function prints a formatted ASCII string to the console output device 
  specified by ConOut in EFI_SYSTEM_TABLE and returns the number of ASCII 
  characters that printed to ConOut.  If the length of the formatted ASCII 
  string is greater than PcdUefiLibMaxPrintBufferSize, then only the first 
  PcdUefiLibMaxPrintBufferSize characters are sent to ConOut.
  If Format is NULL, then ASSERT().
  If gST->ConOut is NULL, then ASSERT().

  @param Format   A null-terminated ASCII format string.
  @param ...      The variable argument list whose contents are accessed based 
                  on the format string specified by Format.
  
  @return Number of ASCII characters printed to ConOut.

**/  
UINTN
EFIAPI
AsciiPrint (
  IN CONST CHAR8  *Format,
  ...
  );

/**
  Prints a formatted Unicode string to a graphics console device specified by 
  ConsoleOutputHandle defined in the EFI_SYSTEM_TABLE at the given (X,Y) coordinates.

  This function prints a formatted Unicode string to the graphics console device 
  specified by ConsoleOutputHandle in EFI_SYSTEM_TABLE and returns the number of 
  Unicode characters displayed, not including partial characters that may be clipped 
  by the right edge of the display.  If the length of the formatted Unicode string is
  greater than PcdUefiLibMaxPrintBufferSize, then at most the first 
  PcdUefiLibMaxPrintBufferSize characters are printed.  The EFI_HII_FONT_PROTOCOL
  is used to convert the string to a bitmap using the glyphs registered with the 
  HII database.  No wrapping is performed, so any portions of the string the fall
  outside the active display region will not be displayed.

  If a graphics console device is not associated with the ConsoleOutputHandle 
  defined in the EFI_SYSTEM_TABLE then no string is printed, and 0 is returned.
  If the EFI_HII_FONT_PROTOCOL is not present in the handle database, then no 
  string is printed, and 0 is returned.
  If Format is NULL, then ASSERT().
  If Format is not aligned on a 16-bit boundary, then ASSERT().
  If gST->ConsoleOutputHandle is NULL, then ASSERT().

  @param  PointX       X coordinate to print the string.
  @param  PointY       Y coordinate to print the string.
  @param  ForeGround   The foreground color of the string being printed.  This is
                       an optional parameter that may be NULL.  If it is NULL,
                       then the foreground color of the current ConOut device
                       in the EFI_SYSTEM_TABLE is used.
  @param  BackGround   The background color of the string being printed.  This is
                       an optional parameter that may be NULL.  If it is NULL, 
                       then the background color of the current ConOut device
                       in the EFI_SYSTEM_TABLE is used.
  @param  Format       A null-terminated Unicode format string.  See Print Library 
                       for the supported format string syntax.
  @param  ...          Variable argument list whose contents are accessed based on 
                       the format string specified by Format.         

  @return  The number of Unicode characters printed.

**/  
UINTN
EFIAPI
PrintXY (
  IN UINTN                            PointX,
  IN UINTN                            PointY,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL    *ForeGround, OPTIONAL
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL    *BackGround, OPTIONAL
  IN CONST CHAR16                     *Format,
  ...
  );

/**
  Prints a formatted ASCII string to a graphics console device specified by 
  ConsoleOutputHandle defined in the EFI_SYSTEM_TABLE at the given (X,Y) coordinates.

  This function prints a formatted ASCII string to the graphics console device 
  specified by ConsoleOutputHandle in EFI_SYSTEM_TABLE and returns the number of 
  ASCII characters displayed, not including partial characters that may be clipped 
  by the right edge of the display.  If the length of the formatted ASCII string is
  greater than PcdUefiLibMaxPrintBufferSize, then at most the first 
  PcdUefiLibMaxPrintBufferSize characters are printed.  The EFI_HII_FONT_PROTOCOL
  is used to convert the string to a bitmap using the glyphs registered with the 
  HII database.  No wrapping is performed, so any portions of the string the fall
  outside the active display region will not be displayed.

  If a graphics console device is not associated with the ConsoleOutputHandle 
  defined in the EFI_SYSTEM_TABLE then no string is printed, and 0 is returned.
  If the EFI_HII_FONT_PROTOCOL is not present in the handle database, then no 
  string is printed, and 0 is returned.
  If Format is NULL, then ASSERT().
  If gST->ConsoleOutputHandle is NULL, then ASSERT().

  @param  PointX       X coordinate to print the string.
  @param  PointY       Y coordinate to print the string.
  @param  ForeGround   The foreground color of the string being printed.  This is
                       an optional parameter that may be NULL.  If it is NULL,
                       then the foreground color of the current ConOut device
                       in the EFI_SYSTEM_TABLE is used.
  @param  BackGround   The background color of the string being printed.  This is
                       an optional parameter that may be NULL.  If it is NULL, 
                       then the background color of the current ConOut device
                       in the EFI_SYSTEM_TABLE is used.
  @param  Format       A null-terminated ASCII format string.  See Print Library 
                       for the supported format string syntax.
  @param  ...          The variable argument list whose contents are accessed based on 
                       the format string specified by Format.         

  @return  The number of ASCII characters printed.

**/  
UINTN
EFIAPI
AsciiPrintXY (
  IN UINTN                            PointX,
  IN UINTN                            PointY,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL    *ForeGround, OPTIONAL
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL    *BackGround, OPTIONAL
  IN CONST CHAR8                      *Format,
  ...
  );      

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
  );

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
  );

/**
  Implements function to wait and read strings from key stroke.

  @param  MaximumLenth     the maxium lenth of strings input

  @retval Return a pointer to read strings from console in

**/
CHAR16*
GetNextLine(
  IN UINTN MaximumLenth
  );

/**
  Implements function to clear screen.

  @param  None  

  @retval None

**/
VOID
ClearScreen(
  VOID
  );

/**
Convert a *.BMP graphics image to a GOP blt buffer. If a NULL Blt buffer
is passed in a GopBlt buffer will be allocated by this routine. If a GopBlt
buffer is passed in it will be used if it is big enough.

@param[in]       BmpImage              Pointer to BMP file
@param[in]       BmpImageSize          Number of bytes in BmpImage
@param[in, out]  GopBlt                Buffer containing GOP version of BmpImage.
@param[in, out]  GopBltSize            Size of GopBlt in bytes.
@param[out]      PixelHeight           Height of GopBlt/BmpImage in pixels
@param[out]      PixelWidth            Width of GopBlt/BmpImage in pixels

@retval          EFI_SUCCESS           GopBlt and GopBltSize are returned.
@retval          EFI_UNSUPPORTED       BmpImage is not a valid *.BMP image
@retval          EFI_BUFFER_TOO_SMALL  The passed in GopBlt buffer is not big enough.
GopBltSize will contain the required size.
@retval          EFI_OUT_OF_RESOURCES  No enough buffer to allocate.

**/
EFI_STATUS
ConvertBmpToGopBlt(
  IN     VOID      *BmpImage,
  IN     UINTN     BmpImageSize,
  IN OUT VOID      **GopBlt,
  IN OUT UINTN     *GopBltSize,
  OUT    UINTN     *PixelHeight,
  OUT    UINTN     *PixelWidth
);

/**
Use SystemTable Conout to stop video based Simple Text Out consoles from going
to the video device. Put up LogoFile on every video device that is a console.

@param[in]  Blt               Blt Data Pointer of logo to display on the center of the screen.
@param[in]  Height            The height of a rectangle in the blt rectangle in pixels.
@param[in]  Width             The width of a rectangle in the blt rectangle in pixels.

@retval     EFI_SUCCESS       ConsoleControl has been flipped to graphics and logo displayed.
@retval     EFI_UNSUPPORTED   Logo not found

**/
EFI_STATUS
EFIAPI
ShowBltLogo(
  IN  VOID   *Blt,
  IN  UINTN  Height,
  IN  UINTN  Width
);

/**
  Show progress bar with title above it. It only works in Graphics mode.

  @param[in] TitleForeground    Foreground color for Title.
  @param[in] TitleBackground    Background color for Title.
  @param[in] Title              Title above progress bar.
  @param[in] ProgressColor      Progress bar color.
  @param[in] Progress           Progress (0-100)
  @param[in] PreviousValue      The previous value of the progress.

  @retval    EFI_STATUS         Success update the progress bar

**/
EFI_STATUS
EFIAPI
ShowProgress(
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL TitleForeground,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL TitleBackground,
  IN CHAR16                        *Title,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL ProgressColor,
  IN UINTN                         Progress,
  IN UINTN                         PreviousValue
);

#endif