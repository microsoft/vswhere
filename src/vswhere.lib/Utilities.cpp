// <copyright file="Utilities.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

std::string to_string(const std::wstring& value) {
    if (value.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, value.data(), (int)value.size(), NULL, 0, NULL, NULL);
    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, value.data(), (int)value.size(), result.data(), size_needed, NULL, NULL);
    return result;
}
