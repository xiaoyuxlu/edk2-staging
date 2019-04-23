## @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

from   ctypes import *

class EFI_GUID(Structure):
    _fields_ = [
        ('Guid1',            c_uint32),
        ('Guid2',            c_uint16),
        ('Guid3',            c_uint16),
        ('Guid4',            ARRAY(c_uint8, 8)),
        ]

class EFI_TIME(Structure):
    _fields_ = [
        ('Year',             c_uint16),
        ('Month',            c_uint8),
        ('Day',              c_uint8),
        ('Hour',             c_uint8),
        ('Minute',           c_uint8),
        ('Second',           c_uint8),
        ('Pad1',             c_uint8),
        ('Nanosecond',       c_uint32),
        ('TimeZone',         c_int16),
        ('Daylight',         c_uint8),
        ('Pad2',             c_uint8),
        ]

EFI_VARIABLE_NON_VOLATILE                            = 0x00000001
EFI_VARIABLE_BOOTSERVICE_ACCESS                      = 0x00000002
EFI_VARIABLE_RUNTIME_ACCESS                          = 0x00000004
EFI_VARIABLE_HARDWARE_ERROR_RECORD                   = 0x00000008
EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS   = 0x00000020
EFI_VARIABLE_APPEND_WRITE                            = 0x00000040
EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS              = 0x00000010
