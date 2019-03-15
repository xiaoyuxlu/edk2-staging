/** @file

Sample driver for exposing capabilities to ACPI through UEFI Runtime Driver

Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


//
// Following are sample codes that handle SCI interrupt and notify the PRM Device
// to invoke PRM handler
//
//  Scope(\_GPE)
//  {
//      Method(_LXX)
//      {
//          If(/* SCI was generated from the expected GPI*/) { 
//            Notify(\PRMD, 0x80) // notify PRM Device in response to the SCI
//          }
//      }
//
//  }
//

    Scope(\)
    {
      Device(PRMD) // PRM Device
      {
        Name (_HID, "80860222")
        Name (_CID, "80860222")
        
        //
        // PRMM is the ACPI method that other ACPI codes or device drivers can call
        // to invoke PRM handler
        //
        Method (PRMM, 0)
        {
          // Notify self to invoke PRM handler
          Notify(\PRMD, 0x80)
        }
      }
    }
