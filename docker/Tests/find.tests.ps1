# Copyright (C) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license. See LICENSE.txt in the project root for license information.

# Instances and results are sorted for consistency.
Describe 'vswhere -sort -find' {
    BeforeAll {
        # Always write to 32-bit registry key.
        $key = New-Item -Path Registry::HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\VisualStudio\Setup\Reboot -Force
        $null = $key | New-ItemProperty -Name 3 -Value 1 -Force

        $files = @(
            'MSBuild\15.0\Bin\MSBuild.exe'
            'MSBuild\15.0\Bin\MSBuild.exe.config'
            'MSBuild\15.0\Bin\amd64\MSBuild.exe'
            'MSBuild\15.0\Bin\amd64\MSBuild.exe.config'
        )
        # Populate each instance with files to find.
        $instances = C:\bin\vswhere.exe -all -prerelease -products * -format json | ConvertFrom-Json
        foreach ($file in $files) {
            $filePath = Join-Path -Path $instances.installationPath -ChildPath $file
            $null = New-Item -Path $filePath -ItemType 'File' -Value '1' -Force
        }
    }

    Context 'msbuild\15.0\bin\msbuild.exe' {
        It 'returns 2 matches' {
            $files = C:\bin\vswhere.exe -sort -find 'msbuild\15.0\bin\msbuild.exe'

            $files.Count | Should Be 2
            $files[0] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\MSBuild.exe'
            $files[1] | Should Be 'C:\VS\Community\MSBuild\15.0\Bin\MSBuild.exe'
        }

        It '-format json returns 2 matches' {
            $files = C:\bin\vswhere.exe -sort -find 'msbuild\15.0\bin\msbuild.exe' -format json | ConvertFrom-Json

            $files.Count | Should Be 2
            $files[0] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\MSBuild.exe'
            $files[1] | Should Be 'C:\VS\Community\MSBuild\15.0\Bin\MSBuild.exe'
        }

        It '-format xml returns 2 matches' {
            $doc = [xml](C:\bin\vswhere.exe -sort -find 'msbuild\15.0\bin\msbuild.exe' -format xml)

            $doc.files.file.Count | Should Be 2
            $doc.files.file[0] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\MSBuild.exe'
            $doc.files.file[1] | Should Be 'C:\VS\Community\MSBuild\15.0\Bin\MSBuild.exe'
        }
    }

    Context 'msbuild\**\msbuild.exe' {
        It 'returns 4 matches' {
            $files = C:\bin\vswhere.exe -sort -find 'msbuild\**\msbuild.exe'

            $files.Count | Should Be 4
            $files[0] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\amd64\MSBuild.exe'
            $files[1] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\MSBuild.exe'
            $files[2] | Should Be 'C:\VS\Community\MSBuild\15.0\Bin\amd64\MSBuild.exe'
            $files[3] | Should Be 'C:\VS\Community\MSBuild\15.0\Bin\MSBuild.exe'
        }

        It '-format json returns 4 matches' {
            $files = C:\bin\vswhere.exe -sort -find 'msbuild\**\msbuild.exe' -format json | ConvertFrom-Json

            $files.Count | Should Be 4
            $files[0] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\amd64\MSBuild.exe'
            $files[1] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\MSBuild.exe'
            $files[2] | Should Be 'C:\VS\Community\MSBuild\15.0\Bin\amd64\MSBuild.exe'
            $files[3] | Should Be 'C:\VS\Community\MSBuild\15.0\Bin\MSBuild.exe'
        }

        It '-format xml returns 4 matches' {
            $doc = [xml](C:\bin\vswhere.exe -sort -find 'msbuild\**\msbuild.exe' -format xml)

            $doc.files.file.Count | Should Be 4
            $doc.files.file[0] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\amd64\MSBuild.exe'
            $doc.files.file[1] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\MSBuild.exe'
            $doc.files.file[2] | Should Be 'C:\VS\Community\MSBuild\15.0\Bin\amd64\MSBuild.exe'
            $doc.files.file[3] | Should Be 'C:\VS\Community\MSBuild\15.0\Bin\MSBuild.exe'
        }
    }

    Context 'msbuild\**\msbuild.* -latest' {
        It 'returns 4 matches' {
            $files = C:\bin\vswhere.exe -sort -find 'msbuild\**\msbuild.*' -latest

            $files.Count | Should Be 4
            $files[0] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\amd64\MSBuild.exe'
            $files[1] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\amd64\MSBuild.exe.config'
            $files[2] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\MSBuild.exe'
            $files[3] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\MSBuild.exe.config'
        }

        It '-format json returns 4 matches' {
            $files = C:\bin\vswhere.exe -sort -find 'msbuild\**\msbuild.*' -latest -format json | ConvertFrom-Json

            $files.Count | Should Be 4
            $files[0] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\amd64\MSBuild.exe'
            $files[1] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\amd64\MSBuild.exe.config'
            $files[2] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\MSBuild.exe'
            $files[3] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\MSBuild.exe.config'
        }

        It '-format xml returns 4 matches' {
            $doc = [xml](C:\bin\vswhere.exe -sort -find 'msbuild\**\msbuild.*' -latest -format xml)

            $doc.files.file.Count | Should Be 4
            $doc.files.file[0] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\amd64\MSBuild.exe'
            $doc.files.file[1] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\amd64\MSBuild.exe.config'
            $doc.files.file[2] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\MSBuild.exe'
            $doc.files.file[3] | Should Be 'C:\VS\Enterprise\MSBuild\15.0\Bin\MSBuild.exe.config'
        }
    }
}
