// <copyright file="CommandParser.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

vector<CommandParser::Token> CommandParser::Parse(_In_ LPCWSTR wszCommandLine)
{
    _ASSERT(wszCommandLine && *wszCommandLine);

    int argc = 0;
    auto argv = ::CommandLineToArgvW(wszCommandLine, &argc);

    if (!argv)
    {
        throw win32_error();
    }

    // Make sure the argument array is released when it falls out of scope.
    unique_ptr<LPWSTR*, void(*)(LPWSTR**)> args(&argv, [](_In_opt_ LPWSTR** ppwsz)
    {
        if (ppwsz)
        {
            ::LocalFree(*ppwsz);
        }
    });

    return Parse(argc, const_cast<LPCWSTR*>(*args));
}

vector<CommandParser::Token> CommandParser::Parse(_In_ int argc, _In_ LPCWSTR argv[])
{
    vector<CommandParser::Token> tokens;

    // Parse program path from first argument.
    if (argc < 1)
    {
        // TODO: Provide localized error message.
        throw win32_error(ERROR_INVALID_PARAMETER, "missing program argument");
    }

    m_path = argv[0];

    // Parse remaining arguments.
    for (auto i = 1; i < argc; ++i)
    {
        auto arg = argv[i];

        if (!arg || !*arg)
        {
            // TODO: Provide localized error message.
            throw win32_error(ERROR_INVALID_PARAMETER, "empty argument");
        }

        if (L'-' == arg[0] || L'/' == arg[0])
        {
            tokens.push_back({ Token::eParameter, &arg[1] });
        }
        else
        {
            tokens.push_back({ Token::eArgument, arg });
        }
    }

    return tokens;
}
