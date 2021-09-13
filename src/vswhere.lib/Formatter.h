// <copyright file="Formatter.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class Formatter
{
public:
    typedef std::function<std::unique_ptr<Formatter>(CommandArgs&, Console&)> FormatterFactory;
    typedef std::map<std::wstring, std::tuple<UINT, FormatterFactory>, ci_less> FormatterMap;

    static std::unique_ptr<Formatter> Create(_In_ const std::wstring& type, _In_ CommandArgs& args, _In_ Console& console);
    static FormatterMap Formatters;

    Formatter(_In_ const Formatter& obj) :
        m_args(obj.m_args),
        m_console(obj.m_console),
        m_properties(obj.m_properties)
    {
    }

    void Write(_In_ const std::wstring& root, _In_ const std::wstring& name, _In_ const std::vector<std::wstring> values);
    void Write(_In_ ISetupInstance* pInstance);
    void Write(_In_ std::vector<ISetupInstancePtr> instances);
    void WriteFiles(_In_ std::vector<ISetupInstancePtr> instances);

    virtual bool ShowLogo() const
    {
        return true;
    }

    virtual bool SupportsPackages() const
    {
        return false;
    }

protected:
    typedef std::function<HRESULT(_In_ ISetupInstance*, _Out_ VARIANT*)> PropertyFunction;
    typedef std::vector<std::pair<std::wstring, PropertyFunction>> PropertyArray;

    static const std::wstring empty_wstring;

    Formatter(_In_ CommandArgs& args, _In_ Console& console);

    static std::wstring FormatDateISO8601(_In_ const FILETIME& value);

    virtual void StartDocument() {}
    virtual void StartArray(_In_opt_ const std::wstring& name = empty_wstring) {}
    virtual void StartObject(_In_opt_ const std::wstring& name = empty_wstring) {}
    virtual void WriteProperty(_In_ const std::wstring& name, _In_ const std::wstring& value) {}
    virtual void EndObject() {}
    virtual void EndArray() {}
    virtual void EndDocument() {}

    virtual void WriteProperty(_In_ const std::wstring& name, _In_ bool value)
    {
        WriteProperty(name, std::to_wstring(value));
    }

    virtual void WriteProperty(_In_ const std::wstring& name, _In_ long long value)
    {
        WriteProperty(name, std::to_wstring(value));
    }

    virtual std::wstring FormatDate(_In_ const FILETIME& value);

    CommandArgs& Args() const noexcept
    {
        return m_args;
    }

    Console& Console() const noexcept
    {
        return m_console;
    }

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
    HRESULT GetIsRebootRequired(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtIsRebootRequired);
    HRESULT GetDisplayName(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtDisplayName);
    HRESULT GetDescription(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtDescription);

    void WriteInternal(_In_ ISetupInstance* pInstance);
    void WritePackage(_In_ ISetupPackageReference* pPackage);
    void WritePackages(_In_ ISetupInstance* pInstance);
    void WriteProperty(_In_ const std::wstring& name, _In_ const variant_t& value);
    bool WriteProperties(_In_ ISetupInstance* pInstance);
    bool WriteProperties(_In_ ISetupPropertyStore* pProperties, _In_opt_ const std::wstring& prefix = empty_wstring);

    CommandArgs& m_args;
    ::Console& m_console;
    PropertyArray m_properties;
};
