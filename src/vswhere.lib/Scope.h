// <copyright file="Scope.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class Scope
{
public:
    Scope(_In_ const std::wstring& padding, _In_ const std::wstring& name, _In_ bool writeEnd = false) :
        m_padding(padding),
        m_name(name),
        m_writeStart(true),
        m_writeEnd(writeEnd)
    {
    }

    Scope(_In_ std::wstring& padding, _In_ std::wstring::const_pointer name, _In_ bool writeEnd = false) :
        m_padding(padding),
        m_name(name),
        m_writeStart(true),
        m_writeEnd(writeEnd)
    {
    }

    Scope(_In_ const Scope& obj) :
        m_padding(obj.m_padding),
        m_name(obj.m_name),
        m_writeStart(obj.m_writeStart),
        m_writeEnd(obj.m_writeEnd)
    {
    }

    void WriteStart(_In_ Console& console)
    {
        if (m_writeStart)
        {
            WriteStartImpl(console);
            m_writeStart = false;
            m_writeEnd = true;
        }
    }

    void WriteEnd(_In_ Console& console)
    {
        if (m_writeEnd)
        {
            WriteEndImpl(console);
        }
    }

protected:
    const std::wstring& Padding() const
    {
        return m_padding;
    }

    const std::wstring& Name() const
    {
        return m_name;
    }

    virtual void WriteStartImpl(_In_ Console& console) = 0;
    virtual void WriteEndImpl(_In_ Console& console) = 0;

private:
    std::wstring m_padding;
    std::wstring m_name;
    bool m_writeStart;
    bool m_writeEnd;
};
