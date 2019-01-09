// <copyright file="TestHelper.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

#define MAKEVERSION(major, minor, build, revision) ULONGLONG(major) << 24 | ULONGLONG(minor) << 16 | ULONGLONG(build) << 8 | ULONGLONG(revision)

class TestHelper :
    public ISetupHelper
{
public:
    TestHelper(_In_ ULONGLONG ullMinimumVersion, _In_ ULONGLONG ullMaximumVersion) :
        m_ullMinimumVersion(ullMinimumVersion),
        m_ullMaximumVersion(ullMaximumVersion),
        m_ulRef(1)
    {
    }

    ~TestHelper()
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
        if (riid == __uuidof(ISetupHelper))
        {
            AddRef();
            *ppvObject = static_cast<ISetupHelper*>(this);
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

    // ISetupHelper
    STDMETHODIMP ParseVersion(
        _In_ LPCOLESTR pwszVersion,
        _Out_ PULONGLONG pullVersion
    )
    {
        static ci_equal equal;

        if (equal(pwszVersion, L"1.0"))
        {
            *pullVersion = MAKEVERSION(1, 0, 0, 0);
            return S_OK;
        }

        if (equal(pwszVersion, L"2.0"))
        {
            *pullVersion = MAKEVERSION(2, 0, 0, 0);
            return S_OK;
        }

        if (equal(pwszVersion, L"10.0"))
        {
            *pullVersion = MAKEVERSION(10, 0, 0, 0);
            return S_OK;
        }

        if (equal(pwszVersion, L"14.0"))
        {
            *pullVersion = MAKEVERSION(14, 0, 0, 0);
            return S_OK;
        }

        return E_NOTIMPL;
    }

    STDMETHODIMP ParseVersionRange(
        _In_ LPCOLESTR pwszVersionRange,
        _Out_ PULONGLONG pullMinVersion,
        _Out_ PULONGLONG pullMaxVersion
    )
    {
        if (s_comparer(pwszVersionRange, L"invalid"))
        {
            return E_INVALIDARG;
        }

        *pullMinVersion = m_ullMinimumVersion;
        *pullMaxVersion = m_ullMaximumVersion;

        return S_OK;
    }

private:
    static const ci_equal s_comparer;

    const ULONGLONG m_ullMinimumVersion;
    const ULONGLONG m_ullMaximumVersion;
    ULONG m_ulRef;
};
