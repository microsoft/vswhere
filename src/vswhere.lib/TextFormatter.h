// <copyright file="TextFormatter.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class TextFormatter :
    public Formatter
{
public:
    static std::unique_ptr<Formatter> Create(_In_ CommandArgs& args, _In_ ::Console& console)
    {
        return std::unique_ptr<TextFormatter>(new TextFormatter(args, console));
    }

    TextFormatter(_In_ CommandArgs& args, _In_ ::Console& console) :
        Formatter(args, console),
        m_first(false)
    {
    }

    TextFormatter(_In_ const TextFormatter& obj) :
        Formatter(obj),
        m_first(obj.m_first),
        m_scopes(obj.m_scopes)
    {
    }

protected:
    void StartArray(_In_opt_ const std::wstring& name = empty_wstring) override;
    void StartObject(_In_opt_ const std::wstring& name = empty_wstring) override;
    void WriteProperty(_In_ const std::wstring& name, _In_ const std::wstring& value) override;
    void EndObject() override;

private:
    bool m_first;
    std::stack<std::wstring> m_scopes;
};
