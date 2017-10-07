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
        console.WriteLine(L",");
    }
    else
    {
        // Do not write new line when starting subsequent object immediately after previous object.
        if (m_objects.empty())
        {
            console.WriteLine();
        }

        m_requiresSep.top() = true;
    }

    m_requiresSep.push(false);
    m_objects.push(name);

    if (!name.empty())
    {
        console.WriteLine(L"%ls\"%ls\": {", m_padding.c_str(), name.c_str());
    }
    else
    {
        console.WriteLine(L"%ls{", m_padding.c_str());
    }

    Push();
}

void JsonFormatter::WriteProperty(_In_ Console& console, _In_ const wstring& name, _In_ const wstring& value)
{
    if (m_requiresSep.top())
    {
        console.WriteLine(L",");
    }
    else
    {
        m_requiresSep.top() = true;
    }

    auto escaped = replace_all(value, L"\\", L"\\\\");
    console.Write(L"%ls\"%ls\": \"%ls\"", m_padding.c_str(), name.c_str(), escaped.c_str());
}

void JsonFormatter::WriteProperty(_In_ Console& console, _In_ const wstring& name, _In_ bool value)
{
    if (m_requiresSep.top())
    {
        console.WriteLine(L",");
    }
    else
    {
        m_requiresSep.top() = true;
    }

    console.Write(L"%ls\"%ls\": %ls", m_padding.c_str(), name.c_str(), (value ? L"true" : L"false"));
}

void JsonFormatter::WriteProperty(_In_ Console& console, _In_ const wstring& name, _In_ long long value)
{
    if (m_requiresSep.top())
    {
        console.WriteLine(L",");
    }
    else
    {
        m_requiresSep.top() = true;
    }

    console.Write(L"%ls\"%ls\": %d", m_padding.c_str(), name.c_str(), value);
}

void JsonFormatter::EndObject(_In_ Console& console)
{
    Pop();

    m_requiresSep.pop();
    m_objects.pop();

    console.Write(L"\n%ls}", m_padding.c_str());
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
