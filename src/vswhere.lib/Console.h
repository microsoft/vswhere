// <copyright file="Console.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class Console
{
public:
    Console(_In_ const CommandArgs& args) :
        m_args(args),
        m_fInitialized(false)
    {
    }

    Console(_In_ const Console& obj) :
        m_args(obj.m_args),
        m_fInitialized(obj.m_fInitialized)
    {
    }

    void __cdecl Write(_In_ LPCWSTR wzFormat, ...);
    void __cdecl Write(_In_ const std::wstring& value);
    void __cdecl WriteLine(_In_ LPCWSTR wzFormat = NULL, ...);
    void __cdecl WriteLine(_In_ const std::wstring& value);

protected:
    virtual void Initialize() noexcept;
    virtual void Write(_In_ LPCWSTR wzFormat, va_list args);

private:
    bool IsConsole(_In_ FILE* f) const noexcept;

    const CommandArgs& m_args;
    bool m_fInitialized;
};