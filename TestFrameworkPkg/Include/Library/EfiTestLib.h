/** @file
  EFI common test library.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_TEST_LIB_H_
#define _EFI_TEST_LIB_H_

//
// Includes
//
#include <Protocol/LoadedImage.h>
#include <Protocol/BbTest.h>
#include <Protocol/WbTest.h>

//
// Structure definitions
//

//
// These test field structures are created based on Black-Box test protocol
// and White-Box test protocol. Using below structures and interfaces to
// generate the test information will be a little easier than using the test
// protocols directly.
//

//
// Black-box test entry field
//
typedef struct {
  EFI_GUID            EntryId;
  CHAR16              *Name;
  CHAR16              *Description;
  EFI_TEST_LEVEL      TestLevelSupportMap;
  EFI_GUID            *SupportProtocols;
  EFI_TEST_ATTRIBUTE  CaseAttribute;
  EFI_BB_ENTRY_POINT  EntryPoint;
} EFI_BB_TEST_ENTRY_FIELD;

//
// Black-box test protocol field
//
typedef struct {
  UINT64              TestRevision;
  EFI_GUID            CategoryGuid;
  CHAR16              *Name;
  CHAR16              *Description;
} EFI_BB_TEST_PROTOCOL_FIELD;

//
// White-box test entry field
//
typedef struct {
  EFI_GUID            EntryId;
  CHAR16              *Name;
  CHAR16              *Description;
  EFI_TEST_LEVEL      TestLevelSupportMap;
  EFI_GUID            *SupportProtocols;
  EFI_TEST_ATTRIBUTE  CaseAttribute;
  EFI_WB_ENTRY_POINT  EntryPoint;
} EFI_WB_TEST_ENTRY_FIELD;

//
// White-box test protocol field
//
typedef struct {
  UINT64              TestRevision;
  EFI_GUID            CategoryGuid;
  CHAR16              *Name;
  CHAR16              *Description;
} EFI_WB_TEST_PROTOCOL_FIELD;

//
// Functions declaration
//

EFI_STATUS
EFIAPI
EfiInitializeTestLib (
  IN  EFI_HANDLE                  ImageHandle,
  IN  EFI_SYSTEM_TABLE            *SystemTable
  )
/*++

Routine Description:

  Intialize test library if it has not yet been initialized.

Arguments:

  ImageHandle   - The firmware allocated handle for the EFI image.

  SystemTable   - A pointer to the EFI System Table.

Returns:

  EFI_SUCCESS is always returned.

--*/
;

EFI_STATUS
EFIAPI
EfiInitAndInstallBBTestInterface (
  IN  EFI_HANDLE                  *Handle,
  IN  EFI_BB_TEST_PROTOCOL_FIELD  *BBTestProtocolField,
  IN  EFI_BB_TEST_ENTRY_FIELD     *BBTestEntryField,
  IN  EFI_IMAGE_UNLOAD            UnloadFunction,
  OUT EFI_BB_TEST_PROTOCOL        **BBTestProtocolInterface
  )
/*++

Routine Description:

  Initialize a black-box test protocol interface from the protocol field and
  entry field, and then install the black-box test protocol on the handle.

Arguments:

  Handle                  - The driver image handle, and the protocol interface
                            is installed on this handle.

  BBTestProtocolField     - The black-box test protocol field to provide the
                            test info for the whole black-box test.

  BBTestEntryField        - An array of black-box test entry field to provide
                            the test info for each test entry point.

  UnloadFunction          - The unload function pointer for the test image.

  BBTestProtocolInterface - Pointer to the black-box test protocol interface.

Returns:

  EFI_SUCCESS if everything is correct.

--*/
;

EFI_STATUS
EFIAPI
EfiInitAndInstallIHVBBTestInterface (
  IN  EFI_HANDLE                  *Handle,
  IN  EFI_BB_TEST_PROTOCOL_FIELD  *BBTestProtocolField,
  IN  EFI_BB_TEST_ENTRY_FIELD     *BBTestEntryField,
  IN  EFI_IMAGE_UNLOAD            UnloadFunction,
  OUT EFI_BB_TEST_PROTOCOL        **BBTestProtocolInterface
  );

EFI_STATUS
EFIAPI
EfiUninstallAndFreeBBTestInterface (
  IN  EFI_HANDLE                  Handle,
  IN  EFI_BB_TEST_PROTOCOL        *BBTestProtocolInterface
  )
/*++

Routine Description:

  Uninstall the black-box test protocol from the handle, and then free the
  black-box test protocol interface.

Arguments:

  Handle                  - The handle on which the protocol interface was
                            installed.

  BBTestProtocolInterface - Pointer to the black-box test protocol interface.

Returns:

  EFI_SUCCESS if everything is correct.

--*/
;

EFI_STATUS
EFIAPI
EfiUninstallAndFreeIHVBBTestInterface (
  IN  EFI_HANDLE                  Handle,
  IN  EFI_BB_TEST_PROTOCOL        *BBTestProtocolInterface
  );

EFI_STATUS
EFIAPI
EfiInitAndInstallWBTestInterface (
  IN  EFI_HANDLE                  *Handle,
  IN  EFI_WB_TEST_PROTOCOL_FIELD  *WBTestProtocolField,
  IN  EFI_WB_TEST_ENTRY_FIELD     *WBTestEntryField,
  IN  EFI_IMAGE_UNLOAD            UnloadFunction,
  OUT EFI_WB_TEST_PROTOCOL        **WBTestProtocolInterface
  )
/*++

Routine Description:

  Initialize a white-box test protocol interface from the protocol field and
  entry field, and then install the white-box test protocol on the handle.

Arguments:

  Handle                  - The driver image handle, and the protocol interface
                            is installed on this handle.

  WBTestProtocolField     - The white-box test protocol field to provide the
                            test info for the whole black-box test.

  WBTestEntryField        - An array of white-box test entry field to provide
                            the test info for each test entry point.

  UnloadFunction          - The unload function pointer for the test image.

  WBTestProtocolInterface - Pointer to the white-box test protocol interface.

Returns:

  EFI_SUCCESS if everything is correct.

--*/
;

EFI_STATUS
EFIAPI
EfiUninstallAndFreeWBTestInterface (
  IN EFI_HANDLE                   Handle,
  IN EFI_WB_TEST_PROTOCOL         *WBTestProtocolInterface
  )
/*++

Routine Description:

  Uninstall the white-box test protocol from the handle, and then free the
  black-box test protocol interface.

Arguments:

  Handle                  - The handle on which the protocol interface was
                            installed.

  WBTestProtocolInterface - Pointer to the white-box test protocol interface.

Returns:

  EFI_SUCCESS if everything is correct.

--*/
;

BOOLEAN
EFIAPI
CheckBBTestCanRunAndRecordAssertion (
  IN EFI_STANDARD_TEST_LIBRARY_PROTOCOL  *StandardLib,
  IN CHAR16                              *AssertionString,
  IN CHAR8                               *FILE,
  IN UINT32                              LINE
  )
/*++

Routine Description:

  Check whether the black-box test case can run, because some UEFI test case
  can not run on EFI.

Arguments:

  StandardLib             - The pointer to the standard test library protocol.
  AssertionString         - The string to be print if test case can not run.
  FILE                    - The name of the test case's source file
  LINE                    - The line number in the test case's source file

Returns:

  TRUE   if the test case can run
  FALSE  if the test case can not run

--*/
;

EFI_STATUS
EFIAPI
EfiTestLibGetSystemConfigurationTable (
  IN EFI_GUID *TableGuid,
  IN OUT VOID **Table
  )
/*++

Routine Description:

  Get table from configuration table by name

Arguments:

  TableGuid       - Table name to search

  Table           - Pointer to the table caller wants

Returns:

  EFI_NOT_FOUND   - Not found the table

  EFI_SUCCESS     - Found the table

--*/
;

//
// Global variables for the services tables
//
extern EFI_SYSTEM_TABLE          *gtST;
extern EFI_BOOT_SERVICES         *gtBS;
extern EFI_RUNTIME_SERVICES      *gtRT;
extern EFI_DXE_SERVICES          *gtDS;

#endif
