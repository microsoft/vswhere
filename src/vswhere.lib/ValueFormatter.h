// <copyright file="ValueFormatter.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class ValueFormatter :
    public Formatter
{
public:
    static std::unique_ptr<Formatter> Create()
    {
        return std::unique_ptr<ValueFormatter>(new ValueFormatter());
    }

    ValueFormatter() :
        Formatter()
    {
    }

    ValueFormatter(_In_ const ValueFormatter& obj) :
        Formatter(obj)
    {
    }

    bool ShowLogo() const override
    {
        return false;
    }

protected:
    void WriteProperty(_In_ std::wostream& out, _In_ const std::wstring& name, _In_ const std::wstring& value) override;
};
