@echo off

REM Copyright (C) Microsoft Corporation. All rights reserved.
REM Licensed under the MIT license. See LICENSE.txt in the project root for license information.

setlocal

if "%script%"=="" set script=%~nx0

set projectDir=%~dp0
set solutionDir=%projectDir:~0,-7%

set configuration=Debug
set name=vswhere/test
set mode=-it

:parse
if "%1"=="" goto :parse_end
if not "%args%"=="" set args=%args% %1& shift& goto :parse
if /i "%1"=="-name" set name=%2& shift& shift& goto :parse
if /i "%1"=="/name" set name=%2& shift& shift& goto :parse
if /i "%1"=="-configuration" set configuration=%2& shift& shift& goto :parse
if /i "%1"=="/configuration" set configuration=%2& shift& shift& goto :parse
if /i "%1"=="-detach" set mode=-d& shift& goto :parse
if /i "%1"=="/detach" set mode=-d& shift& goto :parse
if /i "%1"=="-on" set id=%2& shift& shift& goto :parse
if /i "%1"=="/on" set id=%2& shift& shift& goto :parse
if /i "%1"=="-network" set params=%params% --network "%2"& shift& shift& goto :parse
if /i "%1"=="/network" set params=%params% --network "%2"& shift& shift& goto :parse
if /i "%1"=="-keep" set keep=1& shift& goto :parse
if /i "%1"=="/keep" set keep=1& shift& goto :parse
if "%1"=="-?" goto :help
if "%1"=="/?" goto :help
if /i "%1"=="-help" goto :help
if /i "%1"=="/help" goto :help
if "%1"=="--" set args=%2& shift& shift& goto :parse

echo.
echo Unknown argument: %1
goto :help

:parse_end
if "%keep%"=="" set params=%params% --rm

set outputPath=%solutionDir%bin\%configuration%
set volumes=-v %outputPath%:C:\bin
set volumes=%volumes% -v "%projectDir%Instances:C:\ProgramData\Microsoft\VisualStudio\Packages\_Instances:ro"
set volumes=%volumes% -v C:\VS\Community
set volumes=%volumes% -v C:\VS\Professional
set volumes=%volumes% -v C:\VS\Enterprise
set volumes=%volumes% -v C:\BuildTools
set volumes=%volumes% -v "%projectDir%Tests:C:\Tests"

if "%id%"=="" (
    REM Uses the ENTRYPOINT declaration in the Dockerfile
    set cmd=docker run %mode% %volumes%%params% %name% %args%
) else (
    REM Keep in sync with the ENTRYPOINT in the Dockerfile
    set cmd=docker exec %mode% %id% powershell.exe -ExecutionPolicy Unrestricted %args%
)

echo %cmd%
call %cmd%
@if errorlevel 1 exit /b %ERRORLEVEL%

echo.
goto :EOF

:help
set usage=%script% [options] [-?]
if "%noargs%"=="" (
    set usage=%usage% [-- args]
)
echo.
echo %usage%
echo.
echo Options:
echo -name value           Image name. Defaults to vswhere/test.
echo -configuration value  The build configuration to map. Defaults to Debug.
echo -detach               Detach from the container and show the ID.
echo -on value             Run command on specified container ID.
echo -network value        External network name. Defaults to discovered transparent network.
echo -keep                 Do not delete the container after exiting.
echo -?                    Displays this help message.
echo.
if "%noargs%"=="" (
echo Arguments:
echo --             Any arguments after -- are passed to the container entry point.
echo.
)

exit /b 87
