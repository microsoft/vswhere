@if not defined _echo echo off

REM Copyright (C) Microsoft Corporation. All rights reserved.
REM Licensed under the MIT license. See LICENSE.txt in the project root for license information.

powershell.exe -NoLogo -ExecutionPolicy Bypass -Command "%~dp0\test.ps1" %*
