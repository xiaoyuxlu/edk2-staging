# Micro-Python Test Framework Script Writting Guide
## __init__
Constructor
### Parameters
 * logLevel
    * The level of log. The log which level is not included will be filtered out
 * volomeLabel
    * The volume label of current disk which stores the framework
 * logpath
    * The folder to put logs. Do not specify it or built-in report featre can't be used
### Return Value
 * None
----
## GetEnv
Get the value of a certain environment variable.
### Parameters
 * variableName
    * Specify the variable to be retrieved
### Return Value
 * The value of the variable
----
## SetEnv
Set specified variable a value
### Parameters
 * variableName
    * Specify the variable to be set
 * variableValue
    * Specify the value
### Return Value
 * None
----
## SetTickTock
Set the API execution speed, the higher the parameter,
the slower the API execute
### Parameters
 * interval
    * The interval in milliseconds
### Return Value
 * None
----
## GetTickTock
Set the API execution speed, the higher the parameter,
the slower the API execute
### Parameters
 * interval
    * The interval in milliseconds
### Return Value
 * None
----
## FuncKey
Trigger function key with multiple ways
### Parameters
 * InputSpeed
    * The time interval between each key
 * times
    * The type times of key
### Return Value
 * None
----
## Input
Trigger not only funciton key but also word key
### Parameters
 * interval
    * The interval in milliseconds
 * keystr
    * The word or funciton key
### Return Value
 * None
----
## Delay
Suspend script for given value and return to normal execution flow when time is out
### Parameters
 * msecond
    * The suspend time in milliseconds
### Return Value
 * None
----
## GetScreenSize
Get the size of screen with width and height
### Parameters
### Return Value
 * (width, height)
----
## SetScreenSize
Set the size of screen with width and height
### Parameters
 * y
    * The height of screen
 * x
    * The width of screen
### Return Value
 * None
----
## GetScreen
Get text and its attribute on the screen
### Parameters
 * history
    * Boolean value to specify whether screen contains history info
### Return Value
 * Text and its attribute on the screen
----
## GetRowString
Get a string from the specific line, include spaces.
### Parameters
 * history
    * Boolean value to specify whether history data is queried or not
 * row
    * The number of row to be queried
### Return Value
 * Text and its attribute on the screen
----
## Verify
Verify whether a specified string with color appear in the current console or history buffer or not.
### Parameters
 * history
    * If False, only check the current console. Or else, check the history and current console together
 * string
    * The string to be searched.
 * ForeBackColor
    * Foreground Color [|Background Color]: Colors for the selected string name, include
### Return Value
 * True/False
----
## GetStrColor
Get both forecolor and backcolor of all matching results.  
If the string is not found, return (None, None).  
Else, the items in the return list are formatted as [‘forecolor backcolor’, ‘forecolor backcolor’, ‘forecolor backcolor’, … ].
### Parameters
 * string
    * The target
 * history
    * If False, only check the current console. Or else, check the history and current console together
### Return Value
 * A list of forecolor/background color pairs
----
## GetStrLocation
 Get the locations of all matching results.  
 If the string is not found, return None.  
 Else, the items in the return list are formatted as [‘x1 y1, ‘x2 y2, ‘x3 y3’, …]Get locations of given string
### Parameters
 * string
    * The target
 * history
    * If False, only check the current console. Or else, check the history and current console together
### Return Value
 * A list of locations
----
## WaitUntil
Wait until some specified string’s appearance.  
It returns True if the matched string appears within timeOut,  
else returns False.
### Parameters
 * history
    * If False, only check the current console. Or else, check the history and current console together
 * string
    * The target
 * timeOut
    * Integer type. Maximum seconds to wait. Default value is 10 seconds
 * ForeBackColor
    * String type. The forecolor or backcolor of the string.
### Return Value
 * The string appears or not, True if appears
----
## WaitVanish
Wait until some specified string’s disappearance.  
It returns True if the matched string disappears within timeOut,  
else returns False.
### Parameters
 * history
    * If False, only check the current console. Or else, check the history and current console together
 * string
    * The target
 * timeOut
    * Integer type. Maximum seconds to wait. Default value is 10 seconds
 * ForeBackColor
    * String type. The forecolor or backcolor of the string.
### Return Value
 * The string disappears or not, True if disappears
----
## SelectOption
Find the specified option from EDK2 HII page
### Parameters
 * operation
    * The option string, suchs as mptf.LEFT, mptf.RIGHT, mptf.DOWN, mptf.UP
 * maxNum
    * The max number of do the operation
 * string
    * The option string
 * ForeBackColor
    * Foreground Color [|Background Color]: Colors for the selected string name, include
### Return Value
 * True/False
----
## write2disk
Write message to disk specified by volume label
### Parameters
 * message
    * The log message
 * volumeLabel
    * The volume label of the disk where the log stores
 * logPath
    * The path to log folder
----
## __Log
Redirect message to memory buffer or filesystem
### Parameters
 * message
    * The log message
 * log_level
    * Filter log based on it and indicate the test result
 * flush
    * Dump messages in memory buffer to filesystem if True. Or, dump message to memory buffer
----
## Debug
Redirect message to memory buffer or filesystem in Debug level
### Parameters
 * message
    * The log message
 * flush
    * Boolean value to specify whether flush buffer to disk
### Return Value
 * None
----
## Trace
Redirect message to memory buffer or filesystem in Trace level
### Parameters
 * message
    * The log message
 * flush
    * Boolean value to specify whether flush buffer to disk
### Return Value
 * None
----
## Info
Redirect message to memory buffer or filesystem in Info level
### Parameters
 * message
    * The log message
 * flush
    * Boolean value to specify whether flush buffer to disk
### Return Value
 * None
----
## Warn
Redirect message to memory buffer or filesystem in Warn level
### Parameters
 * message
    * The log message
 * flush
    * Boolean value to specify whether flush buffer to disk
### Return Value
 * None
----
## Fail
Redirect message to memory buffer or filesystem in Fail level
### Parameters
 * message
    * The log message
 * flush
    * Boolean value to specify whether flush buffer to disk
### Return Value
 * None
----
## Pass
Redirect message to memory buffer or filesystem in Pass level
### Parameters
 * message
    * The log message
 * flush
    * Boolean value to specify whether flush buffer to disk
### Return Value
 * None
----
## Close
Quit the test securely  
The log file itself is in JSON format, this method is used to make file valid.  
It's required to invoke this method when Case is finished.
### Parameters
### Return Value
 * None
----
