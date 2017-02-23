// <copyright file="ResourceManager.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

HINSTANCE ResourceManager::s_hInstance = ::GetModuleHandleW(NULL);

wstring ResourceManager::GetString(_In_ DWORD nID)
{
    LPCWSTR wsz = NULL;

    auto ch = ::LoadStringW(s_hInstance, nID, (LPWSTR)&wsz, 0);
    if (!ch)
    {
        throw win32_error();
    }

    return wstring(wsz, wsz + ch);
}

wstring ResourceManager::FormatString(_In_ DWORD nID, _In_ va_list args)
{
    auto fmt = GetString(nID);

    auto ch = _vscwprintf_p(fmt.c_str(), args);
    if (0 > ch)
    {
        throw win32_error(ERROR_INVALID_PARAMETER);
    }

    wstring wsz;
    wsz.resize(++ch);

    if (0 > _vswprintf_p(&wsz[0], ch, fmt.c_str(), args))
    {
        throw win32_error(ERROR_INVALID_PARAMETER);
    }

    // trim the terminating null
    wsz.resize(ch - 1);

    return wsz;
}
