// <copyright file="LegacyInstance.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class LegacyInstance :
    public ISetupInstance
{
public:
    LegacyInstance(_In_ LPCWSTR wzVersion, _In_ LPCWSTR wzPath) :
        m_version(wzVersion),
        m_path(wzPath),
        m_ulRef(1)
    {
    }

    // IUnknown
    STDMETHODIMP QueryInterface(
        _In_ REFIID riid,
        _Outptr_ LPVOID *ppvObject)
    {
        if (!ppvObject)
        {
            return E_POINTER;
        }

        HRESULT hr = S_OK;
        if (riid == __uuidof(ISetupInstance))
        {
            AddRef();
            *ppvObject = static_cast<ISetupInstance*>(this);
        }
        else if (riid == __uuidof(IUnknown))
        {
            AddRef();
            *ppvObject = static_cast<IUnknown*>(this);
        }
        else
        {
            hr = E_NOINTERFACE;
        }

        return hr;
    }

    STDMETHODIMP_(ULONG) AddRef(void)
    {
        return ::InterlockedIncrement(&m_ulRef);
    }

    STDMETHODIMP_(ULONG) Release(void)
    {
        auto ulRef = ::InterlockedDecrement(&m_ulRef);
        if (!ulRef)
        {
            delete this;
        }

        return ulRef;
    }

public:
    // SetupInstance
    STDMETHOD(GetInstanceId)(
        _Out_ BSTR* pbstrInstanceId
        );

    STDMETHOD(GetInstallDate)(
        _Out_ LPFILETIME pInstallDate
        );

    STDMETHOD(GetInstallationName)(
        _Out_ BSTR* pbstrInstallationName
        );

    STDMETHOD(GetInstallationPath)(
        _Out_ BSTR* pbstrInstallationPath
        );

    STDMETHOD(GetInstallationVersion)(
        _Out_ BSTR* pbstrInstallationVersion
        );

    STDMETHOD(GetDisplayName)(
        _In_ LCID lcid,
        _Out_ BSTR* pbstrDisplayName
        );

    STDMETHOD(GetDescription)(
        _In_ LCID lcid,
        _Out_ BSTR* pbstrDescription
        );

    STDMETHOD(ResolvePath)(
        _In_opt_z_ LPCOLESTR pwszRelativePath,
        _Out_ BSTR* pbstrAbsolutePath
        );

private:
    const std::wstring m_version;
    const std::wstring m_path;
    ULONG m_ulRef;
};
