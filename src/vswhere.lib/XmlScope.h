// <copyright file="XmlScope.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class XmlScope :
    public Scope<XmlScope>
{
public:
    XmlScope(_In_opt_ XmlScope* pParent, _In_ ::Console& console, _In_ const std::wstring& padding, _In_ const std::wstring& name) :
        Scope(pParent, console, padding, name)
    {
    }

    XmlScope(_In_opt_ XmlScope* pParent, _In_ ::Console& console, _In_ std::wstring& padding, _In_ std::wstring::const_pointer name) :
        Scope(pParent, console, padding, name)
    {
    }

    XmlScope(_In_ const XmlScope& obj) :
        Scope(obj)
    {
    }

protected:
    void WriteStartImpl() override;
    void WriteEndImpl() override;
};
