// <copyright file="XmlFormatter.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

void XmlFormatter::StartDocument(_In_ Console& console)
{
    console.Write(L"<?xml version=\"1.0\"?>\n");
}

void XmlFormatter::StartArray(_In_ Console& console)
{
    console.Write(L"%ls<instances>\n", m_padding.c_str());
    Push();
}

void XmlFormatter::StartObject(_In_ Console& console)
{
    console.Write(L"%ls<instance>\n", m_padding.c_str());
    Push();
}

void XmlFormatter::WriteProperty(_In_ Console& console, _In_ const wstring& name, _In_ const wstring& value)
{
    console.Write(L"%ls<%ls>%ls</%ls>\n", m_padding.c_str(), name.c_str(), value.c_str(), name.c_str());
}

void XmlFormatter::EndObject(_In_ Console& console)
{
    Pop();
    console.Write(L"%ls</instance>\n", m_padding.c_str());
}

void XmlFormatter::EndArray(_In_ Console& console)
{
    Pop();
    console.Write(L"%ls</instances>\n", m_padding.c_str());
}

wstring XmlFormatter::FormatDate(_In_ const FILETIME& value)
{
    return FormatDateISO8601(value);
}
