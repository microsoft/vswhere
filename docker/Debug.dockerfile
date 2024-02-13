#escape=`

# Copyright (C) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license. See LICENSE.txt in the project root for license information.

# Based on latest image cached by Azure Pipelines: https://docs.microsoft.com/azure/devops/pipelines/agents/hosted#software
FROM mcr.microsoft.com/dotnet/framework/aspnet:4.8-windowsservercore-ltsc2022
SHELL ["powershell.exe", "-ExecutionPolicy", "Bypass", "-Command"]

ENV INSTALLER_VERSION=1.14.190.31519 `
    INSTALLER_URI=https://download.visualstudio.microsoft.com/download/pr/100516681/d68d54e233c956ff79799fdf63753c54/Microsoft.VisualStudio.Setup.Configuration.msi `
    INSTALLER_HASH=8917aa7b4116e574856d43e8e62862c1d6f25512be54917f2ef95f9cac103810

# Download and register the query API
RUN $ErrorActionPreference = 'Stop' ; `
    $VerbosePreference = 'Continue' ; `
    $null = New-Item C:\TEMP -ItemType Directory -ea SilentlyContinue; `
    Invoke-WebRequest -Uri $env:INSTALLER_URI -OutFile C:\TEMP\Microsoft.VisualStudio.Setup.Configuration.msi; `
    if ((Get-FileHash -Path C:\TEMP\Microsoft.VisualStudio.Setup.Configuration.msi -Algorithm SHA256).Hash -ne $env:INSTALLER_HASH) { throw 'Download hash does not match' }; `
    Start-Process -Wait -FilePath C:\Windows\System32\msiexec.exe -ArgumentList '/i C:\TEMP\Microsoft.VisualStudio.Setup.Configuration.msi /qn /l*vx C:\TEMP\Microsoft.VisualStudio.Setup.Configuration.log'

ENTRYPOINT ["powershell.exe", "-ExecutionPolicy", "Bypass"]
CMD ["-NoExit"]

# Download and install Remote Debugger
RUN $ErrorActionPreference = 'Stop' ; `
    $ProgressPreference = 'SilentlyContinue' ; `
    $VerbosePreference = 'Continue' ; `
    New-Item -Path C:\Downloads -Type Directory | Out-Null ; `
    Invoke-WebRequest -Uri 'https://go.microsoft.com/fwlink/?LinkId=746570&clcid=0x409' -OutFile C:\Downloads\vs_remotetools.exe ; `
    Start-Process -Wait -FilePath C:\Downloads\vs_remotetools.exe -ArgumentList '-q' ; `
    Remove-Item -Path C:\Downloads\vs_remotetools.exe

# Configure Remote Debugger
EXPOSE 3702 4022 4023
RUN $ErrorActionPreference = 'Stop' ; `
    $VerbosePreference = 'Continue' ; `
    Start-Process -Wait -FilePath 'C:\Program Files\Microsoft Visual Studio 15.0\Common7\IDE\Remote Debugger\x64\msvsmon.exe' -ArgumentList '/prepcomputer', '/private', '/quiet'

HEALTHCHECK --interval=10s CMD ["powershell.exe", "-ExecutionPolicy", "Bypass", "-Command", "&{ if (Get-Process msvsmon) { exit 0 } else { exit 1 } }"]
