// <copyright file="TextFormatter.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

void PathOnlyFormatter::WriteProperty(_In_ std::wostream& out, _In_ const std::wstring& name, _In_ const std::wstring& value)
{
    if (m_pathWritten) return;
    if (CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, name.c_str(), -1, L"InstallationPath", -1))
    {
        out << value;
        m_pathWritten = true;
    }
}
