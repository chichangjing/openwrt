
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::  convert_slash.bat
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
rem CALL convert_slash.bat %CPSS_TOOLS_COMMON_PATH% %INPUT_NAME% %INPUT_PATH%

@ ECHO OFF

SET INPUT_NAME=%1
SET INPUT_PATH=%2
SET CPSS_TOOLS_COMMON_PATH=%3

SET CPSS_TEMP=c:\temp
IF NOT EXIST %CPSS_TEMP% mkdir %CPSS_TEMP%

SET CONVERT_PATH_SLASH=c:\temp\convert_path_slash_%INPUT_NAME%.bat

:ECHO start %INPUT_NAME% %INPUT_PATH%
python.exe %CPSS_TOOLS_COMMON_PATH%\convert_slash.py %INPUT_NAME% %INPUT_PATH% %CONVERT_PATH_SLASH% 

CALL %CONVERT_PATH_SLASH% 

ECHO end convert_slash.bat %INPUT_NAME% %INPUT_PATH%





