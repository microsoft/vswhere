// <copyright file="LegacyProvider.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

ILegacyProvider& LegacyProvider::Instance = LegacyProvider();

bool LegacyProvider::HasLegacyInstances() const
{
    return NULL != m_hKey;
}

bool LegacyProvider::TryGetLegacyInstance(_In_ LPCWSTR wzVersion, _Out_ ISetupInstance** ppInstance) const
{
    _ASSERT(ppInstance);

    *ppInstance = NULL;

    if (!m_hKey)
    {
        return false;
    }

    DWORD dwType = 0;
    DWORD cbData = 0;

    auto err = ::RegQueryValueExW(m_hKey, wzVersion, NULL, &dwType, NULL, &cbData);
    if (ERROR_FILE_NOT_FOUND == err || REG_SZ != dwType)
    {
        return false;
    }
    else if (ERROR_SUCCESS != err)
    {
        throw win32_error(err);
    }

    wstring sz(cbData / sizeof(wstring::value_type), wstring::value_type());
    auto lpData = reinterpret_cast<LPBYTE>(const_cast<LPWSTR>(sz.c_str()));

    err = ::RegQueryValueExW(m_hKey, wzVersion, NULL, NULL, lpData, &cbData);
    if (ERROR_SUCCESS != err)
    {
        throw win32_error(err);
    }

    *ppInstance = new (nothrow) LegacyInstance(wzVersion, sz.c_str());
    if (!*ppInstance)
    {
        throw win32_error(E_OUTOFMEMORY);
    }

    return true;
}
