// <copyright file="Formatter.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class Formatter
{
public:
    typedef std::map<std::wstring, std::function<std::unique_ptr<Formatter>()>, ci_less> FormatterMap;

    static std::unique_ptr<Formatter> Create(const std::wstring& type);
    static FormatterMap Formatters;

    Formatter(_In_ const Formatter& obj) :
        m_properties(obj.m_properties)
    {
    }

    void Write(_In_ std::wostream& out, _In_ ISetupInstance* pInstance);
    void Write(_In_ std::wostream& out, _In_ std::vector<ISetupInstancePtr> instances);

    virtual bool ShowLogo() const
    {
        return true;
    }

protected:
    typedef std::function<HRESULT(_In_ ISetupInstance*, _Out_ BSTR*)> PropertyFunction;
    typedef std::vector<std::pair<std::wstring, PropertyFunction>> PropertyArray;

    Formatter();

    static std::wstring FormatDateISO8601(_In_ const FILETIME& value);

    virtual void StartDocument(_In_ std::wostream& out) {}
    virtual void StartArray(_In_ std::wostream& out) {}
    virtual void StartObject(_In_ std::wostream& out) {}
    virtual void WriteProperty(_In_ std::wostream& out, _In_ const std::wstring& name, _In_ const std::wstring& value) {}
    virtual void EndObject(_In_ std::wostream& out) {}
    virtual void EndArray(_In_ std::wostream& out) {}
    virtual void EndDocument(_In_ std::wostream& out) {}

    virtual void WriteProperty(_In_ std::wostream& out, _In_ const std::wstring& name, _In_ bool value)
    {
        WriteProperty(out, name, std::to_wstring(value));
    }

    virtual void WriteProperty(_In_ std::wostream& out, _In_ const std::wstring& name, _In_ long long value)
    {
        WriteProperty(out, name, std::to_wstring(value));
    }

    virtual std::wstring FormatDate(_In_ const FILETIME& value);

private:
    HRESULT GetInstanceId(_In_ ISetupInstance* pInstance, _Out_ BSTR* pbstrInstanceId);
    HRESULT GetInstallDate(_In_ ISetupInstance* pInstance, _Out_ BSTR* pbstrInstallDate);
    HRESULT GetInstallationName(_In_ ISetupInstance* pInstance, _Out_ BSTR* pbstrInstallationName);
    HRESULT GetInstallationPath(_In_ ISetupInstance* pInstance, _Out_ BSTR* pbstrInstallationPath);
    HRESULT GetInstallationVersion(_In_ ISetupInstance* pInstance, _Out_ BSTR* pbstrInstallationVersion);
    HRESULT GetDisplayName(_In_ ISetupInstance* pInstance, _Out_ BSTR* pbstrDisplayName);
    HRESULT GetDescription(_In_ ISetupInstance* pInstance, _Out_ BSTR* pbstrDescription);

    void WriteInternal(_In_ std::wostream& out, _In_ ISetupInstance* pInstance);

    PropertyArray m_properties;
};
