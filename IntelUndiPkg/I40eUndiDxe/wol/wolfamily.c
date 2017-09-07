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




#if defined(WOL_40G)
WOL_MAC_TYPE const _WOL_40GBE[] = {
  WOL_MAKE_MACTYPE(WOL_40G, I40E_MAC_XL710),
#ifdef X722_SUPPORT
  WOL_MAKE_MACTYPE(WOL_40G, I40E_MAC_X722),
#endif
  0
};
#endif

extern WOL_STATUS _WolGetOffsetBitmask_PRO100(WOL_ADAPTER_HANDLE_TYPE Handle, UINT16 *Offset, UINT16 *Bitmask);
extern WOL_STATUS _WolGetOffsetBitmask_CORDOVA(WOL_ADAPTER_HANDLE_TYPE Handle, UINT16 *Offset, UINT16 *Bitmask);
extern WOL_STATUS _WolGetOffsetBitmask_KENAI(WOL_ADAPTER_HANDLE_TYPE Handle, UINT16 *Offset, UINT16 *Bitmask);
extern WOL_STATUS _WolGetOffsetBitmask_NAHUM(WOL_ADAPTER_HANDLE_TYPE Handle, UINT16 *Offset, UINT16 *Bitmask);
extern WOL_STATUS _WolGetOffsetBitmask_NAHUM2(WOL_ADAPTER_HANDLE_TYPE Handle, UINT16 *Offset, UINT16 *Bitmask);
extern WOL_STATUS _WolGetOffsetBitmask_BARTONHILLS(WOL_ADAPTER_HANDLE_TYPE Handle, UINT16 *Offset, UINT16 *Bitmask);
extern WOL_STATUS _WolGetOffsetBitmask_IXGBE(WOL_ADAPTER_HANDLE_TYPE Handle, UINT16 *Offset, UINT16 *Bitmask);
extern WOL_STATUS _WolGetOffsetBitmask_40GBE(WOL_ADAPTER_HANDLE_TYPE Handle, UINT16 *Offset, UINT16 *Bitmask);

_WOL_FAMILY_INFO_t const WOL_FAMILY_TABLE[] = {
#if defined(WOL_40G)
  { _WOL_40GBE,         _WolGetOffsetBitmask_40GBE          },
#endif /* WOL_40G */
  { 0,                  0                                   }
};

WOL_MAC_TYPE const WOL_APMPME_TABLE[] = {
  0
};

WOL_MAC_TYPE const WOL_LASER_TABLE[] = {
  0
};

