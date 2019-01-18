// <copyright file="Scope.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

template <typename _Type>
class Scope
{
public:
    Scope(_In_opt_ _Type* pParent, _In_ const std::wstring& padding, _In_ const std::wstring& name) :
        m_pParent(pParent),
        m_padding(padding),
        m_name(name),
        m_writeStart(true),
        m_writeEnd(false)
    {
    }

    Scope(_In_opt_ _Type* pParent, _In_ std::wstring& padding, _In_ std::wstring::const_pointer name) :
        m_pParent(pParent),
        m_padding(padding),
        m_name(name),
        m_writeStart(true),
        m_writeEnd(false)
    {
    }

    Scope(_In_ const Scope& obj) :
        m_pParent(obj.m_pParent),
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
            // Write the parent scope first (may have already been written to console).
            if (m_pParent)
            {
                m_pParent->WriteStart(console);
            }

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
    _Type* Parent() const noexcept
    {
        return m_pParent;
    }

    const std::wstring& Padding() const noexcept
    {
        return m_padding;
    }

    const std::wstring& Name() const noexcept
    {
        return m_name;
    }

    virtual void WriteStartImpl(_In_ Console& console) = 0;
    virtual void WriteEndImpl(_In_ Console& console) = 0;

private:
    _Type* m_pParent;
    const std::wstring m_padding;
    const std::wstring m_name;
    bool m_writeStart;
    bool m_writeEnd;
};
