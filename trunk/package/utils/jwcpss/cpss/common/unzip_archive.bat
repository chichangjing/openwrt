@ ECHO OFF
SET UNZIP_PATH=%3
IF NOT EXIST %1 (
    ECHO NOT EXIST %1
) else (
    ECHO EXIST %1
    rem dir %UNZIP_PATH%
    rem CD %2
    rem %UNZIP_PATH%\unzip.exe -o %1
)

