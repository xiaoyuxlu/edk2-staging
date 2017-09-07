/******************************************************************************
 * INTEL CONFIDENTIAL                                                         *
 * Copyright 2014-2016 Intel Corporation All Rights Reserved.                 *
 *                                                                            *
 * The source code contained or described herein and all documents related    *
 * to the source code ("Material") are owned by Intel Corporation or          *
 * its suppliers or licensors. Title to the Material remains                  *
 * with Intel Corporation or its suppliers and licensors.                     *
 * The Material contains trade secrets and proprietary and confidential       *
 * information of Intel or its suppliers and licensors.                       *
 * The Material is protected by worldwide copyright and trade secret laws     *
 * and treaty provisions. No part of the Material may be used, copied,        *
 * reproduced, modified, published, uploaded, posted, transmitted,            *
 * distributed, or disclosed in any way without Intel's                       *
 * prior express written permission.                                          *
 *                                                                            *
 * No license under any patent, copyright, trade secret or other              *
 * intellectual property right is granted to or conferred upon you            *
 * by disclosure or delivery of the Materials, either expressly,              *
 * by implication, inducement, estoppel or otherwise.                         *
 * Any license under such intellectual property rights                        *
 * must be express and approved by Intel in writing.                          *
 ******************************************************************************/
#include <wol.h>


extern WOL_STATUS _WolEnableLaser(WOL_ADAPTER_HANDLE_TYPE Handle, BOOLEAN Enable);
extern BOOLEAN _WolGetInfoFromEeprom_10G(WOL_ADAPTER_HANDLE_TYPE Handle);
extern BOOLEAN _WolGetInfoFromEeprom_40G(WOL_ADAPTER_HANDLE_TYPE Handle);

static BOOLEAN _WolIsDevInfoEmpty(_WOL_DEVICE_INFO_t const *DeviceInfo)
{
  return DeviceInfo->VendorId == 0 && DeviceInfo->DeviceId == 0 &&
         DeviceInfo->SubVendorId == 0 && DeviceInfo->SubDeviceId == 0;
}

static BOOLEAN _WolMatchId(UINT16 Id, UINT16 Pattern)
{
  return Id == Pattern || Pattern == 0xFFFF;
}

static BOOLEAN _WolMatchDeviceId(
  _WOL_DEVICE_ID_t *DeviceId,
  _WOL_DEVICE_ID_t *Pattern
) {
  return _WolMatchId(DeviceId->VendorId, Pattern->VendorId) &&
         _WolMatchId(DeviceId->DeviceId, Pattern->DeviceId) &&
         _WolMatchId(DeviceId->SubVendorId, Pattern->SubVendorId) &&
         _WolMatchId(DeviceId->SubDeviceId, Pattern->SubDeviceId);
}

static _WOL_DEVICE_INFO_t *_WolFindDeviceInfo(
  _WOL_DEVICE_ID_t *DeviceId,
  _WOL_DEVICE_INFO_t *DeviceInfoTable
) {
  while (!_WolIsDevInfoEmpty(DeviceInfoTable)) {
    if (_WolMatchDeviceId(DeviceId, (_WOL_DEVICE_ID_t *)DeviceInfoTable)) {
      return DeviceInfoTable;
    }
    ++DeviceInfoTable;
  }

  return NULL;
}

static BOOLEAN _WolGetInfo(WOL_ADAPTER_HANDLE_TYPE Handle, _WOL_DEVICE_INFO_t const *DeviceInfo)
{
  switch (DeviceInfo->WolInfo) {
    case 1: /* WOL is supported on the first port on the NIC. */
      /* The first port on the NIC means the first port
       * on the first port controller. As many 4-port legacy 1G NICs are
       * equipped with two port controllers we have to account for that.
       */
      if (_WolIsFirstController(Handle)) {
        return _WolGetLanPort(Handle) == 0;
      } else {
        return FALSE;
      }

    case 0xF: /* WOL is supported on all ports. */
        return TRUE;

    case 0: /* WOL is not supported at all. */
      return FALSE;
  }

  return FALSE;
}

WOL_MAC_TYPE _WolFindMacType(WOL_MAC_TYPE MacType, WOL_MAC_TYPE *MacTypes)
{
  while (WOL_MAC_TYPE_EMPTY != *MacTypes) {
    if (MacType == *MacTypes) {
      return MacType;
    }
    ++MacTypes;
  }

  return WOL_MAC_TYPE_EMPTY;
}

BOOLEAN WolIsWakeOnLanSupported(WOL_ADAPTER_HANDLE_TYPE Handle)
{
#if defined(WOL_40G)
{
  /* In case of FVL read WOL configuration from EEPROM */
  extern WOL_MAC_TYPE const _WOL_40GBE[];
  if (_WolFindMacType(_WolGetMacType(Handle), (WOL_MAC_TYPE *)_WOL_40GBE)) {
    return _WolGetInfoFromEeprom_40G(Handle);
  }
}
#endif
  {
    extern _WOL_DEVICE_INFO_t const WOL_DEVICE_INFO_TABLE[];
    _WOL_DEVICE_ID_t DeviceId;
    _WOL_DEVICE_INFO_t *DeviceInfo;

    _WolGetDeviceId(Handle, &DeviceId);


    /* Handle devices listed in the WOL_DEVICE_INFO_TABLE generated based on INF
     * files from NDIS drivers.
     */
    DeviceInfo = _WolFindDeviceInfo(&DeviceId, (_WOL_DEVICE_INFO_t *)WOL_DEVICE_INFO_TABLE);

    if (NULL != DeviceInfo) {
      return _WolGetInfo(Handle, DeviceInfo);
    }
  }

  return FALSE;
}

WOL_STATUS
_WolGetOffsetBitmask(
  IN    WOL_ADAPTER_HANDLE_TYPE     Handle,
  OUT   UINT16                      *Offset,
  OUT   UINT16                      *Bitmask
  )
{
  extern _WOL_FAMILY_INFO_t const WOL_FAMILY_TABLE[];
  _WOL_FAMILY_INFO_t const *FamilyTable;

  for (FamilyTable = WOL_FAMILY_TABLE; FamilyTable->Family; ++FamilyTable) {
    if (_WolFindMacType(_WolGetMacType(Handle), (WOL_MAC_TYPE *)FamilyTable->Family)) {
      return FamilyTable->WolGetOffsetBitmask(Handle, Offset, Bitmask);
    }
  }

  return WOL_FEATURE_NOT_SUPPORTED;
}

WOL_STATUS
WolGetWakeOnLanStatus(
  IN    WOL_ADAPTER_HANDLE_TYPE     Handle,
  OUT   BOOLEAN                    *WolStatus
  )
{
  WOL_STATUS  Status;
  UINT16      Offset;
  UINT16      Value  = 0;
  UINT16      Mask   = 0;

  if (!WolIsWakeOnLanSupported(Handle)) {
    return WOL_FEATURE_NOT_SUPPORTED;
  }

  Status = _WolGetOffsetBitmask(Handle, &Offset, &Mask);
  if (Status != WOL_SUCCESS) {
    return Status;
  }

  Status = _WolEepromRead16(Handle, Offset, &Value);
  if (Status != WOL_SUCCESS) {
    return Status;
  }

  *WolStatus = (Value & Mask) != 0;
  return WOL_SUCCESS;
}

WOL_STATUS_EX
WolGetWakeOnLanStatusEx(
  IN WOL_ADAPTER_HANDLE_TYPE Handle
) {
  BOOLEAN WolEnabled;

  if (WolIsWakeOnLanSupported(Handle) &&
      (WOL_SUCCESS == WolGetWakeOnLanStatus(Handle, &WolEnabled))) {
    return WolEnabled ? WOL_ENABLE : WOL_DISABLE;
  } else {
    return WOL_NA;
  }
}

WOL_STATUS
WolEnableApm(
  IN    WOL_ADAPTER_HANDLE_TYPE     Handle,
  IN    BOOLEAN                     Enable
  )
{
  WOL_STATUS  Status;
  UINT16      Offset;
  UINT16      Value  = 0;
  UINT16      Mask   = 0;

  Status = _WolGetOffsetBitmask(Handle, &Offset, &Mask);
  if (Status != WOL_SUCCESS) {
    return Status;
  }

  Status = _WolEepromRead16(Handle, Offset, &Value);
  if (Status != WOL_SUCCESS) {
    return Status;
  }

  if (Enable) {
    Value |= Mask;
  } else {
    Value &= ~Mask;
  }

  Status = _WolEepromWrite16(Handle, Offset, Value);
  if (Status != WOL_SUCCESS) {
    return Status;
  }

  return _WolEepromUpdateChecksum(Handle);
}

WOL_STATUS
WolEnableApmPme(
  IN    WOL_ADAPTER_HANDLE_TYPE     Handle,
  IN    BOOLEAN                     Enable
  )
{

  return WOL_FEATURE_NOT_SUPPORTED;
}

WOL_STATUS
WolEnableWakeOnLan (
  IN    WOL_ADAPTER_HANDLE_TYPE     Handle,
  IN    BOOLEAN                     Enable
  )
{
  if (WolIsWakeOnLanSupported(Handle)) {
    WOL_STATUS Status;


    /* Enable/Disable Apm to enable/disable Wol */
    Status = WolEnableApm(Handle, Enable);
    if (Status != WOL_SUCCESS) {
      return Status;
    }


    return Status;
  }
  return WOL_FEATURE_NOT_SUPPORTED;
}

WOL_STATUS_EX
WolEnableWakeOnLanEx(
  IN WOL_ADAPTER_HANDLE_TYPE Handle,
  IN BOOLEAN Enable
) {
  /* We don't check result from WolEnableWakeOnLan() because if it fails,
   * the WolGetWakeOnLanStatusEx() will return WOL_NA anyway.
   */
  WolEnableWakeOnLan(Handle, Enable);
  return WolGetWakeOnLanStatusEx(Handle);
}

