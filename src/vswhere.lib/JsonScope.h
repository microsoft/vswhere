// <copyright file="JsonScope.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class JsonScope :
    public Scope
{
public:
    static const std::wstring empty;

    JsonScope(_In_ const std::wstring& padding, _In_ const std::wstring& name = empty, _In_ bool writeEnd = false) :
        Scope(padding, name, writeEnd)
    {
    }

    JsonScope(_In_ const JsonScope& obj) :
        Scope(obj)
    {
    }

protected:
    void WriteStartImpl(_In_ Console& console) override
    {
        if (Name().empty())
        {
            console.Write(L"\n%ls{", Padding().c_str());
        }
        else
        {
            console.Write(L"\n%ls\"%ls\": {", Padding().c_str(), Name().c_str());
        }
    }

    void WriteEndImpl(_In_ Console& console) override
    {
        console.Write(L"\n%ls}", Padding().c_str());
    }
};
