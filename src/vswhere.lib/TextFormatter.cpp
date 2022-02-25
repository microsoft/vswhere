// <copyright file="TextFormatter.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

void TextFormatter::StartArray(_In_opt_ const std::wstring& name)
{
    m_first = true;
}

void TextFormatter::StartObject(_In_opt_ const wstring& name)
{
    if (!m_first && m_scopes.empty())
    {
        Console().WriteLine();
    }

    m_first = false;

    if (!m_scopes.empty())
    {
        m_scopes.push(m_scopes.top() + name + L"_");
    }
    else if (!name.empty())
    {
        m_scopes.push(name + L"_");
    }
    else
    {
        m_scopes.push(name);
    }
}

void TextFormatter::WriteProperty(_In_ const std::wstring& name, _In_ const std::wstring& value)
{
    wstring prefix = L"";
    if (!m_scopes.empty())
    {
        prefix = m_scopes.top();
    }

    Console().Write(L"%ls%ls%ls%ls: ", Console().Color(ColorName), prefix.c_str(), name.c_str(), Console().ResetColor());
    Console().WriteLine(L"%ls%ls%ls", Console().Color(ColorValue), value.c_str(), Console().ResetColor());
}

void TextFormatter::EndObject()
{
    m_scopes.pop();
}