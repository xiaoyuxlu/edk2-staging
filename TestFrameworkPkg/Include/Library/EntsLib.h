/** @file
  SCT extended data services declarations.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_ENTS_LIB_H
#define _EFI_ENTS_LIB_H

#include <Uefi.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DeviceIo.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/UnicodeCollation.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/PxeBaseCode.h>
#include <Protocol/PxeBaseCodeCallBack.h>
#include <Protocol/NetworkInterfaceIdentifier.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/DiskIo.h>
#include <Protocol/LoadFile.h>
#include <Protocol/SerialIo.h>
#include <Protocol/SimpleNetwork.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/ServiceBinding.h>
#include <Protocol/DriverConfiguration.h>
#include <Protocol/DriverDiagnostics.h>
#include <Protocol/ComponentName.h>
#include <Protocol/ManagedNetwork.h>

#include <Guid/FileInfo.h>
#include <Guid/FileSystemInfo.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Guid/GlobalVariable.h>
#include <Guid/Gpt.h>
#include <Guid/Mps.h>
#include <Guid/Mps.h>
#include <Guid/Acpi.h>
#include <Guid/SmBios.h>
#include <Guid/SalSystemTable.h>
#include <Guid/BlockIoVendor.h>

#include <Library/NetLib.h>


extern EFI_GUID             gtEfiDevicePathProtocolGuid;
extern EFI_GUID             gtEfiLoadedImageProtocolGuid;
extern EFI_GUID             gtEfiSimpleTextInProtocolGuid;
extern EFI_GUID             gtEfiSimpleTextOutProtocolGuid;
extern EFI_GUID             gtEfiBlockIoProtocolGuid;
extern EFI_GUID             gtEfiDiskIoProtocolGuid;
extern EFI_GUID             gtEfiSimpleFileSystemProtocolGuid;
extern EFI_GUID             gtEfiLoadFileProtocolGuid;
extern EFI_GUID             gtEfiDeviceIoProtocolGuid;
extern EFI_GUID             gtEfiLegacyBootProtocolGuid;
extern EFI_GUID             gtEfiUnicodeCollationProtocolGuid;
extern EFI_GUID             gtEfiSerialIoProtocolGuid;
extern EFI_GUID             gtEfiVgaClassProtocolGuid;
extern EFI_GUID             tTextOutSpliterProtocol;
extern EFI_GUID             tErrorOutSpliterProtocol;
extern EFI_GUID             tTextInSpliterProtocol;
extern EFI_GUID             gtEfiSimpleNetworkProtocolGuid;
extern EFI_GUID             gtEfiPxeBaseCodeProtocolGuid;
extern EFI_GUID             gtEfiPxeCallbackProtocolGuid;
extern EFI_GUID             gtEfiNetworkInterfaceIdentifierProtocolGuid;

extern EFI_GUID             tEfiGlobalVariable;
extern EFI_GUID             tGenericFileInfo;
extern EFI_GUID             gtEfiFileSystemInfoGuid;
extern EFI_GUID             gtEfiFileSystemVolumeLabelInfoGuid;
extern EFI_GUID             tPcAnsiProtocol;
extern EFI_GUID             tVt100Protocol;
extern EFI_GUID             tNullGuid;
extern EFI_GUID             gtEfiUnknownDeviceGuid;

extern EFI_GUID             gtEfiPartTypeSystemPartitionGuid;
extern EFI_GUID             gtEfiPartTypeLegacyMbrGuid;

extern EFI_GUID             gtEfiMpsTableGuid;
extern EFI_GUID             gtEfiAcpiTableGuid;
extern EFI_GUID             gtEfiAcpi20TableGuid;
extern EFI_GUID             gtEfiSMBIOSTableGuid;
extern EFI_GUID             gtEfiSalSystemTableGuid;

//
// Public read-only data in the EFI library
//
extern EFI_SYSTEM_TABLE     *gntST;
extern EFI_BOOT_SERVICES    *gntBS;
extern EFI_RUNTIME_SERVICES *gntRT;

extern EFI_MEMORY_TYPE      EntsPoolAllocationType;

extern EFI_UNICODE_COLLATION_PROTOCOL *EntsUnicodeInterface;

extern EFI_DEVICE_PATH_PROTOCOL        *gntDevicePath;
extern CHAR16                          *gntFilePath;
extern EFI_HANDLE                      mImageHandle;

#define SCT_PASSIVE_MODE_RECORD_FILE        L"SCT\\.passive.mode"

//
// External functions declarations
//
EFI_STATUS
EFIAPI
EfiInitializeEntsLib (
  IN  EFI_HANDLE                  ImageHandle,
  IN  EFI_SYSTEM_TABLE            *SystemTable
  )
/*++

Routine Description:

  Initialize Ents library.

Arguments:

  ImageHandle           - The image handle.
  SystemTable           - The system table.

Returns:

  EFI_SUCCESS - Operation succeeded.
  Others      - Some failure happened.

--*/
;

EFI_STATUS
GetImageDevicePath (
  IN EFI_HANDLE                   ImageHandle,
  OUT EFI_DEVICE_PATH_PROTOCOL    **DevicePath,
  OUT CHAR16                      **FilePath
  )
/*++

Routine Description:

  Get device path and file path from the image handle.

Arguments:

  ImageHandle - The image handle.
  DevicePath  - The device path of the image handle.
  FilePath    - The file path of the image handle.

Returns:

  EFI_SUCCESS          - Operation succeeded.
  EFI_OUT_OF_RESOURCES - Memory allocation failed.
  EFI_NOT_FOUND        - File path not found.
  Others               - Some failure happened.

--*/
;

//
// EFI Test Assertion Types
//
typedef enum {
  NET_EFI_TEST_ASSERTION_PASSED,
  NET_EFI_TEST_ASSERTION_WARNING,
  NET_EFI_TEST_ASSERTION_FAILED,
  NET_EFI_TEST_ASSERTION_CASE_BEGIN,
  NET_EFI_TEST_ASSERTION_CASE_OVER
} NET_EFI_TEST_ASSERTION;

EFI_STATUS
RecordMessage (
  IN  OUT CHAR16**ResultBuffer,
  IN  OUT UINTN *ResultBufferSize,
  IN     CHAR16 *Format,
  ...
  )
/*++

Routine Description:

  Record runtime information to a buffer.

Arguments:

  ResultBuffer      - Buffer space.
  ResultBufferSize  - Result buffer size in octets.
  Format            - Format string.
  ...               - Variables.

Returns:

  EFI_SUCCESS - Operation succeeded.
  EFI_OUT_OF_RESOURCES - Memory allocation failed.

--*/
;

EFI_STATUS
NetRecordAssertion (
  IN NET_EFI_TEST_ASSERTION   Type,
  IN EFI_GUID                 EventId,
  IN CHAR16                   *Description
  )
/*++

Routine Description:

  Records the test result to network.

Arguments:

  Type          - Test result.
  EventId       - GUID for the checkpoint.
  Description   - Simple description for the checkpoint.

Returns:

  EFI_SUCCESS           - Record the assertion successfully.
  EFI_INVALID_PARAMETER - Invalid Type.
  Others                - Problems when sending assertion packets.

--*/
;

//
// External functions declarations
//

VOID
EntsStrTrim (
  IN OUT CHAR16   *str,
  IN     CHAR16   c
  );

CHAR16 *
EntsStrDuplicate (
  IN CHAR16                         *Src
  )
;

INTN
EntsLibStubStriCmp (
  IN EFI_UNICODE_COLLATION_PROTOCOL   *This,
  IN CHAR16                           *s1,
  IN CHAR16                           *s2
  )
;

VOID
EntsLibStubStrLwrUpr (
  IN EFI_UNICODE_COLLATION_PROTOCOL   *This,
  IN CHAR16                           *Str
  )
;

BOOLEAN
EntsLibStubMetaiMatch (
  IN EFI_UNICODE_COLLATION_PROTOCOL   *This,
  IN CHAR16                           *String,
  IN CHAR16                           *Pattern
  )
;

EFI_STATUS
Char16ToChar8 (
  IN  CHAR16 *Src,
  OUT CHAR8  *Dest,
  OUT UINTN  Size
  )
/*++

Routine Description:

  Convert a NULL-ended string of CHAR16 to CHAR8.

Arguments:

  Src   - NULL ended string of CHAR16.
  Dest  - String of CHAR8.
  Size  - The length of CHAR8 string

Returns:

  EFI_SUCCESS - Operation succeeded.
  EFI_INVALID_PARAMETER - Parameter invalid.
  EFI_OUT_OF_RESOURCES  - Memory allocation failed.
  Others      - Some failure happened.

--*/
;

EFI_STATUS
Char8ToChar16 (
  IN  CHAR8  *Src,
  IN  UINTN  Size,
  OUT CHAR16 *Dest
  )
/*++

Routine Description:

  Convert a NULL-ended string of CHAR8 to CHAR16.

Arguments:

  Src   - String of CHAR8 which is not necessarily null ended.
  Size  - The byte number of CHAR8 string.
  Dest  - String of coresponding CHAR16,

Returns:

  EFI_INVALID_PARAMETER - Parameter invalid.
  EFI_OUT_OF_RESOURCES - Memory allocation failed.
  EFI_SUCCESS - Operation succeeded.

--*/
;

UINTN
strspn (
  const CHAR16 *s,
  const CHAR16 *accept
  )
/*++

Routine Description:

  Find the first substring.

--*/
;

CHAR16 *
strpbrk (
  const CHAR16 *cs,
  const CHAR16 *ct
  )
/*++

Routine Description:

  Scan strings for characters in specified character sets.

--*/
;

CHAR16 *
strtok_line (
  CHAR16       *s,
  const CHAR16 *ct
  )
;

CHAR16 *
strtok_arg (
  CHAR16       *s,
  const CHAR16 *ct
  )
/*++

Routine Description:

  Find the next token in a string
  Tokens are separated by separators defined in "ct"
  If enclosed in double quotations, other separators are invalid. Then token is
  the content in the quotations.

--*/
;

CHAR16 *
strtok_field (
  CHAR16       *s,
  const CHAR16 *ct
  )
/*++

Routine Description:

  Find the next token in a string.

--*/
;

INTN
EntsStrToUINTN (
  IN CHAR16        *Str,
  IN OUT UINTN     *Value
  )
;

INTN
EntsHexStrToUINTN (
  IN CHAR16        *Str,
  IN OUT UINTN     *Value
  )
;

EFI_STATUS
EntsStrToValue (
  IN CHAR16             *Buffer,
  OUT UINTN             *Value
  )
/*++

Routine Description:

  Convert a string to a value.

Arguments:

  Buffer  - String buffer.
  Value   - Pointer to receive the converted value.

Returns:

  EFI_INVALID_PARAMETER - Parameter invalid.
  EFI_SUCCESS - Operation succeeded.

--*/
;

EFI_STATUS
EntsValueToStr (
  IN UINTN              Value,
  OUT CHAR16            *Buffer
  )
/*++

Routine Description:

  Convert a value to a string.

Arguments:

  Value   - Value to be converted.
  Buffer  - Receive string buffer pointer.

Returns:

  EFI_INVALID_PARAMETER - Parameter invalid.
  EFI_SUCCESS - Operation succeeded.

--*/
;

EFI_STATUS
EntsHexValueToStr (
  IN UINTN              Value,
  OUT CHAR16            *Buffer
  )
/*++

Routine Description:

  Convert a hexadecimal value to a string.

Arguments:

  Value   - Value to be converted.
  Buffer  - Pointer to string receive buffer.

Returns:

  EFI_INVALID_PARAMETER - Parameter invalid.
  EFI_SUCCESS - Operation succeeded.

--*/
;

EFI_STATUS
EntsStrToHexValue (
  IN CHAR16             *Buffer,
  OUT UINTN             *Value
  )
/*++

Routine Description:

  Convert a string to a hexadecimal value.

Arguments:

  Buffer  - String buffer to be converted.
  Value   - Receive value pointer.

Returns:

  EFI_INVALID_PARAMETER - Parameter invalid.
  EFI_SUCCESS - Operation succeeded.

--*/
;

EFI_STATUS
EntsBooleanToStr (
  IN BOOLEAN            Value,
  OUT CHAR16            *Buffer
  )
/*++

Routine Description:

  Convert a boolean to a string.

Arguments:

  Value   - Boolean value.
  Buffer  - Receive string buffer.

Returns:

  EFI_INVALID_PARAMETER - Parameter invalid.
  EFI_SUCCESS - Operation succeeded.

--*/
;

EFI_STATUS
EntsStrToBoolean (
  IN CHAR16             *Buffer,
  OUT BOOLEAN           *Value
  )
/*++

Routine Description:

  Convert a string to a boolean.

Arguments:

  Buffer  - String buffer to be converted.
  Value   - Receive value pointer.

Returns:

  EFI_INVALID_PARAMETER - Parameter invalid.
  EFI_UNSUPPORTED - String not supported.
  EFI_SUCCESS - Operation succeeded.

--*/
;

BOOLEAN
EntsStrBeginWith (
  IN CHAR16             *Str,
  IN CHAR16             *SubStr
  )
;

BOOLEAN
EntsStrEndWith (
  IN CHAR16             *Str,
  IN CHAR16             *SubStr
  )
/*++

Routine Description:

  Test if the string is end with the sub string.

Arguments:

  Str     - NULL ended string.
  SubStr  - NULL ended string.

Returns:

  TRUE    - Str ends with SubStr.
  FALSE   - Str does not end with SubStr.

--*/
;

CHAR16 *
EntsStrChr (
  IN  CHAR16  *Str,
  IN  CHAR16  c
  );

UINTN
EntsStrStr (
  IN  CHAR16  *Str,
  IN  CHAR16  *Pat
  )
/*++

Routine Description:

  Search Pat in Str.

Arguments:

  Str - String to be searched.
  Pat - Search pattern.

Returns:

  0 : not found
  >= 1 : found position + 1

--*/
;

UINTN
HexStringToValue (
  IN CHAR16             *String,
  IN UINTN              Length
  )
/*++

Routine Description:

  Convert a hex string to a value.

Arguments:

  String  - Hex string buffer.
  Length  - Hex string length.

Returns:

  Converted value.

--*/
;

INTN
a2i (
  IN CHAR16       Ch
  )
;

CHAR16
i2A (
  UINTN    x
  )
;

CHAR16
i2a (
  UINTN    x
  )
;

EFI_STATUS
EntsIpToStr (
  IN EFI_IP_ADDRESS       *IpAddr,
  IN UINTN                StrSize,
  IN OUT CHAR16           *Str
  )
;

EFI_STATUS
EntsStrToMac (
  IN CHAR16              *Str,
  IN OUT EFI_MAC_ADDRESS *MacAddr
  )
;

EFI_STATUS
EntsMacToStr (
  IN EFI_MAC_ADDRESS     *MacAddr,
  IN UINTN               StrSize,
  IN OUT CHAR16          *Str
  )
;

EFI_STATUS
EntsTimeToStr (
  IN EFI_TIME            *Time,
  IN UINTN               StrSize,
  IN OUT CHAR16          *Str
  )
;

EFI_STATUS
EntsStatusToStr (
  IN EFI_STATUS          Status,
  IN UINTN               StrSize,
  IN OUT CHAR16          *Str
  )
;

CHAR8
M2S (
  IN CHAR8                     Ch
  )
;

EFI_STATUS
MemToString (
  IN VOID                      *Mem,
  IN OUT CHAR16                *String,
  IN UINT32                    MemSize
  )
/*++

Routine Description:

  Convert Memory to a Unicode string.

Arguments:

  Mem     - Memory buffer to be converted.
  String  - Receive string buffer pointer.
  MemSize - Memory buffer size.

Returns:

  EFI_SUCCESS - Operation succeeded.

--*/
;

CHAR8
S2M (
  IN CHAR8                     Ch
  )
;

EFI_STATUS
StringToMem (
  IN CHAR16                    *String,
  IN OUT VOID                  *Mem,
  IN UINT32                    MemSize
  )
/*++

Routine Description:

  Convert a Unicode string to Memory.

Arguments:

  String  - Unicode string buffer.
  Mem     - Receive memory buffer pointer.
  MemSize - Receive memory buffer size.

Returns:

  EFI_SUCCESS - Operation succeeded.

--*/
;

//
// Debug Level
//
#define EFI_ENTS_D_ERROR    0x01
#define EFI_ENTS_D_WARNING  0x02
#define EFI_ENTS_D_TRACE    0x04
#define EFI_ENTS_D_LEVEL    (EFI_ENTS_D_ERROR)

#define ENTS_DEBUG

#ifdef ENTS_DEBUG
#define EFI_ENTS_DEBUG(a)   EfiEntsDebug a
#define EFI_ENTS_STATUS(a)  EfiEntsStatus a
#else
#define EFI_ENTS_DEBUG(a)
#define EFI_ENTS_STATUS(a)
#endif

#define ENTS_MAX_BUFFER_SIZE              512
#define EFI_ENTS_FILE_LOG                 L"Ents.log"
#define EFI_NET_ASSERTION_RESULT_PASS     "PASS"
#define EFI_NET_ASSERTION_RESULT_FAIL     "FAIL"
#define EFI_NET_ASSERTION_RESULT_WARN     "WARN"
#define EFI_NET_ASSERTION_TYPE_ASSERTION  "ASSERTION"
#define EFI_NET_ASSERTION_TYPE_CASE_BEGN  "CASE_BEGN"
#define EFI_NET_ASSERTION_TYPE_CASE_OVER  "CASE_OVER"
#define EFI_NET_ASSERTION_MAX_LEN         1400
#define NET_ASSERTION_MSG_LEN             1024
#define NET_ASSERTION_MSG_HEADER_LEN      128
#define GUID_STRING_LEN                   37
#define RESULT_STRING_LEN                 5
#define TYPE_STRING_LEN                   10

//
// EFI_ENTS_DEBUG worker function
//
EFI_STATUS
EntsInitializeDebugServices (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevicePath,
  IN CHAR16                       *FilePath
  )
/*++

Routine Description:

  Initialize the debug services.

Arguments:

  DevicePath  - Pointer to EFI_DEVICE_PATH_PROTOCOL instance.
  FilePath    - FilePath string.

Returns:

  EFI_SUCCESS          - Operation succeeded.
  EFI_OUT_OF_RESOURCES - Memory allocation failed.
  Others               - Some failure happened.

--*/
;

EFI_STATUS
CloseDebugServices (
  VOID
  )
/*++

Routine Description:

  Free the debug services.

Arguments:

  None

Returns:

  EFI_SUCCESS - Operation succeeded.
  Others      - Some failure happened.

--*/
;

VOID
EfiEntsDebug (
  IN UINTN              Level,
  IN CHAR16             *Format,
  ...
  )
/*++

Routine Description:

  Ents debugging services.

Arguments:

  Level   - Debug level like ERROR, WARNING, TRACE etc.
  Format  - String format.
  ...     - Variables.

Returns:

  None.

--*/
;

//
// EFI_ENTS_STATUS worker function
//
VOID
EfiEntsStatus (
  IN CHAR16             *Format,
  ...
  )
/*++

Routine Description:

  Worker function for EFI_ENTS_STATUS marco.

Arguments:

  Format  - Format string.
  ...     - Variables.

Returns:

  None.

--*/
;

#define MINS_PER_HOUR   60
#define HOURS_PER_DAY   24
#define SECS_PER_MIN    60
#define SECS_PER_HOUR   (SECS_PER_MIN * MINS_PER_HOUR)
#define SECS_PER_DAY    (SECS_PER_HOUR * HOURS_PER_DAY)
#define DAYS_PER_NYEAR  365
#define DAYS_PER_LYEAR  366

UINT32
SecondsElapsedFromBaseYear (
  IN UINT16             BaseYear,
  IN UINT16             Year,
  IN UINT8              Month,
  IN UINT8              Day,
  IN UINT8              Hour,
  IN UINT8              Minute,
  IN UINT8              Second
  )
/*++

Routine Description:

  Calculate the elapsed seconds from the base year.

Arguments:

  BaseYear  - The base year.
  Year      - Current year.
  Month     - Current month.
  Day       - Current day.
  Hour      - Current hour.
  Minute    - Current minute.
  Second    - Current second.

Returns:

  The elapsed seconds.

--*/
;

#pragma pack(1)
typedef struct ether_packet_struc {
  UINT8   DstMac[6];
  UINT8   SrcMac[6];
  UINT16  EtherType;
  UINT8   data[1];
} ETHER_PACKET;
#pragma pack()

#define ETHER_HEAD_LEN          (sizeof (ETHER_PACKET) - 1)

#define IP_FLAG_DF              (unsigned short) 0x4000
#define IP_FLAG_MF              (unsigned short) 0x2000
#define IP_OFFSET               (unsigned short) 0x1FFF

#define IP_PROTOCOL_ICMP        0x01
#define IP_PROTOCOL_IGMP        0x02
#define IP_PROTOCOL_GGP         0x03
#define IP_PROTOCOL_IP          0x04
#define IP_PROTOCOL_ST          0x05
#define IP_PROTOCOL_TCP         0x06
#define IP_PROTOCOL_UCL         0x07
#define IP_PROTOCOL_EGP         0x08
#define IP_PROTOCOL_IGP         0x09
#define IP_PROTOCOL_BBN_RCC_MON 0x0a
#define IP_PROTOCOL_NVP_II      0x0b
#define IP_PROTOCOL_PUP         0x0c
#define IP_PROTOCOL_ARGUS       0x0d
#define IP_PROTOCOL_EMCON       0x0e
#define IP_PROTOCOL_XNET        0x0f
#define IP_PROTOCOL_CHAOS       0x10
#define IP_PROTOCOL_UDP         (unsigned char) 0x11

#define IP_DEFAULT_TTL          (UINT8) 64
#define IP_DEFAULT_ID           (UINT16) 0x1234
#define IP_DEFAULT_TOS          (UINT8) 0

//
// IP4
//
#pragma pack(1)
typedef struct ipv4_packet_struc {
  char            verhlen;
  char            tos;
  unsigned short  totallen;
  unsigned short  id;
  unsigned short  flagoff;
  unsigned char   ttl;
  char            protocol;
  unsigned short  cksum;
  long            srcadd;
  long            dstadd;
  char            data[1];
} IPV4_PACKET;
#pragma pack()

#define IPV4_HEAD_LEN (sizeof (IPV4_PACKET) - 1)

//
// UDP4
//
#pragma pack(1)
typedef struct udp_packet_struc {
  unsigned short  udp_src;    /* source UDP port number    */
  unsigned short  udp_dst;    /* destination UDP port number   */
  unsigned short  udp_len;    /* length of UDP data     */
  unsigned short  udp_cksum;  /* UDP checksum (0 => none)  */
  char            data[1];
} UDP_PACKET;
#pragma pack()

#define UDP_HEAD_LEN  (sizeof (UDP_PACKET) - 1)

//
// List entry - doubly linked list
// Redefined due to conflicts between Shell and EDK.
//
typedef struct _NET_EFI_LIST_ENTRY {
  struct _NET_EFI_LIST_ENTRY  *Flink;
  struct _NET_EFI_LIST_ENTRY  *Blink;
} NET_EFI_LIST_ENTRY;

#define NetInitializeListHead(ListHead) \
  (ListHead)->ForwardLink = ListHead; \
  (ListHead)->BackLink = ListHead;

#define NetIsListEmpty(ListHead)  ((BOOLEAN) ((ListHead)->ForwardLink == (ListHead)))

#define _NetRemoveEntryList(Entry) { \
    NET_EFI_LIST_ENTRY  *_Blink, *_Flink; \
    _Flink        = (Entry)->ForwardLink; \
    _Blink        = (Entry)->BackLink; \
    _Blink->ForwardLink = _Flink; \
    _Flink->BackLink = _Blink; \
  }

#ifdef EFI_DEBUG
#define NetRemoveEntryList(Entry) \
  _NetRemoveEntryList (Entry); \
  (Entry)->ForwardLink  = (NET_EFI_LIST_ENTRY *) EFI_BAD_POINTER; \
  (Entry)->BackLink  = (NET_EFI_LIST_ENTRY *) EFI_BAD_POINTER;
#else
#define NetRemoveEntryList(Entry) _NetRemoveEntryList (Entry);
#endif

#define NetInsertTailList(ListHead, Entry) { \
    NET_EFI_LIST_ENTRY  *_ListHead, *_Blink; \
    _ListHead         = (ListHead); \
    _Blink            = _ListHead->BackLink; \
    (Entry)->ForwardLink    = _ListHead; \
    (Entry)->BackLink    = _Blink; \
    _Blink->ForwardLink     = (Entry); \
    _ListHead->BackLink  = (Entry); \
  }

#define NetInsertHeadList(ListHead, Entry) { \
    NET_EFI_LIST_ENTRY  *_ListHead, *_Flink; \
    _ListHead         = (ListHead); \
    _Flink            = _ListHead->ForwardLink; \
    (Entry)->ForwardLink    = _Flink; \
    (Entry)->BackLink    = _ListHead; \
    _Flink->BackLink     = (Entry); \
    _ListHead->ForwardLink  = (Entry); \
  }

#define NetSwapListEntries(Entry1, Entry2) { \
    NET_EFI_LIST_ENTRY  *Entry1Flink, *Entry1Blink; \
    NET_EFI_LIST_ENTRY  *Entry2Flink, *Entry2Blink; \
    Entry2Flink         = (Entry2)->ForwardLink; \
    Entry2Blink         = (Entry2)->BackLink; \
    Entry1Flink         = (Entry1)->ForwardLink; \
    Entry1Blink         = (Entry1)->BackLink; \
    Entry2Blink->ForwardLink  = Entry2Flink; \
    Entry2Flink->BackLink  = Entry2Blink; \
    (Entry2)->ForwardLink     = Entry1; \
    (Entry2)->BackLink     = Entry1Blink; \
    Entry1Blink->ForwardLink  = (Entry2); \
    (Entry1)->BackLink     = (Entry2); \
  }

#define NetListEntry(Entry, Type, Field) _CR(Entry, Type, Field)
#define NetListHead(ListHead, Type, Field) \
        NetListEntry((ListHead)->ForwardLink, Type, Field)
//
// NET_DEBUG_CACHE_BUFFER
//
typedef struct _NET_DEBUG_CACHE_BUFFER {
  CHAR8 * Message;
  UINT16 MessageLen;
  NET_EFI_LIST_ENTRY Entry;
} NET_DEBUG_CACHE_BUFFER;

//
// Network helper functions. Implemented in Lib\NetLib.c
//
#define NET_TPL_SYSTEM_POLL       EFI_TPL_NOTIFY
#define NET_TPL_GLOBAL_LOCK       NET_TPL_SYSTEM_POLL
#define NET_TPL_LOCK              (EFI_TPL_CALLBACK+1)
#define NET_TPL_EVENT             EFI_TPL_CALLBACK
#define NET_TPL_RECYCLE           EFI_TPL_CALLBACK
#define NET_TPL_FAST_RECYCLE      NET_TPL_SYSTEM_POLL
#define NET_TPL_SLOW_TIMER        (EFI_TPL_CALLBACK-1)
#define NET_TPL_FAST_TIMER        (EFI_TPL_CALLBACK+1)
#define NET_TPL_TIMER             EFI_TPL_CALLBACK

CHAR8 *
Unicode2Ascii (
  OUT CHAR8          *AsciiStr,
  IN  CHAR16         *UnicodeStr
  );

EFI_STATUS
NetAssertionUtilityInstall (
  VOID
  )
/*++

Routine Description:

  Used to initialize NetAssertionConfigData.

Arguments:

  None

Returns:

  EFI_SUCCESS - Operation succeeded.

--*/
;

VOID
NetAssertionUtilityUninstall (
  VOID
  );

//
// External functions declarations
//
UINTN
EntsPrint (
  IN CHAR16                         *fmt,
  ...
  )
/*++

Routine Description:

    Prints a formatted unicode string to the default console

Arguments:

    fmt         - Format string
    ...         - Variables.

Returns:

    Length of string printed to the console

--*/
;

UINTN
EntsSPrint (
  OUT CHAR16                        *Str,
  IN UINTN                          StrSize,
  IN CHAR16                         *fmt,
  ...
  )
/*++

Routine Description:

    Prints a formatted unicode string to a buffer

Arguments:

    Str         - Output buffer to print the formatted string into

    StrSize     - Size of Str.  String is truncated to this size.
                  A size of 0 means there is no limit

    fmt         - The format string
    ...         - Variables.

Returns:

    String length returned in buffer

--*/
;

UINTN
EntsVSPrint (
  OUT CHAR16                        *Str,
  IN UINTN                          StrSize,
  IN CHAR16                         *fmt,
  IN VA_LIST                        vargs
  )
/*++

Routine Description:

    Prints a formatted unicode string to a buffer

Arguments:

    Str         - Output buffer to print the formatted string into
    StrSize     - Size of Str.  String is truncated to this size.
      A size of 0 means there is no limit
    fmt         - The format string
    vargs       - Variable list.

Returns:

    String length returned in buffer

--*/
;

CHAR16                          *
EntsPoolPrint (
  IN CHAR16                         *fmt,
  ...
  );

typedef struct {
  CHAR16            *str;
  UINTN             len;
  UINTN             maxlen;
} ENTS_POOL_PRINT;

CHAR16 *
EntsCatPrint (
  IN OUT ENTS_POOL_PRINT     *Str,
  IN CHAR16             *fmt,
  ...
  );

BOOLEAN
EntsGrowBuffer (
  IN OUT EFI_STATUS   *Status,
  IN OUT VOID         **Buffer,
  IN UINTN            BufferSize
  )
/*++

Routine Description:

    Helper function called as part of the code needed
    to allocate the proper sized buffer for various
    EFI interfaces.

Arguments:

    Status      - Current status

    Buffer      - Current allocated buffer, or NULL

    BufferSize  - Current buffer size needed

Returns:

    TRUE - if the buffer was reallocated and the caller
    should try the API again.

--*/
;

VOID
EntsGuidToString (
  OUT CHAR16                        *Buffer,
  IN EFI_GUID                       *Guid
  )
/*++

Routine Description:

  Converts Guid to a string

Arguments:

  Buffer      - On return, a pointer to the buffer which contains the string.
  Guid        - guid to compare

Returns:
  none

--*/
;

EFI_STATUS
EntsWaitForSingleEvent (
  IN EFI_EVENT                                                Event,
  IN UINT64                                                   Timeout OPTIONAL
  )
/*++

Routine Description:

  Function waits for a given event to fire, or for an optional timeout to expire.

Arguments:
  Event            - The event to wait for

  Timeout          - An optional timeout value in 100 ns units.

Returns:

  EFI_SUCCESS       - Event fired before Timeout expired.
  EFI_TIME_OUT     - Timout expired before Event fired..

--*/
;

EFI_DEVICE_PATH_PROTOCOL        *
EntsDuplicateDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevPath
  )
/*++

Routine Description:

  Function creates a duplicate copy of an existing device path.

Arguments:

  DevPath        - A pointer to a device path data structure

Returns:

  If the memory is successfully allocated, then the contents of DevPath are copied
  to the newly allocated buffer, and a pointer to that buffer is returned.
  Otherwise, NULL is returned.

--*/
;

EFI_DEVICE_PATH_PROTOCOL        *
EntsUnpackDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevPath
  )
/*++

Routine Description:

  Function unpacks a device path data structure so that all the nodes of a device path
  are naturally aligned.

Arguments:

  DevPath        - A pointer to a device path data structure

Returns:

  If the memory for the device path is successfully allocated, then a pointer to the
  new device path is returned.  Otherwise, NULL is returned.

--*/
;

CHAR16 *
EntsDevicePathToStr (
  EFI_DEVICE_PATH_PROTOCOL                     *DevPath
  );

EFI_STATUS
SetContextRecord (
  IN EFI_DEVICE_PATH_PROTOCOL                  *DevicePath,
  IN CHAR16                                    *FileName,
  IN CHAR16                                    *Key,
  IN UINTN                                     Size,
  IN VOID                                      *Value
  );

EFI_STATUS
GetContextRecord (
  IN EFI_DEVICE_PATH_PROTOCOL                  *DevicePath,
  IN CHAR16                                    *FileName,
  IN CHAR16                                    *Key,
  IN UINTN                                     *BufSize,
  OUT VOID                                     *RecordBuf
  );

EFI_STATUS
PassiveTestContextCreate (
  IN EFI_DEVICE_PATH_PROTOCOL                  *DevicePath,
  IN CHAR16                                    *FileName
  );

EFI_STATUS
PassiveTestContextDelete (
  IN EFI_DEVICE_PATH_PROTOCOL                  *DevicePath,
  IN CHAR16                                    *FileName
  );

EFI_STATUS
EntsNetworkServiceBindingGetControllerHandle (
  IN  EFI_GUID                 *ProtocolGuid,
  OUT EFI_HANDLE               *ClientHandle
  );

VOID
EntsChooseNICAndSave (
  VOID
  );

EFI_STATUS
GetMacAddress (
  OUT UINT8                         *MacAddr
  );

UINTN
Atoi (
  CHAR16  *str
  );

INTN
EFIAPI
StriCmp (
  IN CHAR16   *s1,
  IN CHAR16   *s2
  );

#endif
