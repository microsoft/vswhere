// <copyright file="Console.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

void Console::Initialize() noexcept
{
    if (IsConsole(stdout))
    {
        ::_setmode(_fileno(stdout), _O_WTEXT);
    }
    else
    {
        char sz[10];
        ::sprintf_s(sz, ".%d", ::GetConsoleCP());

        ::setlocale(LC_CTYPE, sz);
    }
}

void Console::Write(_In_ const std::wstring& value)
{
    Write(value.c_str(), NULL);
}

void Console::Write(_In_ LPCWSTR wzFormat, ...)
{
    va_list args;

    va_start(args, wzFormat);
    Write(wzFormat, args);
    va_end(args);
}

void Console::WriteLine()
{
    Write(L"\n", NULL);
}

void Console::WriteLine(_In_ const std::wstring& value)
{
    Write(L"%ls\n", value.c_str());
}

void __cdecl Console::Write(_In_ LPCWSTR wzFormat, va_list args)
{
    ::vwprintf_s(wzFormat, args);
}

bool Console::IsConsole(_In_ FILE* f) const noexcept
{
    auto fno = ::_fileno(f);
    auto hFile = (HANDLE)::_get_osfhandle(fno);
    auto dwType = ::GetFileType(hFile);

    dwType &= ~FILE_TYPE_REMOTE;

    if (FILE_TYPE_CHAR != dwType)
    {
        return false;
    }

    DWORD dwMode;
    if (!::GetConsoleMode(hFile, &dwMode))
    {
        return false;
    }

    return true;
}
