// <copyright file="ResourceManager.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class ResourceManager
{
public:
    static void SetInstance(_In_ HINSTANCE hInstance)
    {
        s_hInstance = hInstance;
    }

    static std::wstring GetString(_In_ DWORD nID);
    static std::wstring __cdecl FormatString(_In_ DWORD nID, ...)
    {
        va_list args;

        va_start(args, nID);
        auto wsz = FormatString(nID, args);
        va_end(args);

        return wsz;
    }

private:
    ResourceManager() {}
    ResourceManager(const ResourceManager& obj) {}
    ResourceManager(ResourceManager&& obj) {}

    static std::wstring FormatString(_In_ DWORD nID, _In_ va_list args);

    static HINSTANCE s_hInstance;
};
