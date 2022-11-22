# Copyright (C) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license. See LICENSE.txt in the project root for license information.

[CmdletBinding()]
param (
    [Parameter()]
    [ValidateNotNullOrEmpty()]
    [string] $Configuration = $env:CONFIGURATION,

    [Parameter()]
    [ValidateNotNullOrEmpty()]
    [string] $Platform = $env:PLATFORM,

    [Parameter()]
    [ValidateSet('Unit', 'Integration', 'Functional', 'Runtime')]
    [string[]] $Type = @('Functional', 'Runtime'),

    [Parameter()]
    [switch] $Download
)

if (-not $Configuration) {
    $Configuration = 'Debug'
}

if (-not $Platform) {
    $Platform = 'x86'
}

[bool] $Failed = $false

if ($Type -contains 'Unit' -or $Type -contains 'Functional')
{
    # Find vstest.console.exe.
    $cmd = get-command vstest.console.exe -ea SilentlyContinue | select-object -expand Path
    if (-not $cmd) {
        $vswhere = get-childitem "$PSScriptRoot\..\bin\*" -filter vswhere.exe -recurse | select-object -first 1 -expand FullName
        if (-not $vswhere) {
            write-error 'Please build vswhere before running tests.'
            exit 1
        }

        $path = & $vswhere -latest -requires Microsoft.VisualStudio.Component.ManagedDesktop.Core -property installationPath
        if (-not $path) {
            write-error 'No instance of Visual Studio found with vstest.console.exe. Please start a developer command prompt.'
            exit 1
        }

        $cmd = join-path $path 'Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe'
    }

    if (-not (test-path $cmd)) {
        write-error 'Could not find vstest.console.exe. Please start a developer command prompt.'
        exit 1
    }

    # Discover test containers for the current configuration.
    $containers = get-childitem "bin\$Configuration" -include *.test.dll -recurse | foreach-object {
        [string] $path = $_.FullName

        write-verbose "Discovered test assembly '$path'."
        "$path"
    }

    # Run functional tests.
    & $cmd $logger $containers /parallel /platform:$Platform
    if (-not $?) {
        $Failed = $true
    }
}

if ($Type -contains 'Integration' -or $Type -contains 'Runtime')
{
    # Run docker runtime tests.
    $cmd = (get-command docker-compose -ea SilentlyContinue).Path
    if (-not $cmd -and $Download) {
        invoke-webrequest 'https://github.com/docker/compose/releases/download/1.11.2/docker-compose-Windows-x86_64.exe' -outfile "${env:TEMP}\docker-compose.exe"
        $cmd = "${env:TEMP}\docker-compose.exe"
    }

    if ($cmd) {
        [string] $path = resolve-path "$PSScriptRoot\..\docker\docker-compose.yml"

        $verbose = if ($VerbosePreference -eq 'Continue') {
            '--verbose'
        }

        write-verbose "Running tests in '$path'"
        & $cmd -f "$path" $verbose run --rm test
        if (-not $?) {
            $Failed = $true
        }
    } else {
        write-warning 'Failed to find docker-compose; integration tests will not be performed.'
    }

    if ($Failed) {
        exit 1
    }
}
