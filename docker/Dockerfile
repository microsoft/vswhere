# Copyright (C) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license. See LICENSE.txt in the project root for license information.

FROM heaths/vssetup:1.11.2290
SHELL ["powershell.exe", "-ExecutionPolicy", "Bypass", "-Command"]

# Download and install Remote Debugger
RUN $ErrorActionPreference = 'Stop' ; \
    $ProgressPreference = 'SilentlyContinue' ; \
    $VerbosePreference = 'Continue' ; \
    New-Item -Path C:\Downloads -Type Directory | Out-Null ; \
    Invoke-WebRequest -Uri 'https://go.microsoft.com/fwlink/?LinkId=746570&clcid=0x409' -OutFile C:\Downloads\vs_remotetools.exe ; \
    Start-Process -Wait -FilePath C:\Downloads\vs_remotetools.exe -ArgumentList '-q' ; \
    Remove-Item -Path C:\Downloads\vs_remotetools.exe

# Configure Remote Debugger
EXPOSE 3702 4022 4023
RUN $ErrorActionPreference = 'Stop' ; \
    $VerbosePreference = 'Continue' ; \
    Start-Process -Wait -FilePath 'C:\Program Files\Microsoft Visual Studio 15.0\Common7\IDE\Remote Debugger\x64\msvsmon.exe' -ArgumentList '/prepcomputer', '/private', '/quiet'

# Start Remote Debugger if no other command is passed to PowerShell
ENTRYPOINT ["powershell.exe", "-ExecutionPolicy", "Unrestricted"]
CMD ["-NoExit", "-Command", "& 'C:\\Program Files\\Microsoft Visual Studio 15.0\\Common7\\IDE\\Remote Debugger\\x64\\msvsmon.exe' /silent /noauth /anyuser"]
