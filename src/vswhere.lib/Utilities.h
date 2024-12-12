// <copyright file="Utilities.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

std::string to_string(const std::wstring& value);

struct ci_equal
{
    bool operator()(const std::wstring& lhs, const std::wstring& rhs) const
    {
        return CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, lhs.c_str(), lhs.size(), rhs.c_str(), rhs.size());
    }
};

struct ci_less
{
    bool operator()(const std::wstring& lhs, const std::wstring& rhs) const
    {
        return CSTR_EQUAL > ::CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, lhs.c_str(), lhs.size(), rhs.c_str(), rhs.size());
    }
};
