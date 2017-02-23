// <copyright file="CoInitializer.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class CoInitializer
{
public:
    CoInitializer()
    {
        auto hr = ::CoInitialize(NULL);
        if (FAILED(hr))
        {
            throw win32_error(hr);
        }
    }

    ~CoInitializer()
    {
        ::CoUninitialize();
    }
};
