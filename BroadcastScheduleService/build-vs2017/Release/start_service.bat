@echo off
setlocal

rem ===== Edit these values before starting the service =====
set LOCATION_ID=1
set DB_HOST=127.0.0.1
set DB_PORT=3306
set DB_USER=pa_service
set PA_SCHEDULER_DB_PASSWORD=change_me
set DB_NAME=pa
set POLL_SECONDS=2
set PA_HOST=127.0.0.1
set PA_PORT=5000
set PA_SERVER_ID=1
set PA_CONSOLE_ID=54
set PA_VERSION=00001
set PA_LINE_ID=000
set PA_STATION_ID=      

"%~dp0BroadcastScheduleService.exe" ^
  --location-id %LOCATION_ID% ^
  --db-host %DB_HOST% --db-port %DB_PORT% --db-user %DB_USER% --db-name %DB_NAME% ^
  --poll-seconds %POLL_SECONDS% ^
  --pa-host %PA_HOST% --pa-port %PA_PORT% ^
  --pa-server-id %PA_SERVER_ID% --pa-console-id %PA_CONSOLE_ID% ^
  --pa-version %PA_VERSION% --pa-line-id %PA_LINE_ID% --pa-station-id "%PA_STATION_ID%"

pause
