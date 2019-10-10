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

    LegacyProvider();

    LegacyProvider(const LegacyProvider& obj) = delete;

    ~LegacyProvider()
    {
        if (m_hKey32)
        {
            ::CloseHandle(m_hKey32);
            m_hKey32 = NULL;
        }
        if (m_hKey64)
        {
            ::CloseHandle(m_hKey64);
            m_hKey64 = NULL;
        }
    }

    bool HasLegacyInstances() const override;
    bool TryGetLegacyInstance(_In_ LPCWSTR wzVersion, _Out_ ISetupInstance** ppInstance) const override;

private:
    HKEY m_hKey32;
    HKEY m_hKey64;
};
