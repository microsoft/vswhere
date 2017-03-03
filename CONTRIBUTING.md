Contributing
============

## Prerequisites

This project uses the following software. Newer versions may work but backward compatibility must be maintained.

* [Visual Studio 2015](https://www.visualstudio.com/en-us/downloads/visual-studio-2015-downloads-vs.aspx)

### Optional

Some projects require optional software to open or otherwise use in Visual Studio. They are not required to build the solution using MSBuild.

* [NuProj Package Project](https://marketplace.visualstudio.com/items?itemName=NuProjTeam.NuGetPackageProject)

## Coding

This project uses a Git flow model releasing from the `master` branch with development based on and stabilize in the `develop` branch. You can view current build status in the [README](README.md) document.

Please follow project code styles including,

* All requires headers are in the precompiled headers, _stdafx.h_.
* Copyright header including proper file name at top of all non-generated source.
* Tabs are 4 spaces.

In general, any new code should be stylistically indistinguishable from existing code.

## Building

Before you can build this project from the command line with MSBuild or within Visual Studio, you must restore packages.

* In Visual Studio, make sure Package Restore is enabled.
* On the command line and assuming _nuget.exe_ is in your `PATH`, in the solution directory run: `nuget restore`

Note again that to build the full solution in Visual Studio some optional software may be required.

## Testing

All available tests are discovered after a complete build in Test Explorer within Visual Studio.

On the command line, you can run the following commands from the solution directory. Replace `<version>` with whatever version was downloaded.

```batch
nuget install xunit.runner.console -outputdirectory packages
packages\xunit.runner.console.<version>\tools\xunit.runner.console bin\Debug\vswhere.test.dll
```

It's also recommended that, if your machine supports it, you install [Docker for Windows](https://www.docker.com/products/overview), switch to Windows containers, and test in isolated containers for runtime behavior.

```batch
REM You only need to build once unless updating the Dockerfile or files it copies.
docker\build

REM This will automatically map build output. Defaults to Debug configuration. Pass -? for options.
docker\test
```

For a faster development process, you can run `docker\run -detach`, copy the container ID printed to the window, then subsequently run `docker\test -on <container id>` replacing `<container id>` with the container ID you copied previously. You can make changes to the test data and even rebuild the project and run this command again as frequently as you need. This is especially handy for quick turn around when debugging and fixing a problem.

To stop the container, run `docker stop <container id>`. If you did not pass `-keep` when you started the container it will be removed automatically.

### Debugging

You can also run `docker\run.cmd` to start an interactive shell for exploratory testing. If no other commands are passed when starting the container, the Visual Studio Remote Debugger will launch by default. Remote debugging services are discoverable on your private network.

You can configure your project to start a remote command on a machine name that matches the short container ID returned if you ran `docker\run -detach`, or that you can discover by running `docker ps` in a separate console.

## Pull Requests

We welcome pull requests for both bug fixes and new features that solve a common enough problem to benefit the community. Please note the following requirements.

1. Code changes for bug fixes and new features are accompanied by new tests or, only if required, modifications to existing tests. Modifying existing tests when not required may introduce regressions.
2. All tests must pass. We have automated PR builds that will verify any PRs before they can be merged, but you are encouraged to run all tests in your development environment prior to pushing to your remote.

Thank you for your contributions!
