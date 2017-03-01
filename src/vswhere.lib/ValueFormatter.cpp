// <copyright file="ValueFormatter.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

void ValueFormatter::WriteProperty(_In_ std::wostream& out, _In_ const std::wstring& name, _In_ const std::wstring& value)
{
    out << value << endl;
}
