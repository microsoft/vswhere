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
    bstr_t bstrValue;
    bool found = false;

    for (const auto property : m_properties)
    {
        if (specified.empty() || PropertyEqual(specified, property))
        {
            found = true;

            auto hr = property.second(pInstance, bstrValue.GetAddress());
            if (SUCCEEDED(hr))
            {
                wstring value = bstrValue;
                WriteProperty(console, property.first, value);
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

HRESULT Formatter::GetInstanceId(_In_ ISetupInstance* pInstance, _Out_ BSTR* pbstrInstanceId)
{
    return pInstance->GetInstanceId(pbstrInstanceId);
}

HRESULT Formatter::GetInstallDate(_In_ ISetupInstance* pInstance, _Out_ BSTR* pbstrInstallDate)
{
    FILETIME ft = {};

    auto hr = pInstance->GetInstallDate(&ft);
    if (SUCCEEDED(hr))
    {
        auto value = FormatDate(ft);
        if (!value.empty())
        {
            *pbstrInstallDate = ::SysAllocString(value.c_str());
            if (!*pbstrInstallDate)
            {
                throw win32_error(ERROR_OUTOFMEMORY);
            }
        }
    }

    return hr;
}

HRESULT Formatter::GetInstallationName(_In_ ISetupInstance* pInstance, _Out_ BSTR* pbstrInstallationName)
{
    return pInstance->GetInstallationName(pbstrInstallationName);
}

HRESULT Formatter::GetInstallationPath(_In_ ISetupInstance* pInstance, _Out_ BSTR* pbstrInstallationPath)
{
    return pInstance->GetInstallationPath(pbstrInstallationPath);
}

HRESULT Formatter::GetInstallationVersion(_In_ ISetupInstance* pInstance, _Out_ BSTR* pbstrInstallationVersion)
{
    return pInstance->GetInstallationVersion(pbstrInstallationVersion);
}

HRESULT Formatter::GetDisplayName(_In_ ISetupInstance* pInstance, _Out_ BSTR* pbstrDisplayName)
{
    auto lcid = ::GetUserDefaultLCID();
    return pInstance->GetDisplayName(lcid, pbstrDisplayName);
}

HRESULT Formatter::GetDescription(_In_ ISetupInstance* pInstance, _Out_ BSTR* pbstrDescription)
{
    auto lcid = ::GetUserDefaultLCID();
    return pInstance->GetDescription(lcid, pbstrDescription);
}
