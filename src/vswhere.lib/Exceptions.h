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
        system_error(code, std::system_category(), message)
    {
    }

    win32_error(_In_ int code, _In_ const std::wstring message) :
        system_error(code, std::system_category(), to_string(message))
    {
    }
};
