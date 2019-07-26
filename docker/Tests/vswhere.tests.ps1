# Copyright (C) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license. See LICENSE.txt in the project root for license information.

Describe 'vswhere' {
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

    Context '(query provider not registered)' {
        BeforeAll {
            Rename-ItemProperty -Path 'Registry::HKEY_LOCAL_MACHINE\Software\Wow6432Node\Classes\CLSID\{177F0C4A-1CD3-4DE7-A32C-71DBBB9FA36D}\InprocServer32' -Name '(Default)' -NewName '_Default'
        }

        AfterAll {
            Rename-ItemProperty -Path 'Registry::HKEY_LOCAL_MACHINE\Software\Wow6432Node\Classes\CLSID\{177F0C4A-1CD3-4DE7-A32C-71DBBB9FA36D}\InprocServer32' -Name '_Default' -NewName '(Default)'
        }

        It 'header contains no query version' {
            $output = C:\bin\vswhere.exe
            $output[0] | Should Match 'Visual Studio Locator version \d+\.\d+\.\d+'
            $output[0] | Should Not Match '\[query version \d+\.\d+.*\]'
        }

        It 'returns 0 instances' {
            $instances = C:\bin\vswhere.exe -format json | ConvertFrom-Json
            $instances.Count | Should Be 0
        }
    }

    Context '(no arguments)' {
        It 'header contains query version' {
            $output = C:\bin\vswhere.exe
            $output[0] | Should Match 'Visual Studio Locator version \d+\.\d+\.\d+'
            $output[0] | Should Match '\[query version \d+\.\d+.*\]'
        }

        It 'returns 2 instances using "text"' {
            $instanceIds = C:\bin\vswhere.exe | Select-String 'instanceId: \w+'
            $instanceIds.Count | Should Be 2
        }

        It 'returns 2 instances using "json"' {
            $instances = C:\bin\vswhere.exe -format json | ConvertFrom-Json
            $instances.Count | Should Be 2
        }

        It 'returns 2 instances using "value"' {
            $instances = C:\bin\vswhere.exe -property instanceId
            $instances.Count | Should Be 2
        }

        It 'returns 2 instances using "xml"' {
            $instances = [xml](C:\bin\vswhere.exe -format xml)
            $instances.instances.instance.Count | Should Be 2
        }
    }

    Context '-' {
        It 'header contains query version' {
            $output = C:\bin\vswhere.exe -
            $output[0] | Should Match 'Visual Studio Locator version \d+\.\d+\.\d+'
            $output[0] | Should Match '\[query version \d+\.\d+.*\]'

            $LASTEXITCODE | Should Be 87
        }
    }

    Context '-help' {
        It 'header contains query version' {
            $output = C:\bin\vswhere.exe -help
            $output[0] | Should Match 'Visual Studio Locator version \d+\.\d+\.\d+'
            $output[0] | Should Match '\[query version \d+\.\d+.*\]'
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

        It 'returns 3 instances using "value"' {
            $instances = C:\bin\vswhere.exe -all -property instanceId
            $instances.Count | Should Be 3
        }

        It 'returns 3 instances using "xml"' {
            $instances = [xml](C:\bin\vswhere.exe -all -format xml)
            $instances.instances.instance.Count | Should Be 3
        }

        It 'returns 1 instance where IsRebootRequired' {
            # Make sure PowerShell converts to a collection of PSCustomObjects before filtering.
            $instances = C:\bin\vswhere.exe -all -format json | ConvertFrom-Json

            $instances = @($instances | Where-Object { $_.IsRebootRequired })
            $instances.Count | Should Be 1
            $instances[0].instanceId | Should Be 3
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

        It 'returns 1 instance using "value"' {
            $instance = C:\bin\vswhere.exe -products microsoft.visualstudio.product.buildtools -property instanceId
            $instance | Should Be 4
        }

        It 'returns 1 instance using "xml"' {
            $instances = [xml](C:\bin\vswhere.exe -products microsoft.visualstudio.product.buildtools -format xml)
            @($instances.instances.instance).Count | Should Be 1
            @($instances.instances.instance)[0].instanceId | Should Be 4
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

        It 'returns 1 instance using "value"' {
            $instance = C:\bin\vswhere.exe -requires microsoft.visualstudio.workload.nativedesktop -property instanceId
            $instance | Should Be 2
        }
 
        It 'returns 1 instance using "xml"' {
            $instances = [xml](C:\bin\vswhere.exe -requires microsoft.visualstudio.workload.nativedesktop -format xml)
            @($instances.instances.instance).Count | Should Be 1
            @($instances.instances.instance)[0].instanceId | Should Be 2
        }

        It 'returns 0 instances with multiple requirements' {
            $instances = C:\bin\vswhere.exe -requires microsoft.visualstudio.workload.azure microsoft.visualstudio.workload.nativedesktop -format json | ConvertFrom-Json
            $instances.Count | Should Be 0
        }
   }

   Context '-requiresAny' {
        It 'returns 1 instance with multiple requirements' {
            $instances = C:\bin\vswhere.exe -requires microsoft.visualstudio.workload.azure microsoft.visualstudio.workload.nativedesktop -requiresAny -format json | ConvertFrom-Json
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

        It 'returns 1 instance using "value"' {
            $instance = C:\bin\vswhere.exe -version '(15.0.26116,]' -property instanceId
            $instance | Should Be 2
        }

        It 'returns 1 instance using "xml"' {
            $instances = [xml](C:\bin\vswhere.exe -version '(15.0.26116,]' -format xml)
            @($instances.instances.instance).Count | Should Be 1
            @($instances.instances.instance)[0].instanceId | Should Be 2
        }

        It 'does not crash when passed %0' {
            $message = C:\bin\vswhere.exe -version '%0'
            $message[-1] | Should Be 'Error 0x57: The version "%0" is not a valid version range'
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

        It 'returns 1 instance using "value"' {
            $instance = C:\bin\vswhere.exe -latest -property instanceId
            $instance | Should Be 2
        }

        It 'returns 1 instance using "xml"' {
            $instances = [xml](C:\bin\vswhere.exe -latest -format xml)
            @($instances.instances.instance).Count | Should Be 1
            @($instances.instances.instance)[0].instanceId | Should Be 2
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

        It '-property productId' {
            $instances = C:\bin\vswhere.exe -latest -property productId -format json | ConvertFrom-Json
            $instances.Count | Should Be 1
            $instances[0].productId | Should Be 'Microsoft.VisualStudio.Product.Enterprise'
        }

        It '-property productPath' {
            $instances = C:\bin\vswhere.exe -latest -property productPath -format json | ConvertFrom-Json
            $instances.Count | Should Be 1
            $instances[0].productPath | Should Be 'C:\VS\Enterprise\Common7\IDE\devenv.exe'
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

        It 'returns 1 instance using "value"' {
            $instance = C:\bin\vswhere.exe -latest -all -property instanceId
            $instance | Should Be 3
        }

        It 'returns 1 instance using "xml"' {
            $instances = [xml](C:\bin\vswhere.exe -latest -all -format xml)
            @($instances.instances.instance).Count | Should Be 1
            @($instances.instances.instance)[0].instanceId | Should Be 3
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

    Context '-property invalid' {
        It 'returns 0 instances using "text"' {
            $instanceIds = C:\bin\vswhere.exe -property invalid -format text | Select-String 'instanceId: \w+'
            $instanceIds | Should BeNullOrEmpty
        }

        It 'returns 0 instances using "json"' {
            $instances = C:\bin\vswhere.exe -property invalid -format json | ConvertFrom-Json
            $instances.Count | Should Be 0
        }

        It 'returns 0 instances using "value"' {
            $instances = C:\bin\vswhere.exe -property invalid
            $instances | Should BeNullOrEmpty
        }

        It 'returns 0 instances using "xml"' {
            $instances = [xml](C:\bin\vswhere.exe -property invalid -format xml)
            $instances.instances.instance.Count | Should Be 0
        }
    }

    Context '-products *' {
        It 'returns 3 instances using "json"' {
            $instances = C:\bin\vswhere.exe -products * -format json | ConvertFrom-Json
            $instances.Count | Should Be 3
            $instances | ForEach-Object { $_.instanceId | Should Not BeNullOrEmpty }
        }
    }

    Context '-prerelease' {
        It 'returns 4 instances' {
            $instances = C:\bin\vswhere.exe -prerelease -format json | ConvertFrom-Json
            $instances.Count | Should Be 3
        }

        It '-products * returns 4 instances' {
            $instances = C:\bin\vswhere.exe -prerelease -products * -format json | ConvertFrom-Json
            $instances.Count | Should Be 4
        }

        It '-latest returns prerelease' {
            $instances = C:\bin\vswhere.exe -prerelease -latest -format json | ConvertFrom-Json
            $instances.Count | Should Be 1
            $instances[0].InstanceId | Should Be 5
            $instances[0].IsPrerelease | Should Be $true
        }
    }

    Context '-path' {
        It 'returns nothing for non-installation path' {
            $instances = C:\bin\vswhere.exe -path C:\ShouldNotExist -format json | ConvertFrom-Json
            $instances | Should BeNullOrEmpty
        }

        It 'returns normal instance' {
            $instances = C:\bin\vswhere.exe -path C:\VS\Enterprise -format json | ConvertFrom-Json
            $instances.Count | Should Be 1
            $instances[0].InstanceId | Should Be 2
        }

        It 'returns normal instance for normalized directory' {
            $instances = C:\bin\vswhere.exe -path C:\VS\Enterprise\ -format json | ConvertFrom-Json
            $instances.Count | Should Be 1
            $instances[0].InstanceId | Should Be 2
        }

        It 'returns normal instance for file path' {
            $instances = C:\bin\vswhere.exe -path C:\VS\Enterprise\Common7\Tools\VSDevCmd.bat -format json | ConvertFrom-Json
            $instances.Count | Should Be 1
            $instances[0].InstanceId | Should Be 2
        }

        It 'returns incomplete instance' {
            $instances = C:\bin\vswhere.exe -path C:\VS\Professional -format json | ConvertFrom-Json
            $instances.Count | Should Be 1
            $instances[0].InstanceId | Should Be 3
        }

        It 'returns other instance' {
            $instances = C:\bin\vswhere.exe -path C:\BuildTools -format json | ConvertFrom-Json
            $instances.Count | Should Be 1
            $instances[0].InstanceId | Should Be 4
        }
    }

    # NOTE: microsoft/windowsservercore does not support setting the code page to anything other than 65001.
    # Context 'encodes ja-JP' {
    #     BeforeAll {
    #         [Console]::OutputEncoding = [System.Text.Encoding]::UTF8

    #         Set-Culture ja-JP
    #     }

    #     AfterAll {
    #         Set-Culture en-US
    #     }

    #     It 'incorrectly without: -utf8' {
    #         $instances = C:\bin\vswhere.exe -latest -format json | ConvertFrom-Json
    #         $instances.Count | Should Be 1
    #         $instances[0].InstanceId | Should Be 2
    #         $instances[0].Description | Should Not Be '生産性向上と、さまざまな規模のチーム間の調整のための Microsoft DevOps ソリューション'
    #     }

    #     It 'correctly with: -utf8' {
    #         $instances = C:\bin\vswhere.exe -latest -format json -utf8 | ConvertFrom-Json
    #         $instances.Count | Should Be 1
    #         $instances[0].InstanceId | Should Be 2
    #         $instances[0].Description | Should Be '生産性向上と、さまざまな規模のチーム間の調整のための Microsoft DevOps ソリューション'
    #     }
    # }
}
