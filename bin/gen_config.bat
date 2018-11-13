
cd Debug
set EXCEL_PATH=../datacfg/

start genconfig_d.exe %EXCEL_PATH%

::利用ping实现延迟处理
@echo off
:loop
echo %time%
ping 1 -n 3 -w 1000 2>nul 1>nul
echo %time%

cd ..

set CPP_SRC_PATH=datacfg\cpp
set CPP_DEST_PATH=..\src\baselib\message\ 
xcopy %CPP_SRC_PATH%\*.* %CPP_DEST_PATH% /S /F /R /Y

pause