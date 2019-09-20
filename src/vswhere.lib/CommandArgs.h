// <copyright file="CommandArgs.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class Console;

class CommandArgs
{
public:
    CommandArgs() noexcept :
        m_all(false),
        m_productsAll(false),
        m_latest(false),
        m_legacy(false),
        m_prerelease(false),
        m_includePackages(false),
        m_nologo(false),
        m_utf8(false),
        m_help(false)
    {
    }

    CommandArgs(const CommandArgs& obj) :
        m_applicationPath(obj.m_applicationPath),
        m_all(obj.m_all),
        m_productsAll(obj.m_productsAll),
        m_products(obj.m_products),
        m_requires(obj.m_requires),
        m_version(obj.m_version),
        m_latest(obj.m_latest),
        m_legacy(obj.m_legacy),
        m_path(obj.m_path),
        m_sort(obj.m_sort),
        m_prerelease(obj.m_prerelease),
        m_format(obj.m_format),
        m_property(obj.m_property),
        m_includePackages(obj.m_includePackages),
        m_find(obj.m_find),
        m_nologo(obj.m_nologo),
        m_utf8(obj.m_utf8),
        m_help(obj.m_help)
    {
    }

    const std::wstring& get_ApplicationPath() const noexcept
    {
        return m_applicationPath;
    }

    const bool get_All() const noexcept
    {
        return m_all;
    }

    const std::vector<std::wstring>& get_Products() const noexcept
    {
        if (!m_productsAll && m_products.empty())
        {
            return s_Products;
        }

        return m_products;
    }

    const std::vector<std::wstring>& get_Requires() const noexcept
    {
        return m_requires;
    }

    const bool get_RequiresAny() const noexcept
    {
        return m_requiresAny;
    }

    const std::wstring& get_Version() const noexcept
    {
        return m_version;
    }

    const bool get_Latest() const noexcept
    {
        return m_latest;
    }

    const bool get_Legacy() const noexcept
    {
        return m_legacy;
    }

    const std::wstring& get_Path() const noexcept
    {
        return m_path;
    }

    const bool get_Sort() const noexcept
    {
        return m_sort;
    }

    const bool get_Prerelease() const noexcept
    {
        return m_prerelease;
    }

    const std::wstring& get_Format() const noexcept
    {
        if (m_format.empty())
        {
            return s_Format;
        }

        return m_format;
    }

    const std::wstring& get_Property() const noexcept
    {
        return m_property;
    }

    const bool get_IncludePackages() const noexcept
    {
        return m_includePackages;
    }

    const std::wstring& get_Find() const noexcept
    {
        return m_find;
    }

    const bool get_Logo() const noexcept
    {
        return !m_nologo;
    }

    const bool get_UTF8() const noexcept
    {
        return m_utf8;
    }

    const bool get_Help() const noexcept
    {
        return m_help;
    }


    void Parse(_In_ LPCWSTR wszCommandLine);
    void Parse(_In_ int argc, _In_ LPCWSTR argv[]);
    void Usage(_In_ Console& console) const;

private:
    static const std::vector<std::wstring> s_Products;
    static const std::wstring s_Format;

    void Parse(_In_ std::vector<CommandParser::Token> args);

    std::wstring m_applicationPath;
    bool m_all;
    bool m_productsAll;
    std::vector<std::wstring> m_products;
    std::vector<std::wstring> m_requires;
    bool m_requiresAny;
    std::wstring m_version;
    bool m_latest;
    bool m_legacy;
    std::wstring m_path;
    bool m_sort;
    bool m_prerelease;
    std::wstring m_format;
    std::wstring m_property;
    bool m_includePackages;
    std::wstring m_find;
    bool m_nologo;
    bool m_utf8;
    bool m_help;
};
