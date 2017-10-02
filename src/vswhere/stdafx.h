// <copyright file="stdafx.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN

// Windows headers
#include <windows.h>

// STL headers
#include <iomanip>

// Project headers
#include <stdafx.h>
#include <vswhere.Version.rc>

_COM_SMARTPTR_TYPEDEF(ISetupConfiguration, __uuidof(ISetupConfiguration));
_COM_SMARTPTR_TYPEDEF(ISetupConfiguration2, __uuidof(ISetupConfiguration2));
