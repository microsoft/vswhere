// <copyright file="TestInstance.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class TestInstance :
    public ISetupInstance2,
    public ISetupInstanceCatalog
{
public:
    typedef ISetupPackageReference* ElementType;
    typedef std::unordered_map<std::wstring, std::wstring, std::hash<std::wstring>, ci_equal> MapType;

    TestInstance(_In_ std::initializer_list<MapType::value_type> list) :
        m_properties(list.begin(), list.end()),
        m_ulRef(1)
    {
        m_locale = ::_wcreate_locale(LC_ALL, L"en-US");
    }

    TestInstance(
        _In_ const ElementType pProduct,
        _In_ const std::vector<ElementType>& packages,
        _In_ const MapType& properties) :
        m_product(pProduct),
        m_properties(properties),
        m_ulRef(1)
    {
        m_locale = ::_wcreate_locale(LC_ALL, L"en-US");

        for (const auto package : packages)
        {
            m_packages.push_back(package);
        }
    }

    ~TestInstance()
    {
        if (m_locale)
        {
            ::_free_locale(m_locale);
        }
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
        else if (riid == __uuidof(ISetupInstance2))
        {
            AddRef();
            *ppvObject = static_cast<ISetupInstance2*>(this);
        }
        else if (riid == __uuidof(ISetupInstanceCatalog) && !m_catalogProperties.empty())
        {
            AddRef();
            *ppvObject = static_cast<ISetupInstanceCatalog*>(this);
        }
        else if (riid == __uuidof(IUnknown))
        {
            AddRef();
            *ppvObject = static_cast<IUnknown*>(static_cast<ISetupInstance2*>(this));
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

    // ISetupInstance
    STDMETHODIMP GetInstanceId(
        _Out_ BSTR* pbstrInstanceId)
    {
        return TryGetBSTR(L"InstanceId", pbstrInstanceId);
    }

    #pragma warning(suppress: 6101) // pInstallDate is not set if value is an invalid date but we still return success
    STDMETHODIMP GetInstallDate(
        _Out_ LPFILETIME pInstallDate)
    {
        std::wstring value;

        auto hr = TryGet(L"installDate", value);
        if (SUCCEEDED(hr))
        {
            const int num_fields = 6;
            SYSTEMTIME st = {};

            if (num_fields == ::swscanf_s(value.c_str(), L"%hu-%hu-%huT%hu:%hu:%hu", &st.wYear, &st.wMonth, &st.wDay, &st.wHour, &st.wMinute, &st.wSecond))
            {
                ::SystemTimeToFileTime(&st, pInstallDate);
            }
        }

        return hr;
    }

    STDMETHODIMP GetInstallationName(
        _Out_ BSTR* pbstrInstallationName
    )
    {
        return TryGetBSTR(L"InstallationName", pbstrInstallationName);
    }

    STDMETHODIMP GetInstallationPath(
        _Out_ BSTR* pbstrInstallationPath
    )
    {
        return TryGetBSTR(L"InstallationPath", pbstrInstallationPath);
    }

    STDMETHODIMP GetInstallationVersion(
        _Out_ BSTR* pbstrInstallationVersion
    )
    {
        return TryGetBSTR(L"InstallationVersion", pbstrInstallationVersion);
    }

    STDMETHODIMP GetDisplayName(
        _In_ LCID lcid,
        _Out_ BSTR* pbstrDisplayName
    )
    {
        return TryGetBSTR(L"DisplayName", pbstrDisplayName);
    }

    STDMETHODIMP GetDescription(
        _In_ LCID lcid,
        _Out_ BSTR* pbstrDescription
    )
    {
        return TryGetBSTR(L"Description", pbstrDescription);
    }

    STDMETHODIMP ResolvePath(
        _In_opt_z_ LPCOLESTR pwszRelativePath,
        _Out_ BSTR* pbstrAbsolutePath
    )
    {
        if (!pbstrAbsolutePath)
        {
            return E_POINTER;
        }

        bstr_t bstrPath;

        auto hr = GetInstallationPath(bstrPath.GetAddress());
        if (SUCCEEDED(hr))
        {
            std::experimental::filesystem::v1::path absolutePath((LPWSTR)bstrPath);

            hr = GetProductPath(bstrPath.GetAddress());
            if (SUCCEEDED(hr))
            {
                absolutePath.append((LPWSTR)bstrPath);

                *pbstrAbsolutePath = ::SysAllocString(absolutePath.c_str());
                if (!*pbstrAbsolutePath)
                {
                    return E_OUTOFMEMORY;
                }

                return S_OK;
            }
        }

        return E_FAIL;
    }

    // ISetupInstance2
    STDMETHODIMP GetState(
        _Out_ InstanceState* pState
    )
    {
        return TryGetULONG(L"State", reinterpret_cast<PULONG>(pState));
    }

    STDMETHODIMP GetPackages(
        _Out_ LPSAFEARRAY* ppsaPackages
    )
    {
        if (!m_packages.empty())
        {
            auto psa = ::SafeArrayCreateVectorEx(VT_UNKNOWN, 0, m_packages.size(), (LPVOID)&__uuidof(ISetupPackageReference));
            auto hr = ::SafeArrayLock(psa);
            if (FAILED(hr))
            {
                throw win32_error(hr, "failed to lock packages array");
            }

            for (size_t i = 0; i < m_packages.size(); ++i)
            {
                const auto package = m_packages[i];
                auto rgData = (ElementType*)psa->pvData;

                rgData[i] = package;
            }

            hr = ::SafeArrayUnlock(psa);
            if (FAILED(hr))
            {
                throw win32_error(hr, "failed to unlock packages array");
            }

            *ppsaPackages = psa;
            return S_OK;
        }

        return E_NOTFOUND;
    }

    STDMETHODIMP GetProduct(
        _Outptr_result_maybenull_ ISetupPackageReference** ppPackage
    )
    {
        *ppPackage = m_product;
        return S_OK;
    }

    STDMETHODIMP GetProductPath(
        _Outptr_result_maybenull_ BSTR* pbstrProductPath
    )
    {
        return TryGetBSTR(L"ProductPath", pbstrProductPath, TRUE);
    }

    STDMETHODIMP GetErrors(
        _Outptr_result_maybenull_ ISetupErrorState** ppErrorState
    )
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP IsLaunchable(
        _Out_ VARIANT_BOOL* pfIsLaunchable
    )
    {
        return TryGetBOOL(L"IsLaunchable", pfIsLaunchable);
    }

    STDMETHODIMP IsComplete(
        _Out_ VARIANT_BOOL* pfIsComplete
    )
    {
        return TryGetBOOL(L"IsComplete", pfIsComplete);
    }

    STDMETHODIMP GetProperties(
        _Outptr_result_maybenull_ ISetupPropertyStore** ppProperties
    )
    {
        if (!ppProperties)
        {
            return E_POINTER;
        }

        auto hr = S_OK;
        *ppProperties = nullptr;

        if (!m_additionalProperties.empty())
        {
            *ppProperties = new (std::nothrow) TestPropertyStore(m_additionalProperties);
            if (!*ppProperties)
            {
                hr = E_OUTOFMEMORY;
            }
        }

        return hr;
    }

    STDMETHODIMP GetEnginePath(
        _Outptr_result_maybenull_ BSTR* pbstrEnginePath
    )
    {
        return TryGetBSTR(L"EnginePath", pbstrEnginePath, TRUE);
    }

    // ISetupInstanceCatalog
    STDMETHODIMP GetCatalogInfo(
        _Out_ ISetupPropertyStore** ppCatalogInfo
    )
    {
        if (!ppCatalogInfo)
        {
            return E_POINTER;
        }

        *ppCatalogInfo = nullptr;

        if (m_catalogProperties.empty())
        {
            return E_NOTFOUND;
        }
        else
        {
            *ppCatalogInfo = new (std::nothrow) TestPropertyStore(m_catalogProperties);
            if (!*ppCatalogInfo)
            {
                return E_OUTOFMEMORY;
            }

            return S_OK;
        }
    }

    STDMETHODIMP IsPrerelease(
        _Out_ VARIANT_BOOL* pfIsPrerelease
    )
    {
        if (!pfIsPrerelease)
        {
            return E_POINTER;
        }

        *pfIsPrerelease = VARIANT_FALSE;

        variant_t vtProductSemanticVersion;
        if (SUCCEEDED(m_catalogProperties.GetValue(L"productSemanticVersion", &vtProductSemanticVersion)))
        {
            *pfIsPrerelease = ::wcschr(vtProductSemanticVersion.bstrVal, L'-') ? VARIANT_TRUE : VARIANT_FALSE;
        }

        return S_OK;
    }

    // Other
    void AssignCatalogProperties(_In_ const TestPropertyStore& catalogProperties)
    {
        m_catalogProperties = catalogProperties;
    }

    void AssignAdditionalProperties(_In_ const TestPropertyStore& additionalProperties)
    {
        m_additionalProperties = additionalProperties;
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

    STDMETHODIMP TryGetBSTR(_In_ LPCWSTR wszName, _Out_ BSTR* pbstrValue, _In_opt_ BOOL fAllowNull = FALSE)
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
            if (!*pbstrValue)
            {
                return E_OUTOFMEMORY;
            }
        }
        else if (fAllowNull && E_NOTFOUND == hr)
        {
            *pbstrValue = NULL;
            return S_OK;
        }

        return hr;
    }

    STDMETHODIMP TryGetBOOL(_In_ LPCWSTR wszName, _Out_ VARIANT_BOOL* pvf)
    {
        if (!pvf)
        {
            return E_POINTER;
        }

        static ci_equal equals;
        std::wstring value;

        auto hr = TryGet(wszName, value);
        if (SUCCEEDED(hr))
        {
            auto f = L"1" == value || equals(L"true", value);
            *pvf = f ? VARIANT_TRUE : VARIANT_FALSE;
        }

        return hr;
    }

    STDMETHODIMP TryGetULONG(_In_ LPCWSTR wszName, _Out_ PULONG pul)
    {
        if (!pul)
        {
            return E_POINTER;
        }

        std::wstring value;

        auto hr = TryGet(wszName, value);
        if (SUCCEEDED(hr))
        {
            *pul = _wcstoul_l(value.c_str(), NULL, 10, m_locale);
            if (*pul == 0 || *pul == ULONG_MAX)
            {
                if (errno == ERANGE)
                {
                    hr = E_INVALIDARG;
                }
            }
        }

        return hr;
    }

    ISetupPackageReferencePtr m_product;
    std::vector<ISetupPackageReferencePtr> m_packages;
    MapType m_properties;
    TestPropertyStore m_catalogProperties;
    TestPropertyStore m_additionalProperties;
    _locale_t m_locale;
    ULONG m_ulRef;
};
