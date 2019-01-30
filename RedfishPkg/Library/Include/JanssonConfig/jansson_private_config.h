/** @file
  Jansson private configurations for UEFI support.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials are licensed and made available under
  the terms and conditions of the BSD License that accompanies this distribution.
  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef JANSSON_PRIVATE_CONFIG_H
#define JANSSON_PRIVATE_CONFIG_H

#define HAVE_UNISTD_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_SYS_TYPES_H 1

#define HAVE_INT32_T 1
#ifndef HAVE_INT32_T
  #define int32_t INT32
#endif

#define HAVE_UINT32_T 1
#ifndef HAVE_UINT32_T
  #define uint32_t UINT32
#endif

#define HAVE_UINT16_T 1
#ifndef HAVE_UINT16_T
  #define uint16_t UINT16
#endif

#define HAVE_UINT8_T 1
#ifndef HAVE_UINT8_T
  #define uint8_t UINT8
#endif

#define HAVE_SSIZE_T 1

#ifndef HAVE_SSIZE_T
  #define ssize_t INTN
#endif

#define INITIAL_HASHTABLE_ORDER 3

#endif
