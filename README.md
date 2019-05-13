Visual Studio Locator
=====================

![build status: master](https://devdiv.visualstudio.com/DevDiv/_apis/build/status/Setup/Setup-vswhere-CI?branchName=master&label=master)
![build status: develop](https://devdiv.visualstudio.com/DevDiv/_apis/build/status/Setup/Setup-vswhere-CI?branchName=develop&label=develop)
[![github release](https://img.shields.io/github/release/Microsoft/vswhere.svg?logo=github&logoColor=white)](https://github.com/Microsoft/vswhere/releases/latest)
[![github releases: all](https://img.shields.io/github/downloads/Microsoft/vswhere/total.svg?logo=github&logoColor=white&label=github)](https://github.com/Microsoft/vswhere/releases)
[![nuget: all](https://img.shields.io/nuget/dt/vswhere.svg?logo=nuget&logoColor=white&label=nuget)](https://nuget.org/packages/vswhere)
[![chocolatey: all](https://img.shields.io/chocolatey/dt/vswhere.svg?label=chocolatey)](https://chocolatey.org/packages/vswhere)

Over the years Visual Studio could be discovered using registry keys, but with recent changes to the deployment and extensibility models a new method is needed to discover possibly more than once installed instance. These changes facilitate a smaller, faster default install complimented by on-demand install of other workloads and components.

_vswhere_ is designed to be a redistributable, single-file executable that can be used in build or deployment scripts to find where Visual Studio - or other products in the Visual Studio family - is located. For example, if you know the relative path to MSBuild, you can find the root of the Visual Studio install and combine the paths to find what you need.

You can emit different formats for information based on what your scripts can consume, including plain text, JSON, and XML. Pull requests may be accepted for other common formats as well.

_vswhere_ is included with the installer as of Visual Studio 2017 version 15.2 and later, and can be found at the following location: `%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe`. The binary may be copied from that location as needed, [installed using Chocolatey](https://chocolatey.org/packages/vswhere), or the latest version may be [downloaded from the releases page](https://github.com/Microsoft/vswhere/releases). More information about how to get _vswhere_ is [on the wiki](https://github.com/Microsoft/vswhere/wiki/Installing). 

## Example

If you wanted to find MSBuild - now installed under the Visual Studio 2017 and newer installation root - you could script a command like the following to run the latest version of MSBuild installed. This example uses the new `-find` parameter in our [latest release](https://github.com/Microsoft/vswhere/releases/latest) that searches selected instances for matching file name patterns. You can tailor what instances you select with parameters like `-version` or `-prerelease` to find specific versions you support, optionally including prereleases.

```batch
@echo off
setlocal enabledelayedexpansion

for /f "usebackq tokens=*" %%i in (`vswhere -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
  "%%i" %*
  exit /b !errorlevel!
)
```

You can find more [examples](https://github.com/Microsoft/vswhere/wiki/Examples) in our wiki.

## Feedback

To file issues or suggestions, please use the [Issues](https://github.com/Microsoft/vswhere/issues) page for this project on GitHub.

## License

This project is licensed under the [MIT license](LICENSE.txt).

## Code of Conduct

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
