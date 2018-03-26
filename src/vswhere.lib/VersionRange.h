// <copyright file="VersionRange.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class VersionRange :
    public ISetupHelper
{
public:
    VersionRange() noexcept :
        m_ulRef(1)
    {
    }

private:
    ~VersionRange()
    {
    }

public:
    // IUnknown
    STDMETHODIMP QueryInterface(
        _In_ REFIID iid,
        _Out_ LPVOID* ppUnk
    )
    {
        HRESULT hr = S_OK;
        IUnknown* pUnk = NULL;

        if (!ppUnk)
        {
            return E_POINTER;
        }

        *ppUnk = NULL;
        if (iid == __uuidof(ISetupHelper))
        {
            pUnk = static_cast<ISetupHelper*>(this);
        }
        else if (iid == IID_IUnknown)
        {
            pUnk = static_cast<IUnknown*>(this);
        }
        else
        {
            hr = E_NOINTERFACE;
        }

        if (pUnk)
        {
            pUnk->AddRef();
            *ppUnk = pUnk;
        }

        return hr;
    }

    STDMETHODIMP_(ULONG) AddRef()
    {
        return ::InterlockedIncrement(&m_ulRef);
    }

    STDMETHODIMP_(ULONG) Release()
    {
        ULONG ulRef = ::InterlockedDecrement(&m_ulRef);
        if (ulRef == 0)
        {
            delete this;
        }

        return ulRef;
    }

public:
    // ISetupHelper
    STDMETHOD(ParseVersion)(
        _In_ LPCOLESTR pwszVersion,
        _Out_ PULONGLONG pullVersion
        ) noexcept;

    STDMETHOD(ParseVersionRange)(
        _In_ LPCOLESTR pwszVersionRange,
        _Out_ PULONGLONG pullMinVersion,
        _Out_ PULONGLONG pullMaxVersion
        );

    static const ULONGLONG MinVersion = 0;
    static const ULONGLONG MaxVersion = 18446744073709551615;

private:
    static HRESULT ParseVersionString(
        _In_ const LPCWSTR pwszVersionBegin,
        _In_ const LPCWSTR pwszVersionEnd,
        _Out_ PULONGLONG pullVersion
    );

    static LPWSTR Trim(
        _In_ const LPCWSTR pwszValue,
        _Out_ LPWSTR* ppwszEnd
    ) noexcept;

    ULONG m_ulRef;
};
