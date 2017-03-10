// <copyright file="TestLegacyProvider.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

bool TestLegacyProvider::HasLegacyInstances() const
{
    return !m_instances.empty();
}

bool TestLegacyProvider::TryGetLegacyInstance(_In_ LPCWSTR wzVersion, _Out_ ISetupInstance** ppInstance) const
{
    _ASSERT(ppInstance);

    *ppInstance = NULL;

    auto it = m_instances.find(wzVersion);
    if (it != m_instances.end())
    {
        *ppInstance = new LegacyInstance(it->first.c_str(), it->second.c_str());
        return true;
    }

    return false;
}
