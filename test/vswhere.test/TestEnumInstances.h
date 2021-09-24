// <copyright file="TestEnumInstances.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class TestEnumInstances :
    public IEnumSetupInstances
{
public:
    typedef ISetupInstance* ElementType;

    TestEnumInstances(_In_ std::initializer_list<ElementType> list) :
        m_instances(list.begin(), list.end()),
        m_i(0),
        m_ulRef(1)
    {
    }

    ~TestEnumInstances()
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
        if (riid == __uuidof(IEnumSetupInstances))
        {
            AddRef();
            *ppvObject = static_cast<IEnumSetupInstances*>(this);
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
        Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(0UL, m_ulRef);
        return ::InterlockedDecrement(&m_ulRef);
    }

    // IEnumSetupInstances
    STDMETHODIMP Next(
        _In_ ULONG celt,
        _Out_writes_to_(celt, *pceltFetched) ISetupInstance** rgelt,
        _Out_ _Deref_out_range_(0, celt) ULONG* pceltFetched
    )
    {
        *pceltFetched = 0;
        auto remaining = m_instances.size() - m_i;
        if (0 == remaining)
        {
            return S_FALSE;
        }
        
        for (unsigned long i = 0; i < celt && m_i < m_instances.size(); ++i, ++m_i, ++*pceltFetched)
        {
            rgelt[i] = m_instances[m_i];
        }

        return S_OK;
    }

    STDMETHODIMP Skip(
        _In_ ULONG celt
    )
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP Reset(void)
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP Clone(
        _Deref_out_opt_ IEnumSetupInstances** ppenum
    )
    {
        return E_NOTIMPL;
    }


private:
    const std::vector<ElementType> m_instances;
    ULONG m_i;
    ULONG m_ulRef;
};
