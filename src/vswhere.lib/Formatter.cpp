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
    { L"json", make_tuple(IDS_FORMAT_TEXT, JsonFormatter::Create) },
    { L"text", make_tuple(IDS_FORMAT_JSON, TextFormatter::Create) },
    { L"path", make_tuple(IDS_FORMAT_PATH, PathOnlyFormatter::Create) },
};

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

void Formatter::Write(_In_ std::wostream& out, _In_ ISetupInstance* pInstance)
{
    StartDocument(out);
    StartArray(out);

    WriteInternal(out, pInstance);

    EndArray(out);
    EndDocument(out);
}

void Formatter::Write(_In_ std::wostream& out, _In_ std::vector<ISetupInstancePtr> instances)
{
    StartDocument(out);
    StartArray(out);

    for (const auto& instance : instances)
    {
        WriteInternal(out, instance);
    }

    EndArray(out);
    EndDocument(out);
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
    auto cch = ::GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &st, NULL, NULL, 0, NULL);
    if (!cch)
    {
        throw win32_error();
    }

    date.resize(cch - 1);
    cch = ::GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &st, NULL, const_cast<LPWSTR>(date.c_str()), cch, NULL);
    if (!cch)
    {
        throw win32_error();
    }

    // Format time
    cch = ::GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, NULL, NULL, 0);
    if (!cch)
    {
        throw win32_error();
    }

    time.reserve(cch - 1);
    cch = ::GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, NULL, const_cast<LPWSTR>(time.c_str()), cch);
    if (!cch)
    {
        throw win32_error();
    }

    return date + L" " + time;
}

void Formatter::WriteInternal(_In_ std::wostream& out, _In_ ISetupInstance* pInstance)
{
    _ASSERTE(pInstance);

    StartObject(out);

    bstr_t bstrValue;

    for (const auto property : m_properties)
    {
        auto hr = property.second(pInstance, bstrValue.GetAddress());
        if (SUCCEEDED(hr))
        {
            wstring value = bstrValue;
            WriteProperty(out, property.first, value);
        }
    }

    WriteProperties(out, pInstance);
    EndObject(out);
}

void Formatter::WriteProperty(_In_ std::wostream& out, _In_ const wstring& name, _In_ const variant_t& value)
{
    switch (value.vt)
    {
    case VT_BOOL:
        WriteProperty(out, name, VARIANT_TRUE == value.boolVal);
        break;

    case VT_BSTR:
        WriteProperty(out, name, wstring(value.bstrVal));
        break;

    case VT_I1:
    case VT_I2:
    case VT_I4:
    case VT_I8:
    case VT_UI1:
    case VT_UI2:
    case VT_UI4:
        WriteProperty(out, name, value.llVal);
        break;
    }
}

void Formatter::WriteProperties(_In_ std::wostream& out, _In_ ISetupInstance* pInstance)
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

    SafeArray<BSTR> saNames(psaNames);
    for (const auto& bstrName : saNames.Elements())
    {
        wstring name(bstrName);
        variant_t vtValue;

        auto it = find_if(m_properties.begin(), m_properties.end(), bind(Formatter::PropertyEqual, name, _1));
        if (it == m_properties.end())
        {
            hr = store->GetValue(bstrName, vtValue.GetAddress());
            if (SUCCEEDED(hr))
            {
                WriteProperty(out, name, vtValue);
            }
        }
    }
}

bool Formatter::PropertyEqual(_In_ const std::wstring& name, _In_ PropertyArray::const_reference property)
{
    static ci_equal equal;
    return equal(name, property.first);
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
