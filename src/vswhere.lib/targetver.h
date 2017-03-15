// <copyright file="targetver.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

#include <winsdkver.h>

#ifdef  _WIN32_WINNT
#undef  _WIN32_WINNT
#endif

#define _WIN32_WINNT 0x0501

#include <sdkddkver.h>
