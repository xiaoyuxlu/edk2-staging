/** @file
  RedfishPlatformDxe is to fill the SMBIOS table 42 before EndofDxe phase. And meanwhile, 
  RedfishPlatformDxe is required to produce the RedfishCredentialProtocol for the consumer
  to get the Redfish credential Info and to restrict Redfish access from UEFI side.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <RedfishPlatformDxe.h>

EFI_GUID gRedfishServiceGuid = { 0x3fb208ac, 0x2185, 0x498c, { 0xbf, 0x46, 0xdc, 0x23, 0xda, 0x58, 0x7b, 0x55 } };

BOOLEAN mSecureBootDisabled = FALSE;

BOOLEAN mStopRedfishService = FALSE;

EFI_REDFISH_CREDENTIAL_PROTOCOL mRedfishCredentialProtocol = {
  RedfishCredentialGetAuthInfo,
  RedfishCredentialStopService
};

VOID
InternalDumpIp4Addr (
  IN EFI_IPv4_ADDRESS   *Ip
  )
{
  UINTN                 Index;

  for (Index = 0; Index < 4; Index++) {
    DEBUG ((DEBUG_INFO, "%d", Ip->Addr[Index]));
    if (Index < 3) {
      DEBUG ((DEBUG_INFO, "."));
    }
  }

  DEBUG ((DEBUG_INFO, "\n"));
}

VOID
InternalDumpIp6Addr (
  IN EFI_IPv6_ADDRESS   *Ip
  )
{
  UINTN                 Index;

  for (Index = 0; Index < 16; Index++) {

    if (Ip->Addr[Index] != 0) {
      DEBUG ((DEBUG_INFO, "%x", Ip->Addr[Index]));
    }
    Index++;
    if (Index > 15) {
      return;
    }
    if (((Ip->Addr[Index] & 0xf0) == 0) && (Ip->Addr[Index - 1] != 0)) {
      DEBUG ((DEBUG_INFO, "0"));
    }
    DEBUG ((DEBUG_INFO, "%x", Ip->Addr[Index]));
    if (Index < 15) {
      DEBUG ((DEBUG_INFO, ":"));
    }
  }

  DEBUG ((DEBUG_INFO, "\n"));
}

VOID
InternalDumpData (
  IN UINT8  *Data,
  IN UINTN  Size
  )
{
  UINTN  Index;
  for (Index = 0; Index < Size; Index++) {
    DEBUG ((DEBUG_INFO, "%02x ", (UINTN)Data[Index]));
  }
}

VOID
InternalDumpHex (
  IN UINT8  *Data,
  IN UINTN  Size
  )
{
  UINTN   Index;
  UINTN   Count;
  UINTN   Left;

#define COLUME_SIZE  (16)

  Count = Size / COLUME_SIZE;
  Left  = Size % COLUME_SIZE;
  for (Index = 0; Index < Count; Index++) {
    InternalDumpData (Data + Index * COLUME_SIZE, COLUME_SIZE);
    DEBUG ((DEBUG_INFO, "\n"));
  }

  if (Left != 0) {
    InternalDumpData (Data + Index * COLUME_SIZE, Left);
    DEBUG ((DEBUG_INFO, "\n"));
  }

  DEBUG ((DEBUG_INFO, "\n"));
}

VOID
DumpRedfishIpProtocolData (
  REDFISH_OVER_IP_PROTOCOL_DATA   *RedfishProtocolData,
  UINT8                           RedfishProtocolDataSize
  )
{
  CHAR16 Hostname[16];

  DEBUG ((DEBUG_INFO, "RedfishProtocolData: \n"));
  InternalDumpHex ((UINT8 *) RedfishProtocolData, RedfishProtocolDataSize);
  
  DEBUG ((DEBUG_INFO, "Parsing as below: \n"));
  
  DEBUG ((DEBUG_INFO, "RedfishProtocolData->ServiceUuid - %g\n", &(RedfishProtocolData->ServiceUuid)));
  
  DEBUG ((DEBUG_INFO, "RedfishProtocolData->HostIpAssignmentType - %d\n", RedfishProtocolData->HostIpAssignmentType));
  
  DEBUG ((DEBUG_INFO, "RedfishProtocolData->HostIpAddressFormat - %d\n", RedfishProtocolData->HostIpAddressFormat));

  DEBUG ((DEBUG_INFO, "RedfishProtocolData->HostIpAddress: \n"));
  if (RedfishProtocolData->HostIpAddressFormat == 0x01) {
    InternalDumpIp4Addr ((EFI_IPv4_ADDRESS *) (RedfishProtocolData->HostIpAddress));
  } else {
    InternalDumpIp6Addr ((EFI_IPv6_ADDRESS *) (RedfishProtocolData->HostIpAddress));
  }

  DEBUG ((DEBUG_INFO, "RedfishProtocolData->HostIpMask: \n"));
  if (RedfishProtocolData->HostIpAddressFormat == 0x01) {
    InternalDumpIp4Addr ((EFI_IPv4_ADDRESS *) (RedfishProtocolData->HostIpMask));
  } else {
    InternalDumpIp6Addr ((EFI_IPv6_ADDRESS *) (RedfishProtocolData->HostIpMask));
  }

  DEBUG ((DEBUG_INFO, "RedfishProtocolData->RedfishServiceIpDiscoveryType - %d\n", RedfishProtocolData->RedfishServiceIpDiscoveryType));

  DEBUG ((DEBUG_INFO, "RedfishProtocolData->RedfishServiceIpAddressFormat - %d\n", RedfishProtocolData->RedfishServiceIpAddressFormat));

  DEBUG ((DEBUG_INFO, "RedfishProtocolData->RedfishServiceIpAddress: \n"));
  if (RedfishProtocolData->RedfishServiceIpAddressFormat == 0x01) {
    InternalDumpIp4Addr ((EFI_IPv4_ADDRESS *) (RedfishProtocolData->RedfishServiceIpAddress));
  } else {
    InternalDumpIp6Addr ((EFI_IPv6_ADDRESS *) (RedfishProtocolData->RedfishServiceIpAddress));
  }

  DEBUG ((DEBUG_INFO, "RedfishProtocolData->RedfishServiceIpMask: \n"));
  if (RedfishProtocolData->RedfishServiceIpAddressFormat == 0x01) {
    InternalDumpIp4Addr ((EFI_IPv4_ADDRESS *) (RedfishProtocolData->RedfishServiceIpMask));
  } else {
    InternalDumpIp6Addr ((EFI_IPv6_ADDRESS *) (RedfishProtocolData->RedfishServiceIpMask));
  }

  DEBUG ((DEBUG_INFO, "RedfishProtocolData->RedfishServiceIpPort - %d\n", RedfishProtocolData->RedfishServiceIpPort));

  DEBUG ((DEBUG_INFO, "RedfishProtocolData->RedfishServiceVlanId - %d\n", RedfishProtocolData->RedfishServiceVlanId));

  DEBUG ((DEBUG_INFO, "RedfishProtocolData->RedfishServiceHostnameLength - %d\n", RedfishProtocolData->RedfishServiceHostnameLength));

  AsciiStrToUnicodeStrS((CHAR8 *) RedfishProtocolData->RedfishServiceHostname, Hostname, sizeof (Hostname) / sizeof (Hostname[0]));
  DEBUG ((DEBUG_INFO, "RedfishProtocolData->RedfishServiceHostname - %s\n", Hostname));
}

/**
  Get the next string, which is distinguished by specified separator.

  @param[in]  String             Pointer to the string.
  @param[in]  Separator          Specified separator used to distinguish where is the beginning
                                 of next string.

  @return     Pointer to the next string.
  @return     NULL if not find or String is NULL.

**/
CHAR8 *
AsciiStrGetNextToken (
  IN CONST CHAR8 *String,
  IN       CHAR8 Separator
  )
{
  CONST CHAR8 *Token;

  Token = String;
  while (TRUE) {
    if (*Token == 0) {
      return NULL;
    }
    if (*Token == Separator) {
      return (CHAR8 *)(Token + 1);
    }
    Token++;
  }
}

EFI_STATUS
GetRedfishRecordFromVariable (
  IN OUT REDFISH_OVER_IP_PROTOCOL_DATA   **RedfishProtocolData,
  IN OUT UINT8                           *RedfishProtocolDataSize
  ) 
{
  EFI_STATUS                      Status;

  UINT8                           HostIpAssignmentType;
  UINTN                           HostIpAssignmentTypeSize;
  EFI_IPv4_ADDRESS                HostIpAddress;  
  UINTN                           IPv4DataSize;
  EFI_IPv4_ADDRESS                HostIpMask;
  EFI_IPv4_ADDRESS                RedfishServiceIpAddress;
  EFI_IPv4_ADDRESS                RedfishServiceIpMask;
  UINT16                          RedfishServiceIpPort;
  UINTN                           IpPortDataSize;
  UINT8                           HostNameSize;
  CHAR8                           RedfishHostName[20];

  if (RedfishProtocolData == NULL || RedfishProtocolDataSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // 1. Retrieve Address Information from variable.
  //
  Status = gRT->GetVariable (L"HostIpAssignmentType",
                  &gRedfishServiceGuid,
                  NULL,
                  &HostIpAssignmentTypeSize,
                  &HostIpAssignmentType
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "RedfishPlatformDxe: GetVariable HostIpAssignmentType - %r\n", Status));
    return Status;
  }

  IPv4DataSize = sizeof (EFI_IPv4_ADDRESS);
  if (HostIpAssignmentType == 1 ) {  
    Status = gRT->GetVariable (L"HostIpAddress",
                    &gRedfishServiceGuid,
                    NULL,
                    &IPv4DataSize,
                    &HostIpAddress
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "RedfishPlatformDxe: GetVariable HostIpAddress - %r\n", Status));
      return Status;
    }

    Status = gRT->GetVariable (L"HostIpMask",
                    &gRedfishServiceGuid,
                    NULL,
                    &IPv4DataSize,
                    &HostIpMask
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "RedfishPlatformDxe: GetVariable HostIpMask - %r\n", Status));
      return Status;
    }
  }

  Status = gRT->GetVariable (L"RedfishServiceIpAddress",
                  &gRedfishServiceGuid,
                  NULL,
                  &IPv4DataSize,
                  &RedfishServiceIpAddress
                  );
  if (EFI_ERROR (Status)) { 
    DEBUG ((EFI_D_ERROR, "RedfishPlatformDxe: GetVariable RedfishServiceIpAddress - %r\n", Status));
    return Status;
  }

  Status = gRT->GetVariable (L"RedfishServiceIpMask",
                  &gRedfishServiceGuid,
                  NULL,
                  &IPv4DataSize,
                  &RedfishServiceIpMask
                  );
  if (EFI_ERROR (Status)) { 
    DEBUG ((EFI_D_ERROR, "RedfishPlatformDxe: GetVariable RedfishServiceIpMask - %r\n", Status));
    return Status;
  }

  Status = gRT->GetVariable (L"RedfishServiceIpPort",
                  &gRedfishServiceGuid,
                  NULL,
                  &IpPortDataSize,
                  &RedfishServiceIpPort
                  );
  if (EFI_ERROR (Status)) { 
    DEBUG ((EFI_D_ERROR, "RedfishPlatformDxe: GetVariable RedfishServiceIpPort - %r\n", Status));
    return Status;
  }
  
  AsciiSPrint (
    RedfishHostName,
    sizeof (RedfishHostName),
    "%d.%d.%d.%d",
    RedfishServiceIpAddress.Addr[0],
    RedfishServiceIpAddress.Addr[1],
    RedfishServiceIpAddress.Addr[2],
    RedfishServiceIpAddress.Addr[3]
    );
  
  HostNameSize = (UINT8) AsciiStrLen (RedfishHostName) + 1;
  
  //
  // 2. Protocol Data Size.
  //
  *RedfishProtocolDataSize = sizeof (REDFISH_OVER_IP_PROTOCOL_DATA) - 1 + HostNameSize;

  //
  // 3. Protocol Data.
  //
  *RedfishProtocolData = (REDFISH_OVER_IP_PROTOCOL_DATA *) AllocateZeroPool (*RedfishProtocolDataSize);
  if (*RedfishProtocolData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyGuid (&(*RedfishProtocolData)->ServiceUuid, &gRedfishServiceGuid);
  
  (*RedfishProtocolData)->HostIpAssignmentType = HostIpAssignmentType;
  (*RedfishProtocolData)->HostIpAddressFormat = 1;   // Only support IPv4
  
  if (HostIpAssignmentType == 1 ) {
    (*RedfishProtocolData)->HostIpAddress[0] = HostIpAddress.Addr[0];
    (*RedfishProtocolData)->HostIpAddress[1] = HostIpAddress.Addr[1];
    (*RedfishProtocolData)->HostIpAddress[2] = HostIpAddress.Addr[2];
    (*RedfishProtocolData)->HostIpAddress[3] = HostIpAddress.Addr[3];

    (*RedfishProtocolData)->HostIpMask[0] = HostIpMask.Addr[0];
    (*RedfishProtocolData)->HostIpMask[1] = HostIpMask.Addr[1];
    (*RedfishProtocolData)->HostIpMask[2] = HostIpMask.Addr[2];
    (*RedfishProtocolData)->HostIpMask[3] = HostIpMask.Addr[3];
  }
  
  (*RedfishProtocolData)->RedfishServiceIpDiscoveryType = 1;  // Use static IP address
  (*RedfishProtocolData)->RedfishServiceIpAddressFormat = 1;  // Only support IPv4

  (*RedfishProtocolData)->RedfishServiceIpAddress[0] = RedfishServiceIpAddress.Addr[0];
  (*RedfishProtocolData)->RedfishServiceIpAddress[1] = RedfishServiceIpAddress.Addr[1];
  (*RedfishProtocolData)->RedfishServiceIpAddress[2] = RedfishServiceIpAddress.Addr[2];
  (*RedfishProtocolData)->RedfishServiceIpAddress[3] = RedfishServiceIpAddress.Addr[3];

  (*RedfishProtocolData)->RedfishServiceIpMask[0] = RedfishServiceIpMask.Addr[0];
  (*RedfishProtocolData)->RedfishServiceIpMask[1] = RedfishServiceIpMask.Addr[1];
  (*RedfishProtocolData)->RedfishServiceIpMask[2] = RedfishServiceIpMask.Addr[2];
  (*RedfishProtocolData)->RedfishServiceIpMask[3] = RedfishServiceIpMask.Addr[3];
  
  (*RedfishProtocolData)->RedfishServiceIpPort = RedfishServiceIpPort;
  (*RedfishProtocolData)->RedfishServiceVlanId = 0xffffffff;
  
  (*RedfishProtocolData)->RedfishServiceHostnameLength = HostNameSize;
  AsciiStrCpyS ((CHAR8 *) ((*RedfishProtocolData)->RedfishServiceHostname), HostNameSize, RedfishHostName);
  
  return Status;
}

EFI_STATUS
RedfishFillSmbiosTable42 (
  VOID
  )
{
  EFI_STATUS                      Status;
  REDFISH_OVER_IP_PROTOCOL_DATA   *RedfishProtocolData;
  UINT8                           RedfishProtocolDataSize;
  
  PROTOCOl_RECORD_DATA            *ProtocolRecord;
  
  REDFISH_INTERFACE_DATA          *InterfaceData;
  UINT8                           InterfaceDataLen;
  
  SMBIOS_TABLE_TYPE42             *Type42Record;
  
  EFI_SMBIOS_PROTOCOL             *Smbios;
  EFI_SMBIOS_HANDLE               MemArrayMappedAddrSmbiosHandle;

  
  RedfishProtocolData     = NULL;
  RedfishProtocolDataSize = 0;
  
  ProtocolRecord = NULL;
  
  InterfaceData    = NULL;
  InterfaceDataLen = 0;
  
  Type42Record = NULL;
  
  //
  // 1. Initialize Redfish Protocol Data.
  //
  Status = GetRedfishRecordFromVariable (&RedfishProtocolData, &RedfishProtocolDataSize);
  DEBUG ((EFI_D_INFO, "RedfishPlatformDxe: GetRedfishRecordFromVariable() - %r\n", Status));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (RedfishProtocolData != NULL);

  DumpRedfishIpProtocolData (RedfishProtocolData, RedfishProtocolDataSize);

  //
  // 2. Initialize Redfish Protocol Record.
  //
  ProtocolRecord = (PROTOCOl_RECORD_DATA *) AllocateZeroPool (sizeof (PROTOCOl_RECORD_DATA));
  if (ProtocolRecord == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }
  
  ProtocolRecord->ProtocolIdentifier = PROTOCOL_REDFISH_OVER_IP;
  ProtocolRecord->ProtocolDataLen = RedfishProtocolDataSize;
  
  //
  // 3. Initialize Redfish Interface Data
  //
  InterfaceDataLen = sizeof (REDFISH_INTERFACE_DATA);
  InterfaceData = (REDFISH_INTERFACE_DATA *) AllocateZeroPool (InterfaceDataLen);
  if (InterfaceData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }
  
  InterfaceData->DeviceType = REDFISH_HOST_INTERFACE_DEVICE_TYPE_PCI_PCIE;
  InterfaceData->DeviceDescriptor.PciPcieDevice.VendorId          = 1;
  InterfaceData->DeviceDescriptor.PciPcieDevice.DeviceId          = 1;
  InterfaceData->DeviceDescriptor.PciPcieDevice.SubsystemVendorId = 1;
  InterfaceData->DeviceDescriptor.PciPcieDevice.SubsystemId       = 1;

  //
  // 4. Fill in SMBIOS type 42 record
  //
  // SMBIOS type 42 Record for Redfish Interface
  // 00h Type BYTE 42 Management Controller Host Interface structure indicator
  // 01h Length BYTE Varies Length of the structure, a minimum of 09h
  // 02h Handle WORD Varies
  // 04h Interface Type BYTE Varies Management Controller Interface Type.
  // 05h Interface Specific Data Length (n)
  // 06h Interface Specific data
  // 06h+n number of protocols defined for the host interface (typically 1)
  // 07h+n Include a Protocol Record for each protocol supported.
  //
  //
  Type42Record = (SMBIOS_TABLE_TYPE42 *) AllocateZeroPool (
                                           sizeof (SMBIOS_TABLE_TYPE42) - 4
                                           + InterfaceDataLen 
                                           + 1 /// For Protocol Record Count
                                           + sizeof (PROTOCOl_RECORD_DATA) - 1
                                           + RedfishProtocolDataSize
                                           );
  if (Type42Record == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  Type42Record->Hdr.Type   = EFI_SMBIOS_TYPE_MANAGEMENT_CONTROLLER_HOST_INTERFACE;
  Type42Record->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE42) - 4
                               + InterfaceDataLen 
                               + 1
                               + sizeof (PROTOCOl_RECORD_DATA) - 1
                               + RedfishProtocolDataSize;
  Type42Record->Hdr.Handle = 0;
  Type42Record->InterfaceType = REDFISH_NETWORK_HOST_INTERFACE_TYPE; // Network Host Interface

  //
  // Fill in InterfaceTypeSpecificDataLength field
  //
  Type42Record->InterfaceTypeSpecificDataLength = InterfaceDataLen;

  //
  // Fill in InterfaceTypeSpecificData field
  //
  CopyMem (Type42Record->InterfaceTypeSpecificData, InterfaceData, InterfaceDataLen);

  //
  // Fill in InterfaceTypeSpecificData Protocol Count field
  //
  * (Type42Record->InterfaceTypeSpecificData + InterfaceDataLen) = 1;

  //
  // Fill in InterfaceTypeSpecificData Protocol Records field
  //
  * (Type42Record->InterfaceTypeSpecificData + InterfaceDataLen + 1) = ProtocolRecord->ProtocolIdentifier;
  * (Type42Record->InterfaceTypeSpecificData + InterfaceDataLen + 2) = ProtocolRecord->ProtocolDataLen;

  //
  // Fill in Redfish Protocol Data
  //
  CopyMem (
    Type42Record->InterfaceTypeSpecificData + InterfaceDataLen + 1 + 2, 
    RedfishProtocolData, ProtocolRecord->ProtocolDataLen
    );

  //
  // 5. Add Redfish interface data record to SMBIOS table 42
  //
  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }
  
  MemArrayMappedAddrSmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->Add (
                    Smbios,
                    NULL,
                    &MemArrayMappedAddrSmbiosHandle,
                    (EFI_SMBIOS_TABLE_HEADER*) Type42Record
                    );
  DEBUG ((EFI_D_INFO, "RedfishPlatformDxe: Smbios->Add() - %r\n", Status));
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

ON_EXIT:
  
  if (RedfishProtocolData != NULL) {
    FreePool (RedfishProtocolData);
  }

  if (ProtocolRecord != NULL) {
    FreePool (ProtocolRecord);
  }

  if (InterfaceData != NULL) {
    FreePool (InterfaceData);
  }

  if (Type42Record != NULL) {
    FreePool (Type42Record);
  }

  return Status;
}

/**
  Callback function executed when the ExitBootServices event group is signaled.

  @param[in]  Event    Event whose notification function is being invoked.
  @param[out] Context  Pointer to the buffer pass in.
**/
VOID
EFIAPI
RedfishPlatformDxeExitBootServicesEventNotify (
  IN  EFI_EVENT  Event,
  OUT VOID       *Context
  ) 
{
  mRedfishCredentialProtocol.StopService (&mRedfishCredentialProtocol);
}

/**
  Callback function executed when the EndOfDxe event group is signaled.

  @param[in]  Event    Event whose notification function is being invoked.
  @param[out] Context  Pointer to the buffer pass in.
**/
VOID
EFIAPI
RedfishPlatformDxeEndOfDxeEventNotify (
  IN  EFI_EVENT  Event,
  OUT VOID       *Context
  )
{
  EFI_STATUS                          Status;
  UINT8                               *SecureBootVar;

  //
  // Check Secure Boot status and lock Redfish service if Secure Boot is disabled.
  //
  Status = GetVariable2 (EFI_SECURE_BOOT_MODE_NAME, &gEfiGlobalVariableGuid, (VOID**)&SecureBootVar, NULL);
  if (EFI_ERROR (Status) || (*SecureBootVar != SECURE_BOOT_MODE_ENABLE)) {
    //
    // Secure Boot is disabled
    //
    mSecureBootDisabled = TRUE;

    mRedfishCredentialProtocol.StopService (&mRedfishCredentialProtocol);
  }

  //
  // Close event, so it will not be invoked again.
  //
  gBS->CloseEvent (Event);
}


EFI_STATUS
GetRedfishCredential (
  OUT  EFI_REDFISH_AUTH_METHOD            *AuthMethod,
  OUT  CHAR8                              **UserId,
  OUT  CHAR8                              **Password
  )
{
  UINTN                        UserIdSize;
  UINTN                        PasswordSize;

  UserIdSize   = AsciiStrSize ("admin");
  PasswordSize = AsciiStrSize ("pwd123456");

  //
  // AuthMethod.
  //
  *AuthMethod = AuthMethodHttpBasic;

  //
  // UserId.
  //
  *UserId = AllocateZeroPool (UserIdSize);
  if (*UserId == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (*UserId, "admin", UserIdSize);

  //
  // Password.
  //
  *Password = AllocateZeroPool (PasswordSize);
  if (*Password == NULL) {
    FreePool (*UserId);
    return EFI_OUT_OF_RESOURCES;
  }
  
  CopyMem (*Password, "pwd123456", PasswordSize);
  
  return EFI_SUCCESS;
}

EFI_STATUS
StopRedfishService (
  VOID
  )
{
  mStopRedfishService = TRUE;
  
  return EFI_SUCCESS;
}

/**
  Retrieve platform's Redfish authentication information.

  This functions returns the Redfish authentication method together with the user Id and
  password. 
  - For AuthMethodNone, the UserId and Password could be used for HTTP header authentication
    as defined by RFC7235.
  - For AuthMethodRedfishSession, the UserId and Password could be used for Redfish 
    session login as defined by  Redfish API specification (DSP0266).  

  Callers are responsible for and freeing the returned string storage. 
  
  @param[in]   This                Pointer to EFI_REDFISH_CREDENTIAL_PROTOCOL instance.
  @param[out]  AuthMethod          Type of Redfish authentication method.  
  @param[out]  UserId              The pointer to store the returned UserId string.  
  @param[out]  Password            The pointer to store the returned Password string.  

  @retval EFI_SUCCESS              Get the authentication information successfully.
  @retval EFI_ACCESS_DENIED        SecureBoot is disabled after EndOfDxe.
  @retval EFI_INVALID_PARAMETER    This or AuthMethod or UserId or Password is NULL.
  @retval EFI_OUT_OF_RESOURCES     There are not enough memory resources.
  @retval EFI_UNSUPPORTED          Unsupported authentication method is found.

**/
EFI_STATUS
EFIAPI
RedfishCredentialGetAuthInfo (
  IN     EFI_REDFISH_CREDENTIAL_PROTOCOL    *This,
     OUT EFI_REDFISH_AUTH_METHOD            *AuthMethod,
     OUT CHAR8                              **UserId,
     OUT CHAR8                              **Password
  )
{
  EFI_STATUS                   Status;

  if (This == NULL || AuthMethod == NULL || UserId == NULL || Password == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mStopRedfishService) {
    return EFI_ACCESS_DENIED;
  }
  
  if (mSecureBootDisabled) {
    Status = This->StopService (This);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "SecureBoot has been disabled, but failed to stop RedfishService - %r\n", Status));
      return Status;
    }
    
    return EFI_ACCESS_DENIED;
  }

  Status = GetRedfishCredential (
             AuthMethod,
             UserId,
             Password
             );
             
  return Status;
}

/**
  Notify the Redfish service provide to stop provide configuration service to this platform.

  This function should be called when the platfrom is about to leave the safe environment.
  It will notify the Redfish service provider to abort all logined session, and prohibit 
  further login with original auth info. GetAuthInfo() will return EFI_UNSUPPORTED once this
  function is returned.  

  @param[in]   This                Pointer to EFI_REDFISH_CREDENTIAL_PROTOCOL instance.

  @retval EFI_SUCCESS              Service has been stoped successfully.
  @retval EFI_INVALID_PARAMETER    This is NULL.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
RedfishCredentialStopService (
  IN     EFI_REDFISH_CREDENTIAL_PROTOCOL    *This
  )
{
  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return StopRedfishService ();
}

/**
  Main entry for this driver.

  @param ImageHandle     Image handle this driver.
  @param SystemTable     Pointer to SystemTable.

  @retval EFI_SUCESS     This function always complete successfully.

**/
EFI_STATUS
EFIAPI
RedfishPlatformDxeDriverEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;
  EFI_EVENT   EndOfDxeEvent;
  EFI_EVENT   ExitBootServiceEvent;

  Handle = NULL;

  //
  // Install the RedfishCredentialProtocol onto Handle.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiRedfishCredentialProtocolGuid,
                  &mRedfishCredentialProtocol,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // After EndOfDxe, if SecureBoot is disabled, Redfish Credential Protocol should return 
  // error code to caller to avoid the 3rd code to bypass Redfish Credential Protocol and 
  // retrieve userid/pwd directly. So, here, we create EndOfDxe Event to check SecureBoot
  // status.
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  RedfishPlatformDxeEndOfDxeEventNotify,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  //
  // After ExitBootServices, Redfish Credential Protocol should stop the service. 
  // So, here, we create ExitBootService Event to stop service.
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  RedfishPlatformDxeExitBootServicesEventNotify,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &ExitBootServiceEvent
                  );
  if (EFI_ERROR (Status)) {
    gBS->CloseEvent (EndOfDxeEvent);
    goto ON_ERROR;
  }
    
  //
  // Fill the SMBIOS table 42.
  //
  Status = RedfishFillSmbiosTable42 ();
  if (EFI_ERROR (Status)) {
    gBS->CloseEvent (EndOfDxeEvent);
    gBS->CloseEvent (ExitBootServiceEvent);
    goto ON_ERROR;
  }

  return EFI_SUCCESS;
  
ON_ERROR:
  
  gBS->UninstallMultipleProtocolInterfaces (
         Handle,
         &gEfiRedfishCredentialProtocolGuid,
         &mRedfishCredentialProtocol,
         NULL
         );
  
  return Status;
}
