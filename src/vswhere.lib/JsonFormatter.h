// <copyright file="JsonFormatter.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class JsonFormatter :
    public Formatter
{
public:
    static std::unique_ptr<Formatter> Create()
    {
        return std::unique_ptr<JsonFormatter>(new JsonFormatter());
    }

    JsonFormatter() :
        Formatter(),
        m_arrayStart(false),
        m_objectStart(false)
    {
    }

    JsonFormatter(_In_ const JsonFormatter& obj) :
        Formatter(obj),
        m_arrayStart(obj.m_arrayStart),
        m_objectStart(obj.m_objectStart),
        m_padding(obj.m_padding)
    {
    }

    bool ShowLogo() const override
    {
        return false;
    }

protected:
    void StartArray(_In_ Console& console) override;
    void StartObject(_In_ Console& console) override;
    void WriteProperty(_In_ Console& console, _In_ const std::wstring& name, _In_ const std::wstring& value) override;
    void WriteProperty(_In_ Console& console, _In_ const std::wstring& name, _In_ bool value) override;
    void WriteProperty(_In_ Console& console, _In_ const std::wstring& name, _In_ long long value) override;
    void EndObject(_In_ Console& console) override;
    void EndArray(_In_ Console& console) override;
    void EndDocument(_In_ Console& console) override;
    std::wstring FormatDate(_In_ const FILETIME& value) override;

private:
    static const size_t padding_size = 2;

    void Push()
    {
        m_padding += std::wstring(padding_size, L' ');
    }

    void Pop()
    {
        if (m_padding.size() > 0)
        {
            m_padding.resize(m_padding.size() - padding_size);
        }
    }

    bool m_arrayStart;
    bool m_objectStart;
    std::wstring m_padding;
};
