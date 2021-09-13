// <copyright file="XmlScope.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

void XmlScope::WriteStartImpl()
{
    Console().Write(Padding());
    Console().WriteLine(L"%ls<%ls>%ls", Console().Color(XmlFormatter::ColorTag), Name().c_str(), Console().ResetColor());
}

void XmlScope::WriteEndImpl()
{
    Console().Write(Padding());
    Console().WriteLine(L"%ls</%ls>%ls", Console().Color(XmlFormatter::ColorTag), Name().c_str(), Console().ResetColor());
}
