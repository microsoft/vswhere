// <copyright file="Formatter.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using namespace std::placeholders;

const std::wstring Formatter::empty_wstring;

Formatter::Formatter()
{
    m_properties =
    {
        { L"instanceId", bind(&Formatter::GetInstanceId, this, _1, _2) },
        { L"installDate", bind(&Formatter::GetInstallDate, this, _1, _2) },
        { L"installationName", bind(&Formatter::GetInstallationName, this, _1, _2) },
        { L"installationPath", bind(&Formatter::GetInstallationPath, this, _1, _2) },
        { L"installationVersion", bind(&Formatter::GetInstallationVersion, this, _1, _2) },
        { L"productId", bind(&Formatter::GetProductId, this, _1, _2) },
        { L"productPath", bind(&Formatter::GetProductPath, this, _1, _2) },
        { L"state", bind(&Formatter::GetState, this, _1, _2) },
        { L"isComplete", bind(&Formatter::GetIsComplete, this, _1, _2) },
        { L"isLaunchable", bind(&Formatter::GetIsLaunchable, this, _1, _2) },
        { L"isPrerelease", bind(&Formatter::GetIsPrerelease, this, _1, _2) },
        { L"isRebootRequired", bind(&Formatter::GetIsRebootRequired, this, _1, _2) },
        { L"displayName", bind(&Formatter::GetDisplayName, this, _1, _2) },
        { L"description", bind(&Formatter::GetDescription, this, _1, _2) },
    };
}

Formatter::FormatterMap Formatter::Formatters =
{
    { L"json", make_tuple(IDS_FORMAT_JSON, JsonFormatter::Create) },
    { L"text", make_tuple(IDS_FORMAT_TEXT, TextFormatter::Create) },
    { L"value", make_tuple(IDS_FORMAT_VALUE, ValueFormatter::Create) },
    { L"xml", make_tuple(IDS_FORMAT_XML, XmlFormatter::Create) },
};

const wstring Formatter::s_delims(L"./_");
ci_equal Formatter::s_comparer;

std::unique_ptr<Formatter> Formatter::Create(const wstring& type)
{
    auto it = Formatters.find(type);
    if (it != Formatters.end())
    {
        FormatterFactory factory;
        tie(ignore, factory) = it->second;

        return factory();
    }

    throw win32_error(ERROR_NOT_SUPPORTED);
}

void Formatter::Write(_In_ Console& console, _In_ const std::wstring& root, _In_ const std::wstring& name, _In_ const std::vector<std::wstring> values)
{
    StartDocument(console);
    StartArray(console, root);

    for (const auto& value : values)
    {
        WriteProperty(console, name, value);
    }

    EndArray(console);
    EndDocument(console);
}

void Formatter::Write(_In_ const CommandArgs& args, _In_ Console& console, _In_ ISetupInstance* pInstance)
{
    StartDocument(console);
    StartArray(console);

    WriteInternal(args, console, pInstance);

    EndArray(console);
    EndDocument(console);
}

void Formatter::Write(_In_ const CommandArgs& args, _In_ Console& console, _In_ vector<ISetupInstancePtr> instances)
{
    StartDocument(console);
    StartArray(console);

    for (const auto& instance : instances)
    {
        WriteInternal(args, console, instance);
    }

    EndArray(console);
    EndDocument(console);
}

void Formatter::WriteFiles(_In_ const CommandArgs& args, _In_ Console& console, vector<ISetupInstancePtr> instances)
{
    StartDocument(console);
    StartArray(console, L"files");

    bstr_t bstrInstallationPath;
    for (const auto& instance : instances)
    {
        auto hr = instance->GetInstallationPath(bstrInstallationPath.GetAddress());
        if (SUCCEEDED(hr))
        {
            Glob glob(static_cast<LPCWSTR>(bstrInstallationPath), args.get_Find());

            auto entries = glob.Entries(args.get_Sort());
            for (const auto& entry : entries)
            {
                WriteProperty(console, L"file", entry);
            }
        }
    }

    EndArray(console);
    EndDocument(console);
}

wstring Formatter::FormatDateISO8601(_In_ const FILETIME& value)
{
    SYSTEMTIME st = {};

    if (!::FileTimeToSystemTime(&value, &st))
    {
        throw win32_error();
    }

    wchar_t wz[21] = {};
    auto cch = ::swprintf_s(wz, L"%04d-%02d-%02dT%02d:%02d:%02dZ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    return wstring(wz, cch);
}

wstring Formatter::FormatDate(_In_ const FILETIME& value)
{
    SYSTEMTIME stUniversal = {};
    SYSTEMTIME stLocal = {};
    wstring date;
    wstring time;

    if (!::FileTimeToSystemTime(&value, &stUniversal))
    {
        throw win32_error();
    }

    if (!::SystemTimeToTzSpecificLocalTime(NULL, &stUniversal, &stLocal))
    {
        throw win32_error();
    }

    // Format date
    auto cch = ::GetDateFormatW(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &stLocal, NULL, NULL, 0);
    if (!cch)
    {
        throw win32_error();
    }

    date.reserve(cch);
    date.resize(cch - 1);
    cch = ::GetDateFormatW(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &stLocal, NULL, const_cast<LPWSTR>(date.c_str()), cch);
    if (!cch)
    {
        throw win32_error();
    }

    // Format time
    cch = ::GetTimeFormatW(LOCALE_USER_DEFAULT, 0, &stLocal, NULL, NULL, 0);
    if (!cch)
    {
        throw win32_error();
    }

    time.reserve(cch);
    time.resize(cch - 1);
    cch = ::GetTimeFormatW(LOCALE_USER_DEFAULT, 0, &stLocal, NULL, const_cast<LPWSTR>(time.c_str()), cch);
    if (!cch)
    {
        throw win32_error();
    }

    return date + L" " + time;
}

void Formatter::WriteInternal(_In_ const CommandArgs& args, _In_ Console& console, _In_ ISetupInstance* pInstance)
{
    _ASSERTE(pInstance);

    StartObject(console);

    wstring specified = args.get_Property();
    variant_t vtValue;
    bool found = false;

    for (const auto& property : m_properties)
    {
        if (specified.empty() || PropertyEqual(specified, property))
        {
            found = true;

            auto hr = property.second(pInstance, vtValue.GetAddress());
            if (SUCCEEDED(hr))
            {
                WriteProperty(console, property.first, vtValue);
            }
        }
    }

    if (specified.empty() || !found)
    {
        found = WriteProperties(args, console, pInstance);

        // Output catalog information.
        if (specified.empty() || !found)
        {
            ISetupInstanceCatalogPtr instanceCatalog;

            auto hr = pInstance->QueryInterface(&instanceCatalog);
            if (SUCCEEDED(hr))
            {
                ISetupPropertyStorePtr store;

                hr = instanceCatalog->GetCatalogInfo(&store);
                if (SUCCEEDED(hr) && !!store)
                {
                    wstring name(L"catalog");
                    StartObject(console, name);

                    found = WriteProperties(args, console, store, name);

                    EndObject(console);
                }
            }
        }

        // Output custom properties.
        if (specified.empty() || !found)
        {
            ISetupInstance2Ptr instance2;

            auto hr = pInstance->QueryInterface(&instance2);
            if (SUCCEEDED(hr))
            {
                ISetupPropertyStorePtr store;

                hr = instance2->GetProperties(&store);
                if (SUCCEEDED(hr) && !!store)
                {
                    wstring name(L"properties");
                    StartObject(console, name);

                    found = WriteProperties(args, console, store, name);

                    EndObject(console);
                }
            }
        }

        if (args.get_IncludePackages() && SupportsPackages())
        {
            if (specified.empty() || s_comparer(specified, L"packages"))
            {
                WritePackages(args, console, pInstance);
            }
        }
    }

    EndObject(console);
}

void Formatter::WritePackage(_In_ Console& console, _In_ ISetupPackageReference* pPackage)
{
    StartObject(console, L"package");

    bstr_t bstr;
    auto hr = pPackage->GetId(bstr.GetAddress());
    if (SUCCEEDED(hr))
    {
        WriteProperty(console, L"id", bstr);
    }

    hr = pPackage->GetVersion(bstr.GetAddress());
    if (SUCCEEDED(hr) && bstr.length())
    {
        WriteProperty(console, L"version", bstr);
    }

    hr = pPackage->GetChip(bstr.GetAddress());
    if (SUCCEEDED(hr) && bstr.length())
    {
        WriteProperty(console, L"chip", bstr);
    }

    hr = pPackage->GetLanguage(bstr.GetAddress());
    if (SUCCEEDED(hr) && bstr.length())
    {
        WriteProperty(console, L"language", bstr);
    }

    hr = pPackage->GetBranch(bstr.GetAddress());
    if (SUCCEEDED(hr) && bstr.length())
    {
        WriteProperty(console, L"branch", bstr);
    }

    hr = pPackage->GetType(bstr.GetAddress());
    if (SUCCEEDED(hr))
    {
        WriteProperty(console, L"type", bstr);
    }

    VARIANT_BOOL vtBool;
    hr = pPackage->GetIsExtension(&vtBool);
    if (SUCCEEDED(hr) && VARIANT_TRUE == vtBool)
    {
        WriteProperty(console, L"extension", true);
    }

    EndObject(console);
}

void Formatter::WritePackages(_In_ const CommandArgs& args, _In_ Console& console, _In_ ISetupInstance* pInstance)
{
    ISetupInstance2Ptr instance2;
    LPSAFEARRAY psaPackages;

    auto hr = pInstance->QueryInterface(&instance2);
    if (SUCCEEDED(hr))
    {
        hr = instance2->GetPackages(&psaPackages);
        if (SUCCEEDED(hr) && psaPackages->rgsabound[0].cElements)
        {
            StartArray(console, L"packages");

            SafeArray<ISetupPackageReference*> saPackages(psaPackages);
            const auto packages = saPackages.Elements();

            for (const auto& package : packages)
            {
                WritePackage(console, package);
            }

            EndArray(console);
        }
    }
}

void Formatter::WriteProperty(_In_ Console& console, _In_ const wstring& name, _In_ const variant_t& value)
{
    switch (value.vt)
    {
    case VT_BOOL:
        WriteProperty(console, name, VARIANT_TRUE == value.boolVal);
        break;

    case VT_BSTR:
        WriteProperty(console, name, wstring(value.bstrVal));
        break;

    case VT_I1:
    case VT_I2:
    case VT_I4:
    case VT_I8:
    case VT_UI1:
    case VT_UI2:
    case VT_UI4:
        WriteProperty(console, name, value.llVal);
        break;
    }
}

bool Formatter::WriteProperties(_In_ const CommandArgs& args, _In_ Console& console, _In_ ISetupInstance* pInstance)
{
    _ASSERTE(pInstance);

    ISetupPropertyStorePtr store;

    auto hr = pInstance->QueryInterface(&store);
    if (FAILED(hr))
    {
        if (E_NOINTERFACE != hr)
        {
            throw win32_error(hr);
        }

        return false;
    }

    return WriteProperties(args, console, store);
}

bool Formatter::WriteProperties(_In_ const CommandArgs& args, _In_ Console& console, _In_ ISetupPropertyStore* pProperties, _In_opt_ const wstring& prefix)
{
    _ASSERTE(pProperties);

    static ci_less less;

    LPSAFEARRAY psaNames = NULL;
    auto found = false;

    auto hr = pProperties->GetNames(&psaNames);
    if (FAILED(hr))
    {
        return false;
    }

    // Trim optional nested object name from specified property if matching current scope.
    wstring specified = args.get_Property();
    if (prefix.size() > 0)
    {
        auto pos = specified.find_first_of(s_delims);
        if (pos != wstring::npos && (pos + 1) < specified.size() && s_comparer(prefix, specified.substr(0, pos)))
        {
            specified = specified.substr(pos + 1);
        }
        else if (s_comparer(prefix, specified))
        {
            // If the current nested object name is specified, clear the prefix to show the whole nested object.
            specified.clear();
        }
    }

    SafeArray<BSTR> saNames(psaNames);
    
    auto elems = saNames.Elements();
    sort(elems.begin(), elems.end(), less);

    for (const auto& bstrName : elems)
    {
        wstring name(bstrName);
        if (specified.empty() || (found = s_comparer(name, specified)))
        {
            variant_t vtValue;

            auto it = m_properties.end();

            // Don't check if we already handled nested properties as top-level properties.
            if (prefix.empty())
            {
                it = find_if(m_properties.begin(), m_properties.end(), bind(Formatter::PropertyEqual, name, _1));
            }

            if (it == m_properties.end())
            {
                hr = pProperties->GetValue(bstrName, vtValue.GetAddress());
                if (SUCCEEDED(hr))
                {
                    WriteProperty(console, name, vtValue);
                }
            }

            if (found)
            {
                return true;
            }
        }
    }

    return false;
}

bool Formatter::PropertyEqual(_In_ const std::wstring& name, _In_ PropertyArray::const_reference property)
{
    return s_comparer(name, property.first);
}

HRESULT Formatter::GetStringProperty(_In_ std::function<HRESULT(_Out_ BSTR*)> pfn, _Out_ VARIANT* pvt)
{
    _ASSERTE(pfn);
    _ASSERTE(pvt);

    variant_t vt;

    auto hr = pfn(&vt.bstrVal);
    if (SUCCEEDED(hr))
    {
        vt.vt = VT_BSTR;
        *pvt = vt.Detach();
    }

    return hr;
}

HRESULT Formatter::GetInstanceId(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtInstanceId)
{
    return GetStringProperty(bind(&ISetupInstance::GetInstanceId, pInstance, _1), pvtInstanceId);
}

HRESULT Formatter::GetInstallDate(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtInstallDate)
{
    FILETIME ft = {};
    variant_t vt;

    auto hr = pInstance->GetInstallDate(&ft);
    if (SUCCEEDED(hr))
    {
        auto value = FormatDate(ft);
        if (!value.empty())
        {
            vt.bstrVal = ::SysAllocString(value.c_str());
            if (!*vt.bstrVal)
            {
                throw win32_error(ERROR_OUTOFMEMORY);
            }

            vt.vt = VT_BSTR;
            *pvtInstallDate = vt.Detach();
        }
    }

    return hr;
}

HRESULT Formatter::GetInstallationName(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtInstallationName)
{
    return GetStringProperty(bind(&ISetupInstance::GetInstallationName, pInstance, _1), pvtInstallationName);
}

HRESULT Formatter::GetInstallationPath(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtInstallationPath)
{
    return GetStringProperty(bind(&ISetupInstance::GetInstallationPath, pInstance, _1), pvtInstallationPath);
}

HRESULT Formatter::GetInstallationVersion(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtInstallationVersion)
{
    return GetStringProperty(bind(&ISetupInstance::GetInstallationVersion, pInstance, _1), pvtInstallationVersion);
}

HRESULT Formatter::GetProductId(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtProductId)
{
    ISetupInstance2Ptr instance;

    auto hr = pInstance->QueryInterface(&instance);
    if (SUCCEEDED(hr))
    {
        ISetupPackageReferencePtr reference;

        hr = instance->GetProduct(&reference);
        if (SUCCEEDED(hr) && !!reference)
        {
            variant_t vt;

            hr = reference->GetId(&vt.bstrVal);
            if (SUCCEEDED(hr))
            {
                vt.vt = VT_BSTR;
                *pvtProductId = vt.Detach();
            }
        }
    }

    return hr;
}

HRESULT Formatter::GetProductPath(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtProductPath)
{
    ISetupInstance2Ptr instance;

    auto hr = pInstance->QueryInterface(&instance);
    if (SUCCEEDED(hr))
    {
        bstr_t bstrProductPath;

        hr = instance->GetProductPath(bstrProductPath.GetAddress());
        if (SUCCEEDED(hr) && !!bstrProductPath)
        {
            variant_t vt;

            hr = instance->ResolvePath(bstrProductPath, &vt.bstrVal);
            if (SUCCEEDED(hr))
            {
                vt.vt = VT_BSTR;
                *pvtProductPath = vt.Detach();
            }
        }
    }

    return hr;
}

HRESULT Formatter::GetState(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtState)
{
    ISetupInstance2Ptr instance;
    variant_t vt;

    auto hr = pInstance->QueryInterface(&instance);
    if (SUCCEEDED(hr))
    {
        hr = instance->GetState(reinterpret_cast<InstanceState*>(&vt.uintVal));
        if (SUCCEEDED(hr))
        {
            vt.vt = VT_UI4;
            *pvtState = vt.Detach();
        }
    }

    return hr;
}

HRESULT Formatter::GetIsComplete(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtIsComplete)
{
    ISetupInstance2Ptr instance;
    variant_t vt;

    auto hr = pInstance->QueryInterface(&instance);
    if (SUCCEEDED(hr))
    {
        hr = instance->IsComplete(&vt.boolVal);
        if (SUCCEEDED(hr))
        {
            vt.vt = VT_BOOL;
            *pvtIsComplete = vt.Detach();
        }
    }

    return hr;
}

HRESULT Formatter::GetIsLaunchable(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtIsLaunchable)
{
    ISetupInstance2Ptr instance;
    variant_t vt;

    auto hr = pInstance->QueryInterface(&instance);
    if (SUCCEEDED(hr))
    {
        hr = instance->IsLaunchable(&vt.boolVal);
        if (SUCCEEDED(hr))
        {
            vt.vt = VT_BOOL;
            *pvtIsLaunchable = vt.Detach();
        }
    }

    return hr;
}

HRESULT Formatter::GetIsPrerelease(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtIsPrerelease)
{
    ISetupInstanceCatalogPtr catalog;
    variant_t vt;

    auto hr = pInstance->QueryInterface(&catalog);
    if (SUCCEEDED(hr))
    {
        hr = catalog->IsPrerelease(&vt.boolVal);
        if (SUCCEEDED(hr))
        {
            vt.vt = VT_BOOL;
            *pvtIsPrerelease = vt.Detach();
        }
    }

    return hr;
}

HRESULT Formatter::GetIsRebootRequired(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtIsRebootRequired)
{
    ISetupInstance2Ptr instance;
    variant_t vt;

    auto hr = pInstance->QueryInterface(&instance);
    if (SUCCEEDED(hr))
    {
        auto state = InstanceState::eNone;

        hr = instance->GetState(&state);
        if (SUCCEEDED(hr))
        {
            vt.vt = VT_BOOL;
            vt.boolVal = (state & InstanceState::eNoRebootRequired) == 0 ? VARIANT_TRUE : VARIANT_FALSE;

            *pvtIsRebootRequired = vt.Detach();
        }
    }

    return hr;
}

HRESULT Formatter::GetDisplayName(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtDisplayName)
{
    auto lcid = ::GetUserDefaultLCID();
    return GetStringProperty(bind(&ISetupInstance::GetDisplayName, pInstance, lcid, _1), pvtDisplayName);
}

HRESULT Formatter::GetDescription(_In_ ISetupInstance* pInstance, _Out_ VARIANT* pvtDescription)
{
    auto lcid = ::GetUserDefaultLCID();
    return GetStringProperty(bind(&ISetupInstance::GetDescription, pInstance, lcid, _1), pvtDescription);
}
