// <copyright file="CommandArgs.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

static bool ArgumentEquals(_In_ const wstring& name, _In_ LPCWSTR expect);

template <class IteratorType>
static wstring ParseArgument(IteratorType& it, const IteratorType& end, const CommandParser::Token& arg);

template <class IteratorType>
static void ParseArgumentArray(IteratorType& it, const IteratorType& end, const CommandParser::Token& arg, vector<wstring>& arr);

template <class IteratorType>
static void ParseRequiresArray(IteratorType& it, const IteratorType& end, const CommandParser::Token& arg, vector<wstring>& literals, vector<wregex>& patterns);

const vector<wstring> CommandArgs::s_Products
{
    L"Microsoft.VisualStudio.Product.Enterprise",
    L"Microsoft.VisualStudio.Product.Professional",
    L"Microsoft.VisualStudio.Product.Community",
};

const wstring CommandArgs::s_Format = L"text";

void CommandArgs::Parse(_In_ LPCWSTR wszCommandLine)
{
    CommandParser parser;
    auto args = parser.Parse(wszCommandLine);

    m_applicationPath = parser.get_Path();

    Parse(args);
}

void CommandArgs::Parse(_In_ int argc, _In_ LPCWSTR argv[])
{
    CommandParser parser;
    auto args = parser.Parse(argc, argv);

    m_applicationPath = parser.get_Path();

    Parse(args);
}

void CommandArgs::Parse(_In_ vector<CommandParser::Token> args)
{
    bool hasSelection = false;

    for (auto it = args.begin(); it != args.end(); ++it)
    {
        auto& arg = *it;

        // No unspecified arguments supported.
        if (CommandParser::Token::eParameter != arg.Type)
        {
            auto message = ResourceManager::GetString(IDS_E_ARGEXPECTED);
            throw win32_error(ERROR_INVALID_PARAMETER, message);
        }

        if (ArgumentEquals(arg.Value, L"all"))
        {
            m_all = true;
            hasSelection = true;
        }
        else if (ArgumentEquals(arg.Value, L"products"))
        {
            ParseArgumentArray(it, args.end(), arg, m_products);
            hasSelection = true;
        }
        else if (ArgumentEquals(arg.Value, L"requires"))
        {
            ParseRequiresArray(it, args.end(), arg, m_requires, m_requiresPattern);
            hasSelection = true;
        }
        else if (ArgumentEquals(arg.Value, L"requiresAny"))
        {
            m_requiresAny = true;
            hasSelection = true;
        }
        else if (ArgumentEquals(arg.Value, L"version"))
        {
            m_version = ParseArgument(it, args.end(), arg);
            hasSelection = true;
        }
        else if (ArgumentEquals(arg.Value, L"latest"))
        {
            m_latest = true;
            hasSelection = true;
        }
        else if (ArgumentEquals(arg.Value, L"sort"))
        {
            m_sort = true;
        }
        else if (ArgumentEquals(arg.Value, L"legacy"))
        {
            m_legacy = true;
            hasSelection = true;
        }
        else if (ArgumentEquals(arg.Value, L"path"))
        {
            m_path = ParseArgument(it, args.end(), arg);
        }
        else if (ArgumentEquals(arg.Value, L"prerelease"))
        {
            m_prerelease = true;
            hasSelection = true;
        }
        else if (ArgumentEquals(arg.Value, L"format"))
        {
            auto format = ParseArgument(it, args.end(), arg);
            auto formatterIt = Formatter::Formatters.find(format);

            if (formatterIt != Formatter::Formatters.end())
            {
                m_format = format;
            }
            else
            {
                auto message = ResourceManager::FormatString(IDS_E_INVALIDFORMAT, format.c_str());
                throw win32_error(ERROR_INVALID_PARAMETER, message);
            }
        }
        else if (ArgumentEquals(arg.Value, L"property"))
        {
            if (m_find.length())
            {
                auto message = ResourceManager::FormatString(IDS_E_ARGINCOMPAT, L"property", L"find");
                throw win32_error(ERROR_INVALID_PARAMETER, message);
            }

            m_property = ParseArgument(it, args.end(), arg);
        }
        else if (ArgumentEquals(arg.Value, L"include"))
        {
            vector<wstring> include;
            ParseArgumentArray(it, args.end(), arg, include);

            for (const auto& value : include)
            {
                if (ArgumentEquals(value, L"packages"))
                {
                    m_includePackages = true;
                }
                else
                {
                    auto message = ResourceManager::FormatString(IDS_E_UNSUPPORTEDARG, value.c_str(), L"include");
                    throw win32_error(ERROR_INVALID_PARAMETER, message);
                }
                
            }
        }
        else if (ArgumentEquals(arg.Value, L"find"))
        {
            if (m_property.length())
            {
                auto message = ResourceManager::FormatString(IDS_E_ARGINCOMPAT, L"find", L"property");
                throw win32_error(ERROR_INVALID_PARAMETER, message);
            }

            m_find = ParseArgument(it, args.end(), arg);
        }
        else if (ArgumentEquals(arg.Value, L"nocolor"))
        {
            m_nocolor = true;
        }
        else if (ArgumentEquals(arg.Value, L"nologo"))
        {
            m_nologo = true;
        }
        else if (ArgumentEquals(arg.Value, L"utf8"))
        {
            m_utf8 = true;
        }
        else if (ArgumentEquals(arg.Value, L"?")
              || ArgumentEquals(arg.Value, L"h")
              || ArgumentEquals(arg.Value, L"help"))
        {
            m_help = true;
        }
        else
        {
            auto message = ResourceManager::FormatString(IDS_E_UNKNOWNPARAM, arg.Value.c_str());
            throw win32_error(ERROR_INVALID_PARAMETER, message);
        }
    }

    // Clear the products array with asterisk to find any product.
    auto it = find(m_products.begin(), m_products.end(), wstring(L"*"));
    if (it != m_products.end())
    {
        m_productsAll = true;
        m_products.clear();
    }

    if (m_legacy)
    {
        if (!m_products.empty() || !m_requires.empty())
        {
            auto message = ResourceManager::GetString(IDS_E_LEGACY);
            throw win32_error(ERROR_INVALID_PARAMETER, message);
        }
    }

    if ((m_property.length() || m_find.length()) && m_format.empty())
    {
        m_format = L"value";
    }

    if (hasSelection && !m_path.empty())
    {
        auto message = ResourceManager::GetString(IDS_E_PATHINCOMPATIBLE);
        throw win32_error(ERROR_INVALID_PARAMETER, message);
    }
}

void CommandArgs::Usage(_In_ Console& console) const
{
    auto pos = m_path.find_last_of(L"\\");
    auto& path = ++pos != wstring::npos ? m_path.substr(pos) : m_path;

    console.WriteLine(ResourceManager::FormatString(IDS_USAGE, path.c_str()));

    for (const auto& formatter : Formatter::Formatters)
    {
        UINT nID;

        tie(nID, ignore) = formatter.second;
        console.WriteLine(ResourceManager::FormatString(nID, formatter.first.c_str()));
    }
}

std::wregex CommandArgs::ParseRegex(_In_ const std::wstring& pattern) noexcept
{
    // Reserve ~125% of the incoming pattern to hold any changes.
    wstring accumulator;
    accumulator.reserve(pattern.size() * 1.25);

    for (auto it = pattern.begin(); it != pattern.end(); ++it)
    {
        switch (*it)
        {
        case L'.':
            accumulator += L"\\.";
            break;

        case L'*':
            accumulator += L".*";
            break;

        case L'?':
            accumulator += L".";
            break;

        default:
            accumulator += *it;
            break;
        }
    }

    return std::move(wregex(accumulator, wregex::basic | wregex::icase | wregex::nosubs));
}

static bool ArgumentEquals(_In_ const wstring& name, _In_ LPCWSTR expect)
{
    _ASSERT(expect && *expect);

    return CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, name.c_str(), -1, expect, -1);
}

template <class IteratorType>
static wstring ParseArgument(IteratorType& it, const IteratorType& end, const CommandParser::Token& arg)
{
    auto param = it->Value;

    ++it;

    // Require argument if the parameter is specified.
    if (it == end || CommandParser::Token::eArgument != it->Type)
    {
        auto message = ResourceManager::FormatString(IDS_E_ARGREQUIRED, param.c_str());
        throw win32_error(ERROR_INVALID_PARAMETER, message);
    }

    return it->Value;
}

template <class IteratorType>
static void ParseArgumentArray(IteratorType& it, const IteratorType& end, const CommandParser::Token& arg, vector<wstring>& arr)
{
    auto param = it->Value;
    auto nit = next(it);

    // Require arguments if the parameter is specified.
    if (nit == end || CommandParser::Token::eArgument != nit->Type)
    {
        auto message = ResourceManager::FormatString(IDS_E_ARGREQUIRED, param.c_str());
        throw win32_error(ERROR_INVALID_PARAMETER, message);
    }

    while (nit != end)
    {
        if (CommandParser::Token::eParameter == nit->Type)
        {
            break;
        }

        ++it;
        ++nit;

        arr.push_back(it->Value);
    }
}

template <class IteratorType>
static void ParseRequiresArray(IteratorType& it, const IteratorType& end, const CommandParser::Token& arg, vector<wstring>& literals, vector<wregex>& patterns)
{
    wstring& param = it->Value;
    auto nit = next(it);

    // Require arguments if the parameter is specified.
    if (nit == end || CommandParser::Token::eArgument != nit->Type)
    {
        auto message = ResourceManager::FormatString(IDS_E_ARGREQUIRED, param.c_str());
        throw win32_error(ERROR_INVALID_PARAMETER, message);
    }

    while (nit != end)
    {
        if (CommandParser::Token::eParameter == nit->Type)
        {
            break;
        }

        ++it;
        ++nit;

        if (it->Value.find(L'*', 0) == wstring::npos && it->Value.find(L'?', 0) == wstring::npos)
        {
            literals.push_back(it->Value);
        }
        else
        {
            auto pattern = CommandArgs::ParseRegex(it->Value);
            patterns.push_back(std::move(pattern));
        }
    }
}
