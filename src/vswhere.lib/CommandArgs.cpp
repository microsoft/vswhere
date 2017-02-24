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

const vector<wstring> CommandArgs::s_Products
{
    L"Microsoft.VisualStudio.Product.Enterprise",
    L"Microsoft.VisualStudio.Product.Professional",
    L"Microsoft.VisualStudio.Product.Community",
};

void CommandArgs::Parse(_In_ LPCWSTR wszCommandLine)
{
    CommandParser parser;
    auto args = parser.Parse(wszCommandLine);

    m_path = parser.get_Path();

    Parse(args);
}

void CommandArgs::Parse(_In_ int argc, _In_ LPCWSTR argv[])
{
    CommandParser parser;
    auto args = parser.Parse(argc, argv);

    m_path = parser.get_Path();

    Parse(args);
}

void CommandArgs::Parse(_In_ vector<CommandParser::Token> args)
{
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
        }
        else if (ArgumentEquals(arg.Value, L"products"))
        {
            ParseArgumentArray(it, args.end(), arg, m_products);
        }
        else if (ArgumentEquals(arg.Value, L"requires"))
        {
            ParseArgumentArray(it, args.end(), arg, m_requires);
        }
        else if (ArgumentEquals(arg.Value, L"version"))
        {
            m_version = ParseArgument(it, args.end(), arg);
        }
        else if (ArgumentEquals(arg.Value, L"latest"))
        {
            m_latest = true;
        }
        else if (ArgumentEquals(arg.Value, L"format"))
        {
            auto format = ParseArgument(it, args.end(), arg);
            auto it = Formatter::Formatters.find(format);

            if (it != Formatter::Formatters.end())
            {
                m_format = format;
            }
            else
            {
                auto message = ResourceManager::FormatString(IDS_E_INVALIDFORMAT, format.c_str());
                throw win32_error(ERROR_INVALID_PARAMETER, message);
            }
        }
        else if (ArgumentEquals(arg.Value, L"nologo"))
        {
            m_nologo = true;
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
}

void CommandArgs::Usage(_In_ std::wostream& out) const
{
    auto pos = m_path.find_last_of(L"\\");
    auto path = ++pos != wstring::npos ? m_path.substr(pos) : m_path;
    out << ResourceManager::FormatString(IDS_USAGE, path.c_str()) << endl;

    for (const auto& formatter : Formatter::Formatters)
    {
        UINT nID;

        tie(nID, ignore) = formatter.second;
        out << ResourceManager::FormatString(nID, formatter.first.c_str()) << endl;
    }
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
