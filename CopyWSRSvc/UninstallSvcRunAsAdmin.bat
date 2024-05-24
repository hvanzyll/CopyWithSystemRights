
REM - a simple batch file that if put in the output directory can uninstall the service.
REM - You can right click on this file in Windows Explorer and select Run as Administrator
REM	  and it should uninstall the service.
sc stop "CopyWSRSvc"
sc delete "CopyWSRSvc" 
pause

