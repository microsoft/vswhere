// <copyright file="TextFormatter.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

void TextFormatter::StartArray(_In_ Console& console)
{
    m_first = true;
}

void TextFormatter::StartObject(_In_ Console& console, _In_opt_ const wstring& name)
{
    if (!m_first && m_objects.empty())
    {
        console.WriteLine();
    }

    m_first = false;

    if (!m_objects.empty())
    {
        m_objects.push(m_objects.top() + name + L"_");
    }
    else if (!name.empty())
    {
        m_objects.push(name + L"_");
    }
    else
    {
        m_objects.push(name);
    }
}

void TextFormatter::WriteProperty(_In_ Console& console, _In_ const std::wstring& name, _In_ const std::wstring& value)
{
    wstring prefix = L"";
    if (!m_objects.empty())
    {
        prefix = m_objects.top();
    }

    console.WriteLine(L"%ls%ls: %ls", prefix.c_str(), name.c_str(), value.c_str());
}

void TextFormatter::EndObject(_In_ Console& console)
{
    m_objects.pop();
}
