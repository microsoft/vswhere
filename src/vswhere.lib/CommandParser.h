// <copyright file="CommandParser.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class CommandParser
{
public:
    struct Token
    {
        enum { eNone, eParameter, eArgument } Type;
        std::wstring Value;
    };

    CommandParser() noexcept
    {
    }

    CommandParser(const CommandParser& obj) :
        m_path(obj.m_path)
    {
    }

    const std::wstring& get_Path() const noexcept
    {
        return m_path;
    }

    std::vector<Token> Parse(_In_ LPCWSTR wszCommandLine);
    std::vector<Token> Parse(_In_ int argc, _In_ LPCWSTR argv[]);

private:
    std::wstring m_path;
};
