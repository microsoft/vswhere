# Copyright (C) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license. See LICENSE.txt in the project root for license information.

Describe 'vswhere' {
    BeforeEach {
        # Make sure localized values are returned consistently across machines.
        $enu = [System.Globalization.CultureInfo]::GetCultureInfo('en-US')

        [System.Globalization.CultureInfo]::CurrentCulture = $enu
        [System.Globalization.CultureInfo]::CurrentUICulture = $enu
    }

    Context '(no arguments)' {
        It 'returns 2 instances using "text"' {
            $instanceIds = C:\bin\vswhere.exe | Select-String 'instanceId: \w+'
            $instanceIds.Count | Should Be 2
        }

        It 'returns 2 instances using "json"' {
            $instances = C:\bin\vswhere.exe -format json | ConvertFrom-Json
            $instances.Count | Should Be 2
        }
    }

    Context '-all' {
        It 'returns 3 instances using "text"' {
            $instanceIds = C:\bin\vswhere.exe -all | Select-String 'instanceId: \w+'
            $instanceIds.Count | Should Be 3
        }

        It 'returns 3 instances using "json"' {
            $instances = C:\bin\vswhere.exe -all -format json | ConvertFrom-Json
            $instances.Count | Should Be 3
        }
    }

    Context '-products' {
        It 'returns 1 instance using "text"' {
            $instanceIds = C:\bin\vswhere.exe -products microsoft.visualstudio.product.buildtools | Select-String 'instanceId: \w+'
            $instanceIds.Count | Should Be 1
            $instanceIds.Matches[0].Value | Should Be 'instanceId: 4'
        }

        It 'returns 1 instance using "json"' {
            $instances = C:\bin\vswhere.exe -products microsoft.visualstudio.product.buildtools -format json | ConvertFrom-Json
            $instances.Count | Should Be 1
            $instances[0].instanceId | Should Be 4
        }
    }

    Context '-requires' {
        It 'returns 1 instance using "text"' {
            $instanceIds = C:\bin\vswhere.exe -requires microsoft.visualstudio.workload.nativedesktop | Select-String 'instanceId: \w+'
            $instanceIds.Count | Should Be 1
            $instanceIds.Matches[0].Value | Should Be 'instanceId: 2'
        }

        It 'returns 1 instance using "json"' {
            $instances = C:\bin\vswhere.exe -requires microsoft.visualstudio.workload.nativedesktop -format json | ConvertFrom-Json
            $instances.Count | Should Be 1
            $instances[0].instanceId | Should Be 2
        }
    }

    Context '-version' {
        It 'returns 1 instance using "text"' {
            $instanceIds = C:\bin\vswhere.exe -version '(15.0.26116,]' | Select-String 'instanceId: \w+'
            $instanceIds.Count | Should Be 1
            $instanceIds.Matches[0].Value | Should Be 'instanceId: 2'
        }

        It 'returns 1 instance using "json"' {
            $instances = C:\bin\vswhere.exe -version '(15.0.26116,]' -format json | ConvertFrom-Json
            $instances.Count | Should Be 1
            $instances[0].instanceId | Should Be 2
        }
    }

    Context '-latest' {
        It 'returns 1 instance using "text"' {
            $instanceIds = C:\bin\vswhere.exe -latest | Select-String 'instanceId: \w+'
            $instanceIds.Count | Should Be 1
            $instanceIds.Matches[0].Value | Should Be 'instanceId: 2'
        }

        It 'returns 1 instance using "json"' {
            $instances = C:\bin\vswhere.exe -latest -format json | ConvertFrom-Json
            $instances.Count | Should Be 1
            $instances[0].instanceId | Should Be 2
        }

        It 'returns all intrinsic properties' {
            $instances = C:\bin\vswhere.exe -latest -format json | ConvertFrom-Json
            $instances.Count | Should Be 1

            $instance = $instances[0]
            $instance.instanceId | Should Be '2'
            $instance.installDate | Should Be '2017-01-18T03:15:00Z'
            $instance.installationName | Should Be 'VisualStudio/public.d15rel/15.0.26117.0'
            $instance.installationVersion | Should Be '15.0.26117'
            $instance.installationPath | Should Be 'C:\VS\Enterprise'
            $instance.displayName | Should Be 'Visual Studio Enterprise 2017'
            $instance.description | Should  Be 'Microsoft DevOps solution for productivity and coordination across teams of any size'
            $instance.channelId | Should Be 'VisualStudio.15.Release/public.d15rel/15.0.26117.0'
            $instance.enginePath | Should Be 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\resources\app\ServiceHub\Services\Microsoft.VisualStudio.Setup.Service'
        }
    }

    Context '-latest -all' {
        It 'returns 1 instance using "text"' {
            $instanceIds = C:\bin\vswhere.exe -latest -all | Select-String 'instanceId: \w+'
            $instanceIds.Count | Should Be 1
            $instanceIds.Matches[0].Value | Should Be 'instanceId: 3'
        }

        It 'returns 1 instance using "json"' {
            $instances = C:\bin\vswhere.exe -latest -all -format json | ConvertFrom-Json
            $instances.Count | Should Be 1
            $instances[0].instanceId | Should Be 3
        }

        It 'returns all intrinsic properties' {
            $instances = C:\bin\vswhere.exe -latest -all -format json | ConvertFrom-Json
            $instances.Count | Should Be 1

            $instance = $instances[0]
            $instance.instanceId | Should Be '3'
            $instance.installDate | Should Be '2017-01-18T03:30:00Z'
            $instance.installationName | Should Be 'VisualStudio/public.d15rel/15.0.26117.0'
            $instance.installationVersion | Should Be '15.0.26117'
            $instance.installationPath | Should Be 'C:\VS\Professional'
            $instance.displayName | Should Be 'Visual Studio Professional 2017'
            $instance.description | Should  Be 'Professional developer tools and services for small teams'
            $instance.channelId | Should Be 'VisualStudio.15.Release/public.d15rel/15.0.26117.0'
            $instance.enginePath | Should Be 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\resources\app\ServiceHub\Services\Microsoft.VisualStudio.Setup.Service'
        }
    }
}
