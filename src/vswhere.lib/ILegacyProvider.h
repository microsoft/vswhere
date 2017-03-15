// <copyright file="ILegacyProvider.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class ILegacyProvider
{
public:
    virtual bool HasLegacyInstances() const = 0;
    virtual bool TryGetLegacyInstance(_In_ LPCWSTR wzVersion, _Out_ ISetupInstance** ppInstance) const = 0;
};
