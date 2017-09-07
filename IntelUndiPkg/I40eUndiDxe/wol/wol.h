/******************************************************************************
 * INTEL CONFIDENTIAL                                                         *
 * Copyright 2014-2014 Intel Corporation All Rights Reserved.                 *
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
#ifndef __WOL_H
#define __WOL_H

#include <wolimpl.h>

#define WOL_DISABLE   0x00
#define WOL_ENABLE    0x01
#define WOL_NA        0x02
#define WOL_STATUS_EX UINT8


BOOLEAN
WolIsWakeOnLanSupported(
  IN    WOL_ADAPTER_HANDLE_TYPE     Handle
);

WOL_STATUS
WolGetWakeOnLanStatus(
  IN    WOL_ADAPTER_HANDLE_TYPE     Handle,
  OUT   BOOLEAN                    *WolStatus
);

WOL_STATUS_EX
WolGetWakeOnLanStatusEx(
  IN WOL_ADAPTER_HANDLE_TYPE Handle
);

WOL_STATUS
WolEnableWakeOnLan(
  IN    WOL_ADAPTER_HANDLE_TYPE     Handle,
  IN    BOOLEAN                     Enable
);

WOL_STATUS_EX
WolEnableWakeOnLanEx(
  IN WOL_ADAPTER_HANDLE_TYPE Handle,
  IN BOOLEAN Enable
);

WOL_STATUS
WolEnableApm(
  IN    WOL_ADAPTER_HANDLE_TYPE     Handle,
  IN    BOOLEAN                     Enable
);

WOL_STATUS
WolEnableApmPme(
  IN    WOL_ADAPTER_HANDLE_TYPE     Handle,
  IN    BOOLEAN                     Enable
);

#endif /* __WOL_H */

