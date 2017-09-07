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
#ifndef __WOLIMPL_H
#define __WOLIMPL_H

#include <I40e.h>
#include <EepromConfig.h>
#define WOL_40G   4

typedef UNDI_PRIVATE_DATA *WOL_ADAPTER_HANDLE_TYPE;
typedef EFI_STATUS WOL_STATUS;

#define WOL_SUCCESS               EFI_SUCCESS
#define WOL_FEATURE_NOT_SUPPORTED EFI_UNSUPPORTED


#define WOL_MAC_TYPE                          UINT32
#define WOL_MAC_TYPE_EMPTY                    0x00000000
#define WOL_MAC_TYPE_UNKNOWN                  0xFFFFFFFF
#define WOL_MAKE_MACTYPE(SpeedClass, MacType) ((SpeedClass << 16) | (MacType & 0xFFFF))

typedef struct {
  UINT16 VendorId;
  UINT16 DeviceId;
  UINT16 SubVendorId;
  UINT16 SubDeviceId;
} _WOL_DEVICE_ID_t;

typedef struct {
  UINT16 VendorId;
  UINT16 DeviceId;
  UINT16 SubVendorId;
  UINT16 SubDeviceId;
  UINT8 WolInfo;
} _WOL_DEVICE_INFO_t;

typedef struct {
  WOL_MAC_TYPE const *Family;
  WOL_STATUS (*WolGetOffsetBitmask)(WOL_ADAPTER_HANDLE_TYPE Handle, UINT16 *Offset, UINT16 *Bitmask);
} _WOL_FAMILY_INFO_t;

WOL_MAC_TYPE _WolFindMacType(WOL_MAC_TYPE MacType, WOL_MAC_TYPE *MacTypes);
UINT8 _WolGetLanPort(WOL_ADAPTER_HANDLE_TYPE Handle);
void _WolGetDeviceId(WOL_ADAPTER_HANDLE_TYPE Handle, _WOL_DEVICE_ID_t *DeviceId);
BOOLEAN _WolIsFirstController(WOL_ADAPTER_HANDLE_TYPE Handle);
WOL_MAC_TYPE _WolGetMacType(WOL_ADAPTER_HANDLE_TYPE Handle);
UINT8 _WolGetFunction(WOL_ADAPTER_HANDLE_TYPE Handle);

WOL_STATUS _WolEepromRead16(WOL_ADAPTER_HANDLE_TYPE Handle, UINT16 Offset, UINT16 *Data);
WOL_STATUS _WolEepromWrite16(WOL_ADAPTER_HANDLE_TYPE Handle, UINT16 Offset, UINT16 Data);
WOL_STATUS _WolEepromUpdateChecksum(WOL_ADAPTER_HANDLE_TYPE Handle);

#endif /* __WOLIMPL_H */
