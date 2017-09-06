Contributing
============

## Prerequisites

This project uses the following software. Newer versions may work but backward compatibility must be maintained.

* [Visual Studio 2017](https://www.visualstudio.com/downloads/) or newer

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
packages\xunit.runner.console.<version>\tools\xunit.runner.console test\VSSetup.PowerShell.Test\bin\Debug\Microsoft.VisualStudio.Setup.PowerShell.Test.dll
```

If your machine supports it, you can install [Docker for Windows][docker], switch to Windows containers, and test in isolated containers for runtime behavior. You can run unit tests and integration tests together.

```batch
tools\test.cmd
```

For a faster development process, you can run `docker-compose run test` from the _docker_ directory to start an interactive PowerShell session in a container running the Visual Studio Remote Debugger. The build output and test scripts are mounted into the container. If you rebuild or modify the tests the container is automatically updated. You can also start the container detached and run tests faster without having to restart the container.

```batch
cd docker
docker-compose up -d

REM Repeat following command as often as desired.
docker-compose exec test powershell.exe -c invoke-pester c:\tests -enableexit

docker-compose stop
```

### Debugging

You can run `docker-compose up -d` from the _docker_ directory to start an interactive shell for exploratory testing. If no other commands are passed when starting the container, the Visual Studio Remote Debugger will launch by default. Remote debugging services are discoverable on your private network.

1. Click *Debug -> Attach to Process*
2. Change *Transport* to "Remote (no authentication)"
3. Click *Find*
4. Click *Select* on the container (host name will be the container name)
5. Select "powershell" under *Available Processes*
6. Click *Attach*

If you know the host name (by default, the container name) or IP address (depending on your network configuration for the container), you can type it into the *Qualifier* directory along with port 4022, e.g. "172.22.0.1:4022".

## Pull Requests

We welcome pull requests for both bug fixes and new features that solve a common enough problem to benefit the community. Please note the following requirements.

1. Code changes for bug fixes and new features are accompanied by new tests or, only if required, modifications to existing tests. Modifying existing tests when not required may introduce regressions.
2. All tests must pass. We have automated PR builds that will verify any PRs before they can be merged, but you are encouraged to run all tests in your development environment prior to pushing to your remote.

Thank you for your contributions!

  [docker]: https://www.docker.com/products/overview
