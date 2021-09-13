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
        m_fInitialized(false),
        m_fColorSupported(false)
    {
    }

    Console(_In_ const Console& obj) :
        m_args(obj.m_args),
        m_fInitialized(obj.m_fInitialized),
        m_fColorSupported(obj.m_fColorSupported)
    {
    }

    LPCWSTR Color(_In_ LPCWSTR wzColor) const;
    LPCWSTR ResetColor() const;

    void __cdecl Write(_In_ LPCWSTR wzFormat, ...);
    void __cdecl Write(_In_ const std::wstring& value);
    void __cdecl WriteLine(_In_ LPCWSTR wzFormat = NULL, ...);
    void __cdecl WriteLine(_In_ const std::wstring& value);

    virtual bool IsColorSupported() const noexcept
    {
        return m_fColorSupported && m_args.get_Color();
    }

protected:
    virtual void Initialize() noexcept;
    virtual void Write(_In_ LPCWSTR wzFormat, va_list args);

    const CommandArgs& Args() const noexcept
    {
        return m_args;
    }

private:
    bool static IsConsole(_In_ FILE* f) noexcept;
    bool static IsVirtualTerminal(_In_ FILE* f) noexcept;

    const CommandArgs& m_args;
    bool m_fInitialized;
    bool m_fColorSupported;
};