// <copyright file="TestPropertyStore.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class TestPropertyStore :
    public ISetupPropertyStore
{
public:
    typedef std::unordered_map<std::wstring, std::tuple<VARENUM, std::wstring>, std::hash<std::wstring>, ci_equal> MapType;

    TestPropertyStore() :
        m_ulRef(1)
    {
    }

    TestPropertyStore(_In_ std::initializer_list<MapType::value_type> list) :
        m_properties(list.begin(), list.end()),
        m_ulRef(1)
    {
    }

    TestPropertyStore(_In_ const MapType& properties) :
        m_properties(properties.begin(), properties.end()),
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
        if (riid == __uuidof(ISetupPropertyStore))
        {
            AddRef();
            *ppvObject = static_cast<ISetupPropertyStore*>(this);
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

    // ISetupPropertyStore
    STDMETHODIMP GetNames(
        _Out_ LPSAFEARRAY* ppsaNames
    )
    {
        if (!ppsaNames)
        {
            return E_POINTER;
        }

        if (!m_properties.empty())
        {
            auto psa = ::SafeArrayCreateVector(VT_BSTR, 0, m_properties.size());
            auto hr = ::SafeArrayLock(psa);
            if (FAILED(hr))
            {
                throw win32_error(hr, "failed to lock packages array");
            }

            auto rgData = (BSTR*)psa->pvData;
            for (const auto& property : m_properties)
            {
                *rgData = ::SysAllocString(property.first.c_str());
                if (!*rgData)
                {
                    throw win32_error(E_OUTOFMEMORY, "failed to allocate memory");
                }

                ++rgData;
            }

            hr = ::SafeArrayUnlock(psa);
            if (FAILED(hr))
            {
                throw win32_error(hr, "failed to unlock packages array");
            }

            *ppsaNames = psa;
            return S_OK;
        }

        return E_NOTFOUND;
    }

    STDMETHODIMP GetValue(
        _In_ LPCOLESTR pwszName,
        _Out_ LPVARIANT pvtValue
    )
    {
        return TryGetVARIANT(pwszName, pvtValue);
    }

    // Other
    bool empty() const
    {
        return m_properties.empty();
    }

private:
    STDMETHODIMP TryGet(_In_ std::wstring name, _Inout_ VARENUM& vt, _Inout_ std::wstring& value)
    {
        auto it = m_properties.find(name);
        if (it != m_properties.end())
        {
            tie(vt, value) = it->second;
            return S_OK;
        }

        return E_NOTFOUND;
    }

    STDMETHODIMP TryGetVARIANT(_In_ LPCWSTR wszName, _Out_ VARIANT* pvtValue)
    {
        if (!pvtValue)
        {
            return E_POINTER;
        }

        static ci_equal equals;
        VARENUM vt = VT_EMPTY;
        std::wstring value;

        auto hr = TryGet(wszName, vt, value);
        if (SUCCEEDED(hr))
        {
            switch (vt)
            {
            case VT_BSTR:
            {
                pvtValue->bstrVal = ::SysAllocString(value.c_str());
                if (!pvtValue->bstrVal)
                {
                    return E_OUTOFMEMORY;
                }
                else
                {
                    pvtValue->vt = VT_BSTR;
                    return S_OK;
                }
            }

            case VT_BOOL:
            {
                auto f = L"1" == value || equals(L"true", value);

                pvtValue->boolVal = f ? VARIANT_TRUE : VARIANT_FALSE;
                pvtValue->vt = VT_BOOL;
                return S_OK;
            }

            case VT_I1:
            case VT_I2:
            case VT_I4:
            {
                auto loc = _wcreate_locale(LC_CTYPE, L"C");
                pvtValue->intVal = ::_wtoi_l(value.c_str(), loc);
                pvtValue->vt = vt;
                _free_locale(loc);

                return S_OK;
            }

            default:
                return E_NOTSUPPORTED;
            }
        }

        return hr;
    }

    MapType m_properties;
    ULONG m_ulRef;
};
