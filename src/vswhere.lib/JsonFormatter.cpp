// <copyright file="JsonFormatter.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

wstring JsonFormatter::Escape(_In_ const wstring& value)
{
    wstring buffer;
    wstring::size_type pos = 0;
    wstring::size_type last = 0;

    while ((pos = value.find_first_of(L"\"\\", last)) != wstring::npos)
    {
        buffer.append(value, last, pos - last);
        buffer.push_back(L'\\');
        buffer.push_back(value[pos]);

        last = ++pos;
    }

    buffer += value.substr(last);
    return buffer;
}

void JsonFormatter::StartArray(_In_opt_ const std::wstring& name)
{
    StartScope(JsonScope::Type::array, name);
}

void JsonFormatter::StartObject(_In_opt_ const wstring& name)
{
    StartScope(JsonScope::Type::object, name);
}

void JsonFormatter::WriteProperty(_In_ const wstring& name, _In_ const wstring& value)
{
    StartProperty(name);

    auto escaped = Escape(value);
    Console().Write(L"\"%ls\"",escaped.c_str());
}

void JsonFormatter::WriteProperty(_In_ const wstring& name, _In_ bool value)
{
    StartProperty(name);
    Console().Write(value ? L"true" : L"false");
}

void JsonFormatter::WriteProperty(_In_ const wstring& name, _In_ long long value)
{
    StartProperty(name);
    Console().Write(L"%I64d", value);
}

void JsonFormatter::EndObject()
{
    EndScope();
}

void JsonFormatter::EndArray()
{
    EndScope();
}

void JsonFormatter::EndDocument()
{
    Console().WriteLine();
}

wstring JsonFormatter::FormatDate(_In_ const FILETIME& value)
{
    return FormatDateISO8601(value);
}

void JsonFormatter::Push()
{
    m_padding += std::wstring(padding_size, L' ');
}

void JsonFormatter::Pop()
{
    if (m_padding.size() > 0)
    {
        m_padding.resize(m_padding.size() - padding_size);
    }
}

void JsonFormatter::StartScope(_In_ JsonScope::Type type, _In_ const std::wstring& name)
{
    JsonScope* pParent = nullptr;
    if (m_scopes.size())
    {
        auto& top = m_scopes.top();
        top.StartScope();

        pParent = &top;
    }

    m_scopes.push(JsonScope(pParent, Console(), m_padding, type, name));

    // Always write the root scope.
    if (m_scopes.size() == 1)
    {
        m_scopes.top().WriteStart();
    }

    Push();
}

void JsonFormatter::StartProperty(_In_ const std::wstring& name)
{
    m_scopes.top().StartProperty();

    Console().Write(L"\n%ls", m_padding.c_str());
    if (m_scopes.top().IsObject())
    {
        Console().Write(L"\"%ls\": ", name.c_str());
    }
}

void JsonFormatter::EndScope()
{
    Pop();

    m_scopes.top().WriteEnd();
    m_scopes.pop();
}
