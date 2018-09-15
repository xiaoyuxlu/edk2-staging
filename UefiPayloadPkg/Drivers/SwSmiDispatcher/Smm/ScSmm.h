/** @file
  Definitions for the Software SMI Dispatcher driver.

  Copyright (c) 2012 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_SMM_H_
#define _SC_SMM_H_

#include <PiSmm.h>
#include <Uefi.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SmmControl2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/PerformanceLib.h>
#include <Library/PlatformLib.h>

#if defined (X64_BUILD_SUPPORT) && (X64_BUILD_SUPPORT == 1)
#define EFI_BAD_POINTER          0xAFAFAFAFAFAFAFAFULL
#else
#define EFI_BAD_POINTER          0xAFAFAFAFUL
#endif

#define MmPciAddress( Segment, Bus, Device, Function, Register ) \
  ( (UINTN)PcdGet64 (PcdPciExpressBaseAddress) + \
    (UINTN)(Bus << 20) + \
    (UINTN)(Device << 15) + \
    (UINTN)(Function << 12) + \
    (UINTN)(Register) \
  )

//
// Address types
//
typedef enum {
  ACPI_ADDR_TYPE,
  GPIO_ADDR_TYPE,
  MEMORY_MAPPED_IO_ADDRESS_TYPE,
  PCIE_ADDR_TYPE,
  PCR_ADDR_TYPE,
  NUM_ADDR_TYPES,                     // count of items in this enum
  SC_SMM_ADDR_TYPE_NULL        = -1   // sentinel to indicate NULL or to signal end of arrays
} ADDR_TYPE;

typedef UINT16 IO_ADDR;
typedef IO_ADDR ACPI_ADDR;  // can omit
typedef IO_ADDR TCO_ADDR;   // can omit
typedef UINTN MEM_ADDR;
typedef MEM_ADDR *MEMORY_MAPPED_IO_ADDRESS;
typedef MEM_ADDR *GPIO_ADDR;

typedef union {
  UINT32  Raw;
  struct {
    UINT8 Reg;
    UINT8 Fnc;
    UINT8 Dev;
    UINT8 Bus;
  } Fields;
} PCIE_ADDR;

typedef union {
  UINT32  Raw;
  struct {
    UINT16 Offset;
    UINT8  Pid;
    UINT8  Base;
  } Fields;
} PCR_ADDR;

//
// Structure describing the address type and the address value
//
typedef struct {
  ADDR_TYPE Type;
  union {
    ///
    /// used to initialize during declaration/definition
    ///
    UINT32                    raw;

    ///
    /// used to access useful data
    ///
    IO_ADDR                   io;
    ACPI_ADDR                 acpi;
    TCO_ADDR                  tco;
    MEM_ADDR                  mem;
    MEMORY_MAPPED_IO_ADDRESS  Mmio;
    PCIE_ADDR                 pcie;
    PCR_ADDR                  Pcr;
  } Data;

} SC_SMM_ADDRESS;

//
// Bit within a register, used to specify a source or enabling bit.
//
typedef struct {
  SC_SMM_ADDRESS Reg;
  UINT8          SizeInBytes;
  UINT8          Bit;
} SC_SMM_BIT_DESC;

#define IS_BIT_DESC_NULL(BitDesc)   ((BitDesc).Reg.Type == SC_SMM_ADDR_TYPE_NULL)
#define NULL_THIS_BIT_DESC(BitDesc) ((BitDesc).Reg.Type = SC_SMM_ADDR_TYPE_NULL)
#define NULL_BIT_DESC_INITIALIZER \
  { \
    { \
      SC_SMM_ADDR_TYPE_NULL, \
      { \
        0 \
      } \
    }, \
    0, 0 \
  }

#define NUM_EN_BITS   2
#define NUM_STS_BITS  1

//
// Flags
//
typedef UINT8 SC_SMM_SOURCE_FLAGS;

#define SC_SMM_NO_FLAGS          0
#define SC_SMM_SCI_EN_DEPENDENT  1

typedef struct {
  SC_SMM_SOURCE_FLAGS  Flags;
  SC_SMM_BIT_DESC      En[NUM_EN_BITS];    // Describes the enable bit(s) for the SMI event
  SC_SMM_BIT_DESC      Sts[NUM_STS_BITS];  // Describes the status bit for the SMI event
} SC_SMM_SOURCE_DESC;

//
// Used to initialize null source descriptor
//
#define NULL_SOURCE_DESC_INITIALIZER \
  { \
    SC_SMM_NO_FLAGS, \
    { \
      NULL_BIT_DESC_INITIALIZER, NULL_BIT_DESC_INITIALIZER \
    }, \
    { \
      NULL_BIT_DESC_INITIALIZER \
    } \
  }

extern SC_SMM_SOURCE_DESC mSwSmiSrcDescriptor;

//
// Define an enumeration for all the supported SMI dispatch protocols
// Only the Software SMI dispatch protocol is used here.
//
typedef enum {
  SwType,
  ScSmmProtocolTypeMax
} SC_SMM_PROTOCOL_TYPE;

//
// SMI context
//
typedef union {
  EFI_SMM_SW_REGISTER_CONTEXT             Sw;
} SC_SMM_CONTEXT;

typedef union {
  UINTN ElapsedTime;
} SC_SMM_MISC_DATA;

typedef struct _DATABASE_RECORD DATABASE_RECORD;

/**
  Get SMM context address

  @param[in]  Record         The Database address
  @param[out] Context        The Smm Context address.

  @retval     None

**/
typedef
VOID
(EFIAPI *GET_CONTEXT) (
  IN  DATABASE_RECORD    *Record,
  OUT SC_SMM_CONTEXT     *Context
  );

/**
  Compare two buffer Context

  @param[IN] Context1        Context1 buffer address.
  @param[IN] Context2        Context2 buffer address.

  @retval    TRUE            Context1 and Context2 are the same.
  @retval    FALSE           Context1 and Context2 are different.

**/
typedef
BOOLEAN
(EFIAPI *CMP_CONTEXT) (
  IN SC_SMM_CONTEXT     *Context1,
  IN SC_SMM_CONTEXT     *Context2
  );

typedef
VOID
(EFIAPI *GET_COMMBUFFER) (
  IN  DATABASE_RECORD    *Record,
  OUT VOID               **CommBuffer,
  OUT UINTN              *CommBufferSize
 );

typedef struct {
  GET_CONTEXT GetContext;
  CMP_CONTEXT CmpContext;
  GET_COMMBUFFER  GetCommBuffer;
} CONTEXT_FUNCTIONS;

extern CONTEXT_FUNCTIONS          ContextFunctions[ScSmmProtocolTypeMax];

#define MAXIMUM_SWI_VALUE 0xFF

/**
  Clear the SMI status bit by set the source bit of SMI status register

  @param[in] SrcDesc         Pointer to the SC SMI source description table

  @return    None

**/
typedef
VOID
(EFIAPI *SC_SMM_CLEAR_SOURCE) (
  IN SC_SMM_SOURCE_DESC *SrcDesc
  );

//
// "DATABASE" RECORD
// Linked list data structures
//
#define DATABASE_RECORD_SIGNATURE SIGNATURE_32 ('D', 'B', 'R', 'C')

typedef struct _DATABASE_RECORD {
  UINT32                Signature;
  LIST_ENTRY            Link;
  BOOLEAN               Processed;

  //
  // Status and Enable bit description
  //
  SC_SMM_SOURCE_DESC    SrcDesc;

  //
  // Callback function
  //
  EFI_SMM_HANDLER_ENTRY_POINT2  Callback;
  SC_SMM_CONTEXT        ChildContext;

  //
  // Function to clear the SMI source
  //
  SC_SMM_CLEAR_SOURCE   ClearSource;

  //
  // Functions that handle contexts
  //
  CONTEXT_FUNCTIONS     ContextFunctions;

  //
  // The protocol that this record dispatches
  //
  SC_SMM_PROTOCOL_TYPE  ProtocolType;

  //
  // Misc data for private usage
  //
  SC_SMM_MISC_DATA     MiscData;

} DATABASE_RECORD;

#define DATABASE_RECORD_FROM_LINK(_record)  CR (_record, DATABASE_RECORD, Link, DATABASE_RECORD_SIGNATURE)
#define DATABASE_RECORD_FROM_CHILDCONTEXT(_record)  CR (_record, DATABASE_RECORD, ChildContext, DATABASE_RECORD_SIGNATURE)

/**
  Register a child SMI dispatch function with a parent SMM driver.

  @param[in]  This                    Pointer to the SC_SMM_GENERIC_PROTOCOL instance.
  @param[in]  DispatchFunction        Pointer to dispatch function to be invoked for this SMI source.
  @param[in]  DispatchContext         Pointer to the dispatch function's context.
  @param[out] DispatchHandle          Handle of dispatch function, for when interfacing
                                      with the parent SMM driver, will be the address of linked
                                      list link in the call back record.

  @retval     EFI_OUT_OF_RESOURCES    Insufficient resources to create database record
  @retval     EFI_INVALID_PARAMETER   The input parameter is invalid
  @retval     EFI_SUCCESS             The dispatch function has been successfully
                                      registered and the SMI source has been enabled.

**/
typedef
EFI_STATUS
(EFIAPI *SC_SMM_GENERIC_REGISTER) (
  IN  VOID                             **This,
  IN  VOID                             *DispatchFunction,
  IN  VOID                             *DispatchContext,
  OUT EFI_HANDLE                       *DispatchHandle
  );

/**
  Unregister a child SMI source dispatch function with a parent SMM driver.

  @param[in] This                    Pointer to the  EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL instance.
  @param[in] DispatchHandle          Handle of dispatch function to deregister.

  @retval    EFI_SUCCESS             The dispatch function has been successfully
                                     unregistered and the SMI source has been disabled
                                     if there are no other registered child dispatch
                                     functions for this SMI source.
  @retval    EFI_INVALID_PARAMETER   Handle is invalid.
  @retval    EFI_SUCCESS             The function has been successfully unregistered child SMI source.

**/
typedef
EFI_STATUS
(EFIAPI *SC_SMM_GENERIC_UNREGISTER) (
  IN  VOID                            **This,
  IN  EFI_HANDLE                      DispatchHandle
  );

typedef struct {
  SC_SMM_GENERIC_REGISTER     Register;
  SC_SMM_GENERIC_UNREGISTER   Unregister;
  UINTN                       Extra1;
  UINTN                       Extra2; // may not need this one
} SC_SMM_GENERIC_PROTOCOL;

/**
  Register a child SMI dispatch function with a parent SMM driver.

  @param[in]  This                    Pointer to the SC_SMM_GENERIC_PROTOCOL instance.
  @param[in]  DispatchFunction        Pointer to dispatch function to be invoked for this SMI source.
  @param[in]  DispatchContext         Pointer to the dispatch function's context.
  @param[out] DispatchHandle          Handle of dispatch function, for when interfacing
                                      with the parent SMM driver, will be the address of linked
                                      list link in the call back record.

  @retval     EFI_OUT_OF_RESOURCES    Insufficient resources to create database record
  @retval     EFI_INVALID_PARAMETER   The input parameter is invalid
  @retval     EFI_SUCCESS             The dispatch function has been successfully
                                      registered and the SMI source has been enabled.

**/
EFI_STATUS
EFIAPI
ScSmmCoreRegister (
  IN  SC_SMM_GENERIC_PROTOCOL          *This,
  IN  EFI_SMM_HANDLER_ENTRY_POINT2     DispatchFunction,
  IN  SC_SMM_CONTEXT                   *DispatchContext,
  OUT EFI_HANDLE                       *DispatchHandle
  );

/**

  Unregister a child SMI source dispatch function with a parent SMM driver.

  @param[in] This                    Pointer to the  EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL instance.
  @param[in] DispatchHandle          Handle of dispatch function to deregister.

  @retval    EFI_SUCCESS             The dispatch function has been successfully
                                     unregistered and the SMI source has been disabled
                                     if there are no other registered child dispatch
                                     functions for this SMI source.
  @retval    EFI_INVALID_PARAMETER   Handle is invalid.
  @retval    EFI_SUCCESS             The function has been successfully unregistered child SMI source.

**/
EFI_STATUS
EFIAPI
ScSmmCoreUnRegister (
  IN  SC_SMM_GENERIC_PROTOCOL        *This,
  IN  EFI_HANDLE                     *DispatchHandle
  );

//
// Generic SMM dispatch Protocol
//
typedef union {
  SC_SMM_GENERIC_PROTOCOL                     Generic;

  EFI_SMM_SW_DISPATCH2_PROTOCOL               Sw;
} SC_SMM_PROTOCOL;

#define PROTOCOL_SIGNATURE  SIGNATURE_32 ('P', 'R', 'O', 'T')

typedef struct {
  UINTN                 Signature;

  SC_SMM_PROTOCOL_TYPE  Type;
  EFI_GUID              *Guid;
  SC_SMM_PROTOCOL       Protocols;
} SC_SMM_QUALIFIED_PROTOCOL;

#define QUALIFIED_PROTOCOL_FROM_GENERIC(_generic) \
  CR ( \
  _generic, \
  SC_SMM_QUALIFIED_PROTOCOL, \
  Protocols, \
  PROTOCOL_SIGNATURE \
  )

//
// Private data for the dispatch protocols
//
typedef struct {
  LIST_ENTRY                  CallbackDataBase;
  EFI_HANDLE                  SmiHandle;
  EFI_HANDLE                  InstallMultProtHandle;
  SC_SMM_QUALIFIED_PROTOCOL   Protocols[ScSmmProtocolTypeMax];
} PRIVATE_DATA;

extern PRIVATE_DATA           mPrivateData;

/**
  Get the Software Smi value

  @param[in]  Record              No use
  @param[out] Context             The context that includes Software Smi value to be filled

**/
VOID
EFIAPI
SwGetContext (
  IN  DATABASE_RECORD    *Record,
  OUT SC_SMM_CONTEXT     *Context
  );

/**
  Check whether software SMI value of two contexts match

  @param[in] Context1             Context 1 that includes software SMI value 1
  @param[in] Context2             Context 2 that includes software SMI value 2

  @retval    FALSE                Software SMI value match
  @retval    TRUE                 Software SMI value don't match

**/
BOOLEAN
EFIAPI
SwCmpContext (
  IN SC_SMM_CONTEXT               *Context1,
  IN SC_SMM_CONTEXT               *Context2
  );

/**
  Gather the CommBuffer information of SmmSwDispatch2.

  @param[in]  Record              No use
  @param[out] CommBuffer          Point to the CommBuffer structure
  @param[out] CommBufferSize      Point to the Size of CommBuffer structure

**/
VOID
EFIAPI
SwGetCommBuffer (
  IN  DATABASE_RECORD             *Record,
  OUT VOID                        **CommBuffer,
  OUT UINTN                       *CommBufferSize
  );


/**
  Init required protocol for Pch Sw Dispatch protocol.

**/
VOID
ScSwDispatchInit (
  VOID
  );

#endif

