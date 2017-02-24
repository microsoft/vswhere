// <copyright file="TestPackageReference.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once


class TestPackageReference :
    public ISetupPackageReference
{
public:
    typedef std::unordered_map<std::wstring, std::wstring, std::hash<std::wstring>, ci_equal> MapType;

    TestPackageReference(_In_ std::initializer_list<MapType::value_type> list) :
        m_properties(list.begin(), list.end()),
        m_ulRef(1)
    {
    }

    ~TestPackageReference()
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
        if (riid == __uuidof(ISetupPackageReference))
        {
            AddRef();
            *ppvObject = static_cast<ISetupPackageReference*>(this);
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

    // ISetupPackageReference
    STDMETHODIMP GetId(
        _Out_ BSTR* pbstrId
    )
    {
        return TryGetBSTR(L"Id", pbstrId);
    }

    STDMETHODIMP GetVersion(
        _Out_ BSTR* pbstrVersion
    )
    {
        return TryGetBSTR(L"Version", pbstrVersion);
    }

    STDMETHODIMP GetChip(
        _Out_ BSTR* pbstrChip
    )
    {
        return TryGetBSTR(L"Chip", pbstrChip);
    }

    STDMETHODIMP GetLanguage(
        _Out_ BSTR* pbstrLanguage
    )
    {
        return TryGetBSTR(L"Language", pbstrLanguage);
    }

    STDMETHODIMP GetBranch(
        _Out_ BSTR* pbstrBranch
        )
    {
        return TryGetBSTR(L"Branch", pbstrBranch);
    }

    STDMETHODIMP GetType(
        _Out_ BSTR* pbstrType
    )
    {
        return TryGetBSTR(L"Type", pbstrType);
    }

    STDMETHODIMP GetUniqueId(
        _Out_ BSTR* pbstrUniqueId
    )
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP GetIsExtension(
        _Out_ VARIANT_BOOL* pfIsExtension
    )
    {
        return E_NOTIMPL;
    }

private:
    STDMETHODIMP TryGet(_In_ std::wstring name, _In_ std::wstring& value)
    {
        auto it = m_properties.find(name);
        if (it != m_properties.end())
        {
            value = it->second;
            return S_OK;
        }

        return E_NOTFOUND;
    }

    STDMETHODIMP TryGetBSTR(_In_ LPCWSTR wszName, _Out_ BSTR* pbstrValue)
    {
        if (!pbstrValue)
        {
            return E_POINTER;
        }

        std::wstring value;

        auto hr = TryGet(wszName, value);
        if (SUCCEEDED(hr))
        {
            *pbstrValue = ::SysAllocString(value.c_str());
        }

        return hr;
    }

    MapType m_properties;
    ULONG m_ulRef;
};
