# Copyright (C) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license. See LICENSE.txt in the project root for license information.

Describe 'vswhere' {
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
    }
}
