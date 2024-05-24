

ECHO ON
ECHO  a simple batch file that if put in the output directory can install and
ECHO  start the service.

set BATCH_DIR=%~dp0
set SVC_PATH=%BATCH_DIR%CopyWSRSvc.exe

echo Installing service from %SVC_PATH%

sc create "CopyWSRSvc" binPath=%SVC_PATH%

echo remove autostart
sc config "CopyWSRSvc" start= demand

pause

