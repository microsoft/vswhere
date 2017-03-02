Visual Studio Locator
=====================

Over the years Visual Studio could be discovered using registry keys, but with recent changes to the deployment and extensibility models a new method is needed to discover possibly more than once installed instance. These changes facilitate a smaller, faster default install complimented by on-demand install of other workloads and components.

_vswhere_ is designed to be a redistributable, single-file executable that can be used in build or deployment scripts to find where Visual Studio - or other products in the Visual Studio family - is located. For example, if you know the relative path to MSBuild, you can find the root of the Visual Studio install and combine the paths to find what you need.

You can emit different formats for information based on what your scripts can consume, including plain text, JSON, and XML. Pull requests may be accepted for other common formats as well.

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

## Status

This project uses a Git flow model releasing from the `master` branch with development based on and stabilized in the `develop` branch.

Branch  | AppVeyor | VSTS
------  | ------ | -------
master  | [![build status: master](https://ci.appveyor.com/api/projects/status/yy3g7rggm2sx4nam/branch/master?svg=true)](https://ci.appveyor.com/project/heaths/vswhere/branch/master) | ![build status: master](https://devdiv.visualstudio.com/_apis/public/build/definitions/0bdbc590-a062-4c3f-b0f6-9383f67865ee/5581/badge)
develop | [![build status: develop](https://ci.appveyor.com/api/projects/status/yy3g7rggm2sx4nam/branch/develop?svg=true)](https://ci.appveyor.com/project/heaths/vswhere/branch/develop)
