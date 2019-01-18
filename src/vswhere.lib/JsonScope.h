// <copyright file="JsonScope.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class JsonScope :
    public Scope<JsonScope>
{
public:
    typedef enum { array, object } Type;

    JsonScope(_In_opt_ JsonScope* pParent, _In_ const std::wstring& padding, _In_ Type type, _In_ const std::wstring& name) :
        Scope(pParent, padding, name),
        m_type(type),
        m_requireSep(false)
    {
    }

    JsonScope(_In_ const JsonScope& obj) :
        Scope(obj),
        m_type(obj.m_type),
        m_requireSep(obj.m_requireSep)
    {
    }

    bool IsObject() const noexcept
    {
        return m_type == Type::object;
    }

    void StartScope(_In_ Console& console);
    void StartProperty(_In_ Console& console);

protected:
    void WriteStartImpl(_In_ Console& console) override;
    void WriteEndImpl(_In_ Console& console) override;

private:
    void RequireSeparator() noexcept;
    void WriteSeparator(_In_ Console& console);

    wchar_t StartChar() const noexcept
    {
        if (m_type == Type::array)
        {
            return L'[';
        }

        return L'{';
    }

    wchar_t EndChar() const noexcept
    {
        if (m_type == Type::array)
        {
            return L']';
        }

        return L'}';
    }

    const Type m_type;
    bool m_requireSep;
};
