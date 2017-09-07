/******************************************************************************
**                                                                           **
** INTEL CONFIDENTIAL                                                        **
**                                                                           **
** Copyright 2016 Intel Corporation All Rights Reserved.                     **
**                                                                           **
** The source code contained or described herein and all documents related   **
** to the source code ("Material") are owned by Intel Corporation or its     **
** suppliers or licensors.  Title to the Material remains with Intel         **
** Corporation or its suppliers and licensors.  The Material contains trade  **
** secrets and proprietary and confidential information of Intel or its      **
** suppliers and licensors.  The Material is protected by worldwide          **
** copyright and trade secret laws and treaty provisions.  No part of the    **
** Material may be used, copied, reproduced, modified, published, uploaded,  **
** posted, transmitted, distributed, or disclosed in any way without Intel's **
** prior express written permission.                                         **
**                                                                           **
** No license under any patent, copyright, trade secret or other             **
** intellectual property right is granted to or conferred upon you by        **
** disclosure or delivery of the Materials, either expressly, by             **
** implication, inducement, estoppel or otherwise.  Any license under such   **
** intellectual property rights must be express and approved by Intel in     **
** writing.                                                                  **
**                                                                           **
******************************************************************************/
#include  <Uefi.h>
#include  <Library/BaseLib.h>
#include  <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h> 

/** Do not define memcpy for IPF+GCC builds.
    For IPF, using a GCC compiler, the memcpy function is converted to
    CopyMem by objcpy during build.
**/
#if  !(defined(MDE_CPU_IPF) && defined(__GNUC__))

#ifdef _SIZE_T_NOTDEFINED
typedef unsigned int size_t;
#endif

/** The memcpy function copies n characters from the object pointed to by s2
    into the object pointed to by s1.

    The implementation is reentrant and handles the case where s2 overlaps s1.

    @return   The memcpy function returns the value of s1.
**/
void *
memcpy(void * __restrict s1, const void * __restrict s2, size_t n)
{
  return CopyMem( s1, s2, n);
}
#endif  /* !(defined(MDE_CPU_IPF) && defined(__GCC)) */
