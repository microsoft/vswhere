// <copyright file="TestInstance.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once


class TestInstance :
    public ISetupInstance2
{
public:
    typedef ISetupPackageReference* ElementType;
    typedef std::unordered_map<std::wstring, std::wstring, std::hash<std::wstring>, ci_equal> MapType;

    TestInstance(_In_ std::initializer_list<MapType::value_type> list) :
        m_properties(list.begin(), list.end()),
        m_ulRef(1)
    {
    }

    TestInstance(
        _In_ const ElementType pProduct,
        _In_ const std::vector<ElementType>& packages,
        _In_ const MapType& properties) :
        m_product(pProduct),
        m_properties(properties),
        m_ulRef(1)
    {
        for (const auto package : packages)
        {
            m_packages.push_back(package);
        }
    }

    ~TestInstance()
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
        else if (riid == __uuidof(ISetupInstance2))
        {
            AddRef();
            *ppvObject = static_cast<ISetupInstance2*>(this);
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

    // ISetupInstance
    STDMETHODIMP GetInstanceId(
        _Out_ BSTR* pbstrInstanceId)
    {
        return TryGetBSTR(L"InstanceId", pbstrInstanceId);
    }

    STDMETHODIMP GetInstallDate(
        _Out_ LPFILETIME pInstallDate)
    {
        // Rather than parsing the value, return a fixed value for "2017-02-23T01:22:35Z".
        auto it = m_properties.find(L"installDate");
        if (it != m_properties.end())
        {
            *pInstallDate =
            {
                1343813982,
                30575987,
            };

            return S_OK;
        }

        return E_NOTFOUND;
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
        return E_NOTIMPL;
    }

    // ISetupInstance2
    STDMETHODIMP GetState(
        _Out_ InstanceState* pState
    )
    {
        return E_NOTIMPL;
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
        if (m_product)
        {
            *ppPackage = m_product;
            return S_OK;
        }

        return E_NOTFOUND;
    }

    STDMETHODIMP GetProductPath(
        _Outptr_result_maybenull_ BSTR* pbstrProductPath
    )
    {
        return TryGetBSTR(L"ProductPath", pbstrProductPath);
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
        return E_NOTIMPL;
    }

    STDMETHODIMP GetEnginePath(
        _Outptr_result_maybenull_ BSTR* pbstrEnginePath
    )
    {
        return TryGetBSTR(L"EnginePath", pbstrEnginePath);
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

    ISetupPackageReferencePtr m_product;
    std::vector<ISetupPackageReferencePtr> m_packages;
    MapType m_properties;
    ULONG m_ulRef;
};
