/** @file
  This driver allows registration of child handlers for software SMIs. This driver
  also implements SMM communications with VariableSmmRuntimeDxe to realize variable
  services.

  Copyright (c) 2012 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScSmmHelpers.h"
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmControl2.h>

//
// Function prototypes
//

EFI_STATUS
EFIAPI
VariableServiceInitialize (
  IN EFI_HANDLE                           ImageHandle,
  IN EFI_SYSTEM_TABLE                     *SystemTable
  );

EFI_STATUS
EFIAPI
CseVariableStorageSmmRuntimeInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );

EFI_STATUS
EFIAPI
ScSmmCoreDispatcher (
  IN EFI_HANDLE             SmmImageHandle,
  IN CONST VOID             *ContextData,            OPTIONAL
  IN OUT VOID               *CommunicationBuffer,    OPTIONAL
  IN OUT UINTN              *SourceSize              OPTIONAL
  );

EFI_STATUS
EFIAPI
SmmCseVariableStorageLibInit (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );

//
// Global data
//

PRIVATE_DATA          mPrivateData = {  // for the structure
  {
    NULL
  },                                    // CallbackDataBase linked list head
  NULL,                                 // Handler returned when calling SmiHandlerRegister
  NULL,                                 // EFI handle returned when calling InstallMultipleProtocolInterfaces
  {                                     // protocol arrays
    {

        (UINTN)PROTOCOL_SIGNATURE,
        SwType,
        &gEfiSmmSwDispatch2ProtocolGuid,
        {{
          (SC_SMM_GENERIC_REGISTER) ScSmmCoreRegister,
          (SC_SMM_GENERIC_UNREGISTER) ScSmmCoreUnRegister,
          (UINTN) MAXIMUM_SWI_VALUE
        }}
      
    }
  }
};

CONTEXT_FUNCTIONS     mContextFunctions[ScSmmProtocolTypeMax] = {
  {
    SwGetContext,
    SwCmpContext,
    SwGetCommBuffer
  }
};

/**
  Publish SMI Dispatch protocols.

**/
VOID
ScSmmPublishDispatchProtocols (
  VOID
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  UINTN      Index;
  //
  // Install protocol interfaces.
  //
  for (Index = 0; Index < ScSmmProtocolTypeMax; Index++) {
    Status = gSmst->SmmInstallProtocolInterface (
                      &mPrivateData.InstallMultProtHandle,
                      mPrivateData.Protocols[Index].Guid,
                      EFI_NATIVE_INTERFACE,
                      &mPrivateData.Protocols[Index].Protocols.Generic
                      );
  }
  ASSERT_EFI_ERROR (Status);
}

/**
  Initializes the SW SMM Dispatcher

  @param[in] ImageHandle          Pointer to the loaded image protocol for this driver
  @param[in] SystemTable          Pointer to the EFI System Table

  @retval    EFI_SUCCESS          PchSmmDispatcher Initialization completed.

**/
EFI_STATUS
EFIAPI
InitializeSwSmmDispatcher (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  
  //
  // Init required protocol for SC Sw Dispatch protocol.
  //
  ScSwDispatchInit ();

  //
  // Register a callback function to handle subsequent SMIs.  This callback
  // will be called by SmmCoreDispatcher.
  //
  Status = gSmst->SmiHandlerRegister (ScSmmCoreDispatcher, NULL, &mPrivateData.SmiHandle);
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize Callback DataBase
  //
  InitializeListHead (&mPrivateData.CallbackDataBase);
  ScSmmPublishDispatchProtocols ();

  //
  // Enable SMIs on the SC now that we have a callback
  //
  SwSmiInitHardware ();

  //
  // Initialize variable service in SMM
  //
  Status = VariableServiceInitialize (ImageHandle, SystemTable);

  return Status;
}


/**
  Check the fed SwSmiInputValue to see if there is a duplicated one in the database

  @param[in] FedSwSmiInputValue      Fed SwSmiInputValue

  @retval    EFI_SUCCESS             There is no duplicated SwSmiInputValue
  @retval    EFI_INVALID_PARAMETER   There is a duplicated SwSmiInputValue

**/
EFI_STATUS
SmiInputValueDuplicateCheck (
  IN UINTN           FedSwSmiInputValue
  )
{

  DATABASE_RECORD    *RecordInDb;
  LIST_ENTRY         *LinkInDb;

  LinkInDb = GetFirstNode (&mPrivateData.CallbackDataBase);
  while (!IsNull (&mPrivateData.CallbackDataBase, LinkInDb)) {
    RecordInDb = DATABASE_RECORD_FROM_LINK (LinkInDb);

    if (RecordInDb->ProtocolType == SwType) {
      if (RecordInDb->ChildContext.Sw.SwSmiInputValue == FedSwSmiInputValue) {
        return EFI_INVALID_PARAMETER;
      }
    }

    LinkInDb = GetNextNode (&mPrivateData.CallbackDataBase, &RecordInDb->Link);
  }

  return EFI_SUCCESS;
}


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
  IN  SC_SMM_GENERIC_PROTOCOL                          *This,
  IN  EFI_SMM_HANDLER_ENTRY_POINT2                     DispatchFunction,
  IN  SC_SMM_CONTEXT                                   *DispatchContext,
  OUT EFI_HANDLE                                       *DispatchHandle
  )
{
  EFI_STATUS                  Status;

  DATABASE_RECORD             *Record;
  SC_SMM_QUALIFIED_PROTOCOL   *Qualified;
  SC_SMM_SOURCE_DESC          NullSourceDesc = NULL_SOURCE_DESC_INITIALIZER;
  UINTN                       Index;

  Index = 0;
  //
  // Create database record and add to database
  //
  if (gSmst == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (DATABASE_RECORD), (VOID**) &Record);
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Gather information about the registration request
  //
  Record->Callback          = DispatchFunction;
  Record->ChildContext      = *DispatchContext;
  Qualified                 = QUALIFIED_PROTOCOL_FROM_GENERIC (This);
  Record->ProtocolType      = Qualified->Type;
  Record->ContextFunctions  = mContextFunctions[Qualified->Type];

  //
  // Perform linked list housekeeping
  //
  Record->Signature = DATABASE_RECORD_SIGNATURE;

  switch (Qualified->Type) {

    case SwType:
      //
      // Check the validity of Context Value
      //
      if (Record->ChildContext.Sw.SwSmiInputValue == (UINTN) - 1) {
        for (Index = 1; Index < MAXIMUM_SWI_VALUE; Index++) {
          if (!EFI_ERROR (SmiInputValueDuplicateCheck (Index))) {
            Record->ChildContext.Sw.SwSmiInputValue = Index;
            break;
          }
        }
        if (Record->ChildContext.Sw.SwSmiInputValue == (UINTN) - 1) {
          goto Error;
        }
      }
      if (Record->ChildContext.Sw.SwSmiInputValue > MAXIMUM_SWI_VALUE) {
        goto Error;
      }

      if (EFI_ERROR (SmiInputValueDuplicateCheck (Record->ChildContext.Sw.SwSmiInputValue))) {
        goto Error;
      }

      InsertTailList (&mPrivateData.CallbackDataBase, &Record->Link);
      CopyMem ((VOID *) &(Record->SrcDesc), (VOID *) (&mSwSmiSrcDescriptor), sizeof (SC_SMM_SOURCE_DESC));
      Record->ClearSource = NULL;
      //
      // use default clear source function
      //
      break;

    default:
      goto Error;
      break;
  }

  if (CompareSources (&Record->SrcDesc, &NullSourceDesc)) {
    goto Error;
  }

  if (Record->ClearSource == NULL) {
    //
    // Clear the SMI associated w/ the source using the default function
    //
    ScSmmClearSource (&Record->SrcDesc);
  } else {
    //
    // This source requires special handling to clear
    //
    Record->ClearSource (&Record->SrcDesc);
  }

  ScSmmEnableSource (&Record->SrcDesc);

  //
  // Child's handle will be the address linked list link in the record
  //
  *DispatchHandle = (EFI_HANDLE) (&Record->Link);
  *DispatchContext = Record->ChildContext;

  return EFI_SUCCESS;

Error:
  Status = gSmst->SmmFreePool (Record);
  return EFI_INVALID_PARAMETER;
}


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
  IN SC_SMM_GENERIC_PROTOCOL                            *This,
  IN EFI_HANDLE                                         *DispatchHandle
  )
{
  DATABASE_RECORD      *RecordToDelete;
  LIST_ENTRY          *LinkInDb;
  BOOLEAN              Found;
  
  if (DispatchHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  RecordToDelete = DATABASE_RECORD_FROM_LINK (DispatchHandle);

  //
  // See if this is a valid entry
  //
  if (RecordToDelete->Link.ForwardLink == (LIST_ENTRY *) EFI_BAD_POINTER) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // See if this entry exists in the database
  //  
  Found = FALSE;
  LinkInDb = GetFirstNode (&mPrivateData.CallbackDataBase);
  while (!IsNull (&mPrivateData.CallbackDataBase, LinkInDb) && !Found) {
    if (&RecordToDelete->Link == LinkInDb) {
      Found = TRUE;
    }
    LinkInDb = GetNextNode (&mPrivateData.CallbackDataBase, LinkInDb);
  }
  if (!Found) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Remove the entry
  //
  RemoveEntryList (&RecordToDelete->Link);

  return EFI_SUCCESS;
}


/**

  The callback function to handle subsequent SMIs.  This callback will be called by SmmCoreDispatcher.

  @param[in]      SmmImageHandle       SMM image handle
  @param[in]      ContextData          Not used
  @param[in, out] CommunicationBuffer  Not used
  @param[in, out] SourceSize           Not used

  @retval         EFI_SUCCESS          Function successfully completed

**/
EFI_STATUS
EFIAPI
ScSmmCoreDispatcher (
  IN EFI_HANDLE             SmmImageHandle,
  IN CONST VOID             *ContextData,            OPTIONAL
  IN OUT VOID               *CommunicationBuffer,    OPTIONAL
  IN OUT UINTN              *SourceSize              OPTIONAL
  )
{
  //
  // Used to prevent infinite loops
  //
  UINTN               EscapeCount;
  BOOLEAN             ContextsMatch;
  BOOLEAN             EosSet;
  DATABASE_RECORD     *RecordInDb;
  LIST_ENTRY          *LinkInDb;
  DATABASE_RECORD     *RecordToExhaust;
  LIST_ENTRY          *LinkToExhaust;
  SC_SMM_CONTEXT      Context;
  VOID                *CommBuffer;
  UINTN               CommBufferSize;
  EFI_STATUS          Status;
  SC_SMM_SOURCE_DESC  ActiveSource = NULL_SOURCE_DESC_INITIALIZER;

  EscapeCount           = 100;
  ContextsMatch         = FALSE;
  EosSet                = FALSE;
//  SxChildWasDispatched  = FALSE;
  Status                = EFI_SUCCESS;

  if (!IsListEmpty (&mPrivateData.CallbackDataBase)) {
    //
    // We have children registered w/ us -- continue
    //
    while ((!EosSet) && (EscapeCount > 0)) {
      EscapeCount--;

      LinkInDb = GetFirstNode (&mPrivateData.CallbackDataBase);

      while (!IsNull (&mPrivateData.CallbackDataBase, LinkInDb)) {
        RecordInDb = DATABASE_RECORD_FROM_LINK (LinkInDb);

        //
        // look for the first active source
        //
        if (!SourceIsActive (&RecordInDb->SrcDesc)) {
          //
          // Didn't find the source yet, keep looking
          //
          LinkInDb = GetNextNode (&mPrivateData.CallbackDataBase, &RecordInDb->Link);

          //
          // if it's the last one, try to clear EOS
          //
          if (IsNull (&mPrivateData.CallbackDataBase, LinkInDb)) {
            EosSet = SwSmiSetAndCheckEos ();
          }
        } else {
          //
          // "cache" the source description and don't query I/O anymore
          //
          CopyMem ((VOID *) &ActiveSource, (VOID *) &(RecordInDb->SrcDesc), sizeof (SC_SMM_SOURCE_DESC));
          LinkToExhaust = LinkInDb;

          //
          // exhaust the rest of the queue looking for the same source
          //
          while (!IsNull (&mPrivateData.CallbackDataBase, LinkToExhaust)) {
            RecordToExhaust = DATABASE_RECORD_FROM_LINK (LinkToExhaust);
            //
            // RecordToExhaust->Link might be removed (unregistered) by Callback function, and then the
            // system will hang in ASSERT() while calling GetNextNode().
            // To prevent the issue, we need to get next record in DB here (before Callback function).
            //
            LinkToExhaust = GetNextNode (&mPrivateData.CallbackDataBase, &RecordToExhaust->Link);

            if (CompareSources (&RecordToExhaust->SrcDesc, &ActiveSource)) {
              //
              // These source descriptions are equal, so this callback should be
              // dispatched.
              //
              if (RecordToExhaust->ContextFunctions.GetContext != NULL) {
                //
                // This child requires that we get a calling context from
                // hardware and compare that context to the one supplied
                // by the child.
                //
                ASSERT (RecordToExhaust->ContextFunctions.CmpContext != NULL);

                //
                // Make sure contexts match before dispatching event to child
                //
                RecordToExhaust->ContextFunctions.GetContext (RecordToExhaust, &Context);
                ContextsMatch = RecordToExhaust->ContextFunctions.CmpContext (&Context, &RecordToExhaust->ChildContext);

              } else {
                //
                // This child doesn't require any more calling context beyond what
                // it supplied in registration.  Simply pass back what it gave us.
                //
                ASSERT (RecordToExhaust->Callback != NULL);
                Context       = RecordToExhaust->ChildContext;
                ContextsMatch = TRUE;
              }

              if (ContextsMatch) {
                if (RecordToExhaust->Callback != NULL) {
                  if (RecordToExhaust->ContextFunctions.GetCommBuffer != NULL) {
                    //
                    // This callback function needs CommBuffer and CommBufferSize.
                    // Get those from child and then pass to callback function.
                    //
                    RecordToExhaust->ContextFunctions.GetCommBuffer (RecordToExhaust, &CommBuffer, &CommBufferSize);
                  } else {
                    //
                    // Child doesn't support the CommBuffer and CommBufferSize.
                    // Just pass NULL value to callback function.
                    //
                    CommBuffer     = NULL;
                    CommBufferSize = 0;
                  }

                  PERF_START_EX (NULL, "SmmFunction", NULL, AsmReadTsc(), RecordToExhaust->ProtocolType);
                  RecordToExhaust->Callback ((EFI_HANDLE) & RecordToExhaust->Link, &Context, CommBuffer, &CommBufferSize);
                  PERF_END_EX (NULL, "SmmFunction", NULL, AsmReadTsc(), RecordToExhaust->ProtocolType);
                } else {
                  ASSERT (FALSE);
                }
              }
            }
          }

          if (RecordInDb->ClearSource == NULL) {
            //
            // Clear the SMI associated w/ the source using the default function
            //
            ScSmmClearSource (&ActiveSource);
          } else {
            //
            // This source requires special handling to clear
            //
            RecordInDb->ClearSource (&ActiveSource);
          }
          //
          // Also, try to clear EOS
          //
          EosSet = SwSmiSetAndCheckEos ();
          //
          // Queue is empty, reset the search
          //
          break;
        }
      }
    }
  }
  BeforeExitSmi ();

  return Status;
}

