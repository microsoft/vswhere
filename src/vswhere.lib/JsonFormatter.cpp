// <copyright file="JsonFormatter.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

void JsonFormatter::StartArray(_In_ Console& console)
{
    m_requiresSep.push(false);

    console.Write(L"%ls[", m_padding.c_str());
    Push();
}

void JsonFormatter::StartObject(_In_ Console& console, _In_opt_ const wstring& name)
{
    if (m_requiresSep.top())
    {
        console.Write(L",");
    }
    else
    {
        // Do not write new line when starting subsequent object immediately after previous object.
        if (m_objects.empty())
        {
            m_requiresSep.top() = true;
        }
    }

    m_requiresSep.push(false);

    if (name.empty())
    {
        m_objects.push(JsonScope(m_padding, JsonScope::empty, true));
        m_objects.top().WriteStart(console);
    }
    else
    {
        m_objects.push(JsonScope(m_padding, name));
    }

    Push();
}

void JsonFormatter::WriteProperty(_In_ Console& console, _In_ const wstring& name, _In_ const wstring& value)
{
    StartProperty(console);

    auto escaped = replace_all(value, L"\\", L"\\\\");
    console.Write(L"\n%ls\"%ls\": \"%ls\"", m_padding.c_str(), name.c_str(), escaped.c_str());
}

void JsonFormatter::WriteProperty(_In_ Console& console, _In_ const wstring& name, _In_ bool value)
{
    StartProperty(console);
    console.Write(L"\n%ls\"%ls\": %ls", m_padding.c_str(), name.c_str(), (value ? L"true" : L"false"));
}

void JsonFormatter::WriteProperty(_In_ Console& console, _In_ const wstring& name, _In_ long long value)
{
    StartProperty(console);
    console.Write(L"\n%ls\"%ls\": %d", m_padding.c_str(), name.c_str(), value);
}

void JsonFormatter::EndObject(_In_ Console& console)
{
    Pop();

    m_requiresSep.pop();

    m_objects.top().WriteEnd(console);
    m_objects.pop();
}

void JsonFormatter::EndArray(_In_ Console& console)
{
    Pop();

    if (m_requiresSep.top())
    {
        console.WriteLine();
    }

    m_requiresSep.pop();

    console.Write(L"%ls]", m_padding.c_str());
}

void JsonFormatter::EndDocument(_In_ Console& console)
{
    console.WriteLine();
}

wstring JsonFormatter::FormatDate(_In_ const FILETIME& value)
{
    return FormatDateISO8601(value);
}

void JsonFormatter::StartProperty(_In_ Console& console)
{
    m_objects.top().WriteStart(console);

    if (m_requiresSep.top())
    {
        console.Write(L",");
    }
    else
    {
        m_requiresSep.top() = true;
    }
}
