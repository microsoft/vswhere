// <copyright file="Module.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class Module
{
public:
    Module() :
        m_hModule(NULL)
    {
    }

    Module(_In_ const Module& obj) :
        m_hModule(obj.m_hModule),
        m_path(obj.m_path),
        m_fileVersion(obj.m_fileVersion)
    {
    }

    ~Module()
    {
        if (m_hModule)
        {
            ::FreeLibrary(m_hModule);
        }
    }

    // Avoid throwing non-catastrophic exceptions since information is for diagnostics only.
    void FromIUnknown(_In_opt_ const IUnknown* pUnk) noexcept;
    const std::wstring& get_Path() noexcept;
    const std::wstring& get_FileVersion() noexcept;

private:
    HMODULE m_hModule;
    std::wstring m_path;
    std::wstring m_fileVersion;
};
