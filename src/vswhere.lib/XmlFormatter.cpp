// <copyright file="XmlFormatter.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

const LPCWSTR XmlFormatter::ColorTag = L"\033[38;2;86;156;214m";

void XmlFormatter::StartDocument()
{
    Console().WriteLine(
        L"%1$ls<?xml%4$ls %2$lsversion%4$ls=%3$ls\"1.0\"%1$ls?>%4$ls",
        Console().Color(ColorTag),
        Console().Color(ColorName),
        Console().Color(ColorValue),
        Console().ResetColor());
}

void XmlFormatter::StartArray(_In_opt_ const wstring& name)
{
    StartScope(name, L"instances");
}

void XmlFormatter::StartObject(_In_opt_ const wstring& name)
{
    StartScope(name, L"instance");
}

void XmlFormatter::WriteProperty(_In_ const wstring& name, _In_ const wstring& value)
{
    m_scopes.top().WriteStart();

    Console().WriteLine(
        L"%1$ls%4$ls<%2$ls>%5$ls%3$ls%4$ls</%2$ls>%5$ls",
        m_padding.c_str(),
        name.c_str(),
        value.c_str(),
        Console().Color(ColorTag),
        Console().ResetColor());
}

void XmlFormatter::EndObject()
{
    EndScope();
}

void XmlFormatter::EndArray()
{
    EndScope();
}

wstring XmlFormatter::FormatDate(_In_ const FILETIME& value)
{
    return FormatDateISO8601(value);
}

void XmlFormatter::Push()
{
    m_padding += std::wstring(padding_size, L' ');
}

void XmlFormatter::Pop()
{
    if (m_padding.size() > 0)
    {
        m_padding.resize(m_padding.size() - padding_size);
    }
}

void XmlFormatter::StartScope(_In_opt_ const wstring& name, _In_ std::wstring::const_pointer fallback)
{
    XmlScope* pParent = nullptr;
    if (m_scopes.size())
    {
        pParent = &m_scopes.top();
    }

    if (name.empty())
    {
        m_scopes.push(XmlScope(pParent, Console(), m_padding, fallback));
    }
    else
    {
        m_scopes.push(XmlScope(pParent, Console(), m_padding, name));
    }

    // Always write the root scope.
    if (m_scopes.size() == 1)
    {
        m_scopes.top().WriteStart();
    }

    Push();
}

void XmlFormatter::EndScope()
{
    Pop();

    m_scopes.top().WriteEnd();
    m_scopes.pop();
}
