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

  void _WolGetDeviceId(WOL_ADAPTER_HANDLE_TYPE Handle, _WOL_DEVICE_ID_t *DeviceId)
  {
    DeviceId->VendorId = Handle->NicInfo.Hw.vendor_id;
    DeviceId->DeviceId = Handle->NicInfo.Hw.device_id;
    DeviceId->SubVendorId = Handle->NicInfo.Hw.subsystem_vendor_id;
    DeviceId->SubDeviceId = Handle->NicInfo.Hw.subsystem_device_id;
  }

  UINT8 _WolGetLanPort(WOL_ADAPTER_HANDLE_TYPE Handle)
  {
    return Handle->NicInfo.PhysicalPortNumber;
  }

  BOOLEAN _WolIsFirstController(WOL_ADAPTER_HANDLE_TYPE Handle)
  {
    return TRUE;
  }

  WOL_MAC_TYPE _WolGetMacType(WOL_ADAPTER_HANDLE_TYPE Handle)
  {
    return WOL_MAKE_MACTYPE(WOL_40G, Handle->NicInfo.Hw.mac.type);
  }

  WOL_STATUS _WolEepromRead16(WOL_ADAPTER_HANDLE_TYPE Handle, UINT16 Offset, UINT16 *Data)
  {
    if (i40e_read_nvm_word(&Handle->NicInfo.Hw, Offset, Data) == I40E_SUCCESS) {
      return EFI_SUCCESS;
    } else {
      return EFI_DEVICE_ERROR;
    }
  }

  WOL_STATUS _WolEepromWrite16(WOL_ADAPTER_HANDLE_TYPE Handle, UINT16 Offset, UINT16 Data)
  {
    if (I40eWriteNvmBufferExt(&Handle->NicInfo, Offset, 1, &Data) == I40E_SUCCESS) {
      return EFI_SUCCESS;
    } else {
      return EFI_DEVICE_ERROR;
    }
  }

  WOL_STATUS _WolEepromUpdateChecksum(WOL_ADAPTER_HANDLE_TYPE Handle)
  {
    return EepromUpdateChecksum(Handle);
  }

  UINT8 _WolGetFunction(WOL_ADAPTER_HANDLE_TYPE Handle)
  {
    return Handle->NicInfo.Hw.pf_id;
  }

