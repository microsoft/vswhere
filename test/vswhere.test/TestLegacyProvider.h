// <copyright file="TestLegacyProvider.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class TestLegacyProvider :
    public ILegacyProvider
{
public:
    typedef std::unordered_map<std::wstring, std::wstring, std::hash<std::wstring>, ci_equal> MapType;

    TestLegacyProvider(_In_ std::initializer_list<MapType::value_type> list) :
        m_instances(list.begin(), list.end())
    {
    }

    bool HasLegacyInstances() const override;
    bool TryGetLegacyInstance(_In_ LPCWSTR wzVersion, _Out_ ISetupInstance** ppInstance) const override;

private:
    MapType m_instances;
};
