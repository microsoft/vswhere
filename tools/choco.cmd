@if not defined _echo echo off
setlocal enabledelayedexpansion

for /d %%i in ("%~dp0..\packages\chocolatey*") do (
    for /f "usebackq delims=" %%j in (`where /r "%%i" choco.exe 2^>nul`) do (
        "%%j" %*
        exit /b !errorlevel!
    )
)
