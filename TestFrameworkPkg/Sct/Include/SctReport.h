/** @file
  This file provides the test report generation services.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_SCT_REPORT_H_
#define _EFI_SCT_REPORT_H_

//
// External functions declarations
//

EFI_STATUS
GetInstanceAssertion (
  IN EFI_DEVICE_PATH_PROTOCOL                         *DevicePath,
  IN CHAR16                                           *MetaName,
  OUT EFI_SCT_LOG_STATE                               *FileState,
  OUT UINT32                                          *Index,
  OUT UINT32                                          *Iteration,
  OUT UINT32                                          *PassNumber,
  OUT UINT32                                          *WarnNumber,
  OUT UINT32                                          *FailNumber
  );

EFI_STATUS
GetInterfaceAssertion (
  IN EFI_DEVICE_PATH_PROTOCOL                         *DevicePath,
  IN CHAR16                                           *MetaName,
  OUT UINT32                                          *PassNumber,
  OUT UINT32                                          *WarnNumber,
  OUT UINT32                                          *FailNumber
  );

EFI_STATUS
GetProtocolAssertion (
  IN EFI_DEVICE_PATH_PROTOCOL                         *DevicePath,
  IN CHAR16                                           *FileName,
  OUT UINT32                                          *PassNumber,
  OUT UINT32                                          *WarnNumber,
  OUT UINT32                                          *FailNumber
  );

EFI_STATUS
GenerateReport (
  IN EFI_DEVICE_PATH_PROTOCOL                         *LogDevicePath,
  IN CHAR16                                           *LogFilePath,
  IN EFI_DEVICE_PATH_PROTOCOL                         *ReportDevicePath,
  IN CHAR16                                           *ReportFileName
  );

#endif
