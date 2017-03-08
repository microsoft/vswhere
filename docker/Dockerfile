# Copyright (C) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license. See LICENSE.txt in the project root for license information.

# Require .NET Framework
FROM microsoft/windowsservercore

# Download and install Remote Debugger
SHELL ["powershell.exe", "-ExecutionPolicy", "Bypass", "-Command"]
RUN $ErrorActionPreference = 'Stop' ; \
    $VerbosePreference = 'Continue' ; \
    New-Item -Path C:\Downloads -Type Directory | Out-Null ; \
    Invoke-WebRequest -Uri 'https://go.microsoft.com/fwlink/?LinkId=615470&clcid=0x409' -OutFile C:\Downloads\rtools_setup_x64.exe ; \
    Start-Process -Wait -FilePath C:\Downloads\rtools_setup_x64.exe -ArgumentList '-q'

# Configure Remote Debugger
EXPOSE 3702 4020 4021
RUN $ErrorActionPreference = 'Stop' ; \
    $VerbosePreference = 'Continue' ; \
    Start-Process -Wait -FilePath 'C:\Program Files\Microsoft Visual Studio 14.0\Common7\IDE\Remote Debugger\x64\msvsmon.exe' -ArgumentList '/prepcomputer', '/private', '/quiet'

# Download and register current query APIs
ENV API_VERSION="1.8.24"
RUN $ErrorActionPreference = 'Stop' ; \
    $VerbosePreference = 'Continue' ; \
    Invoke-WebRequest -Uri "https://www.nuget.org/api/v2/package/Microsoft.VisualStudio.Setup.Configuration.Native/${env:API_VERSION}" -OutFile C:\Downloads\Microsoft.VisualStudio.Setup.Configuration.Native.zip ; \
    Expand-Archive -Path C:\Downloads\Microsoft.VisualStudio.Setup.Configuration.Native.zip -DestinationPath C:\Downloads\Microsoft.VisualStudio.Setup.Configuration.Native ; \
    C:\Windows\System32\regsvr32.exe /s C:\Downloads\Microsoft.VisualStudio.Setup.Configuration.Native\tools\x64\Microsoft.VisualStudio.Setup.Configuration.Native.dll ; \
    C:\Windows\SysWOW64\regsvr32.exe /s C:\Downloads\Microsoft.VisualStudio.Setup.Configuration.Native\tools\x86\Microsoft.VisualStudio.Setup.Configuration.Native.dll

# Install latest version of Pester for integration testing
RUN $ErrorActionPreference = 'Stop' ; \
    $VerbosePreference = 'Continue' ; \
    Install-PackageProvider -Name nuget -MinimumVersion 2.8.5.201 -Force ; \
    Install-Module -Name Pester -Scope CurrentUser -SkipPublisherCheck -Force

# Start Remote Debugger if no other command is passed to PowerShell
ENTRYPOINT ["powershell.exe", "-ExecutionPolicy", "Unrestricted"]
CMD ["-NoExit", "-Command", "& 'C:\\Program Files\\Microsoft Visual Studio 14.0\\Common7\\IDE\\Remote Debugger\\x64\\msvsmon.exe' /silent /noauth /anyuser"]
