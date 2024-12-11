// <copyright file="Glob.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class Glob
{
public:
    Glob(_In_ const std::wstring& root, _In_ const std::wstring& pattern);
    Glob(_In_ const Glob& obj) noexcept :
        m_root(obj.m_root),
        m_root_length(obj.m_root_length),
#ifdef _DEBUG
        m_pattern(obj.m_pattern),
#endif
        m_re(obj.m_re)
    {
    }

    const std::wstring& Root() const noexcept
    {
        return m_root.native();
    }

    const std::vector<std::wstring> Entries(_In_ bool sort = false) const;
    bool Match(_In_ const std::wstring& value) const;

private:
    static bool IsInvalid(_In_ const wchar_t ch);
    static bool RequiresEscape(_In_ const wchar_t ch);
    static void ThrowError(_In_ const std::wstring& pattern);

    std::filesystem::path m_root;
    size_t m_root_length;
    std::wregex m_re;

#ifdef _DEBUG
public:
    const std::wstring& Pattern() const noexcept
    {
        return m_pattern;
    }

private:
    std::wstring m_pattern;
#endif
};
