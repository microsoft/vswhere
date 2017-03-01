// <copyright file="XmlFormatter.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

void XmlFormatter::StartDocument(_In_ wostream& out)
{
    out << L"<?xml version=\"1.0\" encoding=\"utf-16\"?>" << endl;
}

void XmlFormatter::StartArray(_In_ wostream& out)
{
    out << m_padding << L"<instances>" << endl;
    Push();
}

void XmlFormatter::StartObject(_In_ wostream& out)
{
    out << m_padding << L"<instance>" << endl;
    Push();
}

void XmlFormatter::WriteProperty(_In_ wostream& out, _In_ const wstring& name, _In_ const wstring& value)
{
    out << m_padding << L"<" << name << L">" << value << L"</" << name << L">" << endl;
}

void XmlFormatter::EndObject(_In_ wostream& out)
{
    Pop();
    out << m_padding << L"</instance>" << endl;
}

void XmlFormatter::EndArray(_In_ wostream& out)
{
    Pop();
    out << m_padding << L"</instances>" << endl;
}

wstring XmlFormatter::FormatDate(_In_ const FILETIME& value)
{
    return FormatDateISO8601(value);
}
