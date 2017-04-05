@ ECHO OFF
IF EXIST %1 (
    ECHO rd /s /q %1
    rd /s /q %1
)
ECHO mkdir %1
mkdir %1
