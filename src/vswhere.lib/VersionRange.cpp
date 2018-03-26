// <copyright file="VersionRange.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

#define _nameof(x) #x
#define ExitOnFailure(x, s, ...) if (FAILED(x)) { goto Exit; }
#define ExitOnNull(p, x, e, s, ...) if (NULL == p) { x = e; goto Exit; }

using namespace std;

static locale neutral_locale("C", LC_ALL);

STDMETHODIMP VersionRange::ParseVersion(
    _In_ LPCOLESTR pwszVersion,
    _Out_ PULONGLONG pullVersion
) noexcept
{
    HRESULT hr = S_OK;
    LPWSTR pwszVersionEnd = NULL;

    ExitOnNull(pwszVersion, hr, E_INVALIDARG, "Missing required parameter: %hs", _nameof(pwszVersion));
    ExitOnNull(pullVersion, hr, E_POINTER, "Output parameter is NULL.");

    *pullVersion = 0;

    LPWSTR pwszVersionRangeEnd = NULL;
    pwszVersion = Trim(pwszVersion, &pwszVersionEnd);

    if (pwszVersion > pwszVersionEnd)
    {
        // Should not be possible but check anyway.
        ExitOnFailure(hr = E_UNEXPECTED, "Invalid string.");
    }

    const size_t cchVersion = pwszVersionEnd - pwszVersion;
    ExitOnNull(cchVersion, hr, E_INVALIDARG, "Version required.");

    hr = ParseVersionString(pwszVersion, pwszVersionEnd, pullVersion);
    ExitOnFailure(hr, "Failed to parse version: %ls", pwszVersion);

Exit:
    return hr;
}

STDMETHODIMP VersionRange::ParseVersionRange(
    _In_ LPCOLESTR pwszVersionRange,
    _Out_ PULONGLONG pullMinVersion,
    _Out_ PULONGLONG pullMaxVersion
)
{
    HRESULT hr = S_OK;
    ULONGLONG ullMinVersion = MinVersion;
    ULONGLONG ullMaxVersion = MaxVersion;
    LPCWSTR pwszVersionStart = NULL;
    LPCWSTR pwszVersionEnd = NULL;
    bool sep = false;
    bool single = false;
    bool isMinInclusive = true;
    bool isMaxInclusive = true;

    ExitOnNull(pwszVersionRange, hr, E_INVALIDARG, "Missing required parameter: %hs", _nameof(pwszVersionRange));
    ExitOnNull(pullMinVersion, hr, E_POINTER, "Output parameter is NULL.");
    ExitOnNull(pullMaxVersion, hr, E_POINTER, "Output parameter is NULL.");

    *pullMinVersion = 0;
    *pullMaxVersion = 0;

    LPWSTR pwszVersionRangeEnd = NULL;
    pwszVersionRange = Trim(pwszVersionRange, &pwszVersionRangeEnd);

    if (pwszVersionRange > pwszVersionRangeEnd)
    {
        // Should not be possible but check anyway.
        ExitOnFailure(hr = E_UNEXPECTED, "Invalid string.");
    }

    const size_t cchVersionRange = pwszVersionRangeEnd - pwszVersionRange;
    ExitOnNull(cchVersionRange, hr, E_INVALIDARG, "Version range required.");

    for (size_t i = 0; i < cchVersionRange; ++i)
    {
        const auto pwsz = &pwszVersionRange[i];
        const auto ch = *pwsz;

        if (L'(' == ch || L'[' == ch)
        {
            if (0 != i)
            {
                ExitOnFailure(hr = E_INVALIDARG, "Invalid format.");
            }
            else if (L'(' == ch)
            {
                isMinInclusive = false;
            }
        }
        else if (L')' == ch || L']' == ch)
        {
            if (cchVersionRange - 1 != i)
            {
                ExitOnFailure(hr = E_INVALIDARG, "Invalid format.");
            }
            else if (L')' == ch)
            {
                isMaxInclusive = false;
            }

            // Only a single version if the separator was not yet parsed.
            single = !sep;
        }
        else if (L',' == ch)
        {
            if (sep)
            {
                // Already parsed a separator.
                ExitOnFailure(hr = E_INVALIDARG, "Invalid format.");
            }

            sep = true;

            if (pwszVersionEnd > pwszVersionStart)
            {
                hr = ParseVersionString(pwszVersionStart, pwszVersionEnd, &ullMinVersion);
                ExitOnFailure(hr, "Failed to parse version.");
            }

            pwszVersionStart = NULL;
            pwszVersionEnd = NULL;
        }
        else if (isspace(ch, neutral_locale))
        {
            continue;
        }
        else if (!pwszVersionStart)
        {
            pwszVersionStart = pwsz;
            pwszVersionEnd = pwsz + 1;
        }
        else
        {
            pwszVersionEnd = pwsz + 1;
        }
    }

    if (pwszVersionEnd > pwszVersionStart)
    {
        auto pullVersion = sep ? &ullMaxVersion : &ullMinVersion;

        hr = ParseVersionString(pwszVersionStart, pwszVersionEnd, pullVersion);
        ExitOnFailure(hr, "Failed to parse version.");

        if (single)
        {
            ullMaxVersion = ullMinVersion;
        }
    }

    if (!isMinInclusive)
    {
        if (_UI64_MAX == ullMinVersion)
        {
            ExitOnFailure(hr = E_INVALIDARG, "Arithmetic overflow.");
        }

        ullMinVersion++;
    }

    if (!isMaxInclusive)
    {
        if (0 == ullMaxVersion)
        {
            ExitOnFailure(hr = E_INVALIDARG, "Arithmetic underflow.");
        }

        ullMaxVersion--;
    }

    if (ullMinVersion > ullMaxVersion)
    {
        ExitOnFailure(hr = E_INVALIDARG, "Min greater than max.");
    }

    *pullMinVersion = ullMinVersion;
    *pullMaxVersion = ullMaxVersion;

Exit:
    return hr;
}

HRESULT VersionRange::ParseVersionString(
    _In_ const LPCWSTR pwszVersionBegin,
    _In_ const LPCWSTR pwszVersionEnd,
    _Out_ PULONGLONG pullVersion
)
{
    _ASSERT(pwszVersionBegin);
    _ASSERT(pwszVersionEnd);
    _ASSERT(pullVersion);

    HRESULT hr = S_OK;
    LPWSTR pwszFieldBegin = const_cast<LPWSTR>(pwszVersionBegin);
    LPWSTR pwszFieldEnd = const_cast<LPWSTR>(pwszVersionBegin);
    WORD cFields = 0;
    ULONG ulField = 0;
    ULONGLONG ullVersion = 0;

    *pullVersion = 0;

    for (;;)
    {
        if (4 <= cFields)
        {
            ExitOnFailure(hr = E_INVALIDARG, "Too many version fields.");
        }

        while (pwszFieldEnd < pwszVersionEnd && L'.' != *pwszFieldEnd)
        {
            if (isspace(*pwszFieldEnd, neutral_locale))
            {
                ExitOnFailure(hr = E_INVALIDARG, "Space in version field.");
            }

            ++pwszFieldEnd;
        }

        if (pwszFieldBegin == pwszFieldEnd)
        {
            ExitOnFailure(hr = E_INVALIDARG, "Empty version field.");
        }

        LPWSTR pwszEnd = NULL;
        ulField = ::wcstoul(pwszFieldBegin, &pwszEnd, 10);

        if (USHRT_MAX < ulField)
        {
            ExitOnFailure(hr = E_INVALIDARG, "Version field too large.");
        }
        else if (0 == ulField && pwszEnd != pwszFieldEnd)
        {
            ExitOnFailure(hr = E_INVALIDARG, "NaN");
        }

        pwszFieldEnd = pwszEnd;

        ullVersion |= (ULONGLONG)ulField << ((3 - cFields) * 16);
        ++cFields;

        if (pwszFieldEnd >= pwszVersionEnd)
        {
            break;
        }

        pwszFieldBegin = ++pwszFieldEnd;
    }

    *pullVersion = ullVersion;

Exit:
    return hr;
}

LPWSTR VersionRange::Trim(
    _In_ const LPCWSTR pwszValue,
    _Out_ LPWSTR* ppwszEnd
) noexcept
{
    auto pwszResult = const_cast<LPWSTR>(pwszValue);

    // Trim left.
    while (isspace(*pwszResult, neutral_locale))
    {
        ++pwszResult;
    }

    // Trim right.
    auto cchResult = ::wcslen(pwszResult);
    for (; cchResult > 0 && isspace(pwszResult[cchResult - 1], neutral_locale); --cchResult);

    // Set end to where terminating null is or would be.
    *ppwszEnd = &pwszResult[cchResult];
    return pwszResult;
}
