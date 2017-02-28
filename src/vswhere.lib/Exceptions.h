// <copyright file="Exceptions.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class win32_error :
    public std::system_error
{
public:
    win32_error() :
        win32_error(::GetLastError())
    {
    }

    win32_error(_In_ int code, _In_ const std::string message = "") :
        system_error(code, std::system_category(), message),
        m_message(format_message(code))
    {
    }

    win32_error(_In_ int code, _In_ const std::wstring message) :
        system_error(code, std::system_category(), to_string(message)),
        m_message(message)
    {
    }

    const wchar_t* wwhat() const
    {
        return m_message.c_str();
    }

private:
    static std::wstring format_message(_In_ int code);

    std::wstring m_message;
};
