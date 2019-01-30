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

void JsonFormatter::StartArray(_In_ Console& console, _In_opt_ const std::wstring& name)
{
    StartScope(console, JsonScope::Type::array, name);
}

void JsonFormatter::StartObject(_In_ Console& console, _In_opt_ const wstring& name)
{
    StartScope(console, JsonScope::Type::object, name);
}

void JsonFormatter::WriteProperty(_In_ Console& console, _In_ const wstring& name, _In_ const wstring& value)
{
    StartProperty(console, name);

    auto escaped = Escape(value);
    console.Write(L"\"%ls\"",escaped.c_str());
}

void JsonFormatter::WriteProperty(_In_ Console& console, _In_ const wstring& name, _In_ bool value)
{
    StartProperty(console, name);
    console.Write(value ? L"true" : L"false");
}

void JsonFormatter::WriteProperty(_In_ Console& console, _In_ const wstring& name, _In_ long long value)
{
    StartProperty(console, name);
    console.Write(L"%I64d", value);
}

void JsonFormatter::EndObject(_In_ Console& console)
{
    EndScope(console);
}

void JsonFormatter::EndArray(_In_ Console& console)
{
    EndScope(console);
}

void JsonFormatter::EndDocument(_In_ Console& console)
{
    console.WriteLine();
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

void JsonFormatter::StartScope(_In_ Console& console, _In_ JsonScope::Type type, _In_ const std::wstring& name)
{
    JsonScope* pParent = nullptr;
    if (m_scopes.size())
    {
        auto& top = m_scopes.top();
        top.StartScope(console);

        pParent = &top;
    }

    m_scopes.push(JsonScope(pParent, m_padding, type, name));

    // Always write the root scope.
    if (m_scopes.size() == 1)
    {
        m_scopes.top().WriteStart(console);
    }

    Push();
}

void JsonFormatter::StartProperty(_In_ Console& console, _In_ const std::wstring& name)
{
    m_scopes.top().StartProperty(console);

    console.Write(L"\n%ls", m_padding.c_str());
    if (m_scopes.top().IsObject())
    {
        console.Write(L"\"%ls\": ", name.c_str());
    }
}

void JsonFormatter::EndScope(_In_ Console& console)
{
    Pop();

    m_scopes.top().WriteEnd(console);
    m_scopes.pop();
}
