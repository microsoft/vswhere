// <copyright file="TextFormatter.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class TextFormatter :
    public Formatter
{
public:
    static std::unique_ptr<Formatter> Create()
    {
        return std::unique_ptr<TextFormatter>(new TextFormatter());
    }

    TextFormatter() :
        Formatter(),
        m_objectEnd(false)
    {
    }

    TextFormatter(_In_ const TextFormatter& obj) :
        Formatter(obj),
        m_objectEnd(obj.m_objectEnd)
    {
    }

    bool ShowLogo() const override
    {
        return true;
    }

protected:
    void StartObject(_In_ std::wostream& out) override;
    void WriteProperty(_In_ std::wostream& out, _In_ const std::wstring& name, _In_ const std::wstring& value) override;
    void EndObject(_In_ std::wostream& out) override;

private:
    bool m_objectEnd;
};
