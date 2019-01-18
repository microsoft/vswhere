// <copyright file="Utilities.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

typedef std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> wstring_converter;

std::string to_string(const std::wstring& value);

struct ci_equal : public std::binary_function<std::wstring, std::wstring, bool>
{
    bool operator()(const std::wstring& lhs, const std::wstring& rhs) const
    {
        return CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, lhs.c_str(), lhs.size(), rhs.c_str(), rhs.size());
    }
};

struct ci_less : public std::binary_function<std::wstring, std::wstring, bool>
{
    bool operator()(const std::wstring& lhs, const std::wstring& rhs) const
    {
        return CSTR_EQUAL > ::CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, lhs.c_str(), lhs.size(), rhs.c_str(), rhs.size());
    }
};

template <class _Elem, class _Traits = std::char_traits<_Elem>>
static inline std::basic_string<_Elem, _Traits> replace_all(const std::basic_string<_Elem, _Traits>& source, const _Elem* from, const _Elem* to)
{
    std::basic_string<_Elem, _Traits> buffer;
    std::basic_string<_Elem, _Traits> _from(from);

    std::basic_string<_Elem, _Traits>::size_type pos = 0;
    std::basic_string<_Elem, _Traits>::size_type last = 0;

    while ((pos = source.find(from, last)) != std::basic_string<_Elem, _Traits>::npos)
    {
        buffer.append(source, last, pos - last);
        buffer += to;

        last = pos + _from.length();
    }

    buffer += source.substr(last);
    return buffer;
}
