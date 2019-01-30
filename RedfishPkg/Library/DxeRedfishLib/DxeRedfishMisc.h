/** @file
  Internal Functions for RedfishLib.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __DXE_REDFISH_MISC_LIB_H__
#define __DXE_REDFISH_MISC_LIB_H__

#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/RedfishLib.h>
#include <Protocol/RedfishCredential.h>
#include <redfish.h>

#define ARRAY_SIZE(Array) (sizeof (Array) / sizeof ((Array)[0]))


/**
  Creates a REDFISH_SERVICE which can be later used to access the Redfish resources.

  This function will configure REST EX child according to parameters described in
  Redfish network host interface in SMBIOS type 42 record. The service enumerator will also
  handle the authentication flow automatically if HTTP basic auth or Redfish session
  login is configured to use.

  @param[in]       Image        The image handle used to open service.
  @param[in]       Controller   The controller which has the REST EX service installed.
  @param[in]       RedfishData  The Redfish Host Interface record.
  @param[in]       AuthMethod   None, HTTP basic auth, or Redfish session login.
  @param[in]       UserId       User Name used for authentication.
  @param[in]       Password     Password used for authentication.

  @return     New created Redfish service, or NULL if error happens.

**/
REDFISH_SERVICE
RedfishCreateLibredfishService (
  IN EFI_HANDLE                    Image,
  IN EFI_HANDLE                    Controller,
  IN REDFISH_OVER_IP_PROTOCOL_DATA *RedfishData,
  IN EFI_REDFISH_AUTH_METHOD       AuthMethod,
  IN CHAR8                         *UserId,
  IN CHAR8                         *Password
  );

/**
  Retrieve platform's Redfish authentication information.

  This functions returns the Redfish authentication method together with the user
  Id and password.
  For AuthMethodNone, UserId and Password will point to NULL which means authentication
  is not required to access the Redfish service.
  For AuthMethodHttpBasic, the UserId and Password could be used for
  HTTP header authentication as defined by RFC7235. For AuthMethodRedfishSession,
  the UserId and Password could be used for Redfish session login as defined by
  Redfish API specification (DSP0266).

  Callers are responsible for freeing the returned string storage pointed by UserId
  and Password.

  @param[out]  AuthMethod          Type of Redfish authentication method.
  @param[out]  UserId              The pointer to store the returned UserId string.
  @param[out]  Password            The pointer to store the returned Password string.

  @retval EFI_SUCCESS              Get the authentication information successfully.
  @retval EFI_INVALID_PARAMETER    AuthMethod or UserId or Password is NULL.
  @retval EFI_OUT_OF_RESOURCES     There are not enough memory resources.
  @retval EFI_UNSUPPORTED          Unsupported authentication method is found.
**/
EFI_STATUS
RedfishGetAuthInfo (
  OUT  EFI_REDFISH_AUTH_METHOD            *AuthMethod,
  OUT  CHAR8                              **UserId,
  OUT  CHAR8                              **Password
  );

#endif
