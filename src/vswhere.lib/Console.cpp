// <copyright file="Console.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

void Console::Initialize() noexcept
{
    if (!m_fInitialized)
    {
        if (m_args.get_UTF8())
        {
            static_cast<void>(::_setmode(_fileno(stdout), _O_U8TEXT));
        }
        else if (IsConsole(stdout))
        {
            static_cast<void>(::_setmode(_fileno(stdout), _O_WTEXT));
        }
        else
        {
            char sz[10];
            ::sprintf_s(sz, ".%u", ::GetConsoleCP());

            ::setlocale(LC_CTYPE, sz);
        }

        m_fColorSupported = IsVirtualTerminal(stdout);
        m_fInitialized = true;
    }
}

LPCWSTR Console::Color(_In_ LPCWSTR wzColor) const
{
    if (IsColorSupported())
    {
        return wzColor;
    }

    return L"";
}

LPCWSTR Console::ResetColor() const
{
    if (IsColorSupported())
    {
        return L"\033[0m";
    }

    return L"";
}

void __cdecl Console::Write(_In_ LPCWSTR wzFormat, ...)
{
    va_list args;

    va_start(args, wzFormat);
    Write(wzFormat, args);
    va_end(args);
}

void __cdecl Console::Write(_In_ const std::wstring& value)
{
    Write(value.c_str(), NULL);
}

void __cdecl Console::WriteLine(_In_opt_ LPCWSTR wzFormat, ...)
{
    if (wzFormat)
    {
        va_list args;

        va_start(args, wzFormat);
        Write(wzFormat, args);
        va_end(args);
    }

    Write(L"\n", NULL);
}

void __cdecl Console::WriteLine(_In_opt_ const std::wstring& value)
{
    Write(L"%ls\n", value.c_str());
}

void Console::Write(_In_ LPCWSTR wzFormat, va_list args)
{
    _ASSERTE(m_fInitialized);
    ::_vwprintf_p(wzFormat, args);
}

bool Console::IsConsole(_In_ FILE* f) noexcept
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

bool Console::IsVirtualTerminal(_In_ FILE* f) noexcept
{
    auto fno = ::_fileno(f);
    auto hFile = (HANDLE)::_get_osfhandle(fno);

    DWORD dwMode;
    if (::GetConsoleMode(hFile, &dwMode))
    {
        return 0 != ::SetConsoleMode(hFile, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }

    return false;
}