// <copyright file="Module.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

wstring __cdecl format(_In_ LPCWSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    auto ch = _vscwprintf_p(fmt, args);
    if (0 > ch)
    {
        throw win32_error(ERROR_INVALID_PARAMETER);
    }

    wstring wsz;
    wsz.resize(++ch);

    if (0 > _vswprintf_p(&wsz[0], ch, fmt, args))
    {
        throw win32_error(ERROR_INVALID_PARAMETER);
    }

    va_end(args);

    // trim the terminating null
    wsz.resize(ch - 1);

    return wsz;
}

TEST_MODULE_INITIALIZE(ModuleInitialize)
{
    #pragma warning(suppress: 6387) // Ignore potential failure when getting module handle
    ResourceManager::SetInstance(::GetModuleHandleW(L"vswhere.test.dll"));
}
