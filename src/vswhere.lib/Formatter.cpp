// <copyright file="Formatter.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using namespace std::placeholders;

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
        { L"isPrerelease", bind(&Formatter::GetIsPrerelease, this, _1, _2) },
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

ci_equal Formatter::s_comparer;

std::unique_ptr<Formatter> Formatter::Create(const std::wstring& type)
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

void Formatter::Write(_In_ const CommandArgs& args, _In_ Console& console, _In_ ISetupInstance* pInstance)
{
    StartDocument(console);
    StartArray(console);

    WriteInternal(args, console, pInstance);

    EndArray(console);
    EndDocument(console);
}

void Formatter::Write(_In_ const CommandArgs& args, _In_ Console& console, _In_ std::vector<ISetupInstancePtr> instances)
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
    FILETIME local = {};
    SYSTEMTIME st = {};
    wstring date;
    wstring time;

    if (!::FileTimeToLocalFileTime(&value, &local))
    {
        throw win32_error();
    }

    if (!::FileTimeToSystemTime(&local, &st))
    {
        throw win32_error();
    }

    // Format date
    auto cch = ::GetDateFormatW(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, NULL, NULL, 0);
    if (!cch)
    {
        throw win32_error();
    }

    date.resize(cch - 1);
    cch = ::GetDateFormatW(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, NULL, const_cast<LPWSTR>(date.c_str()), cch);
    if (!cch)
    {
        throw win32_error();
    }

    // Format time
    cch = ::GetTimeFormatW(LOCALE_USER_DEFAULT, 0, &st, NULL, NULL, 0);
    if (!cch)
    {
        throw win32_error();
    }

    time.reserve(cch - 1);
    cch = ::GetTimeFormatW(LOCALE_USER_DEFAULT, 0, &st, NULL, const_cast<LPWSTR>(time.c_str()), cch);
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

    const auto& specified = args.get_Property();
    variant_t vtValue;
    bool found = false;

    for (const auto property : m_properties)
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
        WriteProperties(args, console, pInstance);
    }

    EndObject(console);
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

void Formatter::WriteProperties(_In_ const CommandArgs& args, _In_ Console& console, _In_ ISetupInstance* pInstance)
{
    _ASSERTE(pInstance);

    ISetupPropertyStorePtr store;
    LPSAFEARRAY psaNames = NULL;

    auto hr = pInstance->QueryInterface(&store);
    if (FAILED(hr))
    {
        if (E_NOINTERFACE != hr)
        {
            throw win32_error(hr);
        }

        return;
    }

    hr = store->GetNames(&psaNames);
    if (FAILED(hr))
    {
        return;
    }

    const auto& specified = args.get_Property();
    SafeArray<BSTR> saNames(psaNames);

    for (const auto& bstrName : saNames.Elements())
    {
        wstring name(bstrName);
        if (specified.empty() || s_comparer(name, specified))
        {
            variant_t vtValue;

            auto it = find_if(m_properties.begin(), m_properties.end(), bind(Formatter::PropertyEqual, name, _1));
            if (it == m_properties.end())
            {
                hr = store->GetValue(bstrName, vtValue.GetAddress());
                if (SUCCEEDED(hr))
                {
                    WriteProperty(console, name, vtValue);
                }
            }
        }
    }
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
        if (SUCCEEDED(hr))
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
        if (SUCCEEDED(hr))
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
