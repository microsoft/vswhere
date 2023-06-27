// <copyright file="Glob.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using namespace std::experimental::filesystem;

std::wstring rtrim(const std::wstring& value);

Glob::Glob(_In_ const wstring& root, _In_ const wstring& pattern) :
    m_root(rtrim(root))
{
    wstring value;
    wstring value_raw;
    wstring accumulator = L"^\\\\";
    bool found_globstar = false;
    bool found_wildcard = false;

    wchar_t lastch = 0;
    for (auto ch : pattern)
    {
        switch (ch)
        {
        // Append directories to root until wildcard found, then accumulate to optimize match.
        case L'/':
        case L'\\':
            if (lastch == L'/' || lastch == L'\\')
            {
                // Ignore subsequent directory separators.
                continue;
            }

            if (value.length())
            {
                if (found_globstar)
                {
                    // Replace globstar with any character match plus directory separator.
                    accumulator += L"(.*\\\\)?";
                }
                else if (value_raw.length() == 1 && value_raw == L".")
                {
                    // Ignore current directory references.
                    value.clear();
                    value_raw.clear();

                    continue;
                }
                else if (value_raw.length() == 2 && value_raw == L"..")
                {
                    // Block parent directory references.
                    ThrowError(pattern);
                }
                else if (found_wildcard)
                {
                    // A single star (asterisk) searches the any subdirectory not including the current directory.
                    if (value.length() == 1 && value[0] == L'*')
                    {
                        accumulator += L"[^\\]*";
                    }
                    else
                    {
                        accumulator += value;
                    }
                }
                else
                {
                    m_root /= value_raw;
                }

                value.clear();
                value_raw.clear();
            }

            if (found_wildcard)
            {
                if (found_globstar)
                {
                    found_globstar = false;
                }
                else
                {
                    // Only match directory separator if we didn't find a globstar.
                    accumulator += L"\\\\";
                }
            }

            break;

        case L'*':
            if (found_globstar)
            {
                ThrowError(pattern);
            }

            found_wildcard = true;

            if (value.length() == 1 && value[0] == L'*')
            {
                found_globstar = true;
                value += L'*';
            }
            else if (value.length() > 0)
            {
                if (lastch == L'*')
                {
                    // Invalid trailing globstar after non-separator.
                    ThrowError(pattern);
                }

                value += L"[^\\]*";
            }
            else
            {
                value += L'*';
            }
            break;

        // Convert single character match.
        case L'?':
            if (found_globstar)
            {
                ThrowError(pattern);
            }

            found_wildcard = true;
            value += L"[^\\]";
            break;

        default:
            if (found_globstar || IsInvalid(ch))
            {
                // Invalid leading globstar before non-separator or invalid character.
                ThrowError(pattern);
            }
            else if (found_wildcard)
            {
                // A single star (asterisk) searches the any subdirectory not including the current directory.
                if (value.length() == 1 && value[0] == L'*')
                {
                    accumulator += L"[^\\]*";
                }
                else
                {
                    accumulator += value;
                }

                value.clear();
            }

            if (RequiresEscape(ch))
            {
                value += L'\\';
            }

            value += ch;
            value_raw += ch;

            break;
        }

        lastch = ch;
    }

    // Do not skip the directory separator: verify it as part of the regex.
    m_root_length = m_root.native().length();

    if (value.length())
    {
        if (found_globstar && value.length() == 2)
        {
            // Match anything after the directory separator for a trailing globstar.
            accumulator += L".*";
        }
        else
        {
            accumulator += value;
        }
    }

    accumulator += L"$";

#ifdef _DEBUG
    m_pattern = accumulator;
#endif

    try
    {
        m_re = wregex(accumulator, wregex::extended | wregex::icase | wregex::nosubs | wregex::optimize);
    }
    catch (const regex_error& ex)
    {
        _RPTN(_CRT_ERROR, "regex parse error: %s", ex.what());
        ThrowError(pattern);
    }
}

const vector<wstring> Glob::Entries(_In_ bool sort) const
{
    vector<wstring> entries;

    const auto& root = m_root.native();
    for (const auto& entry : recursive_directory_iterator(m_root))
    {
        const auto& path = entry.path().native();
        if (Match(path))
        {
            entries.push_back(path);
        }
    }

    if (sort)
    {
        static ci_less less;
        std::sort(entries.begin(), entries.end(), less);
    }

    return entries;
}

bool Glob::Match(_In_ const wstring& value) const
{
    if (m_root_length < value.length())
    {
        return regex_match(value.begin() + m_root_length, value.end(), m_re);
    }

    return false;
}

bool Glob::IsInvalid(_In_ const wchar_t ch)
{
    return ch < 32 || ch == 124;
}

bool Glob::RequiresEscape(_In_ const wchar_t ch)
{
    static const wchar_t escape[] =
    {
        L'.',
        L'(',
        L')',
        L'$',
        L'[',
        L']',
        L'{',
        L'}',
        L'+',
    };
    static const size_t escape_len = sizeof(escape) / sizeof(*escape);

    for (unsigned char i = 0; i < escape_len; ++i)
    {
        if (ch == escape[i])
        {
            return true;
        }
    }

    return false;
}

void Glob::ThrowError(_In_ const wstring& pattern)
{
    auto message = ResourceManager::FormatString(IDS_E_INVALIDPATTERN, pattern.c_str());
    throw win32_error(ERROR_INVALID_PARAMETER, message);
}

std::wstring rtrim(const std::wstring& value)
{
    if (value.back() != L'\\' && value.back() != L'/')
    {
        return value;
    }

    wstring copy(value);
    wstring::reverse_iterator it = copy.rbegin();
    while (it != copy.rend() && (*it == L'\\' || *it == L'/'))
    {
        it++;
    }

    copy.erase(it.base(), copy.end());
    return copy;
}