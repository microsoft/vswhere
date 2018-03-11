// <copyright file="Utf8Console.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>
#include "stdafx.h"
#include "Utf8Console.h"

namespace
{
    std::string WcsToMbsString(const wchar_t *wstr, int wlen, int cp)
    {
        auto len = WideCharToMultiByte(cp, 0, wstr, wlen, NULL, 0, NULL, NULL);
        std::vector<char> mbuf(len, 0);
        WideCharToMultiByte(cp, 0, wstr, wlen, mbuf.data(), len, NULL, NULL);
        return std::string(mbuf.begin(), mbuf.end());
    }
    bool IsOutputConsole(FILE *f)
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
}

void Utf8Console::Initialize() noexcept
{
    if (m_Output == NULL)
    {
        return;
    }
    if (IsOutputConsole(m_Output))
    {
        // _O_U8TEXT+_vfwprintf_p output string with console codepage(ex. cp932)
        ::_setmode(_fileno(m_Output), _O_TEXT);
    }
    else
    {
        char sz[10];
        ::sprintf_s(sz, ".%d", ::GetConsoleCP());

        ::setlocale(LC_CTYPE, sz);
    }
}

void Utf8Console::Write(_In_ LPCWSTR wzFormat, va_list args)
{
    if (m_Output == NULL)
    {
        throw std::runtime_error("output file handle is null");
    }
    auto wlen = ::_vscwprintf_p(wzFormat, args);
    std::vector<wchar_t> wbuf(wlen + 1, 0);
    ::_vswprintf_p(wbuf.data(), wlen + 1, wzFormat, args);
    auto mstr = WcsToMbsString(wbuf.data(), wlen, CP_UTF8);
    fprintf(m_Output, "%s", mstr.c_str());
}