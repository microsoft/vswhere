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
        m_first(false)
    {
    }

    TextFormatter(_In_ const TextFormatter& obj) :
        Formatter(obj),
        m_first(obj.m_first),
        m_objects(obj.m_objects)
    {
    }

protected:
    void StartArray(_In_ Console& console) override;
    void StartObject(_In_ Console& console, _In_opt_ const std::wstring& name = empty_wstring) override;
    void WriteProperty(_In_ Console& console, _In_ const std::wstring& name, _In_ const std::wstring& value) override;
    void EndObject(_In_ Console& console) override;

private:
    bool m_first;
    std::stack<std::wstring> m_objects;
};
