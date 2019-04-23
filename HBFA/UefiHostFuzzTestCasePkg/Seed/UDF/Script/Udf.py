## @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

from   ctypes import *

UDF_LOGICAL_SECTOR_SIZE = 0x800
UDF_VRS_START_OFFSET = 0x8000

MAX_CORRECTION_BLOCKS_NUM = 512

UDF_BEA_IDENTIFIER   = "BEA01"
UDF_NSR2_IDENTIFIER  = "NSR02"
UDF_NSR3_IDENTIFIER  = "NSR03"
UDF_TEA_IDENTIFIER   = "TEA01"


class UDF_VOLUME_DESCRIPTOR_ID:
    UdfPrimaryVolumeDescriptor = 1
    UdfAnchorVolumeDescriptorPointer = 2
    UdfVolumeDescriptorPointer = 3
    UdfImplemenationUseVolumeDescriptor = 4
    UdfPartitionDescriptor = 5
    UdfLogicalVolumeDescriptor = 6
    UdfUnallocatedSpaceDescriptor = 7
    UdfTerminatingDescriptor = 8
    UdfLogicalVolumeIntegrityDescriptor = 9
    UdfFileSetDescriptor = 256
    UdfFileIdentifierDescriptor = 257
    UdfAllocationExtentDescriptor = 258
    UdfFileEntry = 261
    UdfExtendedFileEntry = 266

    TagStringTable = [
        [UdfPrimaryVolumeDescriptor,          "UdfPrimaryVolumeDescriptor"],
        [UdfAnchorVolumeDescriptorPointer,    "UdfAnchorVolumeDescriptorPointer"],
        [UdfVolumeDescriptorPointer,          "UdfVolumeDescriptorPointer"],
        [UdfImplemenationUseVolumeDescriptor, "UdfImplemenationUseVolumeDescriptor"],
        [UdfPartitionDescriptor,              "UdfPartitionDescriptor"],
        [UdfLogicalVolumeDescriptor,          "UdfLogicalVolumeDescriptor"],
        [UdfUnallocatedSpaceDescriptor,       "UdfUnallocatedSpaceDescriptor"],
        [UdfTerminatingDescriptor,            "UdfTerminatingDescriptor"],
        [UdfLogicalVolumeIntegrityDescriptor, "UdfLogicalVolumeIntegrityDescriptor"],
        [UdfFileSetDescriptor,                "UdfFileSetDescriptor"],
        [UdfFileIdentifierDescriptor,         "UdfFileIdentifierDescriptor"],
        [UdfAllocationExtentDescriptor,       "UdfAllocationExtentDescriptor"],
        [UdfFileEntry,                        "UdfFileEntry"],
        [UdfExtendedFileEntry,                "UdfExtendedFileEntry"],
    ]

class CDROM_VOLUME_DESCRIPTOR(Structure):
    _fields_ = [
        ('Type',                                   c_uint8),
        ('Id',                                     ARRAY(c_uint8, 5)),
        ('Reserved',                               ARRAY(c_uint8, 82)),
    ]

class UDF_DESCRIPTOR_TAG(Structure):
    _fields_ = [
        ('TagIdentifier',                          c_uint16),
        ('DescriptorVersion',                      c_uint16),
        ('TagChecksum',                            c_uint8),
        ('Reserved',                               c_uint8),
        ('TagSerialNumber',                        c_uint16),
        ('DescriptorCRC',                          c_uint16),
        ('DescriptorCRCLength',                    c_uint16),
        ('TagLocation',                            c_uint32),
        ]

class UDF_EXTENT_AD(Structure):
    _fields_ = [
        ('ExtentLength',                           c_uint32),
        ('ExtentLocation',                         c_uint32),
        ]

class UDF_CHAR_SPEC(Structure):
    _fields_ = [
        ('CharacterSetType',                       c_uint8),
        ('CharacterSetInfo',                       ARRAY(c_uint8, 63)),
        ]

class UDF_ENTITY_ID_DOMAIN(Structure):
    _fields_ = [
        ('Flags',                                  c_uint8),
        ('Identifier',                             ARRAY(c_uint8, 23)),
        ('UdfRevision',                            c_uint16),
        ('DomainFlags',                            c_uint8),
        ('Reserved',                               ARRAY(c_uint8, 5)),
        ]

class UDF_ENTITY_ID_ENTITY(Structure):
    _fields_ = [
        ('Flags',                                  c_uint8),
        ('Identifier',                             ARRAY(c_uint8, 23)),
        ('UdfRevision',                            c_uint16),
        ('OSClass',                                c_uint8),
        ('OSIdentifier',                           c_uint8),
        ('Reserved',                               ARRAY(c_uint8, 4)),
        ]

class UDF_ENTITY_ID_IMPLEMENTATION_ENTITY(Structure):
    _fields_ = [
        ('Flags',                                  c_uint8),
        ('Identifier',                             ARRAY(c_uint8, 23)),
        ('OSClass',                                c_uint8),
        ('OSIdentifier',                           c_uint8),
        ('ImplementationUseArea',                  ARRAY(c_uint8, 6)),
        ]

class UDF_ENTITY_ID_APPLICATION_ENTITY(Structure):
    _fields_ = [
        ('Flags',                                  c_uint8),
        ('Identifier',                             ARRAY(c_uint8, 23)),
        ('ApplicationUseArea',                     ARRAY(c_uint8, 8)),
        ]

class UDF_ENTITY_ID(Structure):
    _fields_ = [
        ('Flags',                                  c_uint8),
        ('Identifier',                             ARRAY(c_uint8, 23)),
        ('Data',                                   ARRAY(c_uint8, 8)),
        ]

class UDF_TIMESTAMP(Structure):
    _fields_ = [
        ('TypeAndTimezone',                        c_uint16),
        ('Year',                                   c_int16),
        ('Month',                                  c_uint8),
        ('Day',                                    c_uint8),
        ('Hour',                                   c_uint8),
        ('Minute',                                 c_uint8),
        ('Second',                                 c_uint8),
        ('Centiseconds',                           c_uint8),
        ('HundredsOfMicroseconds',                 c_uint8),
        ('Microseconds',                           c_uint8),
        ]

class UDF_LONG_ALLOCATION_DESCRIPTOR(Structure):
    _fields_ = [
        ('ExtentLength',                           c_uint32),
        ('ExtentLocationLogicalBlockNumber',       c_uint32),
        ('ExtentLocationPartitionReferenceNumber', c_uint16),
        ('ImplementationUse',                      ARRAY(c_uint8, 6)),
        ]

class UDF_ANCHOR_VOLUME_DESCRIPTOR_POINTER(Structure):
    _fields_ = [
        ('DescriptorTag',                          UDF_DESCRIPTOR_TAG),
        ('MainVolumeDescriptorSequenceExtent',     UDF_EXTENT_AD),
        ('ReserveVolumeDescriptorSequenceExtent',  UDF_EXTENT_AD),
        ('Reserved',                               ARRAY(c_uint8, 480)),
        ]

class UDF_LOGICAL_VOLUME_DESCRIPTOR(Structure):
    _fields_ = [
        ('DescriptorTag',                          UDF_DESCRIPTOR_TAG),
        ('VolumeDescriptorSequenceNumber',         c_uint32),
        ('DescriptorCharacterSet',                 UDF_CHAR_SPEC),
        ('LogicalVolumeIdentifier',                ARRAY(c_uint8, 128)),
        ('LogicalBlockSize',                       c_uint32),
        ('DomainIdentifier',                       UDF_ENTITY_ID_DOMAIN),
        ('LogicalVolumeContentsUse',               UDF_LONG_ALLOCATION_DESCRIPTOR),
        ('MapTableLength',                         c_uint32),
        ('NumberOfPartitionMaps',                  c_uint32),
        ('ImplementationIdentifier',               UDF_ENTITY_ID_IMPLEMENTATION_ENTITY),
        ('ImplementationUse',                      ARRAY(c_uint8, 128)),
        ('IntegritySequenceExtent',                UDF_EXTENT_AD),
        ('PartitionMaps',                          ARRAY(c_uint8, 6)),
        ]

class UDF_PARTITION_DESCRIPTOR(Structure):
    _fields_ = [
        ('DescriptorTag',                          UDF_DESCRIPTOR_TAG),
        ('VolumeDescriptorSequenceNumber',         c_uint32),
        ('PartitionFlags',                         c_uint16),
        ('PartitionNumber',                        c_uint16),
        ('PartitionContents',                      UDF_ENTITY_ID),
        ('PartitionContentsUse',                   ARRAY(c_uint8, 128)),
        ('AccessType',                             c_uint32),
        ('PartitionStartingLocation',              c_uint32),
        ('PartitionLength',                        c_uint32),
        ('ImplementationIdentifier',               UDF_ENTITY_ID_IMPLEMENTATION_ENTITY),
        ('ImplementationUse',                      ARRAY(c_uint8, 128)),
        ('Reserved',                               ARRAY(c_uint8, 128)),
        ]

class UDF_FILE_SET_DESCRIPTOR(Structure):
    _fields_ = [
        ('DescriptorTag',                          UDF_DESCRIPTOR_TAG),
        ('RecordingDateAndTime',                   UDF_TIMESTAMP),
        ('InterchangeLevel',                       c_uint16),
        ('MaximumInterchangeLevel',                c_uint16),
        ('CharacterSetList',                       c_uint32),
        ('MaximumCharacterSetList',                c_uint32),
        ('FileSetNumber',                          c_uint32),
        ('FileSetDescriptorNumber',                c_uint32),
        ('LogicalVolumeIdentifierCharacterSet',    UDF_CHAR_SPEC),
        ('LogicalVolumeIdentifier',                ARRAY(c_uint8, 128)),
        ('FileSetCharacterSet',                    UDF_CHAR_SPEC),
        ('FileSetIdentifier',                      ARRAY(c_uint8, 32)),
        ('CopyrightFileIdentifier',                ARRAY(c_uint8, 32)),
        ('AbstractFileIdentifier',                 ARRAY(c_uint8, 32)),
        ('RootDirectoryIcb',                       UDF_LONG_ALLOCATION_DESCRIPTOR),
        ('DomainIdentifier',                       UDF_ENTITY_ID_DOMAIN),
        ('NextExtent',                             UDF_LONG_ALLOCATION_DESCRIPTOR),
        ('SystemStreamDirectoryIcb',               UDF_LONG_ALLOCATION_DESCRIPTOR),
        ('Reserved',                               ARRAY(c_uint8, 32)),
        ]

class UDF_ICB_TAG(Structure):
    _fields_ = [
        ('PriorRecordNumberOfDirectEntries',       c_uint32),
        ('StrategyType',                           c_uint16),
        ('StrategyParameter',                      c_uint16),
        ('MaximumNumberOfEntries',                 c_uint16),
        ('Reserved',                               c_uint8),
        ('FileType',                               c_uint8),
        ('ParentIcbLocationLogicalBlockNumber',    c_uint32),
        ('ParentIcbLocationPartitionReferenceNumber', c_uint16),
        ('Flags',                                  c_uint16),
        ]

class UDF_EXTENDED_FILE_ENTRY(Structure):
    _fields_ = [
        ('DescriptorTag',                          UDF_DESCRIPTOR_TAG),
        ('IcbTag',                                 UDF_ICB_TAG),
        ('Uid',                                    c_uint32),
        ('Gid',                                    c_uint32),
        ('Permissions',                            c_uint32),
        ('FileLinkCount',                          c_uint16),
        ('RecordFormat',                           c_uint8),
        ('RecordDisplayAttributes',                c_uint8),
        ('RecordLength',                           c_uint32),
        ('InformationLength',                      c_uint64),
        ('ObjectSize',                             c_uint64),
        ('LogicalBlocksRecorded',                  c_uint64),
        ('AccessTime',                             UDF_TIMESTAMP),
        ('ModificationTime',                       UDF_TIMESTAMP),
        ('CreationTime',                           UDF_TIMESTAMP),
        ('AttributeTime',                          UDF_TIMESTAMP),
        ('CheckPoint',                             c_uint32),
        ('Reserved',                               c_uint32),
        ('ExtendedAttributeIcb',                   UDF_LONG_ALLOCATION_DESCRIPTOR),
        ('StreamDirectoryIcb',                     UDF_LONG_ALLOCATION_DESCRIPTOR),
        ('ImplementationIdentifier',               UDF_ENTITY_ID_IMPLEMENTATION_ENTITY),
        ('UniqueId',                               c_uint64),
        ('LengthOfExtendedAttributes',             c_uint32),
        ('LengthOfAllocationDescriptors',          c_uint32),
        ]

class UDF_FILE_ENTRY(Structure):
    _fields_ = [
        ('DescriptorTag', UDF_DESCRIPTOR_TAG),
        ('IcbTag', UDF_ICB_TAG),
        ('Uid', c_uint32),
        ('Gid', c_uint32),
        ('Permissions', c_uint32),
        ('FileLinkCount', c_uint16),
        ('RecordFormat', c_uint8),
        ('RecordDisplayAttributes', c_uint8),
        ('RecordLength', c_uint32),
        ('InformationLength', c_uint64),
        ('LogicalBlocksRecorded', c_uint64),
        ('AccessTime', UDF_TIMESTAMP),
        ('ModificationTime', UDF_TIMESTAMP),
        ('AttributeTime', UDF_TIMESTAMP),
        ('CheckPoint', c_uint32),
        ('ExtendedAttributeIcb', UDF_LONG_ALLOCATION_DESCRIPTOR),
        ('ImplementationIdentifier', UDF_ENTITY_ID_IMPLEMENTATION_ENTITY),
        ('UniqueId', c_uint64),
        ('LengthOfExtendedAttributes', c_uint32),
        ('LengthOfAllocationDescriptors', c_uint32),
    ]

class UDF_FILE_IDENTIFIER_DESCRIPTOR(Structure):
    _fields_ = [
        ('DescriptorTag',                          UDF_DESCRIPTOR_TAG),
        ('FileVersionNumber',                      c_uint16),
        ('FileCharacteristics',                    c_uint8),
        ('LengthOfFileIdentifier',                 c_uint8),
        ('Icb',                                    UDF_LONG_ALLOCATION_DESCRIPTOR),
        ('LengthOfImplementationUse',              c_uint16),
        ('Pad',                                    c_uint16),
        ]

class UDF_PRIMARY_VOLUME_DESCRIPTOR(Structure):
    _fields_ = [
        ('DescriptorTag',                          UDF_DESCRIPTOR_TAG),
        ('VolumeDescriptorSequenceNumber',         c_uint32),
        ('PrimaryVolumeDescriptorNumber',          c_uint32),
        ('VolumeIdentifier',                       ARRAY(c_uint8, 32)),
        ('VolumeSequenceNumber',                   c_uint16),
        ('MaximumVolumeSequenceNumber',            c_uint16),
        ('InterchangeLevel',                       c_uint16),
        ('MaximumInterchangeLevel',                c_uint16),
        ('CharacterSetList',                       c_uint32),
        ('MaximumCharacterSetList',                c_uint32),
        ('VolumeSetIdentifier',                    ARRAY(c_uint8, 128)),
        ('DescriptorCharacterSet',                 UDF_CHAR_SPEC),
        ('ExplanatoryCharacterSet',                UDF_CHAR_SPEC),
        ('VolumeAbstract',                         UDF_EXTENT_AD),
        ('VolumeCopyrightNotice',                  UDF_EXTENT_AD),
        ('ApplicationIdentifier',                  UDF_ENTITY_ID_APPLICATION_ENTITY),
        ('RecordingDateAndTime',                   UDF_TIMESTAMP),
        ('ImplementationIdentifier',               UDF_ENTITY_ID_IMPLEMENTATION_ENTITY),
        ('ImplementationUse',                      ARRAY(c_uint8, 64)),
        ('PredecessorVolumeDescriptorSequenceLocation', c_uint32),
        ('Flags',                                  c_uint16),
        ('Reserved',                               ARRAY(c_uint8, 22)),
        ]
