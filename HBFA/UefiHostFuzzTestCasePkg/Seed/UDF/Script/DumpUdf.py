## @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

'''
DumpUdf
'''

import os
import sys
import argparse
import subprocess
import uuid
import struct
import collections
import binascii
from   ctypes import *
from   Udf import *

#
# Globals for help information
#
__prog__      = 'DumpUdf'
__version__   = '%s Version %s' % (__prog__, '0.1 ')
__copyright__ = 'Copyright (c) 2018, Intel Corporation. All rights reserved.'
__usage__     = '%s [options] <input_file>' % (__prog__)

class UDF_FILE_NAME(Structure):
    _fields_ = [
        ('Name',                                   ARRAY(c_uint8, 1024)),
        ]

def DumpHexData(Data):
    CharPerLine = 8
    DataLen = len(Data)
    Count = DataLen / CharPerLine
    Rem   = DataLen % CharPerLine
    for CountIndex in range (0, Count) :
        TempData = Data[CountIndex * CharPerLine:CharPerLine]
        print("    %04x: "%(CountIndex*CharPerLine) + binascii.hexlify(Data[CountIndex * CharPerLine:CountIndex * CharPerLine + CharPerLine]))
    if Rem != 0 :
        print("    %04x: "%(Count*CharPerLine) + binascii.hexlify(Data[Count * CharPerLine:Count * CharPerLine + Rem]))

def GetHexString (Data):
    AsciiName = []
    for Int in Data :
        if Int == 0:
            continue
        if Int < 0x20:
            Int = 0x2E # 0x2E='.'
        AsciiName.append(chr(Int))
    AsciiName = ''.join(AsciiName)
    return AsciiName

def DumpFileName (Data, Length):
    AsciiName = []
    Index = 0;
    for Int in Data :
        if (Index == 0) :
            Index = Index + 1
            continue
        if (Index >= Length) :
            break
        AsciiName.append(chr(Int))
        Index = Index + 1
    AsciiName = ''.join(AsciiName)
    print("  FileIdentifier                 : " + AsciiName)
    return 

def DescriptorTagToString(Data):
    for Tag in UDF_VOLUME_DESCRIPTOR_ID.TagStringTable:
        if (Tag[0] == Data):
            return Tag[1]
    return "<Unknown>"  

def IcbTagFileTypeToString(Data):
    if Data == 4 :
        return "Directory"
    elif Data == 5 :
        return "File"
    elif Data == 12 :
        return "SymLink"
    else :
        return "<Unknown>"

def IcbTagFlagsToString(Data):
    if Data == 0 :
        return "ShortAdsSequence"
    elif Data == 1 :
        return "LongAdsSequence"
    elif Data == 2 :
        return "ExtendedAdsSequence"
    elif Data == 3 :
        return "InlineData"
    else :
        return "<Unknown>"

def DumpDescriptorTag(Data):
    print("    TagIdentifier                : 0x%04x (%s)" % (Data.TagIdentifier, DescriptorTagToString(Data.TagIdentifier)))
    print("    DescriptorVersion            : 0x%04x" % Data.DescriptorVersion)
    print("    TagChecksum                  : 0x%02x" % Data.TagChecksum)
    print("    TagSerialNumber              : 0x%04x" % Data.TagSerialNumber)
    print("    DescriptorCRC                : 0x%04x" % Data.DescriptorCRC)
    print("    DescriptorCRCLength          : 0x%04x" % Data.DescriptorCRCLength)
    print("    TagLocation                  : 0x%08x" % Data.TagLocation)

def DumpExtendAd(Data):
    print("    ExtentLength                 : 0x%08x" % Data.ExtentLength)
    print("    ExtentLocation               : 0x%08x" % Data.ExtentLocation)

def DumpDescriptorCharacterSet(Data):
    print("    CharacterSetType             : 0x%02x" % Data.CharacterSetType)
    print("    CharacterSetInfo             : " + binascii.hexlify(Data.CharacterSetInfo) + " (%s)" % GetHexString(Data.CharacterSetInfo))

def DumpEntityIdDomain(Data):
    print("    Flags                        : 0x%02x" % Data.Flags)
    print("    Identifier                   : " + binascii.hexlify(Data.Identifier) + " (%s)" % GetHexString(Data.Identifier))
    print("    UdfRevision                  : 0x%04x" % Data.UdfRevision)
    print("    DomainFlags                  : 0x%02x" % Data.DomainFlags)

def DumpEntityIdEntity(Data):
    print("    Flags                        : 0x%02x" % Data.Flags)
    print("    Identifier                   : " + binascii.hexlify(Data.Identifier) + " (%s)" % GetHexString(Data.Identifier))
    print("    UdfRevision                  : 0x%04x" % Data.UdfRevision)
    print("    OSClass                      : 0x%02x" % Data.OSClass)
    print("    OSIdentifier                 : 0x%02x" % Data.OSIdentifier)

def DumpEntityIdImplementationUseArea(Data):
    print("    Flags                        : 0x%02x" % Data.Flags)
    print("    Identifier                   : " + binascii.hexlify(Data.Identifier) + " (%s)" % GetHexString(Data.Identifier))
    print("    OSClass                      : 0x%02x" % Data.OSClass)
    print("    OSIdentifier                 : 0x%02x" % Data.OSIdentifier)
    print("    ImplementationUseArea        : " + binascii.hexlify(Data.ImplementationUseArea) + " (%s)" % GetHexString(Data.ImplementationUseArea))

def DumpEntityIdApplicationEntity(Data):
    print("    Flags                        : 0x%02x" % Data.Flags)
    print("    Identifier                   : " + binascii.hexlify(Data.Identifier) + " (%s)" % GetHexString(Data.Identifier))
    print("    ApplicationUseArea           : " + binascii.hexlify(Data.ApplicationUseArea) + " (%s)" % GetHexString(Data.ApplicationUseArea))

def DumpEntityId(Data):
    print("    Flags                        : 0x%02x" % Data.Flags)
    print("    Identifier                   : " + binascii.hexlify(Data.Identifier) + " (%s)" % GetHexString(Data.Identifier))
    print("    Data                         : " + binascii.hexlify(Data.Data) + " (%s)" % GetHexString(Data.Data))

def DumpLongAllocationDesc(Data):
    print("    ExtentLength                 : 0x%08x" % Data.ExtentLength)
    print("    ExtLocLogicalBlockNumber     : 0x%08x" % Data.ExtentLocationLogicalBlockNumber)
    print("    ExtLocPartitionReferenceNumb : 0x%04x" % Data.ExtentLocationPartitionReferenceNumber)
    print("    ImplementationUse            : " + binascii.hexlify(Data.ImplementationUse) + " (%s)" % GetHexString(Data.ImplementationUse))

def DumpIcbTag(Data):
    print("    PriorRecordNumberOfDirectEnt : 0x%08x" % Data.PriorRecordNumberOfDirectEntries)
    print("    StrategyType                 : 0x%04x" % Data.StrategyType)
    print("    StrategyParameter            : 0x%04x" % Data.StrategyParameter)
    print("    MaximumNumberOfEntries       : 0x%04x" % Data.MaximumNumberOfEntries)
    print("    FileType                     : 0x%02x" % Data.FileType)
    print("    ParentIcbLocationLogicalBlock: 0x%08x" % Data.ParentIcbLocationLogicalBlockNumber)
    print("    ParentIcbLocationPartitionRef: 0x%04x" % Data.ParentIcbLocationPartitionReferenceNumber)
    print("    Flags                        : 0x%04x" % Data.Flags)

def DumpTimestamp(Data):
    print("    TypeAndTimezone              : 0x%04x (%d)" % (Data.TypeAndTimezone, Data.TypeAndTimezone))
    print("    Year                         : 0x%04x (%d)" % (Data.Year, Data.Year))
    print("    Month                        : 0x%02x (%d)" % (Data.Month, Data.Month))
    print("    Day                          : 0x%02x (%d)" % (Data.Day, Data.Day))
    print("    Hour                         : 0x%02x (%d)" % (Data.Hour, Data.Hour))
    print("    Minute                       : 0x%02x (%d)" % (Data.Minute, Data.Minute))
    print("    Second                       : 0x%02x (%d)" % (Data.Second, Data.Second))
    print("    Centiseconds                 : 0x%02x (%d)" % (Data.Centiseconds, Data.Centiseconds))
    print("    HundredsOfMicroseconds       : 0x%02x (%d)" % (Data.HundredsOfMicroseconds, Data.HundredsOfMicroseconds))
    print("    Microseconds                 : 0x%02x (%d)" % (Data.Microseconds, Data.Microseconds))

def DumpAnchorVolumeDesc(Data):
    print("AnchorVolumeDesc:")
    print("  DescriptorTag:")
    DumpDescriptorTag (Data.DescriptorTag)
    print("  MainVolumeDescriptorSequenceExtent:")
    DumpExtendAd (Data.MainVolumeDescriptorSequenceExtent)
    print("  ReserveVolumeDescriptorSequenceExtent:")
    DumpExtendAd (Data.ReserveVolumeDescriptorSequenceExtent)

def DumpLogicalVolumeDesc(Data):
    print("LogicalVolumeDesc:")
    print("  DescriptorTag:")
    DumpDescriptorTag (Data.DescriptorTag)
    print("  VolumeDescriptorSequenceNumber : 0x%08x" % Data.VolumeDescriptorSequenceNumber)
    print("  DescriptorCharacterSet:")
    DumpDescriptorCharacterSet (Data.DescriptorCharacterSet)
    print("  LogicalVolumeIdentifier        : " + binascii.hexlify(Data.LogicalVolumeIdentifier) + " (%s)" % GetHexString(Data.LogicalVolumeIdentifier))
    print("  LogicalBlockSize               : 0x%08x" % Data.LogicalBlockSize)
    print("  DomainIdentifier:")
    DumpEntityIdDomain (Data.DomainIdentifier)
    print("  LogicalVolumeContentsUse:")
    DumpLongAllocationDesc (Data.LogicalVolumeContentsUse)
    print("  MapTableLength                 : 0x%08x" % Data.MapTableLength)
    print("  NumberOfPartitionMaps          : 0x%08x" % Data.NumberOfPartitionMaps)
    print("  ImplementationIdentifier:")
    DumpEntityIdImplementationUseArea (Data.ImplementationIdentifier)
    print("  ImplementationUse              : " + binascii.hexlify(Data.ImplementationUse) + " (%s)" % GetHexString(Data.ImplementationUse))
    print("  IntegritySequenceExtent:")
    DumpExtendAd (Data.IntegritySequenceExtent)
    print("  PartitionMaps                  : " + binascii.hexlify(Data.PartitionMaps))

def DumpPartitionDesc(Data):
    print("PartitionDesc:")
    print("  DescriptorTag:")
    DumpDescriptorTag (Data.DescriptorTag)
    print("  VolumeDescriptorSequenceNumber : 0x%08x" % Data.VolumeDescriptorSequenceNumber)
    print("  PartitionFlags                 : 0x%04x" % Data.PartitionFlags)
    print("  PartitionNumber                : 0x%04x" % Data.PartitionNumber)
    print("  PartitionContents:")
    DumpEntityId (Data.PartitionContents)
    print("  PartitionContentsUse           : " + binascii.hexlify(Data.PartitionContentsUse) + " (%s)" % GetHexString(Data.PartitionContentsUse))
    print("  AccessType                     : 0x%08x" % Data.AccessType)
    print("  PartitionStartingLocation      : 0x%08x" % Data.PartitionStartingLocation)
    print("  PartitionLength                : 0x%08x" % Data.PartitionLength)
    print("  ImplementationIdentifier:")
    DumpEntityIdImplementationUseArea (Data.ImplementationIdentifier)
    print("  ImplementationUse              : " + binascii.hexlify(Data.ImplementationUse) + " (%s)" % GetHexString(Data.ImplementationUse))

def DumpFileSetDesc(Data):
    print("FileSetDesc:")
    print("  DescriptorTag:")
    DumpDescriptorTag (Data.DescriptorTag)
    print("  RecordingDateAndTime:")
    DumpTimestamp(Data.RecordingDateAndTime)
    print("  InterchangeLevel               : 0x%04x" % Data.InterchangeLevel)
    print("  MaximumInterchangeLevel        : 0x%04x" % Data.MaximumInterchangeLevel)
    print("  CharacterSetList               : 0x%08x" % Data.CharacterSetList)
    print("  MaximumCharacterSetList        : 0x%08x" % Data.MaximumCharacterSetList)
    print("  FileSetNumber                  : 0x%08x" % Data.FileSetNumber)
    print("  FileSetDescriptorNumber        : 0x%08x" % Data.FileSetDescriptorNumber)
    print("  LogicalVolumeIdentifierCharacterSet:")
    DumpDescriptorCharacterSet(Data.LogicalVolumeIdentifierCharacterSet)
    print("  LogicalVolumeIdentifier        : " + binascii.hexlify(Data.LogicalVolumeIdentifier) + " (%s)" % GetHexString(Data.LogicalVolumeIdentifier))
    print("  FileSetCharacterSet:")
    DumpDescriptorCharacterSet(Data.FileSetCharacterSet)
    print("  FileSetIdentifier              : " + binascii.hexlify(Data.FileSetIdentifier) + " (%s)" % GetHexString(Data.FileSetIdentifier))
    print("  CopyrightFileIdentifier        : " + binascii.hexlify(Data.CopyrightFileIdentifier) + " (%s)" % GetHexString(Data.CopyrightFileIdentifier))
    print("  AbstractFileIdentifier         : " + binascii.hexlify(Data.AbstractFileIdentifier) + " (%s)" % GetHexString(Data.AbstractFileIdentifier))
    print("  RootDirectoryIcb:")
    DumpLongAllocationDesc(Data.RootDirectoryIcb)
    print("  DomainIdentifier:")
    DumpEntityIdDomain(Data.DomainIdentifier)
    print("  NextExtent:")
    DumpLongAllocationDesc(Data.NextExtent)
    print("  SystemStreamDirectoryIcb:")
    DumpLongAllocationDesc(Data.SystemStreamDirectoryIcb)

def DumpExtFileEntry(Data):
    print("ExtFileEntry:")
    print("  DescriptorTag:")
    DumpDescriptorTag (Data.DescriptorTag)
    print("  IcbTag:")
    DumpIcbTag (Data.IcbTag)
    print("  Uid                            : 0x%08x" % Data.Uid)
    print("  Gid                            : 0x%08x" % Data.Gid)
    print("  Permissions                    : 0x%08x" % Data.Permissions)
    print("  FileLinkCount                  : 0x%04x" % Data.FileLinkCount)
    print("  RecordFormat                   : 0x%02x" % Data.RecordFormat)
    print("  RecordDisplayAttributes        : 0x%02x" % Data.RecordDisplayAttributes)
    print("  RecordLength                   : 0x%08x" % Data.RecordLength)
    print("  InformationLength              : 0x%016lx" % Data.InformationLength)
    print("  ObjectSize                     : 0x%016lx" % Data.ObjectSize)
    print("  LogicalBlocksRecorded          : 0x%016lx" % Data.LogicalBlocksRecorded)
    print("  AccessTime:")
    DumpTimestamp (Data.AccessTime)
    print("  ModificationTime:")
    DumpTimestamp (Data.ModificationTime)
    print("  CreationTime:")
    DumpTimestamp (Data.CreationTime)
    print("  AttributeTime:")
    DumpTimestamp (Data.AttributeTime)
    print("  CheckPoint                     : 0x%08x" % Data.CheckPoint)
    print("  ExtendedAttributeIcb:")
    DumpLongAllocationDesc (Data.ExtendedAttributeIcb)
    print("  StreamDirectoryIcb:")
    DumpLongAllocationDesc (Data.StreamDirectoryIcb)
    print("  ImplementationIdentifier:")
    DumpEntityIdImplementationUseArea (Data.ImplementationIdentifier)
    print("  UniqueId                       : 0x%016lx" % Data.UniqueId)
    print("  LengthOfExtendedAttributes     : 0x%08x" % Data.LengthOfExtendedAttributes)
    print("  LengthOfAllocationDescriptors  : 0x%08x" % Data.LengthOfAllocationDescriptors)

def DumpFileEntry(Data):
    print("FileEntry:")
    print("  DescriptorTag:")
    DumpDescriptorTag (Data.DescriptorTag)
    print("  IcbTag:")
    DumpIcbTag (Data.IcbTag)
    print("  Uid                            : 0x%08x" % Data.Uid)
    print("  Gid                            : 0x%08x" % Data.Gid)
    print("  Permissions                    : 0x%08x" % Data.Permissions)
    print("  FileLinkCount                  : 0x%04x" % Data.FileLinkCount)
    print("  RecordFormat                   : 0x%02x" % Data.RecordFormat)
    print("  RecordDisplayAttributes        : 0x%02x" % Data.RecordDisplayAttributes)
    print("  RecordLength                   : 0x%08x" % Data.RecordLength)
    print("  InformationLength              : 0x%016lx" % Data.InformationLength)
    print("  LogicalBlocksRecorded          : 0x%016lx" % Data.LogicalBlocksRecorded)
    print("  AccessTime:")
    DumpTimestamp (Data.AccessTime)
    print("  ModificationTime:")
    DumpTimestamp (Data.ModificationTime)
    print("  AttributeTime:")
    DumpTimestamp (Data.AttributeTime)
    print("  CheckPoint                     : 0x%08x" % Data.CheckPoint)
    print("  ExtendedAttributeIcb:")
    DumpLongAllocationDesc (Data.ExtendedAttributeIcb)
    print("  ImplementationIdentifier:")
    DumpEntityIdImplementationUseArea (Data.ImplementationIdentifier)
    print("  UniqueId                       : 0x%016lx" % Data.UniqueId)
    print("  LengthOfExtendedAttributes     : 0x%08x" % Data.LengthOfExtendedAttributes)
    print("  LengthOfAllocationDescriptors  : 0x%08x" % Data.LengthOfAllocationDescriptors)

def DumpFileIdentifierDesc(Data):
    print("FileIdentifierDesc:")
    print("  DescriptorTag:")
    DumpDescriptorTag (Data.DescriptorTag)
    print("  FileVersionNumber              : 0x%04x" % Data.FileVersionNumber)
    print("  FileCharacteristics            : 0x%02x" % Data.FileCharacteristics)
    print("  LengthOfFileIdentifier         : 0x%02x" % Data.LengthOfFileIdentifier)
    print("  Icb:")
    DumpLongAllocationDesc (Data.Icb)
    print("  LengthOfImplementationUse      : 0x%04x" % Data.LengthOfImplementationUse)

def DumpPrimaryVolumeDesc(Data):
    print("PrimaryVolumeDesc:")
    print("  DescriptorTag:")
    DumpDescriptorTag (Data.DescriptorTag);
    print("  VolumeDescriptorSequenceNumber : 0x%08x" % Data.VolumeDescriptorSequenceNumber)
    print("  PrimaryVolumeDescriptorNumber  : 0x%08x" % Data.PrimaryVolumeDescriptorNumber)
    print("  VolumeIdentifier               : " + binascii.hexlify(Data.VolumeIdentifier) + " (%s)" % GetHexString(Data.VolumeIdentifier))
    print("  VolumeSequenceNumber           : 0x%04x" % Data.VolumeSequenceNumber)
    print("  MaximumVolumeSequenceNumber    : 0x%04x" % Data.MaximumVolumeSequenceNumber)
    print("  InterchangeLevel               : 0x%04x" % Data.InterchangeLevel)
    print("  MaximumInterchangeLevel        : 0x%04x" % Data.MaximumInterchangeLevel)
    print("  CharacterSetList               : 0x%08x" % Data.CharacterSetList)
    print("  MaximumCharacterSetList        : 0x%08x" % Data.MaximumCharacterSetList)
    print("  VolumeSetIdentifier            : " + binascii.hexlify(Data.VolumeSetIdentifier) + " (%s)" % GetHexString(Data.VolumeSetIdentifier))
    print("  DescriptorCharacterSet:")
    DumpDescriptorCharacterSet (Data.DescriptorCharacterSet)
    print("  ExplanatoryCharacterSet:")
    DumpDescriptorCharacterSet (Data.ExplanatoryCharacterSet)
    print("  VolumeAbstract:")
    DumpExtendAd (Data.VolumeAbstract)
    print("  VolumeCopyrightNotice:")
    DumpExtendAd (Data.VolumeCopyrightNotice)
    print("  ApplicationIdentifier:")
    DumpEntityIdApplicationEntity (Data.ApplicationIdentifier)
    print("  RecordingDateAndTime:")
    DumpTimestamp (Data.RecordingDateAndTime)
    print("  ImplementationIdentifier:")
    DumpEntityIdImplementationUseArea (Data.ImplementationIdentifier)
    print("  ImplementationUse              : " + binascii.hexlify(Data.ImplementationUse) + " (%s)" % GetHexString(Data.ImplementationUse))
    print("  PredecessorVolumeDescriptorSeq : 0x%08x" % Data.PredecessorVolumeDescriptorSequenceLocation)
    print("  Flags                          : 0x%04x" % Data.Flags)

def IsUdfTag (Tag):
    if Tag == UDF_VOLUME_DESCRIPTOR_ID.UdfPrimaryVolumeDescriptor or \
         Tag == UDF_VOLUME_DESCRIPTOR_ID.UdfAnchorVolumeDescriptorPointer or \
         Tag == UDF_VOLUME_DESCRIPTOR_ID.UdfVolumeDescriptorPointer or \
         Tag == UDF_VOLUME_DESCRIPTOR_ID.UdfImplemenationUseVolumeDescriptor or \
         Tag == UDF_VOLUME_DESCRIPTOR_ID.UdfPartitionDescriptor or \
         Tag == UDF_VOLUME_DESCRIPTOR_ID.UdfLogicalVolumeDescriptor or \
         Tag == UDF_VOLUME_DESCRIPTOR_ID.UdfUnallocatedSpaceDescriptor or \
         Tag == UDF_VOLUME_DESCRIPTOR_ID.UdfTerminatingDescriptor or \
         Tag == UDF_VOLUME_DESCRIPTOR_ID.UdfLogicalVolumeIntegrityDescriptor or \
         Tag == UDF_VOLUME_DESCRIPTOR_ID.UdfFileSetDescriptor or \
         Tag == UDF_VOLUME_DESCRIPTOR_ID.UdfFileIdentifierDescriptor or \
         Tag == UDF_VOLUME_DESCRIPTOR_ID.UdfAllocationExtentDescriptor or \
         Tag == UDF_VOLUME_DESCRIPTOR_ID.UdfFileEntry or \
         Tag == UDF_VOLUME_DESCRIPTOR_ID.UdfExtendedFileEntry :
            return True
    return False

def ScanUdf (Data):
    for index in range (0, 0x100):
        if (2048 * index > args.InputFileSize):
            break
        VolDesc = CDROM_VOLUME_DESCRIPTOR.from_buffer (bytearray(args.InputFileBuffer), 2048 * index)
        if (VolDesc.Id[0] != 0 and VolDesc.Id[1] != 0 and VolDesc.Id[2] != 0 and VolDesc.Id[3] != 0 and VolDesc.Id[4] != 0) :
        print("  VolDesc(0x%x, %d) : %c%c%c%c%c" % (2048 * index, 2048 * index, VolDesc.Id[0], VolDesc.Id[1], VolDesc.Id[2], VolDesc.Id[3], VolDesc.Id[4]))
    print("\n\n")

    Count = 1024 * 1024 / args.BlockSize
    print('UDF binary:')
    for index in range(0, Count):
    if (args.BlockSize * index >= args.InputFileSize):
        break

    Tag = UDF_DESCRIPTOR_TAG.from_buffer (bytearray(args.InputFileBuffer), args.BlockSize * index)
    if IsUdfTag(Tag.TagIdentifier) :
        print("(LBA: 0x%x, %d) (BlockSize * LBA: 0x%x, %d):" % (index, index, args.BlockSize * index, args.BlockSize * index))
        if Tag.TagIdentifier == UDF_VOLUME_DESCRIPTOR_ID.UdfAnchorVolumeDescriptorPointer:
            AnchorVolumeDesc = UDF_ANCHOR_VOLUME_DESCRIPTOR_POINTER.from_buffer(bytearray(args.InputFileBuffer),
                                                                                args.BlockSize * index)
            DumpAnchorVolumeDesc(AnchorVolumeDesc)
        elif Tag.TagIdentifier == UDF_VOLUME_DESCRIPTOR_ID.UdfLogicalVolumeDescriptor:
            LogicalVolumeDesc = UDF_LOGICAL_VOLUME_DESCRIPTOR.from_buffer(bytearray(args.InputFileBuffer),
                                                                          args.BlockSize * index)
            DumpLogicalVolumeDesc(LogicalVolumeDesc)
        elif Tag.TagIdentifier == UDF_VOLUME_DESCRIPTOR_ID.UdfPartitionDescriptor:
            PartitionDesc = UDF_PARTITION_DESCRIPTOR.from_buffer(bytearray(args.InputFileBuffer), args.BlockSize * index)
            DumpPartitionDesc(PartitionDesc)
        elif Tag.TagIdentifier == UDF_VOLUME_DESCRIPTOR_ID.UdfPrimaryVolumeDescriptor:
            PrimaryVolumeDesc = UDF_PRIMARY_VOLUME_DESCRIPTOR.from_buffer(bytearray(args.InputFileBuffer),
                                                                          args.BlockSize * index)
            DumpPrimaryVolumeDesc(PrimaryVolumeDesc)
        elif Tag.TagIdentifier == UDF_VOLUME_DESCRIPTOR_ID.UdfFileSetDescriptor:
            FileSetDesc = UDF_FILE_SET_DESCRIPTOR.from_buffer(bytearray(args.InputFileBuffer), args.BlockSize * index)
            DumpFileSetDesc(FileSetDesc)
        elif Tag.TagIdentifier == UDF_VOLUME_DESCRIPTOR_ID.UdfExtendedFileEntry:
          ExtFileEntry = UDF_EXTENDED_FILE_ENTRY.from_buffer(bytearray(args.InputFileBuffer), args.BlockSize * index)
          DumpExtFileEntry(ExtFileEntry)
          TotalSize = ExtFileEntry.LengthOfAllocationDescriptors
          Offset = sizeof(UDF_EXTENDED_FILE_ENTRY) + ExtFileEntry.LengthOfExtendedAttributes
          Size = 0
          while (Size < TotalSize):
              FileIdentifierDesc = UDF_FILE_IDENTIFIER_DESCRIPTOR.from_buffer(bytearray(args.InputFileBuffer),
                                                                              args.BlockSize * index + Offset + Size)
              if (FileIdentifierDesc.DescriptorTag.TagIdentifier != UDF_VOLUME_DESCRIPTOR_ID.UdfFileIdentifierDescriptor):
                  break
              DumpFileIdentifierDesc(FileIdentifierDesc)
              FileName = UDF_FILE_NAME.from_buffer(bytearray(args.InputFileBuffer),
                                                   args.BlockSize * index + Offset + Size + sizeof(
                                                     UDF_FILE_IDENTIFIER_DESCRIPTOR) - sizeof(
                                                     c_uint16) + FileIdentifierDesc.LengthOfImplementationUse)
              DumpFileName(FileName.Name, FileIdentifierDesc.LengthOfFileIdentifier)
              Size += sizeof(UDF_FILE_IDENTIFIER_DESCRIPTOR) - sizeof(
                c_uint16) + FileIdentifierDesc.LengthOfImplementationUse + FileIdentifierDesc.LengthOfFileIdentifier
              Size = (Size + 3) & ~0x3
        elif Tag.TagIdentifier == UDF_VOLUME_DESCRIPTOR_ID.UdfFileEntry:
            FileEntry = UDF_FILE_ENTRY.from_buffer(bytearray(args.InputFileBuffer), args.BlockSize * index)
            DumpFileEntry(FileEntry)
            TotalSize = FileEntry.LengthOfAllocationDescriptors
            Offset = sizeof(UDF_EXTENDED_FILE_ENTRY) + FileEntry.LengthOfExtendedAttributes
            Size = 0
            while (Size < TotalSize):
                FileIdentifierDesc = UDF_FILE_IDENTIFIER_DESCRIPTOR.from_buffer(bytearray(args.InputFileBuffer),
                                                                                args.BlockSize * index + Offset + Size)
                if (FileIdentifierDesc.DescriptorTag.TagIdentifier != UDF_VOLUME_DESCRIPTOR_ID.UdfFileIdentifierDescriptor):
                    break
                DumpFileIdentifierDesc(FileIdentifierDesc)
                FileName = UDF_FILE_NAME.from_buffer(bytearray(args.InputFileBuffer),
                                                     args.BlockSize * index + Offset + Size + sizeof(
                                                       UDF_FILE_IDENTIFIER_DESCRIPTOR) - sizeof(
                                                       c_uint16) + FileIdentifierDesc.LengthOfImplementationUse)
                DumpFileName(FileName.Name, FileIdentifierDesc.LengthOfFileIdentifier)
                Size += sizeof(UDF_FILE_IDENTIFIER_DESCRIPTOR) - sizeof(
                  c_uint16) + FileIdentifierDesc.LengthOfImplementationUse + FileIdentifierDesc.LengthOfFileIdentifier
                Size = (Size + 3) & ~0x3

        else:
            print("  DescriptorTag:")
            DumpDescriptorTag(Tag)
        print("\n")

def DetectUdfBlockSize(Data):
    Tag = UDF_DESCRIPTOR_TAG.from_buffer(bytearray(args.InputFileBuffer), 512 * 0x100)
    if (Tag.TagIdentifier == UDF_VOLUME_DESCRIPTOR_ID.UdfAnchorVolumeDescriptorPointer):
        return 512
    Tag = UDF_DESCRIPTOR_TAG.from_buffer (bytearray(args.InputFileBuffer), 1024 * 0x100)
    if (Tag.TagIdentifier == UDF_VOLUME_DESCRIPTOR_ID.UdfAnchorVolumeDescriptorPointer) :
        return 1024
    Tag = UDF_DESCRIPTOR_TAG.from_buffer (bytearray(args.InputFileBuffer), 2048 * 0x100)
    if (Tag.TagIdentifier == UDF_VOLUME_DESCRIPTOR_ID.UdfAnchorVolumeDescriptorPointer) :
        return 2048
    Tag = UDF_DESCRIPTOR_TAG.from_buffer (bytearray(args.InputFileBuffer), 4096 * 0x100)
    if (Tag.TagIdentifier == UDF_VOLUME_DESCRIPTOR_ID.UdfAnchorVolumeDescriptorPointer) :
        return 4096
    return 0

def PrintUdf (Data, Lba, BlockSize):

  print('UDF binary (LBA: 0x%x, BlockSize: 0x%x)' % (args.Lba, args.BlockSize))

  Tag = UDF_DESCRIPTOR_TAG.from_buffer (bytearray(args.InputFileBuffer), args.BlockSize * args.Lba)
  if Tag.TagIdentifier == UDF_VOLUME_DESCRIPTOR_ID.UdfAnchorVolumeDescriptorPointer :
      AnchorVolumeDesc = UDF_ANCHOR_VOLUME_DESCRIPTOR_POINTER.from_buffer (bytearray(args.InputFileBuffer), args.BlockSize * args.Lba)
      DumpAnchorVolumeDesc(AnchorVolumeDesc)
  elif Tag.TagIdentifier == UDF_VOLUME_DESCRIPTOR_ID.UdfLogicalVolumeDescriptor :
      LogicalVolumeDesc = UDF_LOGICAL_VOLUME_DESCRIPTOR.from_buffer (bytearray(args.InputFileBuffer), args.BlockSize * args.Lba)
      DumpLogicalVolumeDesc(LogicalVolumeDesc)
  elif Tag.TagIdentifier == UDF_VOLUME_DESCRIPTOR_ID.UdfPartitionDescriptor :
      PartitionDesc = UDF_PARTITION_DESCRIPTOR.from_buffer (bytearray(args.InputFileBuffer), args.BlockSize * args.Lba)
      DumpPartitionDesc(PartitionDesc)
  elif Tag.TagIdentifier == UDF_VOLUME_DESCRIPTOR_ID.UdfPrimaryVolumeDescriptor :
      PrimaryVolumeDesc = UDF_PRIMARY_VOLUME_DESCRIPTOR.from_buffer (bytearray(args.InputFileBuffer), args.BlockSize * args.Lba)
      DumpPrimaryVolumeDesc (PrimaryVolumeDesc)
  elif Tag.TagIdentifier == UDF_VOLUME_DESCRIPTOR_ID.UdfFileSetDescriptor :
      FileSetDesc = UDF_FILE_SET_DESCRIPTOR.from_buffer (bytearray(args.InputFileBuffer), args.BlockSize * args.Lba)
      DumpFileSetDesc (FileSetDesc)
  elif Tag.TagIdentifier == UDF_VOLUME_DESCRIPTOR_ID.UdfExtendedFileEntry :
      ExtFileEntry = UDF_EXTENDED_FILE_ENTRY.from_buffer (bytearray(args.InputFileBuffer), args.BlockSize * args.Lba)
      DumpExtFileEntry (ExtFileEntry)
      TotalSize = ExtFileEntry.LengthOfAllocationDescriptors
      Offset = sizeof(UDF_EXTENDED_FILE_ENTRY) + ExtFileEntry.LengthOfExtendedAttributes
      Size = 0
      while (Size < TotalSize) :
          FileIdentifierDesc = UDF_FILE_IDENTIFIER_DESCRIPTOR.from_buffer (bytearray(args.InputFileBuffer), args.BlockSize * args.Lba + Offset + Size)
          if (FileIdentifierDesc.DescriptorTag.TagIdentifier != UDF_VOLUME_DESCRIPTOR_ID.UdfFileIdentifierDescriptor) :
              break
          DumpFileIdentifierDesc (FileIdentifierDesc)
          FileName = UDF_FILE_NAME.from_buffer (bytearray(args.InputFileBuffer), args.BlockSize * args.Lba + Offset + Size + sizeof(UDF_FILE_IDENTIFIER_DESCRIPTOR) - sizeof(c_uint16) + FileIdentifierDesc.LengthOfImplementationUse)
          DumpFileName (FileName.Name, FileIdentifierDesc.LengthOfFileIdentifier)
          Size += sizeof(UDF_FILE_IDENTIFIER_DESCRIPTOR) - sizeof(c_uint16) + FileIdentifierDesc.LengthOfImplementationUse + FileIdentifierDesc.LengthOfFileIdentifier
          Size = (Size + 3) & ~0x3
  elif Tag.TagIdentifier == UDF_VOLUME_DESCRIPTOR_ID.UdfFileEntry:
      FileEntry = UDF_FILE_ENTRY.from_buffer(bytearray(args.InputFileBuffer), args.BlockSize * args.Lba)
      DumpFileEntry(FileEntry)
      TotalSize = FileEntry.LengthOfAllocationDescriptors
      Offset = sizeof(UDF_EXTENDED_FILE_ENTRY) + FileEntry.LengthOfExtendedAttributes
      Size = 0
      while (Size < TotalSize):
          FileIdentifierDesc = UDF_FILE_IDENTIFIER_DESCRIPTOR.from_buffer(bytearray(args.InputFileBuffer), args.BlockSize * args.Lba + Offset + Size)
          if (FileIdentifierDesc.DescriptorTag.TagIdentifier != UDF_VOLUME_DESCRIPTOR_ID.UdfFileIdentifierDescriptor):
              break
          DumpFileIdentifierDesc(FileIdentifierDesc)
          FileName = UDF_FILE_NAME.from_buffer(bytearray(args.InputFileBuffer), args.BlockSize * args.Lba + Offset + Size + sizeof(UDF_FILE_IDENTIFIER_DESCRIPTOR) - sizeof(c_uint16) + FileIdentifierDesc.LengthOfImplementationUse)
          DumpFileName(FileName.Name, FileIdentifierDesc.LengthOfFileIdentifier)
          Size += sizeof(UDF_FILE_IDENTIFIER_DESCRIPTOR) - sizeof(c_uint16) + FileIdentifierDesc.LengthOfImplementationUse + FileIdentifierDesc.LengthOfFileIdentifier
          Size = (Size + 3) & ~0x3
      
  else :
      print("  DescriptorTag:")
      DumpDescriptorTag (Tag)

if __name__ == '__main__':
    #
    # Create command line argument parser object
    #
    parser = argparse.ArgumentParser(prog=__prog__, version=__version__, usage=__usage__, description=__copyright__, conflict_handler='resolve')
    parser.add_argument("-v", "--verbose", dest='Verbose', action="store_true", help="increase output messages")
    parser.add_argument("-q", "--quiet", dest='Quiet', action="store_true", help="reduce output messages")
    parser.add_argument("--Lba", dest='Lba_Str', type=str, help="specify the LBA in UDF file.")
    parser.add_argument("--BlockSize", dest='BlockSize_Str', type=str, help="specify the block size in UDF file.")
    parser.add_argument(metavar="input_file", dest='InputFile', type=argparse.FileType('rb'), help="specify the input UDF file")

    #
    # Parse command line arguments
    #
    args = parser.parse_args()

    #
    # Read input file into a buffer and save input filename
    #
    args.InputFileName   = args.InputFile.name
    args.InputFileBuffer = args.InputFile.read()
    args.InputFile.close()
    args.InputFileSize   = os.path.getsize(args.InputFileName)

    args.Lba = 0x0
    if args.Lba_Str:
        try:
            if args.Lba_Str.upper().startswith('0X'):
                args.Lba = (int)(args.Lba_Str, 16)
            else:
                args.Lba = (int)(args.Lba_Str)
        except:
            pass

    args.BlockSize = 512
    if args.BlockSize_Str:
        try:
            if args.BlockSize_Str.upper().startswith('0X'):
              args.BlockSize = (int)(args.BlockSize_Str, 16)
            else:
              args.BlockSize = (int)(args.BlockSize_Str)
        except:
            pass
    else :
        args.BlockSize = DetectUdfBlockSize (args.InputFileBuffer)
        print("detect BlockSize - %d" % args.BlockSize)
        if args.BlockSize == 0:
            exit

    if args.Lba == 0:
        ScanUdf (args.InputFileBuffer)
        exit
    else:
        PrintUdf (args.InputFileBuffer, args.Lba, args.BlockSize)