@echo off

REM Copyright (C) Microsoft Corporation. All rights reserved.
REM Licensed under the MIT license. See LICENSE.txt in the project root for license information.

setlocal enabledelayedexpansion

set projectDir=%~dp0
set solutionDir=%projectDir:~0,-12%

set name=vswhere/test

:parse
if "%1"=="" goto :parse_end
if /i "%1"=="-name" set name=%2& shift& shift& goto :parse
if /i "%1"=="/name" set name=%2& shift& shift& goto :parse
if /i "%1"=="-network" set network=%2& shift& shift& goto :parse
if /i "%1"=="/network" set network=%2& shift& shift& goto :parse
if "%1"=="-?" goto :help
if "%1"=="/?" goto :help
if /i "%1"=="-help" goto :help
if /i "%1"=="/help" goto :help

echo.
echo Unknown argument: %1
goto :help

:parse_end
if "%network%"=="" (
    for /f "usebackq tokens=*" %%i in (`docker network ls --filter driver^=transparent --format "{{.Name}}"`) do (
        echo Discovered transparent network: %%i
        set network=%%i
    )

    echo Using network: !network!
)

if "%network%"=="" (
    echo Error: No network selected. Cannot retrieve online resources.
    exit /b 87
)

REM Remove trailing backslash or command may fail.
set root=%projectDir:~0,-1%

@echo on
docker build --network "%network%" --tag %name% "%root%"
@if errorlevel 1 exit /b %ERRORLEVEL%

@echo off
echo.
goto :EOF

:help
echo.
echo %~nx0 [options] [-?]
echo.
echo Options:
echo -name    Image name. Defaults to vswhere/test.
echo -network External network name. Defaults to discovered transparent network.
echo -?       Displays this help message.
echo.

exit /b 87
