// <copyright file="TestConsole.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class TestConsole :
    public Console
{
public:
    TestConsole(_In_ const CommandArgs& args) :
        Console(args)
    {
    }

    TestConsole(_In_ const TestConsole& obj) :
        Console(obj),
        m_output(obj.m_output)
    {
    }

    void Initialize() noexcept override
    {
    }

    operator const wchar_t*() const
    {
        return m_output.c_str();
    }

protected:
    void Write(_In_ LPCWSTR wzFormat, va_list args) override;

private:
    std::wstring m_output;
};
