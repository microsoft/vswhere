Contributing
============

## Prerequisites

This project uses the following software. Newer versions may work but backward compatibility must be maintained.

* [Visual Studio 2017](https://www.visualstudio.com/downloads/) or newer

## Coding

This project uses a GitHub flow model with development and releases based on the `main` branch. You can view current build status in the [README](README.md) document.

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

If your machine supports it, you can install [Docker for Windows][docker], switch to Windows containers, and test in isolated containers for runtime behavior. You can run functional tests and runtime tests together.

```batch
tools\test.cmd
```

You can also run tests directly with `docker-compose`:

```batch
docker-compose -f docker\docker-compose.yml run test
```

### Debugging

You can use the following steps to start an environment for exploratory testing or to run and debug tests. The Visual Studio Remote Debugger will launch by default and should be discoverable on your private network.

1. Run:
   ```batch
   docker-compose -f docker\docker-compose.yml -f docker\docker-compose.debug.yml up -d

   REM Start an interactive shell
   docker-compose -f docker\docker-compose.yml -f docker\docker-compose.debug.yml exec test powershell.exe
   ```
2. Click *Debug -> Attach to Process*
3. Change *Transport* to "Remote (no authentication)"
4. Click *Find*
5. Click *Select* on the container (host name will be the container name)
6. Select "powershell" under *Available Processes*
7. Click *Attach*
8. Run any commands you like in the interactive shell, or run all tests:
   ```powershell
   Invoke-Pester C:\Tests -EnableExit
   ```
9. When finished, run:
   ```batch
   docker-compose  -f docker\docker-compose.yml -f docker\docker-compose.debug.yml down
   ```

If you know the host name (by default, the container name) or IP address (depending on your network configuration for the container), you can type it into the *Qualifier* directory along with port 4022, e.g. "172.22.0.1:4022".

## Pull Requests

We welcome pull requests for both bug fixes and new features that solve a common enough problem to benefit the community. Please note the following requirements.

1. Code changes for bug fixes and new features are accompanied by new tests or, only if required, modifications to existing tests. Modifying existing tests when not required may introduce regressions.
2. All tests must pass. We have automated PR builds that will verify any PRs before they can be merged, but you are encouraged to run all tests in your development environment prior to pushing to your remote.

Thank you for your contributions!

  [docker]: https://www.docker.com/products/overview
