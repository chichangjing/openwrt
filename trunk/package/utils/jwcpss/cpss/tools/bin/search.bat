@rem This batch file receives 3 argument:
@rem "%1" --> directory name: full or relative path.
@rem "%2" --> file name containing a list of strings, each string in different line.
@rem          the file can be full or relative path.
@rem "%3" --> file name: used for writing the outputs of the batch file. 
@rem          full or relative path.
@rem          This is optional variable default name is dump.txt to current directory.
@rem

@if "%1" == ""  goto arg_not_ok
@if "%2" == ""  goto arg_not_ok
@goto input_ok

:arg_not_ok
@echo 'usage : search <directory_name> <word_list_file> [result_file]')

:input_ok
set OUTPUT_FILE="%3%"
if "%3" == ""  set OUTPUT_FILE=dump.txt

@rem clean the output file.
@Echo "Results of search for forbidden strings in code"> %OUTPUT_FILE%

for /F "eol=# delims=" %%i in (%2) do (
@echo " " >> %OUTPUT_FILE% 
@echo " " >> %OUTPUT_FILE% 
@echo %%i >> %OUTPUT_FILE% 
grep.exe -idn- %%i "%1" >> %OUTPUT_FILE%
)

@goto:EOF
