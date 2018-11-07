
cd Debug
set EXCEL_PATH=../datacfg/

start genconfig_d.exe %EXCEL_PATH%

cd ..

set CPP_SRC_PATH=datacfg\cpp
set CPP_DEST_PATH=..\src\lib\message\ 
xcopy %CPP_SRC_PATH%\*.* %CPP_DEST_PATH% /S /F /R /Y

pause