// <copyright file="XmlFormatter.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class XmlFormatter :
    public Formatter
{
public:
    static std::unique_ptr<Formatter> Create()
    {
        return std::unique_ptr<XmlFormatter>(new XmlFormatter());
    }

    XmlFormatter() :
        Formatter()
    {
    }

    XmlFormatter(_In_ const XmlFormatter& obj) :
        Formatter(obj),
        m_padding(obj.m_padding),
        m_scopes(obj.m_scopes)
    {
    }

    bool ShowLogo() const override
    {
        return false;
    }

protected:
    void StartDocument(_In_ Console& console) override;
    void StartArray(_In_ Console& console, _In_opt_ const std::wstring& name = empty_wstring) override;
    void StartObject(_In_ Console& console, _In_opt_ const std::wstring& name = empty_wstring) override;
    void WriteProperty(_In_ Console& console, _In_ const std::wstring& name, _In_ const std::wstring& value) override;
    void EndObject(_In_ Console& console) override;
    void EndArray(_In_ Console& console) override;
    std::wstring FormatDate(_In_ const FILETIME& value) override;

private:
    static const size_t padding_size = 2;

    void Push();
    void Pop();

    void StartScope(_In_ Console& console, _In_opt_ const std::wstring& name, _In_ std::wstring::const_pointer fallback);
    void EndScope(_In_ Console& console);

    std::wstring m_padding;
    std::stack<XmlScope> m_scopes;
};
