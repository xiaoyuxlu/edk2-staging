/** @file
  This is the include file for file system access API. 

  Copyright (c) 2007 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __FS_ACCESS_LIB__
#define __FS_ACCESS_LIB__

//*******************************************************
// File Attribute Bits
//*******************************************************
#define EFI_FILE_READ_ONLY   0x0000000000000001
#define EFI_FILE_HIDDEN      0x0000000000000002
#define EFI_FILE_SYSTEM      0x0000000000000004
#define EFI_FILE_RESERVED    0x0000000000000008
#define EFI_FILE_DIRECTORY   0x0000000000000010
#define EFI_FILE_ARCHIVE     0x0000000000000020
#define EFI_FILE_VALID_ATTR  0x0000000000000037

//
// EFI Time Abstraction:
//  Year:       2000 - 20XX
//  Month:      1 - 12
//  Day:        1 - 31
//  Hour:       0 - 23
//  Minute:     0 - 59
//  Second:     0 - 59
//  Nanosecond: 0 - 999,999,999
//  TimeZone:   -1440 to 1440 or 2047
//
typedef struct {
  UINT16  Year;
  UINT8   Month;
  UINT8   Day;
  UINT8   Hour;
  UINT8   Minute;
  UINT8   Second;
  UINT8   Pad1;
  UINT32  Nanosecond;
  INT16   TimeZone;
  UINT8   Daylight;
  UINT8   Pad2;
} TIME;

typedef struct {
	///
	/// The size of the EFI_FILE_INFO structure, including the Null-terminated FileName string.
	///
	UINT64    Size;
	///
	/// The size of the file in bytes.
	///
	UINT64    FileSize;
	///
	/// PhysicalSize The amount of physical space the file consumes on the file system volume.
	///
	UINT64    PhysicalSize;
	///
	/// The time the file was created.
	///
	TIME  CreateTime;
	///
	/// The time when the file was last accessed.
	///
	TIME  LastAccessTime;
	///
	/// The time when the file's contents were last modified.
	///
	TIME  ModificationTime;
	///
	/// The attribute bits for the file.
	///
	UINT64    Attribute;
	///
	/// The Null-terminated name of the file.
	///
	CHAR16    FileName[1];
} EFI_FILE_INFO;

/** 
  Read file (part of file) from media by file name and EFI patition name

  @param PartitionName   The EFI Partition name.
  @param FileName        The file name.
  @param offset          The offset in file to start reading.
  @param BufferLenth     The lenth of file to read. 
                         input:  0 means to read in full lenth
                         output: exact length to read. 0 means failure
  
  @return Return the pointer of file read in memory.

**/
VOID *
ReadFromDevice(
  IN CHAR16     *PartitionName,
  IN CHAR16     *FileName,
  IN UINT64     Offset,
  IN OUT UINT64 *BufferLenth
  );

/**
  Write file to media by file name and EFI patition name

  @param PartitionName   The EFI Partition name.
  @param FileName        The file name.
  @param FileSize        The file size.
  @param FileBuffer      The file content.
  @param Offset          The offset in file to start writing.

  @return Return  
  EFI_SUCCESS           - Set the file info successfully.
  EFI_WRITE_PROTECTED   - The disk is write protect.
  EFI_ACCESS_DENIED     - The file is read-only.
  EFI_DEVICE_ERROR      - The OFile is not valid.
  EFI_UNSUPPORTED       - The open file is not a file.
                        - The writing file size is larger than 4GB.
  other                 - An error occurred when operation.

**/
EFI_STATUS
WriteToDevice(
  IN CHAR16 *PartitionName,
  IN CHAR16 *FileName,
  IN UINT64 FileSize,
  IN VOID   *FileBuffer,
  IN UINT64 Offset
  );

/**
  Create directory on media by name and EFI patition name

  This function can create the directory with full path and directory name.
  the full name should be end with "\\"

  @param PartitionName   The EFI Partition name.
  @param FileName        The directory path and name.

  @return Return  
  EFI_INVALID_PARAMETER - The FileName is NULL or the file string is empty.
  The OpenMode is not supported.
  The Attributes is not the valid attributes.
  EFI_OUT_OF_RESOURCES  - Can not allocate the memory for file string.
  EFI_SUCCESS           - Open the file successfully.
  Others                - The status of open file.
**/
EFI_STATUS
CreateDirectory(
  IN CHAR16 *PartitionName,
  IN CHAR16 *FileName
  );

/**
  delete file/directory on media by file/directory name and EFI patition Path

  This function can only delete one file or empty directory. directory name shouldn't
  suffix "\\"

  @param PartitionName   The EFI Partition name.
  @param FileName        The file or directory name.

  @return Return  
  EFI_SUCCESS              - Delete the file successfully.
  EFI_WARN_DELETE_FAILURE  - Fail to delete the file.

**/
EFI_STATUS
DeleteFileOnDevice(
  IN CHAR16 *PartitionName,
  IN CHAR16 *FileName
  );

/**
  Run/Boot EFI PE file from media by file name and EFI patition path

  @param PartitionName     The EFI media Partition path.
  @param FileName          The file name.
  @param OptionalData      The pointer of comdline input Para.
  @param OptionalDataSize  The input Para Length

  @retval  EFI_SUCCESS            Run/Boot EFI PE file successfully.
  @retval  EFI_NOT_FOUND          The specified file could not be found.
  @retval  EFI_OUT_OF_RESOURCES   There are not enough resources available to retrieve 
                                  the matching FFS section.
  @retval  RETURN_LOAD_ERROR      The image fail to load.
  @retval  EFI_INVALID_PARAMETER  Invalid input Paras.

**/
EFI_STATUS
RunFromEfiPeFile(
  IN CHAR16   *PartitionName,
  IN CHAR16   *FileName,
  IN UINT16   *OptionalData,
  IN UINT32   OptionalDataSize
);

/**
  This function search all the folders/files under specific directory path and get back their info.
  the info is the table of EFI_FILE_INFO.

  @param PartitionName     The pointer of EFI Partition path.
  @param FileName          The pointer of directory path name.
  @param BufferLenth       The pointer of pointer array size.

  @return Return the address of pointer arrary which include 
  EFI_FILE_INFO pointers for directory info.

**/
VOID *
ReadDirectoryFromDevice(
  IN CHAR16  *PartitionName,
  IN CHAR16  *FileName,
  OUT UINT64 *BufferLenth
  );

/**
  Searches all the available firmware volumes and returns the first matching FFS section. 

  This function searches all the firmware volumes for FFS files with an FFS filename specified by NameGuid.  
  The order in which the firmware volumes are searched is not deterministic. For each FFS file found, a search 
  is made for FFS sections of type SectionType. If the FFS file contains at least SectionInstance instances 
  of the FFS section specified by SectionType, then the SectionInstance instance is returned in Buffer. 
  Buffer is allocated using AllocatePool(), and the size of the allocated buffer is returned in Size. 
  It is the caller's responsibility to use FreePool() to free the allocated buffer.  
  See EFI_FIRMWARE_VOLUME2_PROTOCOL.ReadSection() for details on how sections 
  are retrieved from an FFS file based on SectionType and SectionInstance.

  If SectionType is EFI_SECTION_TE, and the search with an FFS file fails, 
  the search will be retried with a section type of EFI_SECTION_PE32.
  This function must be called with a TPL <= TPL_NOTIFY.

  If NameGuid is NULL, then ASSERT().
  If Buffer is NULL, then ASSERT().
  If Size is NULL, then ASSERT().


  @param  NameGuid                A pointer to to the FFS filename GUID to search for  
                                  within any of the firmware volumes in the platform. 
  @param  Buffer                  On output, a pointer to a callee-allocated buffer 
                                  containing the FFS file section that was found.  
                                  It is the caller's responsibility to free this 
                                  buffer using FreePool().
  @param  Size                    On output, a pointer to the size, in bytes, of Buffer.

  @retval  EFI_SUCCESS            The specified FFS section was returned.
  @retval  EFI_NOT_FOUND          The specified FFS section could not be found.
  @retval  EFI_OUT_OF_RESOURCES   There are not enough resources available to retrieve 
                                  the matching FFS section.
  @retval  EFI_DEVICE_ERROR       The FFS section could not be retrieves due to a 
                                  device error.
  @retval  EFI_ACCESS_DENIED      The FFS section could not be retrieves because the 
                                  firmware volume that contains the matching FFS 
                                  section does not allow reads.
  @retval  EFI_INVALID_PARAMETER  Invalid input Paras.                               
**/ 
EFI_STATUS
EFIAPI
GetFileFromFlash  (
  IN  EFI_GUID                *NameGuid,
  OUT VOID                    **Buffer,
  OUT UINTN                   *Size
  );

/**
This function get all the File Partition info on the connected storages and get back.
the info is the EFI device Path String table of Char*.

@param  BufferLenth               On output, a pointer to the size, in bytes,

@return Return the address of pointer arrary which include
String pointers for Partition device Path info.

**/
VOID *
GetAllPartitionPaths(
	OUT UINT64 *BufferLenth
);

#endif