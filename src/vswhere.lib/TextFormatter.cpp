// <copyright file="TextFormatter.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

void TextFormatter::StartObject(_In_ Console& console)
{
    if (m_objectEnd)
    {
        console.WriteLine();
    }
}

void TextFormatter::WriteProperty(_In_ Console& console, _In_ const std::wstring& name, _In_ const std::wstring& value)
{
    console.WriteLine(L"%ls: %ls", name.c_str(), value.c_str());
}

void TextFormatter::EndObject(_In_ Console& console)
{
    m_objectEnd = true;
}
