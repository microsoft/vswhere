// <copyright file="LegacyProvider.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class LegacyProvider :
    public ILegacyProvider
{
public:
    static ILegacyProvider& Instance;

    LegacyProvider() :
        m_hKey(NULL)
    {
        auto err = ::RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VS7", 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &m_hKey);
        if (ERROR_SUCCESS != err && ERROR_FILE_NOT_FOUND != err)
        {
            throw win32_error(err);
        }
    }

    LegacyProvider(const LegacyProvider& obj) = delete;

    ~LegacyProvider()
    {
        if (m_hKey)
        {
            ::CloseHandle(m_hKey);
            m_hKey = NULL;
        }
    }

    bool HasLegacyInstances() const override;
    bool TryGetLegacyInstance(_In_ LPCWSTR wzVersion, _Out_ ISetupInstance** ppInstance) const override;

private:
    HKEY m_hKey;
};
