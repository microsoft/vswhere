# Copyright (C) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license. See LICENSE.txt in the project root for license information.

Describe 'vswhere -legacy' {
    BeforeAll {
        # Always write to 32-bit registry key.
        $key = New-Item -Path Registry::HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\VisualStudio\Setup\Reboot -Force
        $null = $key | New-ItemProperty -Name 3 -Value 1 -Force
    }

    BeforeEach {
        # Make sure localized values are returned consistently across machines.
        $enu = [System.Globalization.CultureInfo]::GetCultureInfo('en-US')

        [System.Globalization.CultureInfo]::CurrentCulture = $enu
        [System.Globalization.CultureInfo]::CurrentUICulture = $enu
    }

    AfterEach {
        # Make sure the registry is cleaned up.
        Remove-Item HKLM:\Software\WOW6432Node\Microsoft\VisualStudio\SxS\VS7 -Force -ErrorAction 'SilentlyContinue'
    }

    Context 'no legacy' {
        It 'returns 2 instances' {
            $instances = C:\bin\vswhere.exe -legacy -format json | ConvertFrom-Json
            $instances.Count | Should Be 2
        }
    }

    Context 'has legacy' {
        BeforeEach {
             New-Item HKLM:\Software\WOW6432Node\Microsoft\VisualStudio\SxS\VS7 -Force | ForEach-Object {
                 foreach ($version in '10.0', '14.0') {
                     $_ | New-ItemProperty -Name $version -Value "C:\VisualStudio\$version" -Force
                 }
             }
        }

        It 'returns 4 instances' {
            $instances = C:\bin\vswhere.exe -legacy -format json | ConvertFrom-Json
            $instances.Count | Should Be 4
        }

        It '-version "10.0" returns 4 instances' {
            $instances = C:\bin\vswhere.exe -legacy -version '10.0' -format json | ConvertFrom-Json
            $instances.Count | Should Be 4
        }

        It '-version "14.0" returns 3 instances' {
            $instances = C:\bin\vswhere.exe -legacy -version '14.0' -format json | ConvertFrom-Json
            $instances.Count | Should Be 3
        }

        It '-version "[10.0,15.0)" returns 2 instances' {
            $instances = C:\bin\vswhere.exe -legacy -version '[10.0,15.0)' -format json | ConvertFrom-Json
            $instances.Count | Should Be 2
        }
    }

    Context 'no instances' {
        BeforeEach {
            New-Item HKLM:\Software\WOW6432Node\Microsoft\VisualStudio\SxS\VS7 -Force | ForEach-Object {
                foreach ($version in '10.0', '14.0') {
                    $_ | New-ItemProperty -Name $version -Value "C:\VisualStudio\$version" -Force
                }
            }

            Rename-ItemProperty -Path 'Registry::HKEY_LOCAL_MACHINE\Software\Wow6432Node\Classes\CLSID\{177F0C4A-1CD3-4DE7-A32C-71DBBB9FA36D}\InprocServer32' -Name '(Default)' -NewName '_Default'
        }

        AfterEach {
            Rename-ItemProperty -Path 'Registry::HKEY_LOCAL_MACHINE\Software\Wow6432Node\Classes\CLSID\{177F0C4A-1CD3-4DE7-A32C-71DBBB9FA36D}\InprocServer32' -Name '_Default' -NewName '(Default)'
        }

        It 'returns 2 instances' {
            $instances = C:\bin\vswhere.exe -legacy -format json | ConvertFrom-Json
            $instances.Count | Should Be 2
        }

        It '-latest returns latest instance' {
            $instances = C:\bin\vswhere.exe -legacy -latest -format json | ConvertFrom-Json
            $instances.Count | Should Be 1
            $instances[0].instanceId | Should Be 'VisualStudio.14.0'
            $instances[0].installationPath | Should Be 'C:\VisualStudio\14.0'
        }

        It '-version is supported' {
            $instances = C:\bin\vswhere.exe -legacy -latest -format json | ConvertFrom-Json
            $instances.Count | Should Be 1
            $instances[0].instanceId | Should Be 'VisualStudio.14.0'
            $instances[0].installationPath | Should Be 'C:\VisualStudio\14.0'
        }
    }

    Context '-legacy' {
        It '-products "any" is not supported' {
            C:\bin\vswhere.exe -legacy -products any
            $LASTEXITCODE | Should Be 87
        }

        It '-requires "any" is not supported' {
            C:\bin\vswhere.exe -legacy -requires any
            $LASTEXITCODE | Should Be 87
        }
    }
}
