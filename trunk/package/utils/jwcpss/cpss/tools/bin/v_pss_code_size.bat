@rem This batch file receive 3 argument:
@rem "%1" --> Full path to directory where unzipped release should be located.
@rem "%2" --> YES/NO run unzip or not (default NO).
@rem "%3" --> if %2 is YES this argument specifies the full path to release zip file

::Set environment variables

set WORK_DIRECTORY=%1%\tmp
mkdir %WORK_DIRECTORY%
copy X:\cpss\SW\prestera\tools\bin\code_size.SPT %WORK_DIRECTORY%
copy X:\cpss\SW\prestera\tools\bin\AWK.EXE %WORK_DIRECTORY%
set ZIP_FULL_PATH=%3   
set TMP_DIRECTORY=%1
@rem set the path to sizeppc.exe in tornado.
set path=C:\Tornado_2_1\host\x86-win32\bin;%path%

if "%2"=="YES" goto runUnzip

goto processing

:runUnzip
  if %ZIP_FULL_PATH%=="" GOTO end
  "C:\Program Files\WinZip\WZUNZIP.EXE"  -d -o   %ZIP_FULL_PATH%  %TMP_DIRECTORY% 
  If ErrorLevel 1 Goto unziperror	
  GOTO processing


:processing

 ECHO CPSS CODE SIZE > %TMP_DIRECTORY%\doc\code_size.txt

:CPSS_COMMON
 SET V1=common\libs
 SET V2=common.o
 SET V3=CPSS_COMMON
 SET RETBACK=CPSS_PP
 GOTO resultgeneration

:CPSS_PP
 SET V1=mainPpDrv\libs
 SET V2=cpss
 SET V3=CPSS_PP
 SET RETBACK=CPSS_FA
 GOTO resultgeneration


:CPSS_FA
 SET V1=mainFaDrv\libs
 SET V2=mainFaDrv.o
 SET V3=CPSS_FA
 SET RETBACK=CPSS_XBAR
 GOTO resultgeneration

:CPSS_XBAR
 SET V1=mainXbarDrv\libs
 SET V2=mainXbarDrv.o
 SET V3=CPSS_XBAR
 SET RETBACK=CPSS_FX
 GOTO resultgeneration

:CPSS_FX
 SET V1=mainFxDrv\libs
 SET V2=mainFxDrv.o
 SET V3=CPSS_FX
 SET RETBACK=CPSS_UTF
 GOTO resultgeneration

:CPSS_UTF
 SET V1=mainUT\libs
 SET V2=.lib
 SET V3=CPSS_UTF
 SET RETBACK=workfileerase
 GOTO resultgeneration


:resultgeneration
 ECHO working step step1  > %WORK_DIRECTORY%\step1.txt
 ECHO working step step2  > %WORK_DIRECTORY%\step2.txt
 ECHO script arguments %V2% >> %WORK_DIRECTORY%\step1.txt
 ECHO script arguments %V3% >> %WORK_DIRECTORY%\step2.txt
 dir %TMP_DIRECTORY%\%V1% /s  >> %WORK_DIRECTORY%\step1.txt
 %WORK_DIRECTORY%\AWK.EXE  -f %WORK_DIRECTORY%\code_size.spt %WORK_DIRECTORY%\step1.txt > %WORK_DIRECTORY%\step1.bat
 call %WORK_DIRECTORY%\step1.bat >> %WORK_DIRECTORY%\step2.txt
 %WORK_DIRECTORY%\AWK.EXE  -f %WORK_DIRECTORY%\code_size.spt %WORK_DIRECTORY%\step2.txt >> %TMP_DIRECTORY%\doc\code_size.txt
 GOTO %RETBACK%


:workfileerase
: erase %WORK_DIRECTORY%\step1.txt
: erase %WORK_DIRECTORY%\step1.bat
: erase %WORK_DIRECTORY%\step2.txt
:rmdir /Q /S %WORK_DIRECTORY%
 GOTO success

:unziperror
 @echo. ***********************************************
 @echo  ** FAILED : Zip file not specified correctly **
 @echo. ***********************************************
 pause
 @GOTO end 

:success
 @echo. ***********************************************
 @echo. **        SUCCESS CPSS code size done        **
 @echo. ***********************************************
 @GOTO end

:end

