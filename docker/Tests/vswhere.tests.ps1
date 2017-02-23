# Copyright (C) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license. See LICENSE.txt in the project root for license information.

Describe 'vswhere' {
    Context 'format: text (default)' {
        It 'returns 2 instances' {
            $instanceIds = C:\bin\vswhere.exe | Select-String 'instanceId: \w+'
            $instanceIds.Count | Should Be 2
        }

        It '-all returns 3 instances' {
            $instanceIds = C:\bin\vswhere.exe -all | Select-String 'instanceId: \w+'
            $instanceIds.Count | Should Be 3
        }

        It '-products returns 1 instance' {
            $instanceIds = C:\bin\vswhere.exe -products microsoft.visualstudio.product.buildtools | Select-String 'instanceId: \w+'
            $instanceIds.Count | Should Be 1
        }

        It '-requires returns 1 instance' {
            $instanceIds = C:\bin\vswhere.exe -requires microsoft.visualstudio.workload.nativedesktop | Select-String 'instanceId: \w+'
            $instanceIds.Count | Should Be 1
        }

        It '-version returns 1 instance' {
            $instanceIds = C:\bin\vswhere.exe -version '(15.0.26116,]' | Select-String 'instanceId: \w+'
            $instanceIds.Count | Should Be 1
        }
    }

    Context 'format: json' {
        It 'returns 2 instances' {
            $instances = C:\bin\vswhere.exe -format json | ConvertFrom-Json
            $instances.Count | Should Be 2
        }

        It '-all returns 3 instances' {
            $instances = C:\bin\vswhere.exe -all -format json | ConvertFrom-Json
            $instances.Count | Should Be 3
        }

        It '-products returns 1 instance' {
            $instance = C:\bin\vswhere.exe -products microsoft.visualstudio.product.buildtools -format json | ConvertFrom-Json
            $instance.Count | Should Be 1
        }

        It '-requires returns 1 instance' {
            $instances = C:\bin\vswhere.exe -requires microsoft.visualstudio.workload.nativedesktop -format json | ConvertFrom-Json
            $instances.Count | Should Be 1
        }

        It '-version returns 1 instance' {
            $instances = C:\bin\vswhere.exe -version '(15.0.26116,]' -format json | ConvertFrom-Json
            $instances.Count | Should Be 1
        }
    }
}
