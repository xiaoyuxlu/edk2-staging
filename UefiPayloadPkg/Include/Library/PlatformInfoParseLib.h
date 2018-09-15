/** @file
  This library parses the platform information passed from the previous stage
  firmware.

  Copyright (c) 2014 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <PiPei.h>
#include <Guid/FrameBufferInfoGuid.h>
#include <Guid/SerialPortInfoGuid.h>
#include <Guid/SystemTableInfoGuid.h>
#include <Guid/MemoryMapInfoGuid.h>
#include <Guid/LoaderFspInfoGuid.h>
#include <Guid/TpmInfoGuid.h>

typedef VOID \
        (*SBL_MEM_INFO_CALLBACK) (MEMROY_MAP_ENTRY  *MemoryMapEntry, VOID *Param);

typedef RETURN_STATUS \
        (*CB_MEM_INFO_CALLBACK) (UINT64 Base, UINT64 Size, UINT32 Type, VOID *Param);

/**
  Determine if Coreboot exists in the system

  @param  (VOID)

  @retval TRUE     Coreboot exists in the system
  @retval FALSE    Coreboot doesn't exist in the system

**/
BOOLEAN
EFIAPI
CorebootExists (
  VOID
  );

/**
  Acquire the memory information from the coreboot table in memory.

  @param  MemInfoCallback     The callback routine
  @param  pParam              Pointer to the callback routine parameter

  @retval RETURN_SUCCESS     Successfully find out the memory information.
  @retval RETURN_NOT_FOUND   Failed to find the memory information.

**/
RETURN_STATUS
EFIAPI
ParseMemoryInfoByCb (
  IN  CB_MEM_INFO_CALLBACK  MemInfoCallback,
  IN  VOID                  *pParam
  );


/**
  Acquire the memory information from Slim Bootloader

  @param  MemInfoCallback     The callback routine
  @param  pParam              Pointer to the callback routine parameter

  @retval RETURN_SUCCESS     Successfully find out the memory information.
  @retval RETURN_NOT_FOUND   Failed to find the memory information.

**/
RETURN_STATUS
EFIAPI
ParseMemoryInfoByHob (
  IN  SBL_MEM_INFO_CALLBACK  MemInfoCallback,
  IN  VOID                  *pParam
  );

/**
  Acquire the acpi table from coreboot

  @param  pMemTable          Pointer to the base address of the memory table
  @param  pMemTableSize      Pointer to the size of the memory table

  @retval RETURN_SUCCESS     Successfully find out the memory table.
  @retval RETURN_INVALID_PARAMETER  Invalid input parameters.
  @retval RETURN_NOT_FOUND   Failed to find the memory table.

**/
RETURN_STATUS
EFIAPI
ParseAcpiTableByCb (
  IN VOID**     pMemTable,
  IN UINT32*    pMemTableSize
  );


/**
  Acquire the smbios table from coreboot

  @param  pMemTable          Pointer to the base address of the memory table
  @param  pMemTableSize      Pointer to the size of the memory table

  @retval RETURN_SUCCESS     Successfully find out the memory table.
  @retval RETURN_INVALID_PARAMETER  Invalid input parameters.
  @retval RETURN_NOT_FOUND   Failed to find the memory table.

**/
RETURN_STATUS
EFIAPI
ParseSmbiosTableByCb (
  IN VOID**     pMemTable,
  IN UINT32*    pMemTableSize
  );


/**
  Acquire the acpi table from coreboot

  @param  pMemTable          Pointer to the base address of the memory table
  @param  pMemTableSize      Pointer to the size of the memory table

  @retval RETURN_SUCCESS     Successfully find out the memory table.
  @retval RETURN_INVALID_PARAMETER  Invalid input parameters.
  @retval RETURN_NOT_FOUND   Failed to find the memory table.

**/
RETURN_STATUS
EFIAPI
ParseAcpiTableByHob (
  IN VOID**     pMemTable,
  IN UINT32*    pMemTableSize
  );

/**
  Acquire the smbios table from coreboot

  @param  pMemTable          Pointer to the base address of the memory table
  @param  pMemTableSize      Pointer to the size of the memory table

  @retval RETURN_SUCCESS     Successfully find out the memory table.
  @retval RETURN_INVALID_PARAMETER  Invalid input parameters.
  @retval RETURN_NOT_FOUND   Failed to find the memory table.

**/
RETURN_STATUS
EFIAPI
ParseSmbiosTableByHob (
  IN VOID**     pMemTable,
  IN UINT32*    pMemTableSize
  );

/**
  Find the required fadt information from Coreboot

  @param  pPmCtrlReg         Pointer to the address of power management control register
  @param  pPmTimerReg        Pointer to the address of power management timer register
  @param  pResetReg          Pointer to the address of system reset register
  @param  pResetValue        Pointer to the value to be writen to the system reset register
  @param  pPmEvtReg          Pointer to the address of power management event register
  @param  pPmGpeEnReg        Pointer to the address of power management GPE enable register

  @retval RETURN_SUCCESS     Successfully find out all the required fadt information.
  @retval RETURN_NOT_FOUND   Failed to find the fadt table.

**/
RETURN_STATUS
EFIAPI
ParseFadtInfoByCb (
  IN UINTN*     pPmCtrlReg,
  IN UINTN*     pPmTimerReg,
  IN UINTN*     pResetReg,
  IN UINTN*     pResetValue,
  IN UINTN*     pPmEvtReg,
  IN UINTN*     pPmGpeEnReg
  );

/**
  Find the required fadt information from Slim Bootloader

  @param  pPmCtrlReg         Pointer to the address of power management control register
  @param  pPmTimerReg        Pointer to the address of power management timer register
  @param  pResetReg          Pointer to the address of system reset register
  @param  pResetValue        Pointer to the value to be writen to the system reset register
  @param  pPmEvtReg          Pointer to the address of power management event register
  @param  pPmGpeEnReg        Pointer to the address of power management GPE enable register

  @retval RETURN_SUCCESS     Successfully find out all the required fadt information.
  @retval RETURN_NOT_FOUND   Failed to find the fadt table.

**/
RETURN_STATUS
EFIAPI
ParseFadtInfoByHob (
  IN UINTN*     pPmCtrlReg,
  IN UINTN*     pPmTimerReg,
  IN UINTN*     pResetReg,
  IN UINTN*     pResetValue,
  IN UINTN*     pPmEvtReg,
  IN UINTN*     pPmGpeEnReg
  );

/**
  Find the serial port information from Coreboot

  @param  pRegBase           Pointer to the base address of serial port registers
  @param  pRegAccessType     Pointer to the access type of serial port registers
  @param  pRegWidth          Pointer to the register width in bytes
  @param  pBaudrate          Pointer to the serial port baudrate
  @param  pInputHertz        Pointer to the input clock frequency
  @param  pUartPciAddr       Pointer to the UART PCI bus, dev and func address

  @retval RETURN_SUCCESS     Successfully find the serial port information.
  @retval RETURN_NOT_FOUND   Failed to find the serial port information .

**/
RETURN_STATUS
EFIAPI
ParseSerialInfoByCb (
  OUT UINT32     *pRegBase,
  OUT UINT32     *pRegAccessType,
  OUT UINT32     *pRegWidth,
  OUT UINT32     *pBaudrate,
  OUT UINT32     *pInputHertz,
  OUT UINT32     *pUartPciAddr
  );


/**
  Find the serial port information from Slim Bootloader

  @param  pRegBase           Pointer to the base address of serial port registers
  @param  pRegAccessType     Pointer to the access type of serial port registers
  @param  pRegWidth          Pointer to the register width in bytes
  @param  pBaudrate          Pointer to the serial port baudrate
  @param  pInputHertz        Pointer to the input clock frequency
  @param  pUartPciAddr       Pointer to the UART PCI bus, dev and func address

  @retval RETURN_SUCCESS     Successfully find the serial port information.
  @retval RETURN_NOT_FOUND   Failed to find the serial port information .

**/
RETURN_STATUS
EFIAPI
ParseSerialInfoByHob (
  OUT UINT32     *pRegBase,
  OUT UINT32     *pRegAccessType,
  OUT UINT32     *pRegWidth,
  OUT UINT32     *pBaudrate,
  OUT UINT32     *pInputHertz,
  OUT UINT32     *pUartPciAddr
  );

/**
  Search for the Coreboot table header

  @param  Level              Level of the search depth
  @param  HeaderPtr          Pointer to the pointer of coreboot table header

  @retval RETURN_SUCCESS     Successfully find the coreboot table header .
  @retval RETURN_NOT_FOUND   Failed to find the coreboot table header .

**/
RETURN_STATUS
EFIAPI
GetCbHeaderInDepth (
  IN UINTN  Level,
  IN VOID** HeaderPtr
  );

/**
  Find the video frame buffer information from Slim Bootloader

  @param  pFbInfo            Pointer to the FRAME_BUFFER_INFO structure

  @retval RETURN_SUCCESS     Successfully find the video frame buffer information.
  @retval RETURN_NOT_FOUND   Failed to find the video frame buffer information .

**/
RETURN_STATUS
EFIAPI
ParseFrameBufferInfoByHob (
  IN FRAME_BUFFER_INFO*     pFbInfo
  );

/**
  Find the video frame buffer information from Coreboot

  @param  pFbInfo            Pointer to the FRAME_BUFFER_INFO structure

  @retval RETURN_SUCCESS     Successfully find the video frame buffer information.
  @retval RETURN_NOT_FOUND   Failed to find the video frame buffer information .

**/
RETURN_STATUS
EFIAPI
ParseFrameBufferInfoByCb (
  IN FRAME_BUFFER_INFO*     pFbInfo
  );

/**
  Find FSP information from Slim Bootloader

  @param  LdrFspInfo         Pointer to the LOADER_FSP_INFO structure

  @retval RETURN_SUCCESS     Successfully find the video frame buffer information.
  @retval RETURN_NOT_FOUND   Failed to find the video frame buffer information .

**/
RETURN_STATUS
EFIAPI
ParseFspInfoByHob (
  OUT LOADER_FSP_INFO *LdrFspInfo  
  );


/**
  Acquire the TPM table from Slim Bootloader

  @param  None

  @retval RETURN_SUCCESS     Successfully find out the TPM table.
  @retval RETURN_NOT_FOUND   Failed to find the TPM table.

**/
EFI_STATUS
EFIAPI
ParseTpmTable(
  BOOLEAN Coreboot,
  UINTN   **Lasa,
  UINT8   **Checksum
);
