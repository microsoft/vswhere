// <copyright file="XmlScope.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class XmlScope :
    public Scope
{
public:
    XmlScope(_In_ const std::wstring& padding, _In_ const std::wstring& name, _In_ bool writeEnd = false) :
        Scope(padding, name, writeEnd)
    {
    }

    XmlScope(_In_ std::wstring& padding, _In_ std::wstring::const_pointer name, _In_ bool writeEnd = false) :
        Scope(padding, name, writeEnd)
    {
    }

    XmlScope(_In_ const XmlScope& obj) :
        Scope(obj)
    {
    }

protected:
    void WriteStartImpl(_In_ Console& console) override
    {
        console.WriteLine(L"%ls<%ls>", Padding().c_str(), Name().c_str());
    }

    void WriteEndImpl(_In_ Console& console) override
    {
        console.WriteLine(L"%ls</%ls>", Padding().c_str(), Name().c_str());
    }
};
