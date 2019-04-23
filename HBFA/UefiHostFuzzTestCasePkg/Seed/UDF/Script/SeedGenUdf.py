## @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

'''
SeedGenUdf
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
__prog__      = 'SeedGenUdf'
__version__   = '%s Version %s' % (__prog__, '0.1 ')
__copyright__ = 'Copyright (c) 2018, Intel Corporation. All rights reserved.'
__usage__     = '%s [options] -o <output_file>' % (__prog__)

TOTAL_SIZE = 1 * 1024 * 1024
BLOCK_SIZE = 2048

if __name__ == '__main__':
  #
  # Create command line argument parser object
  #
  parser = argparse.ArgumentParser(prog=__prog__, version=__version__, usage=__usage__, description=__copyright__, conflict_handler='resolve')
  parser.add_argument("-v", "--verbose", dest='Verbose', action="store_true", help="increase output messages")
  parser.add_argument("-q", "--quiet", dest='Quiet', action="store_true", help="reduce output messages")
  parser.add_argument("-o", "--output", dest='OutputFileName', type=str, metavar='filename', help="specify the output filename", required=True)

  #
  # Parse command line arguments
  #
  args = parser.parse_args()

  #
  # Write output file
  #
  args.OutputFile = open(args.OutputFileName, 'wb')

  DummyBuf = create_string_buffer(TOTAL_SIZE)
  CdromDesc = CDROM_VOLUME_DESCRIPTOR.from_buffer (DummyBuf, UDF_VRS_START_OFFSET)
  CdromDesc.Id[0] = (ord('B'))
  CdromDesc.Id[1] = (ord('E'))
  CdromDesc.Id[2] = (ord('A'))
  CdromDesc.Id[3] = (ord('0'))
  CdromDesc.Id[4] = (ord('1'))

  CdromDesc = CDROM_VOLUME_DESCRIPTOR.from_buffer (DummyBuf, UDF_VRS_START_OFFSET + UDF_LOGICAL_SECTOR_SIZE)
  CdromDesc.Id[0] = (ord('N'))
  CdromDesc.Id[1] = (ord('S'))
  CdromDesc.Id[2] = (ord('R'))
  CdromDesc.Id[3] = (ord('0'))
  CdromDesc.Id[4] = (ord('2'))

  CdromDesc = CDROM_VOLUME_DESCRIPTOR.from_buffer (DummyBuf, UDF_VRS_START_OFFSET + UDF_LOGICAL_SECTOR_SIZE * 2)
  CdromDesc.Id[0] = (ord('T'))
  CdromDesc.Id[1] = (ord('E'))
  CdromDesc.Id[2] = (ord('A'))
  CdromDesc.Id[3] = (ord('0'))
  CdromDesc.Id[4] = (ord('1'))

  AnchorDesc = UDF_ANCHOR_VOLUME_DESCRIPTOR_POINTER.from_buffer (DummyBuf, 0x100 * BLOCK_SIZE)
  AnchorDesc.DescriptorTag.TagIdentifier = UDF_VOLUME_DESCRIPTOR_ID.UdfAnchorVolumeDescriptorPointer

  LastBlock = (TOTAL_SIZE + BLOCK_SIZE - 1) / BLOCK_SIZE - 1
  #Offset = (LastBlock - 0x100) * BLOCK_SIZE
  Offset = (LastBlock - MAX_CORRECTION_BLOCKS_NUM) * BLOCK_SIZE
  AnchorDesc = UDF_ANCHOR_VOLUME_DESCRIPTOR_POINTER.from_buffer (DummyBuf, Offset)
  AnchorDesc.DescriptorTag.TagIdentifier = UDF_VOLUME_DESCRIPTOR_ID.UdfAnchorVolumeDescriptorPointer

  SeqBlocksNum = 16
  SeqStartBlock = 16
  AnchorDesc.MainVolumeDescriptorSequenceExtent.ExtentLength = SeqBlocksNum * BLOCK_SIZE
  AnchorDesc.MainVolumeDescriptorSequenceExtent.ExtentLocation = SeqStartBlock

  Offset = SeqStartBlock * BLOCK_SIZE
  DescTag = UDF_DESCRIPTOR_TAG.from_buffer (DummyBuf, Offset)
  DescTag.TagIdentifier = UDF_VOLUME_DESCRIPTOR_ID.UdfTerminatingDescriptor

  args.OutputFile.write(DummyBuf)

  args.OutputFile.close()

