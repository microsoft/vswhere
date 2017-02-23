// <copyright file="TextFormatter.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

void TextFormatter::StartObject(_In_ std::wostream& out)
{
    if (m_objectEnd)
    {
        out << endl;
    }
}

void TextFormatter::WriteProperty(_In_ std::wostream& out, _In_ const std::wstring& name, _In_ const std::wstring& value)
{
    out << name << L": " << value << endl;
}

void TextFormatter::EndObject(_In_ std::wostream& out)
{
    m_objectEnd = true;
}
