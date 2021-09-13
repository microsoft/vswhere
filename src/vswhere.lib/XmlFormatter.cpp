// <copyright file="XmlFormatter.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

void XmlFormatter::StartDocument()
{
    Console().WriteLine(L"<?xml version=\"1.0\"?>");
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
    Console().WriteLine(L"%1$ls<%2$ls>%3$ls</%2$ls>", m_padding.c_str(), name.c_str(), value.c_str());
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
