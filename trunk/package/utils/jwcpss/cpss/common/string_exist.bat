::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::  string_exist.bat
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
@ ECHO OFF

:SET SUB_STRING=%1
:SET PATH_STRING=%2
:
:SET PATH=c:\Program Files (x86)\Git\bin;C:\Python27;%PATH%.
:echo PATH    : %PATH%


:echo
SET PATH=c:\Program Files (x86)\Git\bin;C:\Python27;%PATH%.
:echo PATH    : %PATH%
:echo

python.exe %CPSS_PATH%\tools\common\string_exist.py "c:\Program Files (x86)\Git\bin" "%PATH%"
CALL c:\temp\add_2_path.bat
:echo PATH    : %PATH%
:echo

python.exe %CPSS_PATH%\tools\common\string_exist.py "C:\Python27" "%PATH%"
CALL c:\temp\add_2_path.bat

echo PATH    : %PATH%



: ------------------------------------------------------------
: ------------------------------------------------------------
