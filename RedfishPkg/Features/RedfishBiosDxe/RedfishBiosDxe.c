/** @file
  Redfish configure handler driver to manipulate the Redfish BIOS Attributes
  related resources.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "RedfishBiosDxe.h"

REDFISH_ODATA_TYPE_MAPPING    mOdataTypeList[] = {
  {"ComputerSystem", "#ComputerSystem.v1_1_0.ComputerSystem"},
  {"ComputerSystem", "#ComputerSystem.v1_5_1.ComputerSystem"},
  {"Bios", "#Bios.v1_0_2.Bios"},
  {"Bios", "#Bios.v1_0_4.Bios"},
  {"AttributeRegistry", "#AttributeRegistry.v1_0_0.AttributeRegistry"},
  {"AttributeRegistry", "#AttributeRegistry.v1_2_1.AttributeRegistry"},
  {"@Redfish.Settings", "#Settings.v1_0_4.Settings"},
  {"SettingsObject", "#Bios.v1_0_2.Bios"},
  {"SettingsObject", "#Bios.v1_0_4.Bios"}
};

/**
  Get the BIOS resource for this platform from Redfish service.

  @param[in]  RedfishService        The REDFISH_SERVICE instance to access the Redfish servier.

  @return     Redfish BIOS payload or NULL if operation failed.

**/
REDFISH_PAYLOAD
RedfishGetBiosPayload (
  IN REDFISH_SERVICE                 RedfishService
  )
{
  EFI_STATUS                      Status;
  CHAR8                           *RedPath;
  REDFISH_RESPONSE                RedResponse;

  ASSERT (RedfishService != NULL);

  RedPath = RedfishBuildPathWithSystemUuid ("/v1/Systems[UUID~%g]/Bios");
  if (RedPath == NULL) {
    return NULL;
  }

  DEBUG ((EFI_D_INFO, "[Redfish] RedfishGetBiosPayload for RedPath: %a\n", RedPath));

  Status = RedfishGetByService (RedfishService, RedPath, &RedResponse);
  FreePool (RedPath);
  if (EFI_ERROR (Status) || !RedfishIsValidOdataType (RedResponse.Payload, "Bios", mOdataTypeList, ARRAY_SIZE (mOdataTypeList))) {

    DEBUG ((EFI_D_ERROR, "[%a:%d] Error happened!\n",__FUNCTION__, __LINE__));
    if (RedResponse.Payload != NULL) {
      DEBUG ((EFI_D_ERROR, "[%a:%d] Error Message:\n",__FUNCTION__, __LINE__));
      RedfishDumpPayload (RedResponse.Payload);

      RedfishFreeResponse (
        NULL,
        0,
        NULL,
        RedResponse.Payload
        );
    }

    return NULL;
  }

  RedfishFreeResponse (
    RedResponse.StatusCode,
    RedResponse.HeaderCount,
    RedResponse.Headers,
    NULL
    );

  return RedResponse.Payload;
}

/**
  Get the ComputerSystem resource from Redfish service.

  This function will retrive the ComputerSystem from Redfish server, the ComputerSystem
  member is selected by comparing the ComputerSystem/UUID property and the system GUID.

  @param[in]  RedfishService        The REDFISH_SERVICE instance to access the Redfish servier.

  @return     Redfish ComputerSystem payload or NULL if operation failed.
**/
REDFISH_PAYLOAD
RedfishGetComputerSystemPayload (
  IN REDFISH_SERVICE                        RedfishService
  )
{
  EFI_STATUS                      Status;
  CHAR8                           *RedPath;
  REDFISH_RESPONSE                RedResponse;

  ASSERT (RedfishService != NULL);

  RedPath = RedfishBuildPathWithSystemUuid ("/v1/Systems[UUID~%g]");
  if (RedPath == NULL) {
    return NULL;
  }

  DEBUG ((EFI_D_INFO, "[Redfish] RedfishGetComputerSystemPayload for RedPath: %a\n", RedPath));

  Status = RedfishGetByService (RedfishService, RedPath, &RedResponse);
  FreePool (RedPath);
  if (EFI_ERROR (Status) || !RedfishIsValidOdataType (RedResponse.Payload, "ComputerSystem", mOdataTypeList, ARRAY_SIZE (mOdataTypeList))) {
    DEBUG ((EFI_D_ERROR, "[%a:%d] Error happened!\n",__FUNCTION__, __LINE__));
    if (RedResponse.Payload != NULL) {
      DEBUG ((EFI_D_ERROR, "[%a:%d] Error Message:\n",__FUNCTION__, __LINE__));
      RedfishDumpPayload (RedResponse.Payload);

      RedfishFreeResponse (
        NULL,
        0,
        NULL,
        RedResponse.Payload
        );
    }

    return NULL;
  }

  RedfishFreeResponse (
    RedResponse.StatusCode,
    RedResponse.HeaderCount,
    RedResponse.Headers,
    NULL
    );

  return RedResponse.Payload;
}

/**
  Extract the current Attribute Registry to JSON represent text data.

  @param[out]  DataSize       The buffer size of the Data.
  @param[out]  Data           The buffer point saved the returned JSON text.

  @retval EFI_SUCCESS               The operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES      The request could not be completed due to a lack of resources.

**/
EFI_STATUS
RedfishExtractAttributeRegistry (
  OUT  UINTN             *DataSize,
  OUT  VOID              **Data
 )
{
  EFI_STATUS                      Status;

  ASSERT (DataSize != NULL);
  ASSERT (Data != NULL);

  *DataSize = 0;
  Status = HiiRedfishExtractAttributeRegistryJson (DataSize, NULL);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return Status;
  }

  *Data = AllocateZeroPool (*DataSize);
  if (Data == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_ERROR;
  }

  Status = HiiRedfishExtractAttributeRegistryJson (DataSize, *Data);
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  return EFI_SUCCESS;

ON_ERROR:
  if (*Data != NULL) {
    FreePool (*Data);
    *Data = NULL;
  }

  *DataSize = 0;
  return Status;
}

/**
  Generate a new AttributeRegistry resource from system and patch it to Redfish service.

  @param[in]  RedfishService        The REDFISH_SERVICE instance to access the Redfish servier.
  @param[in]  BiosPayload           The Redfish payload of the current settings BIOS resource.

  @retval EFI_SUCCESS               The operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES      The request could not be completed due to a lack of resources.
  @retval EFI_DEVICE_ERROR          Failed to generate a AttributeRegistry from system.

**/
EFI_STATUS
RedfishPatchAttributeRegistry (
  IN REDFISH_SERVICE                   RedfishService,
  IN REDFISH_PAYLOAD                   *BiosPayload
  )
{
  EFI_STATUS                      Status;
  REDFISH_RESPONSE                AttrRegResponse;
  EDKII_JSON_VALUE                AttrRegUriJsonValue;
  CONST CHAR8                     *AttrRegUri;
  CHAR8                           *Buffer;
  UINTN                           BufLen;
  REDFISH_RESPONSE                PatchResponse;

  ASSERT (RedfishService != NULL);
  ASSERT (BiosPayload != NULL);

  Buffer = NULL;
  AttrRegUriJsonValue = NULL;
  AttrRegUri = NULL;

  Status = RedfishGetByPayload (BiosPayload, "AttributeRegistry", &AttrRegResponse);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[%a:%d] Error happened!\n",__FUNCTION__, __LINE__));
    if (AttrRegResponse.Payload != NULL) {
      DEBUG ((EFI_D_ERROR, "[%a:%d] Error Message:\n",__FUNCTION__, __LINE__));
      RedfishDumpPayload (AttrRegResponse.Payload);
    }

    goto ON_EXIT;
  }

  ASSERT (AttrRegResponse.Payload != NULL);
  AttrRegUriJsonValue = RedfishJsonInPayload (AttrRegResponse.Payload);
  if (AttrRegUriJsonValue == NULL || !JsonValueIsString (JsonObjectGetValue(AttrRegUriJsonValue, "AttributeRegistry"))) {
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  AttrRegUri = JsonValueGetAsciiString (JsonObjectGetValue(AttrRegUriJsonValue, "AttributeRegistry"));
  if (AttrRegUri == NULL) {
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  Status = RedfishExtractAttributeRegistry (&BufLen, &Buffer);
  if (EFI_ERROR (Status)) {
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  ASSERT (BufLen != 0 && Buffer != NULL);

  Status = RedfishPatchToUri (RedfishService, AttrRegUri, Buffer, &PatchResponse);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[%a:%d] Error happened!\n",__FUNCTION__, __LINE__));
    if (PatchResponse.Payload != NULL) {
      DEBUG ((EFI_D_ERROR, "[%a:%d] Error Message:\n",__FUNCTION__, __LINE__));
      RedfishDumpPayload (PatchResponse.Payload);
    }
  }

  RedfishFreeResponse (
    PatchResponse.StatusCode,
    PatchResponse.HeaderCount,
    PatchResponse.Headers,
    PatchResponse.Payload
    );

  if (Buffer != NULL) {
    FreePool (Buffer);
  }

ON_EXIT:
  RedfishFreeResponse (
    AttrRegResponse.StatusCode,
    AttrRegResponse.HeaderCount,
    AttrRegResponse.Headers,
    AttrRegResponse.Payload
    );

  return Status;
}

/**
  This function will get the pending Settings of Redfish BIOS resource, and route the
  attribute configurations to the system firmware.

  @param[in]  BiosPayload           The Redfish payload of the pending Settings resource.

  @retval EFI_SUCCESS               The operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES      The request could not be completed due to a lack of resources.
  @retval EFI_UNSUPPORTED           Unsupported pending settings received.

**/
EFI_STATUS
RedfishRoutePendingSettings (
  IN REDFISH_PAYLOAD                 *BiosPayload
  )
{
  EFI_STATUS                      Status;
  REDFISH_RESPONSE                RedResponse;
  CHAR8                           *Result;

  ASSERT (BiosPayload != NULL);

  Result = NULL;

  Status = RedfishGetByPayload (BiosPayload, "@Redfish.Settings/SettingsObject", &RedResponse);
  if (EFI_ERROR (Status) || !RedfishIsValidOdataType (RedResponse.Payload, "SettingsObject", mOdataTypeList, ARRAY_SIZE (mOdataTypeList))) {

    DEBUG ((EFI_D_ERROR, "[%a:%d] Error happened!\n",__FUNCTION__, __LINE__));
    if (RedResponse.Payload != NULL) {
      DEBUG ((EFI_D_ERROR, "[%a:%d] Error Message:\n",__FUNCTION__, __LINE__));
      RedfishDumpPayload (RedResponse.Payload);
    }

    if (!EFI_ERROR (Status)) {
      Status = EFI_UNSUPPORTED;
    }
    goto ON_EXIT;
  }

  ASSERT (RedResponse.Payload != NULL);

  Status = HiiRedfishRouteBiosSettingsJson (RedfishJsonInPayload (RedResponse.Payload), NULL, &Result);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "RedfishRoutePendingSettings Failed: %a\n", Result));
  }

ON_EXIT:
  RedfishFreeResponse (
    RedResponse.StatusCode,
    RedResponse.HeaderCount,
    RedResponse.Headers,
    RedResponse.Payload
    );

  if (Result != NULL) {
    FreePool (Result);
  }

  return Status;
}

/**
  This function cleanup the pending Settings of Redfish BIOS resource, in order to avoid
  duplicate configuration in future.

  @param[in]  RedfishService        The REDFISH_SERVICE instance to access the Redfish servier.
  @param[in]  BiosPayload           The Redfish payload of the pending settings BIOS resource.

  @retval EFI_SUCCESS               The operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES      The request could not be completed due to a lack of resources.
  @retval EFI_INVALID_PARAMETER     RedfishService is NULL or BiosPayload is NULL.
  @retval EFI_DEVICE_ERROR          Failed to cleanup the pending Settings resource.

**/
EFI_STATUS
RedfishCleanupPendingSettings (
  IN REDFISH_SERVICE                   RedfishService,
  IN REDFISH_PAYLOAD                   *BiosPayload
  )
{
  EFI_STATUS                      Status;
  REDFISH_RESPONSE                GetResponse;
  EDKII_JSON_VALUE                EmptyAttributeJsonValue;
  REDFISH_PAYLOAD                 EmptyAttributePayload;
  REDFISH_RESPONSE                PatchResponse;

  if (BiosPayload == NULL || RedfishService == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EmptyAttributeJsonValue = NULL;
  EmptyAttributePayload = NULL;

  Status = RedfishGetByPayload (BiosPayload, "@Redfish.Settings/SettingsObject", &GetResponse);
  if (EFI_ERROR (Status) || !RedfishIsValidOdataType (GetResponse.Payload, "SettingsObject", mOdataTypeList, ARRAY_SIZE (mOdataTypeList))) {
    DEBUG ((EFI_D_ERROR, "[%a:%d] Error happened!\n",__FUNCTION__, __LINE__));
    if (GetResponse.Payload != NULL) {
      DEBUG ((EFI_D_ERROR, "[%a:%d] Error Message:\n",__FUNCTION__, __LINE__));
      RedfishDumpPayload (GetResponse.Payload);
    }

    if (!EFI_ERROR (Status)) {
      Status = EFI_UNSUPPORTED;
    }

    goto ON_EXIT;
  }

  ASSERT (GetResponse.Payload != NULL);

  EmptyAttributeJsonValue = TextToJson ("{\"Attributes\":{}}");
  if (EmptyAttributeJsonValue == NULL) {
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  EmptyAttributePayload = RedfishCreatePayload (EmptyAttributeJsonValue, RedfishService);
  JsonValueFree (EmptyAttributeJsonValue);
  if (EmptyAttributePayload == NULL) {
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  Status = RedfishPatchToPayload (GetResponse.Payload, EmptyAttributePayload, &PatchResponse);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[%a:%d] Error happened!\n",__FUNCTION__, __LINE__));
    if (PatchResponse.Payload != NULL) {
      DEBUG ((EFI_D_ERROR, "[%a:%d] Error Message:\n",__FUNCTION__, __LINE__));
      RedfishDumpPayload (PatchResponse.Payload);
    }
  }

  RedfishFreeResponse (
    PatchResponse.StatusCode,
    PatchResponse.HeaderCount,
    PatchResponse.Headers,
    PatchResponse.Payload
    );

  if (EmptyAttributePayload != NULL) {
    RedfishCleanupPayload (EmptyAttributePayload);
  }

ON_EXIT:
  RedfishFreeResponse (
    GetResponse.StatusCode,
    GetResponse.HeaderCount,
    GetResponse.Headers,
    GetResponse.Payload
    );

  return Status;
}

/**
  This function will extract the current configuration data from the system firmware and
  convert it to Redfish BIOS resource.

  @param[out]  DataSize       The buffer size of the Data.
  @param[out]  Data           The buffer point saved the returned JSON text.

  @retval EFI_SUCCESS               The operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES      The request could not be completed due to a lack of resources.

**/
EFI_STATUS
RedfishExtractBios (
  OUT  UINTN             *DataSize,
  OUT  VOID              **Data
 )
{
  EFI_STATUS                      Status;

  ASSERT (DataSize != NULL);
  ASSERT (Data != NULL);

  *DataSize = 0;
  Status = HiiRedfishExtractBiosJson (DataSize, NULL);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return Status;
  }

  *Data = AllocateZeroPool (*DataSize);
  if (Data == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_ERROR;
  }

  Status = HiiRedfishExtractBiosJson (DataSize, *Data);
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  return EFI_SUCCESS;

ON_ERROR:
  if (*Data != NULL) {
    FreePool (*Data);
    *Data = NULL;
  }

  *DataSize = 0;
  return Status;
}

/**
  This function will patch the current firmware configurations to the BIOS resource
  in Redfish server.

  @param[in]  RedfishService    The REDFISH_SERVICE instance to access the Redfish servier.
  @param[in]  BiosPayload       The payload of Redfish BIOS current setting resource.

  @retval EFI_SUCCESS               The operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES      The request could not be completed due to a lack of resources.

**/
EFI_STATUS
RedfishPatchBios (
  IN REDFISH_SERVICE                   RedfishService,
  IN REDFISH_PAYLOAD                   *BiosPayload
  )
{
  EFI_STATUS                      Status;
  EDKII_JSON_VALUE                NewBiosJsonValue;
  REDFISH_PAYLOAD                 NewBiosPayload;
  CHAR8                           *Buffer;
  UINTN                           BufLen;
  REDFISH_RESPONSE                RedResponse;

  ASSERT (BiosPayload != NULL);

  Buffer = NULL;
  NewBiosJsonValue = NULL;
  NewBiosPayload = NULL;

  Status = RedfishExtractBios (&BufLen, &Buffer);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }
  ASSERT (BufLen != 0 && Buffer != NULL);

  NewBiosJsonValue = TextToJson (Buffer);
  if (NewBiosJsonValue == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  NewBiosPayload = RedfishCreatePayload (NewBiosJsonValue, RedfishService);
  JsonValueFree (NewBiosJsonValue);
  if (NewBiosPayload == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  Status = RedfishPatchToPayload (BiosPayload, NewBiosPayload, &RedResponse);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[%a:%d] Error happened!\n",__FUNCTION__, __LINE__));
    if (RedResponse.Payload != NULL) {
      DEBUG ((EFI_D_ERROR, "[%a:%d] Error Message:\n",__FUNCTION__, __LINE__));
      RedfishDumpPayload (RedResponse.Payload);
    }
  }

  RedfishFreeResponse (
    RedResponse.StatusCode,
    RedResponse.HeaderCount,
    RedResponse.Headers,
    RedResponse.Payload
    );

ON_EXIT:
  if (NewBiosPayload != NULL) {
    RedfishCleanupPayload (NewBiosPayload);
  }

  if (Buffer != NULL) {
    FreePool (Buffer);
  }

  return Status;
}

/**
  Callback function executed when the ReadyToBoot event group is signaled.

  @param[in]  Event    Event whose notification function is being invoked.
  @param[in]  Context  Pointer to the REDFISH_BIOS_PRIVATE_DATA buffer.

**/
VOID
EFIAPI
RedfishBiosReadyToBootCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                      Status;
  REDFISH_SERVICE                 RedfishService;
  REDFISH_PAYLOAD                 SystemPayload;
  REDFISH_PAYLOAD                 BiosPayload;
  REDFISH_BIOS_PRIVATE_DATA       *Private;

  if (Context == NULL) {
    return;
  }

  Private = (REDFISH_BIOS_PRIVATE_DATA *) Context;
  RedfishService = Private->RedfishService;
  if (RedfishService == NULL) {
    gBS->CloseEvent(Event);
    return;
  }

  SystemPayload = NULL;
  BiosPayload = NULL;

  SystemPayload = RedfishGetComputerSystemPayload (RedfishService);
  if (SystemPayload == NULL) {
    goto ON_EXIT;
  }

  BiosPayload = RedfishGetBiosPayload (RedfishService);
  if (BiosPayload == NULL) {
    goto ON_EXIT;
  }

  Status = RedfishPatchAttributeRegistry (RedfishService, BiosPayload);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  Status = RedfishRoutePendingSettings (BiosPayload);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Redfish: Failed to apply the Bios Attributes settings.\n"));
  }

  Status = RedfishCleanupPendingSettings (RedfishService, BiosPayload);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Redfish: Failed to cleanup the pending setting.\n"));
  }

  Status = RedfishPatchBios (RedfishService, BiosPayload);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Redfish: Failed to extract the Bios Attributes settings.\n"));
  }

ON_EXIT:
  gBS->CloseEvent(Event);
  RedfishCleanupPayload (BiosPayload);
  RedfishCleanupPayload (SystemPayload);
  RedfishCleanupService (RedfishService);
  Private->RedfishService = NULL;
}

/**
  Initialize a Redfish configure handler.

  This function will be called by the Redfish config driver to initialize each Redfish configure
  handler.

  @param[in]   This                    Pointer to EFI_REDFISH_CONFIG_HANDLER_PROTOCOL instance.
  @param[in]   DriverBindingHandle     The driver binding handle used to open the REST EX service.
                                       This parameter is passed to config handler protocol for
                                       building the UEFI driver model relationship.
  @param[in]   ControllerHandle        The controller which has the REST EX service installed.
  @param[in]   RedfishData             The Redfish service data.

  @retval EFI_SUCCESS                  The handler has been initialized successfully.
  @retval EFI_DEVICE_ERROR             Failed to create or configure the REST EX protocol instance.
  @retval EFI_ALREADY_STARTED          This handler has already been initialized.
  @retval Other                        Error happens during the initialization.

**/
EFI_STATUS
EFIAPI
RedfishBiosInit (
  IN  EFI_REDFISH_CONFIG_HANDLER_PROTOCOL       *This,
  IN  EFI_HANDLE                                DriverBindingHandle,
  IN  EFI_HANDLE                                ControllerHandle,
  IN  REDFISH_OVER_IP_PROTOCOL_DATA             *RedfishData
  )
{
  EFI_STATUS                      Status;
  REDFISH_BIOS_PRIVATE_DATA       *Private;

  Private = REDFISH_BIOS_PRIVATE_DATA_FROM_PROTOCOL (This);
  if (Private->RedfishService != NULL) {
    return EFI_ALREADY_STARTED;
  }

  Private->RedfishService = RedfishCreateService (DriverBindingHandle, ControllerHandle, RedfishData);
  if (Private->RedfishService == NULL) {
    return EFI_DEVICE_ERROR;
  }

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  RedfishBiosReadyToBootCallback,
                  (VOID*) Private,
                  &gEfiEventReadyToBootGuid,
                  &Private->Event
                  );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  return EFI_SUCCESS;

ON_EXIT:
  RedfishCleanupService (Private->RedfishService);
  Private->RedfishService = NULL;
  return Status;
}

/**
  Stop a Redfish configure handler.

  @param[in]   This                Pointer to EFI_REDFISH_CONFIG_HANDLER_PROTOCOL instance.

  @retval EFI_SUCCESS              This handler has been stoped successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
RedfishBiosStop (
  IN  EFI_REDFISH_CONFIG_HANDLER_PROTOCOL       *This
  )
{
  REDFISH_BIOS_PRIVATE_DATA     *Private;

  Private = REDFISH_BIOS_PRIVATE_DATA_FROM_PROTOCOL (This);

  if (Private->Event != NULL) {
    gBS->CloseEvent (Private->Event);
    Private->Event = NULL;
  }

  if (Private->RedfishService != NULL) {
    RedfishCleanupService (Private->RedfishService);
    Private->RedfishService = NULL;
  }

  return EFI_SUCCESS;
}

/**
  This is the declaration of an EFI image entry point. This entry point is
  the same for UEFI Applications, UEFI OS Loaders, and UEFI Drivers including
  both device drivers and bus drivers.

  @param[in]  ImageHandle           The firmware allocated handle for the UEFI image.
  @param[in]  SystemTable           A pointer to the EFI System Table.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval Others                An unexpected error occurred.
**/
EFI_STATUS
EFIAPI
RedfishBiosDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status;
  REDFISH_BIOS_PRIVATE_DATA      *Private;

  Private = AllocateZeroPool (sizeof (REDFISH_BIOS_PRIVATE_DATA));
  if (Private == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Private->Signature          = REDFISH_BIOS_PRIVATE_DATA_SIGNATURE;
  Private->ConfigHandler.Init = RedfishBiosInit;
  Private->ConfigHandler.Stop = RedfishBiosStop;

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gEfiRedfishConfigHandlerProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &Private->ConfigHandler
                  );
  if (EFI_ERROR (Status)) {
    FreePool (Private);
  }

  return Status;
}

