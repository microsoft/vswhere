// <copyright file="Utilities.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

std::string to_string(const std::wstring& value)
{
    static wstring_converter converter;

    return converter.to_bytes(value);
}
