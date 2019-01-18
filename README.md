Visual Studio Locator
=====================

![build status: master](https://devdiv.visualstudio.com/DevDiv/_apis/build/status/Setup/Setup-vswhere-CI?branchName=master&label=master)
![build status: develop](https://devdiv.visualstudio.com/DevDiv/_apis/build/status/Setup/Setup-vswhere-CI?branchName=develop&label=develop)
[![github release](https://img.shields.io/github/release/Microsoft/vswhere.svg?logo=github)](https://github.com/Microsoft/vswhere/releases/latest)
[![github releases: all](https://img.shields.io/github/downloads/Microsoft/vswhere/total.svg?logo=github&label=github)](https://github.com/Microsoft/vswhere/releases)
[![chocolatey: all](https://img.shields.io/chocolatey/dt/vswhere.svg)](https://chocolatey.org/packages/vswhere)
[![nuget: all](https://img.shields.io/nuget/dt/vswhere.svg?label=nuget)](https://nuget.org/packages/vswhere)

Over the years Visual Studio could be discovered using registry keys, but with recent changes to the deployment and extensibility models a new method is needed to discover possibly more than once installed instance. These changes facilitate a smaller, faster default install complimented by on-demand install of other workloads and components.

_vswhere_ is designed to be a redistributable, single-file executable that can be used in build or deployment scripts to find where Visual Studio - or other products in the Visual Studio family - is located. For example, if you know the relative path to MSBuild, you can find the root of the Visual Studio install and combine the paths to find what you need.

You can emit different formats for information based on what your scripts can consume, including plain text, JSON, and XML. Pull requests may be accepted for other common formats as well.

_vswhere_ is included with the installer as of Visual Studio 2017 version 15.2 and later, and can be found at the following location: `%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe`. The binary may be copied from that location as needed, [installed using Chocolatey](https://chocolatey.org/packages/vswhere), or the latest version may be [downloaded from the releases page](https://github.com/Microsoft/vswhere/releases). More information about how to get _vswhere_ is [on the wiki](https://github.com/Microsoft/vswhere/wiki/Installing). 

## Example

If you wanted to find MSBuild - now installed under the Visual Studio 2017 and newer installation root - you could script a command like the following to find the latest version installed.

```batch
@echo off

for /f "usebackq tokens=1* delims=: " %%i in (`vswhere -latest -requires Microsoft.Component.MSBuild`) do (
  if /i "%%i"=="installationPath" set InstallDir=%%j
)

if exist "%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" (
  "%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" %*
)
```

You can find more [examples](https://github.com/Microsoft/vswhere/wiki/Examples) in our wiki.

## Feedback

To file issues or suggestions, please use the [Issues](https://github.com/Microsoft/vswhere/issues) page for this project on GitHub.

## License

This project is licensed under the [MIT license](LICENSE.txt).