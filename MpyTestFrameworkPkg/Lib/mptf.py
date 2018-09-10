# # @file
# This file is a python wrapper for tester to get Micropython Test Framework module from encapsulated APIs
#
# INTEL CONFIDENTIAL
# Copyright (c) 2017-2018, Intel Corporation. All rights reserved.<BR>
#
# The source code contained or described herein and all documents
# related to the source code ("Material") are owned by Intel Corporation
# or its suppliers or licensors. Title to the Material remains with Intel
# Corporation or its suppliers and licensors. The Material contains trade
# secrets and proprietary and confidential information of Intel or its
# suppliers and licensors. The Material is protected by worldwide copyright
# and trade secret laws and treaty provisions. No part of the Material may be
# used, copied, reproduced, modified, published, uploaded, posted, transmitted,
# distributed, or disclosed in any way without Intel's prior express written
# permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or
# delivery of the Materials, either expressly, by implication, inducement,
# estoppel or otherwise. Any license under such intellectual property
# rights must be express and approved by Intel in writing.
#


##
# Import Modules
#
import ure
import array
import uefi
import _ets
import utime

##
# Color definition
#
BLACK = 'MPTF_FRE_GROUND_COLOR_BLACK'
BLUE = 'MPTF_FRE_GROUND_COLOR_BLUE'
GREEN = 'MPTF_FRE_GROUND_COLOR_GREEN'
CYAN = 'MPTF_FRE_GROUND_COLOR_CYAN'
RED = 'MPTF_FRE_GROUND_COLOR_RED'
MAGENTA = 'MPTF_FRE_GROUND_COLOR_MAGENTA'
BROWN = 'MPTF_FRE_GROUND_COLOR_BROWN'
LIGHTGRAY = 'MPTF_FRE_GROUND_COLOR_LIGHTGRAY'
BRIGHT = 'MPTF_FRE_GROUND_COLOR_BRIGHT'
DARKGRAY = 'MPTF_FRE_GROUND_COLOR_DARKGRAY'
LIGHTBLUE = 'MPTF_FRE_GROUND_COLOR_LIGHTBLUE'
LIGHTGREEN = 'MPTF_FRE_GROUND_COLOR_LIGHTGREEN'
LIGHTCYAN = 'MPTF_FRE_GROUND_COLOR_LIGHTCYAN'
LIGHTRED = 'MPTF_FRE_GROUND_COLOR_LIGHTRED'
LIGHTMAGENTA = 'MPTF_FRE_GROUND_COLOR_LIGHTMAGENTA'
YELLOW = 'MPTF_FRE_GROUND_COLOR_YELLOW'
WHITE = 'MPTF_FRE_GROUND_COLOR_WHITE'

BACKBLACK = 'MPTF_BACK_GROUND_COLOR_BACKBLACK'
BACKBLUE = 'MPTF_BACK_GROUND_COLOR_BACKBLUE'
BACKGREEN = 'MPTF_BACK_GROUND_COLOR_BACKGREEN'
BACKCYAN = 'MPTF_BACK_GROUND_COLOR_BACKCYAN'
BACKRED = 'MPTF_BACK_GROUND_COLOR_BACKRED'
BACKMAGENTA = 'MPTF_BACK_GROUND_COLOR_BACKMAGENTA'
BACKBROWN = 'MPTF_BACK_GROUND_COLOR_BACKBROWN'
BACKLIGHTGRAY = 'MPTF_BACK_GROUND_COLOR_LIGHTGRAY'

#
# EFI Console Colours
#
EFI_BLACK            =     0x00
EFI_BLUE             =     0x01
EFI_GREEN            =     0x02
EFI_CYAN             =     (EFI_BLUE | EFI_GREEN)
EFI_RED              =     0x04
EFI_MAGENTA          =     (EFI_BLUE | EFI_RED)
EFI_BROWN            =     (EFI_GREEN | EFI_RED)
EFI_LIGHTGRAY        =     (EFI_BLUE | EFI_GREEN | EFI_RED)
EFI_BRIGHT           =     0x08
EFI_DARKGRAY         =     (EFI_BLACK | EFI_BRIGHT)
EFI_LIGHTBLUE        =     (EFI_BLUE | EFI_BRIGHT)
EFI_LIGHTGREEN       =     (EFI_GREEN | EFI_BRIGHT)
EFI_LIGHTCYAN        =     (EFI_CYAN | EFI_BRIGHT)
EFI_LIGHTRED         =     (EFI_RED | EFI_BRIGHT)
EFI_LIGHTMAGENTA     =     (EFI_MAGENTA | EFI_BRIGHT)
EFI_YELLOW           =     (EFI_BROWN | EFI_BRIGHT)
EFI_WHITE            =     (EFI_BLUE | EFI_GREEN | EFI_RED | EFI_BRIGHT)

EFI_BACKGROUND_BLACK     = 0x00
EFI_BACKGROUND_BLUE      = 0x10
EFI_BACKGROUND_GREEN     = 0x20
EFI_BACKGROUND_CYAN      = (EFI_BACKGROUND_BLUE | EFI_BACKGROUND_GREEN)
EFI_BACKGROUND_RED       = 0x40
EFI_BACKGROUND_MAGENTA   = (EFI_BACKGROUND_BLUE | EFI_BACKGROUND_RED)
EFI_BACKGROUND_BROWN     = (EFI_BACKGROUND_GREEN | EFI_BACKGROUND_RED)
EFI_BACKGROUND_LIGHTGRAY = (EFI_BACKGROUND_BLUE | EFI_BACKGROUND_GREEN | EFI_BACKGROUND_RED)

BLACK = 'MPTF_FRE_GROUND_COLOR_BLACK'
BLUE = 'MPTF_FRE_GROUND_COLOR_BLUE'
GREEN = 'MPTF_FRE_GROUND_COLOR_GREEN'
CYAN = 'MPTF_FRE_GROUND_COLOR_CYAN'
RED = 'MPTF_FRE_GROUND_COLOR_RED'
MAGENTA = 'MPTF_FRE_GROUND_COLOR_MAGENTA'
BROWN = 'MPTF_FRE_GROUND_COLOR_BROWN'
LIGHTGRAY = 'MPTF_FRE_GROUND_COLOR_LIGHTGRAY'
BRIGHT = 'MPTF_FRE_GROUND_COLOR_BRIGHT'
DARKGRAY = 'MPTF_FRE_GROUND_COLOR_DARKGRAY'
LIGHTBLUE = 'MPTF_FRE_GROUND_COLOR_LIGHTBLUE'
LIGHTGREEN = 'MPTF_FRE_GROUND_COLOR_LIGHTGREEN'
LIGHTCYAN = 'MPTF_FRE_GROUND_COLOR_LIGHTCYAN'
LIGHTRED = 'MPTF_FRE_GROUND_COLOR_LIGHTRED'
LIGHTMAGENTA = 'MPTF_FRE_GROUND_COLOR_LIGHTMAGENTA'
YELLOW = 'MPTF_FRE_GROUND_COLOR_YELLOW'
WHITE = 'MPTF_FRE_GROUND_COLOR_WHITE'

BACKBLACK = 'MPTF_BACK_GROUND_COLOR_BACKBLACK'
BACKBLUE = 'MPTF_BACK_GROUND_COLOR_BACKBLUE'
BACKGREEN = 'MPTF_BACK_GROUND_COLOR_BACKGREEN'
BACKCYAN = 'MPTF_BACK_GROUND_COLOR_BACKCYAN'
BACKRED = 'MPTF_BACK_GROUND_COLOR_BACKRED'
BACKMAGENTA = 'MPTF_BACK_GROUND_COLOR_BACKMAGENTA'
BACKBROWN = 'MPTF_BACK_GROUND_COLOR_BACKBROWN'
BACKLIGHTGRAY = 'MPTF_BACK_GROUND_COLOR_LIGHTGRAY'

ColorMapDict = {
    BLACK:'BLACK',
    BLUE:'BLUE',
    GREEN:'GREEN',
    CYAN:'CYAN',
    RED:'RED',
    MAGENTA:'MAGENTA',
    BROWN:'BROWN',
    LIGHTGRAY:'LIGHTGRAY',
    BRIGHT:'BRIGHT',
    DARKGRAY:'DARKGRAY',
    LIGHTBLUE:'LIGHTBLUE',
    LIGHTGREEN:'LIGHTGREEN',
    LIGHTCYAN:'LIGHTCYAN',
    LIGHTRED:'LIGHTRED',
    LIGHTMAGENTA:'LIGHTMAGENTA',
    YELLOW:'YELLOW',
    WHITE:'WHITE',
    BACKBLACK:'BACKBLACK',
    BACKBLUE:'BACKBLUE',
    BACKGREEN:'BACKGREEN',
    BACKCYAN:'BACKCYAN',
    BACKRED:'BACKRED',
    BACKMAGENTA:'BACKMAGENTA',
    BACKBROWN:'BACKBROWN',
    BACKLIGHTGRAY:'BACKLIGHTGRAY'
    }

ColorDict = {
    BLACK:EFI_BLACK,
    BLUE:EFI_BLUE,
    GREEN:EFI_GREEN,
    CYAN:EFI_CYAN,
    RED:EFI_RED,
    MAGENTA:EFI_MAGENTA,
    BROWN:EFI_BROWN,
    LIGHTGRAY:EFI_LIGHTGRAY,
    BRIGHT:EFI_BRIGHT,
    DARKGRAY:EFI_DARKGRAY,
    LIGHTBLUE:EFI_LIGHTBLUE,
    LIGHTGREEN:EFI_LIGHTGREEN,
    LIGHTCYAN:EFI_LIGHTCYAN,
    LIGHTRED:EFI_LIGHTRED,
    LIGHTMAGENTA:EFI_LIGHTMAGENTA,
    YELLOW:EFI_YELLOW,
    WHITE:EFI_WHITE,
    BACKBLACK:EFI_BACKGROUND_BLACK,
    BACKBLUE:EFI_BACKGROUND_BLUE,
    BACKGREEN:EFI_BACKGROUND_GREEN,
    BACKCYAN:EFI_BACKGROUND_CYAN,
    BACKRED:EFI_BACKGROUND_RED,
    BACKMAGENTA:EFI_BACKGROUND_MAGENTA,
    BACKBROWN:EFI_BACKGROUND_BROWN,
    BACKLIGHTGRAY:EFI_BACKGROUND_LIGHTGRAY
    }

#
# key definition
#
ENTER = '_MPTF_ENTER'
TAB = '_MPTF_TAB'
ESC = '_MPTF_ESC'
BACKSPACE = '_MPTF_BACKSPACE'
DELETE = '_MPTF_DELETE'
SPACE = '_MPTF_SPACE'
HOME = '_MPTF_ENTER'
END = '_MPTF_TAB'
LEFT = '_MPTF_LEFT'
RIGHT = '_MPTF_RIGHT'
DOWN = '_MPTF_DOWN'
UP = '_MPTF_UP'
PAGE_DOWN = '_MPTF_PAGE_DOWN'
PAGE_UP = '_MPTF_PAGE_UP'
PRINTSCREEN = '_MPTF_PRINTSCREEN'
PAUSE = '_MPTF_PAUSE'
ALT = '_MPTF_ALT'
CMD = '_MPTF_CMD'
CTRL = '_MPTF_CTRL'
META = '_MPTF_META'
SHIFT = '_MPTF_SHIFT'
WIN = '_MPTF_WIN'
ALTGR = '_MPTF_ALTGR'
CAPS_LOCK = '_MPTF_CAPS_LOCK'
SCROLL_LOCK = '_MPTF_SCROLL_LOCK'
NUM_LOCK = '_MPTF_NUM_LOCK'

F1 = '_MPTF_F1'
F2 = '_MPTF_F2'
F3 = '_MPTF_F3'
F4 = '_MPTF_F4'
F5 = '_MPTF_F5'
F6 = '_MPTF_F6'
F7 = '_MPTF_F7'
F8 = '_MPTF_F8'
F9 = '_MPTF_F9'
F10 = '_MPTF_F10'
F11 = '_MPTF_F11'
F12 = '_MPTF_F12'

#
# Required unicode control chars
#
CHAR_BACKSPACE = 0x0008
CHAR_TAB = 0x0009
CHAR_LINEFEED  =       0x000A
CHAR_CARRIAGE_RETURN = 0x000D

#
# EFI Scan codes
#
SCAN_NULL  =   0x0000
SCAN_UP    =   0x0001
SCAN_DOWN  =   0x0002
SCAN_RIGHT =   0x0003
SCAN_LEFT  =   0x0004
SCAN_HOME    = 0x0005
SCAN_END     = 0x0006
SCAN_INSERT  = 0x0007
SCAN_DELETE  = 0x0008
SCAN_PAGE_UP = 0x0009
SCAN_PAGE_DOWN = 0x000A
SCAN_F1        = 0x000B
SCAN_F2        = 0x000C
SCAN_F3        = 0x000D
SCAN_F4        = 0x000E
SCAN_F5        = 0x000F
SCAN_F6        = 0x0010
SCAN_F7        = 0x0011
SCAN_F8        = 0x0012
SCAN_F9        = 0x0013
SCAN_F10       = 0x0014
SCAN_ESC       = 0x0017
SCAN_F11       =           0x0015
SCAN_F12       =           0x0016
SCAN_PAUSE     =           0x0048
SCAN_F13       =           0x0068
SCAN_F14       =           0x0069
SCAN_F15       =           0x006A
SCAN_F16       =           0x006B
SCAN_F17       =           0x006C
SCAN_F18       =           0x006D
SCAN_F19       =           0x006E
SCAN_F20       =           0x006F
SCAN_F21       =           0x0070
SCAN_F22       =           0x0071
SCAN_F23       =           0x0072
SCAN_F24       =           0x0073
SCAN_MUTE      =           0x007F
SCAN_VOLUME_UP     =       0x0080
SCAN_VOLUME_DOWN   =       0x0081
SCAN_BRIGHTNESS_UP =       0x0100
SCAN_BRIGHTNESS_DOWN=      0x0101
SCAN_SUSPEND        =      0x0102
SCAN_HIBERNATE      =      0x0103
SCAN_TOGGLE_DISPLAY =      0x0104
SCAN_RECOVERY       =      0x0105
SCAN_EJECT          =      0x0106

#
# Any Shift or Toggle State that is valid should have
# high order bit set.
#
# Shift state
#
EFI_SHIFT_STATE_VALID     =0x80000000
EFI_RIGHT_SHIFT_PRESSED   =0x00000001
EFI_LEFT_SHIFT_PRESSED    =0x00000002
EFI_RIGHT_CONTROL_PRESSED =0x00000004
EFI_LEFT_CONTROL_PRESSED  =0x00000008
EFI_RIGHT_ALT_PRESSED     =0x00000010
EFI_LEFT_ALT_PRESSED      =0x00000020
EFI_RIGHT_LOGO_PRESSED    =0x00000040
EFI_LEFT_LOGO_PRESSED     =0x00000080
EFI_MENU_KEY_PRESSED      =0x00000100
EFI_SYS_REQ_PRESSED       =0x00000200

#
# Toggle state
#
EFI_TOGGLE_STATE_VALID    =0x80
EFI_KEY_STATE_EXPOSED     =0x40
EFI_SCROLL_LOCK_ACTIVE    =0x01
EFI_NUM_LOCK_ACTIVE       =0x02
EFI_CAPS_LOCK_ACTIVE      =0x04

KeyShitDict = {
    SHIFT:EFI_LEFT_SHIFT_PRESSED,
    ALT:EFI_LEFT_ALT_PRESSED,
    ALTGR:EFI_RIGHT_ALT_PRESSED,
    WIN:EFI_LEFT_LOGO_PRESSED,
    CTRL:EFI_LEFT_CONTROL_PRESSED,
    PRINTSCREEN:EFI_SYS_REQ_PRESSED,
    META:EFI_LEFT_LOGO_PRESSED,
    CMD:SCAN_NULL #???
    }

KeyToggleDict = {
    CAPS_LOCK:EFI_CAPS_LOCK_ACTIVE,
    SCROLL_LOCK:EFI_SCROLL_LOCK_ACTIVE,
    NUM_LOCK:EFI_NUM_LOCK_ACTIVE,
    }
#
# EFI Scan codes
#

KeyUnicodeCharDict = {
    ENTER:CHAR_CARRIAGE_RETURN,
    TAB:CHAR_TAB,
    BACKSPACE:CHAR_BACKSPACE#,
    #LINEFEED:CHAR_LINEFEED
    }

KeyScanCodeDict = {
    ESC:SCAN_ESC,
    DELETE:SCAN_DELETE,
    SPACE:SCAN_NULL, #???
    HOME:SCAN_HOME,
    END:SCAN_END,
    LEFT:SCAN_LEFT,
    RIGHT:SCAN_RIGHT,
    DOWN:SCAN_DOWN,
    UP:SCAN_UP,
    PAGE_DOWN:SCAN_DOWN,
    PAGE_UP:SCAN_UP,
    PAUSE:SCAN_PAUSE,
    F1:SCAN_F1,
    F2:SCAN_F2,
    F3:SCAN_F3,
    F4:SCAN_F4,
    F5:SCAN_F5,
    F6:SCAN_F6,
    F7:SCAN_F7,
    F8:SCAN_F8,
    F9:SCAN_F9,
    F10:SCAN_F10,
    F11:SCAN_F11,
    F12:SCAN_F12
}

#
# Log level definition
#
LOG_DEBUG = "DEBUG"
LOG_TRACE = "TRACE"
LOG_INFO = "INFO"
LOG_WARN = "WARN"
LOG_FAIL = "FAIL"
LOG_PASS = "PASS"

##
# The python wrapper for tester to get Micropython Test Framework module from encapsulated APIs
#
class mptf(object):

    ## Constructor
    #
    # Initialize the instance with parameters provided
    # If no parameter, default value used
    #
    # @param logpath:       The folder to put logs. Do not specify it or built-in report featre can't be used
    # @param volomeLabel:   The volume label of current disk which stores the framework
    # @param logLevel:      The level of log. The log which level is not included will be filtered out
    #
    # @retval:              None
    #
    def __init__(self, logPath='log', volumeLabel='MPTF', logLevel='TRACE | INFO | PASS | FAIL | WARN'):
        root = "mpytest" + "\\log\\"
        self.logPath = root + logPath + ".json"
        self.volumeLabel = volumeLabel
        self.logstr = ''
        self.interval = 1000
        self.envDict = {}
        self.logLevel = logLevel
        self.logInitialize = False

    ## Get environment variable
    #
    # Get the value of a certain environment variable.
    #
    # @param variableName:  Specify the variable to be retrieved
    #
    # @retval:              The value of the variable
    #
    def GetEnv(self, variableName = None):
        if variableName in self.envDict.keys():
            self.Trace("GetEnv: " + str(variableName) + " is " + str(self.envDict[variableName]))
            return self.envDict[variableName]
        else:
            self.Trace("GetEnv: " + "Nothing found")
            return None

    ## Set environment variable
    #
    # Set specified variable a value
    #
    # @param variableName:  Specify the variable to be set
    # @param variableValue: Specify the value
    #
    # @retval:              None
    #
    def SetEnv(self, variableName, variableValue):
        self.Trace("Set Env: Key is: " + str(variableName) + "; Value is: " + str(variableValue))
        self.envDict[variableName] = variableValue
        return True

    ## Set the API execution speed
    #
    # Set the API execution speed, the higher the parameter,
    # the slower the API execute
    #
    # @param interval:      The interval in milliseconds
    #
    # @retval:              None
    #
    def SetTickTock (self, interval = 1000):
        self.Trace("SetTickTock: interval is: " + str(interval))
        self.interval = interval

    ## Set the API execution speed
    #
    # Set the API execution speed, the higher the parameter,
    # the slower the API execute
    #
    # @param interval:      The interval in milliseconds
    #
    # @retval:              None
    #
    def GetTickTock (self):
        self.Trace("GetTickTock: " + str(self.interval))
        return self.interval


    ## Trigger function key
    #
    #  Triger function key with multiple ways
    #
    #  @param times:        The type times of key
    #  @param InputSpeed:   The time interval between each key
    #
    #  @retval:             None
    #
    #  @samples
    #  FuncKey (mptf.ENTER)
    #  FuncKey (mptf.ENTER, 10)
    #  FuncKey (mptf.SHIFT+mptf.ALT, 10, 500)
    def FuncKey (self, funckeystr, times = 1, InputSpeed = 100):
        self.Trace("FuncKey: " + funckeystr)
        UnicodeChar = 0x0
        Scancode    = 0x0
        KeyShift    = 0x0
        KeyToggle   = 0x0

        _ets.suspend(self.interval)

        for key in KeyUnicodeCharDict.keys():
            if key in funckeystr:
                UnicodeChar = KeyUnicodeCharDict[key]

        for key in KeyScanCodeDict.keys():
            if key in funckeystr:
                Scancode = KeyScanCodeDict[key]

        for key in KeyShitDict.keys():
            if key in funckeystr:
                KeyShift = KeyShitDict[key]

        for key in KeyToggleDict.keys():
            if key in funckeystr:
                KeyToggle = KeyToggleDict[key]

        key = (UnicodeChar, Scancode, KeyShift, KeyToggle)

        if key == (0x0, 0x0, 0x0, 0x0):
            return

        while times > 0:
            _ets.press(*key)
            _ets.suspend(InputSpeed)
            times -= 1

    ## Trigger function key and other key event
    #
    #  Triger not only funciton key but also word key
    #
    # @param keystr:        The word or funciton key
    # @param interval:      The interval in milliseconds
    #
    # @retval:              None
    #
    # @samples
    # Input('Helloworld.efi' + mptf.ENTER + mptf.ENTER)
    #
    def Input(self, keystr, InputSpeed = 100):
        self.Trace("Input: " + keystr)
        index  = 0

        _ets.suspend(self.interval)

        while index < len(keystr):
            FuncKey = False
            for key in KeyUnicodeCharDict.keys():
                if keystr[index:].startswith(key):
                    self.FuncKey(key, 1, InputSpeed)
                    index = index + len(key)
                    FuncKey = True

            for key in KeyScanCodeDict.keys():
                if keystr[index:].startswith(key):
                    self.FuncKey(key, 1, InputSpeed)
                    index = index + len(key)
                    FuncKey = True

            for key in KeyShitDict.keys():
                if keystr[index:].startswith(key):
                    self.FuncKey(key, 1, InputSpeed)
                    index = index + len(key)
                    FuncKey = True

            for key in KeyToggleDict.keys():
                if keystr[index:].startswith(key):
                    FuncKey(key, 1, InputSpeed)
                    index = index + len(key)
                    FuncKey = True

            if not FuncKey:
                _ets.press(keystr[index])
                _ets.suspend(InputSpeed)
                index = index + 1

    def GetToggleStatus(self):
        self.Trace("GetToggleStatus")
        return

    ## Suspend script for given value
    #
    #  Suspend script for given value and return to normal execution flow when time is out
    #
    # @param msecond:       The suspend time in milliseconds
    #
    # @retval:              None
    #
    def Delay (self, msecond):
        self.Trace("Delay: " + str(msecond))
        _ets.suspend(msecond)


    ## Reserve for debug output
    #
    def Debug (self, string, debuglevel='INFO'):
        if debuglevel == 'WARN':
            _ets.debug(string, 0x00000002)
        elif debuglevel == 'ERROR':
            _ets.debug(string, 0x80000000)
        elif debuglevel == 'VERBOSE':
            _ets.debug(string, 0x00400000)
        else:
            _ets.debug(string)

    ## Get the size of screen
    #
    #  Get the size of screen with width and height
    #
    # @retval:              (width, height)
    #
    def GetScreenSize (self):
        self.Trace("GetScreenSize")
        (x, y) = (80, 25)
        return x, y

    ## Set the size of screen
    #
    #  Set the size of screen with width and height
    #
    # @param x:             The width of screen
    # @param y:             The height of screen
    # @retval:              None
    #
    def SetScreenSize (self, x, y):
        self.Trace("SetScreenSize")
        print ("The only supported screen size is 80 * 25 for now")
        return True

    ## Get Screen Info
    #
    #  Get text and its attribute on the screen
    #
    # @param history:       Boolean value to specify whether screen contains history info
    #
    # @retval:              Text and its attribute on the screen
    #
    def GetScreen(self, history = False):
        self.Trace("GetScreen")
        if not history:
            text, attr = _ets.snapshot()
        else:
            text, attr = _ets.snapshot()
        return (text, attr)

    ## Get a row in screen
    #
    #  Get a string from the specific line, include spaces.
    #
    # @param row:           The number of row to be queried
    # @param history:       Boolean value to specify whether history data is queried or not
    #
    # @retval:              Text and its attribute on the screen
    #
    def GetRowString(self, row, history = False):
        text, attr = _ets.snapshot(history)
        length = len(text)
        w,h = self.GetScreenSize()
        string = []
        for i in range(length):
            if (row == i//w):
                string += text[i]
        self.Trace("GetRowString: row number is " + str(row) + ";    " + "row content is " + "".join(string))
        return "".join(string)

    ## Clear screen buffer
    def ClearScreen(self, history = False):
        self.Trace("ClearScreen")
        return True

    ## Verify whether a specified string with color appear in the current console or history buffer or not.
    #
    # @param string:         The string to be searched.
    # @param ForeBackColor   Foreground Color [|Background Color]: Colors for the selected string name, include
    # Black, Red, Green, Yellow, Blue, Magenta, Cyan, White,
    # BackBlack, BackRed, BackGreen, BackYellow, BackBlue, BackMagenta, BackCyan, BackWhite. No color means every color is ok.
    # @param history        If False, only check the current console. Or else, check the history and current console together
    #
    # @retval True/False
    #
    # @samples
    #   mptf.Verify('FS0')
    #   mptf.Verify('FS0', 'YELLOW' + "BACKBLACK")
    def Verify(self, string, ForeBackColor = None, history = False):

        _ets.suspend(self.interval)

        allColor = self.GetStrColor(string, history)

        if allColor == (None, None):
            self.Trace("Verify: " + string + ";      " + "result: False" + " reason: string not found")
            return False
        else:
            if ForeBackColor == None:
                self.Trace("Verify: " + string + ";     " + "result: True")
                return True
            else:
                for color in allColor:
                    ForeColor, BackColor = color.split(' ')
                    if ForeColor in ForeBackColor and BackColor in ForeBackColor:
                        self.Trace("Verify: " + string + ";     " + "result: True")
                        return True
                self.Trace("Verify: " + string + ";      " + "result: False" + " reason: string found; color not match. " + "The color is: " + ForeBackColor + " and the color list are: ")
                return False


    ## Get front color and background color of given string
    #
    #  Get both forecolor and backcolor of all matching results.
    #  If the string is not found, return (None, None).
    #  Else, the items in the return list are formatted as [‘forecolor backcolor’, ‘forecolor backcolor’, ‘forecolor backcolor’, … ].
    #
    #  @param  string:       The target
    #  @param history:       If False, only check the current console. Or else, check the history and current console together
    #
    #  @retval               A list of forecolor/background color pairs
    def GetStrColor (self, string, history = False):

        ForeColor  = None
        BackColor = None

        _ets.suspend(self.interval)

        text, attr = _ets.snapshot(history)
        #p = ure.compile(string + '.*')
        p = ure.compile(string)

        index = []
        while text:
            result = p.split(text)
            if len(result) > 1:
                resultLen = [len(x) for x in result]
                for i in range(len(result)-1):
                    index.append(sum(resultLen[:i+1]) + i*len(string))
                colorvalues = [attr[x] for x in index]
                break
            else:

                self.Trace("GetStrColor: " + string + ";    " + "result: None")
                return None, None

        ForeBackColor = []
        for colorvalue in colorvalues:
            for colorkey in ColorDict.keys():
                if ColorDict[colorkey] == (colorvalue & 0xf):
                    ForeColor = ColorMapDict[colorkey]
                    break
            for colorkey in ColorDict.keys():
                if ColorDict[colorkey] == (colorvalue & 0xf0):
                    if colorkey == BLACK:
                        BackColor = ColorMapDict[BACKBLACK]
                    else:
                        BackColor = ColorMapDict[colorkey]
                    break
            ForeBackColor.append(ForeColor+' '+BackColor)

        self.Trace("GetStrColor: " + string + ";    " + "result: Found")
        return ForeBackColor

    ## Get locations of given string
    #
    #  Get the locations of all matching results.
    #  If the string is not found, return None.
    #  Else, the items in the return list are formatted as [‘x1 y1, ‘x2 y2, ‘x3 y3’, …]
    #
    #  @param  string:       The target
    #  @param history:       If False, only check the current console. Or else, check the history and current console together
    #
    #  @retval               A list of locations
    def GetStrLocation(self, string, history = False):
        self.Trace("GetStrLocation: " + string)

        text, attr = _ets.snapshot(history)
        p = ure.compile(string)

        index = []
        while text:
            result = p.split(text)
            if len(result) > 1:
                resultLen = [len(x) for x in result]
                for i in range(len(result)-1):
                    index.append(sum(resultLen[:i+1]) + i*len(string))
                break
            else:
                self.Trace("GetStrLocation: " + string + ";     " + "result: None")
                return None
        w,h = self.GetScreenSize()
        locations = []
        for ind in index:
            locations.append(str(ind%w)+' '+str(ind//w))

        self.Trace("GetStrLocation: " + string + ";     " + "result: Found")
        return locations

    ## Wait until a specified string appears
    #
    #  Wait until some specified string’s appearance.
    #  It returns True if the matched string appears within timeOut,
    #  else returns False.
    #
    #  @param  string:          The target
    #  @param  timeOut:         Integer type. Maximum seconds to wait. Default value is 10 seconds
    #  @param  ForeBackColor:   String type. The forecolor or backcolor of the string.
    #  Default value is None, which means no restriction on the color.
    #  @param  history:         If False, only check the current console. Or else, check the history and current console together
    #
    #  @retval                  The string appears or not, True if appears
    def WaitUntil(self, string, timeOut = 10, ForeBackColor = None, history = True):
        time   = 0
        result = False
        _ets.suspend(self.interval)
        while time < timeOut*1000:
            result = self.Verify (string, ForeBackColor, history)
            if result:
                self.Trace("WaitUntil: " + string + ";  " + "result: True")
                return True
            _ets.suspend(100)
            time = time + 100

        self.Trace("WaitUntil: " + string + ";  " + "result: False")
        return False

    ## Wait until a specified string disappears
    #
    #  Wait until some specified string’s disappearance.
    #  It returns True if the matched string disappears within timeOut,
    #  else returns False.
    #
    #  @param  string:          The target
    #  @param  timeOut:         Integer type. Maximum seconds to wait. Default value is 10 seconds
    #  @param  ForeBackColor:   String type. The forecolor or backcolor of the string.
    #  Default value is None, which means no restriction on the color.
    #  @param  history:        If False, only check the current console. Or else, check the history and current console together
    #
    #  @retval                  The string disappears or not, True if disappears
    def WaitVanish(self, string, timeOut = 10, ForeBackColor = None, history = True):
        time   = 0
        result = False
        _ets.suspend(self.interval)
        while time < timeOut*1000:
            result = self.Verify (string, ForeBackColor, history)
            if not result:
                self.Trace("WaitVanish: " + string + ";     " + "result: True")
                return True
            _ets.suspend(100)
            time = time + 100

        self.Trace("WaitVanish: " + string + ";     " + "result: False")
        return False

    ## Find the specified option from EDK2 HII page
    #
    # @param string              The option string
    # @param operation           The option string, suchs as mptf.LEFT, mptf.RIGHT, mptf.DOWN, mptf.UP
    # @param maxNum              The max number of do the operation
    # @param ForeBackColor       Foreground Color [|Background Color]: Colors for the selected string name, include
    # Black, Red, Green, Yellow, Blue, Magenta, Cyan, White,
    # BackBlack, BackRed, BackGreen, BackYellow, BackBlue, BackMagenta, BackCyan, BackWhite. No color means every color is ok.
    #
    # @retval True/False
    def SelectOption(self, string, ForeBackColor = None, operation = DOWN, maxNum = 10):

        #
        # Wait 5 seconds in case the UI response isn't fast enough
        #
        index = 0
        prev_text, prev_attr = "", []
        _ets.suspend(self.interval)
        while index < 5000:
            text,attr=_ets.snapshot()
            if text == prev_text and attr == prev_attr:
                _ets.suspend(100)
                index = index + 100
                prev_text = text
                prev_attr = attr
            else:
                break

        index = 0
        while index < maxNum:
            if self.Verify(string, ForeBackColor):
                self.Trace("SelectOption: " + string + ";   " + "result: True")
                return True
            else:
                self.FuncKey(operation)
            _ets.suspend(200)
            index = index + 1

        self.Trace("SelectOption: " + string + ";   " + "result: False")
        return False

    ## Write message to disk
    #
    #  Write message to disk specified by volume label
    #
    #  @param lgoPath:           The path to log folder
    #  @param message:           The log message
    #  @param volumeLabel:       The volume label of the disk where the log stores
    def write2disk(self, logPath, message, volumeLabel):
        with open("\\\\LABEL:" + volumeLabel + "\\" + logPath, "a+") as f:
            f.write(message + "\n")

    ## Redirect message to memory buffer or filesystem
    #
    #  Private method for internal use by Debug, Trace, Info, Warn, Fail and Pass
    #  @param message:           The log message
    #  @param log_level:         Filter log based on it and indicate the test result
    #  @param flush:             Dump messages in memory buffer to filesystem if True. Or, dump message to memory buffer
    def __Log(self, message, log_level, flush = False):
        _ets.suspend(self.interval)
        localtime = utime.localtime()
        year = "{0:04d}".format(localtime[0])
        month = "{0:02d}".format(localtime[1])
        day = "{0:02d}".format(localtime[2])
        hour = "{0:02d}".format(localtime[3])
        minute = "{0:02d}".format(localtime[4])
        second = "{0:02d}".format(localtime[5])
        time_in_format = year + "-" + month + "-" + day + " " + hour + ":" + minute + ":" + second
        log_level = "%-6s" % log_level
        message = "{" + "\"time_stamp\"" + ":" + "\"" + time_in_format + "\"" + "," + "\n" + "\"level\"" + ":" + "\"" + log_level + "\"" + "," + "\n" + "\"message\"" + ":" + "\"" + message + "\"" + "\n" + "}" + "\n"

        if self.logInitialize == False:
            self.write2disk(self.logPath, "{\n", self.volumeLabel)
            self.write2disk(self.logPath, "\"log\": [\n", self.volumeLabel)
            self.logInitialize = True
        else:
            message = ",\n" + message

        self.logstr = self.logstr + message
        if flush:
            if self.logstr != '':
                self.write2disk(self.logPath, self.logstr, self.volumeLabel)
            self.logstr = ''
        return True

    ## Redirect message to memory buffer or filesystem in Debug level
    #
    #  This message if for developer to debug
    #
    # @param:message:      The log message
    # @param flush:        Boolean value to specify whether flush buffer to disk
    #
    # @retval:             None
    def Debug(self, message, flush = False):
        if LOG_DEBUG in self.logLevel:
            self.__Log(str(message), LOG_DEBUG, flush)


    ## Redirect message to memory buffer or filesystem in Trace level
    #
    #  Trace the API execution
    #
    # @param:message:      The log message
    # @param flush:        Boolean value to specify whether flush buffer to disk
    #
    # @retval:             None
    def Trace(self, message, flush = False):
        if LOG_TRACE in self.logLevel:
            self.__Log(str(message), LOG_TRACE, flush)

    ## Redirect message to memory buffer or filesystem in Info level
    #
    #  Use it for normal info in test script
    #
    # @param:message:      The log message
    # @param flush:        Boolean value to specify whether flush buffer to disk
    #
    # @retval:             None
    def Info(self, message, flush = False):
        if LOG_INFO in self.logLevel:
            self.__Log(str(message), LOG_INFO, flush)


    ## Redirect message to memory buffer or filesystem in Warn level
    #
    #  Use it for warn prompt, the test will be treated as warn if this message printed
    #
    # @param:message:      The log message
    # @param flush:        Boolean value to specify whether flush buffer to disk
    #
    # @retval:             None
    def Warn(self, message, flush = False):
        if LOG_WARN in self.logLevel:
            self.__Log(str(message), LOG_WARN, flush)

    ## Redirect message to memory buffer or filesystem in Fail level
    #
    #  Use it for fail prompt, the test will be treated as fail if this message printed
    #
    # @param:message:      The log message
    # @param flush:        Boolean value to specify whether flush buffer to disk
    #
    # @retval:             None
    def Fail(self, message, flush = False):
        if LOG_FAIL in self.logLevel:
            self.__Log(str(message), LOG_FAIL, flush)


    ## Redirect message to memory buffer or filesystem in Pass level
    #
    #  Use it for pass prompt, only for pass checkpoint. Only a test without warn and error will be treated as pass
    #
    # @param:message:      The log message
    # @param flush:        Boolean value to specify whether flush buffer to disk
    #
    # @retval:             None
    def Pass(self, message, flush = False):
        if LOG_PASS in self.logLevel:
            self.__Log(str(message), LOG_PASS, flush)

    ## Quit the test securely
    #
    #  The log file itself is in JSON format, this method is used to make file valid.
    #  It's required to invoke this method when Case is finished.
    #
    # @retval:             None
    def Close(self):
        self.Trace("Close")
        self.write2disk(self.logPath, "]}\n", self.volumeLabel)
        self.write2disk(self.logPath, " ", self.volumeLabel)



_ets.debug("Edk2 Test Suit initializing ...:\r\n")
