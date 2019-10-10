// <copyright file="LegacyProvider.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

ILegacyProvider& LegacyProvider::Instance = LegacyProvider();

LegacyProvider::LegacyProvider() :
    m_hKey32(NULL), m_hKey64(NULL)
{
    auto err = ::RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VS7", 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &m_hKey32);
    if (ERROR_SUCCESS != err && ERROR_FILE_NOT_FOUND != err)
    {
        throw win32_error(err);
    }

    err = ::RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VS7", 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &m_hKey64);
    if (ERROR_SUCCESS != err && ERROR_FILE_NOT_FOUND != err)
    {
        throw win32_error(err);
    }
}

bool LegacyProvider::HasLegacyInstances() const
{
    return !(NULL == m_hKey32 && NULL == m_hKey64);
}

static bool TryGetLegacyInstanceImpl(_In_ HKEY m_hKey, _In_ LPCWSTR wzVersion, _Out_ ISetupInstance** ppInstance)
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

bool LegacyProvider::TryGetLegacyInstance(_In_ LPCWSTR wzVersion, _Out_ ISetupInstance** ppInstance) const
{
    if (TryGetLegacyInstanceImpl(m_hKey32, wzVersion, ppInstance))
    {
        return true;
    }
    if (TryGetLegacyInstanceImpl(m_hKey64, wzVersion, ppInstance))
    {
        return true;
    }
    return false;
}
