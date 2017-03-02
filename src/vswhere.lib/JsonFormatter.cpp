// <copyright file="JsonFormatter.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

void JsonFormatter::StartArray(_In_ Console& console)
{
    m_arrayStart = true;

    console.Write(L"%ls[", m_padding.c_str());
    Push();
}

void JsonFormatter::StartObject(_In_ Console& console)
{
    m_objectStart = true;

    if (m_arrayStart)
    {
        console.WriteLine();
    }
    else
    {
        console.Write(L",\n");
    }

    console.Write(L"%ls{\n", m_padding.c_str());
    Push();

    m_arrayStart = false;
}

void JsonFormatter::WriteProperty(_In_ Console& console, _In_ const wstring& name, _In_ const wstring& value)
{
    if (!m_objectStart)
    {
        console.Write(L",\n");
    }

    auto escaped = replace_all(value, L"\\", L"\\\\");
    console.Write(L"%ls\"%ls\": \"%ls\"", m_padding.c_str(), name.c_str(), escaped.c_str());

    m_objectStart = false;
}

void JsonFormatter::WriteProperty(_In_ Console& console, _In_ const wstring& name, _In_ bool value)
{
    if (!m_objectStart)
    {
        console.Write(L",\n");
    }

    console.Write(L"%ls\"%ls\": %ls", m_padding.c_str(), name.c_str(), (value ? L"true" : L"false"));

    m_objectStart = false;
}

void JsonFormatter::WriteProperty(_In_ Console& console, _In_ const wstring& name, _In_ long long value)
{
    if (!m_objectStart)
    {
        console.Write(L",\n");
    }

    console.Write(L"%ls\"%ls\": %d", m_padding.c_str(), name.c_str(), value);

    m_objectStart = false;
}

void JsonFormatter::EndObject(_In_ Console& console)
{
    Pop();
    console.Write(L"\n%ls}", m_padding.c_str());
}

void JsonFormatter::EndArray(_In_ Console& console)
{
    Pop();

    if (!m_arrayStart)
    {
        console.WriteLine();
    }

    console.Write(L"%ls]", m_padding.c_str());
}

void JsonFormatter::EndDocument(_In_ Console& console)
{
    console.Write(L"\n");
}

wstring JsonFormatter::FormatDate(_In_ const FILETIME& value)
{
    return FormatDateISO8601(value);
}
