// <copyright file="Exceptions.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

wstring win32_error::format_message(_In_ int code)
{
    const size_t max = 65536;

    wstring err(max, wstring::value_type());
    if (auto ch = ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, code, 0, const_cast<LPWSTR>(err.c_str()), err.size(), NULL))
    {
        err.resize(ch);
        return err;
    }

    return L"unknown error";
}
