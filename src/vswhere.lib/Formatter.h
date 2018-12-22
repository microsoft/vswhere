// <copyright file="Formatter.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class Formatter
{
public:
    typedef std::function<std::unique_ptr<Formatter>()> FormatterFactory;
    typedef std::map<std::wstring, std::tuple<UINT, FormatterFactory>, ci_less> FormatterMap;

    static std::unique_ptr<Formatter> Create(const std::wstring& type);
    static FormatterMap Formatters;

    Formatter(_In_ const Formatter& obj) :
        m_properties(obj.m_properties)
    {
    }

    void Write(_In_ const CommandArgs& args, _In_ Console& console, _In_ ISetupInstance* pInstance);
    void Write(_In_ const CommandArgs& args, _In_ Console& console, _In_ std::vector<ISetupInstancePtr> instances);

    virtual bool ShowLogo() const
    {
        return true;
    }

protected:
    typedef std::function<HRESULT(_In_ ISetupInstance*, _Out_ VARIANT*)> PropertyFunction;
    typedef std::vector<std::pair<std::wstring, PropertyFunction>> PropertyArray;

    static const std::wstring empty_wstring;

    Formatter();

    static std::wstring FormatDateISO8601(_In_ const FILETIME& value);

    virtual void StartDocument(_In_ Console& console) {}
    virtual void StartArray(_In_ Console& console) {}
    virtual void StartObject(_In_ Console& console, _In_opt_ const std::wstring& name = empty_wstring) {}
    virtual void WriteProperty(_In_ Console& console, _In_ const std::wstring& name, _In_ const std::wstring& value) {}
    virtual void EndObject(_In_ Console& console) {}
    virtual void EndArray(_In_ Console& console) {}
    virtual void EndDocument(_In_ Console& console) {}

    virtual void WriteProperty(_In_ Console& console, _In_ const std::wstring& name, _In_ bool value)
    {
        WriteProperty(console, name, std::to_wstring(value));
    }

    virtual void WriteProperty(_In_ Console& console, _In_ const std::wstring& name, _In_ long long value)
    {
        WriteProperty(console, name, std::to_wstring(value));
    }

    virtual std::wstring FormatDate(_In_ const FILETIME& value);

private:
    static bool PropertyEqual(_In_ const std::wstring& name, _In_ PropertyArray::const_reference property);
    static HRESULT GetStringProperty(_In_ std::function<HRESULT(_Out_ BSTR*)> pfn, _Out_ VARIANT* pvt);

    static const std::wstring s_delims;
    static ci_equal s_comparer;

    HRESULT GetInstanceId(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtInstanceId);
    HRESULT GetInstallDate(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtInstallDate);
    HRESULT GetInstallationName(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtInstallationName);
    HRESULT GetInstallationPath(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtInstallationPath);
    HRESULT GetInstallationVersion(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtInstallationVersion);
    HRESULT GetProductId(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtProductId);
    HRESULT GetProductPath(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtProductPath);
    HRESULT GetState(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtState);
    HRESULT GetIsComplete(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtIsComplete);
    HRESULT GetIsLaunchable(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtIsLaunchable);
    HRESULT GetIsPrerelease(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtIsPrerelease);
    HRESULT GetDisplayName(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtDisplayName);
    HRESULT GetDescription(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtDescription);

    void WriteInternal(_In_ const CommandArgs& args, _In_ Console& console, _In_ ISetupInstance* pInstance);
    void WriteProperty(_In_ Console& console, _In_ const std::wstring& name, _In_ const variant_t& value);
    bool WriteProperties(_In_ const CommandArgs& args, _In_ Console& console, _In_ ISetupInstance* pInstance);
    bool WriteProperties(_In_ const CommandArgs& args, _In_ Console& console, _In_ ISetupPropertyStore* pProperties, _In_opt_ const std::wstring& prefix = empty_wstring);

    PropertyArray m_properties;
};
