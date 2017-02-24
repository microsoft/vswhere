// <copyright file="JsonFormatter.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

void JsonFormatter::StartArray(_In_ wostream& out)
{
    m_arrayStart = true;

    out << m_padding << L"[";
    Push();
}

void JsonFormatter::StartObject(_In_ wostream& out)
{
    m_objectStart = true;

    if (m_arrayStart)
    {
        out << endl;
    }
    else
    {
        out << L"," << endl;
    }

    out << m_padding << L"{" << endl;
    Push();

    m_arrayStart = false;
}

void JsonFormatter::WriteProperty(_In_ wostream& out, _In_ const wstring& name, _In_ const wstring& value)
{
    if (!m_objectStart)
    {
        out << L"," << endl;
    }

    auto escaped = replace_all(value, L"\\", L"\\\\");
    out << m_padding << L"\"" << name << L"\": \"" << escaped << L"\"";

    m_objectStart = false;
}

void JsonFormatter::WriteProperty(_In_ wostream& out, _In_ const wstring& name, _In_ bool value)
{
    if (!m_objectStart)
    {
        out << L"," << endl;
    }

    out << m_padding << L"\"" << name << L"\": " << (value ? L"true" : L"false");

    m_objectStart = false;
}

void JsonFormatter::WriteProperty(_In_ wostream& out, _In_ const wstring& name, _In_ long long value)
{
    if (!m_objectStart)
    {
        out << L"," << endl;
    }

    out << m_padding << L"\"" << name << L"\": " << value;

    m_objectStart = false;
}

void JsonFormatter::EndObject(_In_ wostream& out)
{
    Pop();
    out << endl << m_padding << L"}";
}

void JsonFormatter::EndArray(_In_ wostream& out)
{
    Pop();

    if (!m_arrayStart)
    {
        out << endl;
    }

    out << m_padding << L"]";
}

void JsonFormatter::EndDocument(_In_ wostream& out)
{
    out << endl;
}

wstring JsonFormatter::FormatDate(_In_ const FILETIME& value)
{
    return FormatDateISO8601(value);
}
