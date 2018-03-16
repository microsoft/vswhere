# Copyright (C) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license. See LICENSE.txt in the project root for license information.

[CmdletBinding()]
param (
    [Parameter(Mandatory = $true)]
    [string] $Token,

    [Parameter()]
    [ValidateNotNullOrEmpty()]
    [string] $Owner = 'Microsoft',

    [Parameter(Mandatory = $true)]
    [string] $Repository,

    [Parameter()]
    [ValidateNotNullOrEmpty()]
    [string] $Tag = $env:BUILD_BUILDNUMBER,

    [Parameter()]
    [string] $Commit = $env:BUILD_SOURCEVERSION,

    [Parameter()]
    [string] $Name,

    [Parameter()]
    [string] $Description,

    [Parameter()]
    [switch] $Draft,

    [Parameter()]
    [switch] $Prerelease,

    [Parameter(Mandatory = $true)]
    [string] $Path,

    [Parameter()]
    [ValidateNotNullOrEmpty()]
    [string] $ContentType = 'application/octet-stream',

    [Parameter()]
    [string] $ContentName,

    [Parameter()]
    [string] $ContentLabel
)

$ErrorActionPreference = 'Stop'

if (-not $Tag) {
    throw '$Tag is required (default value not available)'
}

$uri = "https://api.github.com/repos/$Owner/$Repository/releases"
$values = @{
    tag_name = $Tag
    draft = $Draft.ToBool()
    prerelease = $Prerelease.ToBool()
}

if ($Commit) {
    $values.target_commitish = $Commit
}

if ($Name) {
    $values.name = $Name
} else {
    $values.name = $Tag
}

if ($Description) {
    $values.body = $Description
}

$request = ConvertTo-Json $values
$headers = @{
    Authorization = "token $Token"
    Accept = 'application/vnd.github.v3+json'
}

# Make sure TLS 1.2 is supported now that GitHub requires it.
[System.Net.ServicePointManager]::SecurityProtocol += 'Tls12'

$release = Invoke-RestMethod -Uri $uri -Method Post -Body $request -Headers $headers
$upload_url = $release.upload_url -replace '\{\?.*$', ''

Write-Verbose "Release '$($release.id)' created; upload URI: $upload_url"

$Path = Resolve-Path $Path | Convert-Path

$headers.Add('Content-Type', $ContentType)
if (-not $ContentName) {
    $ContentName = [System.IO.Path]::GetFileName($Path)
}

$uri = $upload_url + "?name=$ContentName"
if ($ContentLabel) {
    $uri += "&label=$ContentLabel"
}

$asset = Invoke-RestMethod -Uri $uri -Method Post -InFile $Path -Headers $headers
if ($asset.state -eq 'uploaded') {
    Write-Verbose "Successfully uploaded: $($asset.browser_download_url)"
}
