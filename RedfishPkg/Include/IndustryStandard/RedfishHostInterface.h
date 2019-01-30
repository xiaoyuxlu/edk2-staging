/** @file
  This file defines the Redfish Interface Specific Data.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __REDFISH_HOST_INTERFACE__
#define __REDFISH_HOST_INTERFACE__

#define PROTOCOL_REDFISH_OVER_IP 0x04

#define REDFISH_NETWORK_HOST_INTERFACE_TYPE 0x40

#define REDFISH_HOST_INTERFACE_DEVICE_TYPE_USB      0x02
#define REDFISH_HOST_INTERFACE_DEVICE_TYPE_PCI_PCIE 0x03
#define REDFISH_HOST_INTERFACE_PROTOCOL_ID_OVER_IP  0x04

#define REDFISH_HOST_INTERFACE_HOST_IP_ASSIGNMENT_TYPE_UNKNOWN            0x00
#define REDFISH_HOST_INTERFACE_HOST_IP_ASSIGNMENT_TYPE_STATIC             0x01
#define REDFISH_HOST_INTERFACE_HOST_IP_ASSIGNMENT_TYPE_DHCP               0x02
#define REDFISH_HOST_INTERFACE_HOST_IP_ASSIGNMENT_TYPE_AUTO_CONFIGURE     0x03
#define REDFISH_HOST_INTERFACE_HOST_IP_ASSIGNMENT_TYPE_HOST_SELECTED      0x04

#define REDFISH_HOST_INTERFACE_HOST_IP_ADDRESS_FORMAT_UNKNOWN      0x00
#define REDFISH_HOST_INTERFACE_HOST_IP_ADDRESS_FORMAT_IP4          0x01
#define REDFISH_HOST_INTERFACE_HOST_IP_ADDRESS_FORMAT_IP6          0x02

#pragma pack(1)

//
//  Device Descriptors for USB
//  Device Type:
//  -idVendor(2-bytes),
//  -idProduct(2-bytes),
//  -iSerialNumber:
//  --- bLength(1-Byte),
//  --- bDescriptorType(1-Byte),
//  --- bString(Varies))
//
typedef struct {
  UINT8               bLength;
  UINT8               bDescriptorType;
  UINT8               bString[1];
} DEVICE_SERIAL_NUMBER;

typedef struct {
  UINT16                  IdVendor;
  UINT16                  IdProduct;
  DEVICE_SERIAL_NUMBER    ISecialNumber;
} USB_INTERFACE_DEVICE_DESCRIPTOR;

//
//  Device Descriptors for PCI/PCIe
//  Device Type:
//  -VendorID(2-Bytes),
//  -DeviceID(2-Bytes),
//  -Subsystem_Vendor_ID(2-bytes),
//  -Subsystem_ID(2-bytes)
//
typedef struct {
  UINT16                  VendorId;
  UINT16                  DeviceId;
  UINT16                  SubsystemVendorId;
  UINT16                  SubsystemId;
} PCI_OR_PCIE_INTERFACE_DEVICE_DESCRIPTOR;

//
//  Device Descriptors for OEM
//  Device Type:
//  -vendor_IANA(4-bytes),
//  -OEM defined data
//
typedef struct {
  UINT32                  VendorIana;
  UINT8                   OemDefinedData[1];
} OEM_DEVICE_DESCRIPTOR;

//
//  Interface Specific Data starts at offset 06h of the SMBIOS Type 42 struct.
//  This table defines the Interface Specific data for Interface Type 40h. There
//  are 3 types of Device Descriptor3 defined , however only 1 may be used in
//  specific Tape 42 table.
//
typedef struct {

  //
  //  USB Network Interface=02h,
  //  PCI/PCIe Network Interface=03h,
  //  OEM=80h-FFh,
  //  other values reserved
  //
  UINT8                       DeviceType;

  union {
    USB_INTERFACE_DEVICE_DESCRIPTOR           UsbDevice;
    PCI_OR_PCIE_INTERFACE_DEVICE_DESCRIPTOR   PciPcieDevice;
    OEM_DEVICE_DESCRIPTOR                     OemDevice;
  } DeviceDescriptor; // Device descriptor data formated based on Device Type.

} REDFISH_INTERFACE_DATA;

typedef struct {
  UINT8                        ProtocolIdentifier;
  UINT8                        ProtocolDataLen;
  UINT8                        ProtocolData[1];
} PROTOCOl_RECORD_DATA;

//
//  the protocol-specific data for the "Redfish Over IP" protocol
//
typedef struct {
  EFI_GUID            ServiceUuid;  //same as Redfish Service UUID in Redfish Service Root resource

  //
  //  Unknown=00h,
  //  Static=01h,
  //  DHCP=02h,
  //  AutoConfigure=03h,
  //  HostSelected=04h,
  //  other values reserved
  //
  UINT8               HostIpAssignmentType;

  //
  //  Unknown=00h,
  //  Ipv4=01h,
  //  Ipv6=02h,
  //  other values reserved
  //
  UINT8               HostIpAddressFormat;

  //
  //  Used for Static and AutoConfigure.
  //  For IPV4, use the first 4 Bytes and zero fill the remaining bytes.
  //
  UINT8               HostIpAddress[16];

  //
  //  Used for Static and AutoConfigure.
  //  For IPV4, use the first 4 Bytes and zero fill the remaining bytes.
  //
  UINT8               HostIpMask[16];

  //
  //  Unknown=00h,
  //  Static=01h,
  //  DHCP=02h,
  //  AutoConfigure=03h,
  //  HostSelected=04h,
  //  other values reserved
  //
  UINT8               RedfishServiceIpDiscoveryType;

  //
  //  Unknown=00h,
  //  Ipv4=01h,
  //  Ipv6=02h,
  //  other values reserved
  //
  UINT8               RedfishServiceIpAddressFormat;

  //
  //  Used for Static and AutoConfigure.
  //  For IPV4, use the first 4 Bytes and zero fill the remaining bytes.
  //
  UINT8               RedfishServiceIpAddress[16];

  //
  //  Used for Static and AutoConfigure.
  //  For IPV4, use the first 4 Bytes and zero fill the remaining bytes.
  //
  UINT8               RedfishServiceIpMask[16];

  UINT16              RedfishServiceIpPort;  // Used for Static and AutoConfigure.
  UINT32              RedfishServiceVlanId;  // Used for Static and AutoConfigure.
  UINT8               RedfishServiceHostnameLength;   // length of the following hostname string
  UINT8               RedfishServiceHostname[1];  // hostname of Redfish Service
} REDFISH_OVER_IP_PROTOCOL_DATA;

#pragma pack()

#endif

