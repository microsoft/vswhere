// <copyright file="InstanceSelector.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using std::placeholders::_1;

ci_equal InstanceSelector::s_comparer;

InstanceSelector::InstanceSelector(_In_ const CommandArgs& args, _In_ ILegacyProvider& provider, _In_opt_ ISetupHelper* pHelper) :
    m_args(args),
    m_provider(provider),
    m_ullMinimumVersion(0),
    m_ullMaximumVersion(0)
{
    m_helper = pHelper;
    if (m_helper)
    {
        auto& version = args.get_Version();
        if (!version.empty())
        {
            auto hr = m_helper->ParseVersionRange(version.c_str(), &m_ullMinimumVersion, &m_ullMaximumVersion);
            if (FAILED(hr))
            {
                // Late evaluation of the -version parameter so throw like any other invalid parameter.
                auto message = ResourceManager::FormatString(IDS_E_INVALIDVERSION, version.c_str());
                throw win32_error(ERROR_INVALID_PARAMETER, message);
            }
        }
    }
}

bool InstanceSelector::Less(const ISetupInstancePtr& a, const ISetupInstancePtr&  b) const
{
    static ci_less less;

    bstr_t bstrVersionA, bstrVersionB;
    ULONGLONG ullVersionA, ullVersionB;
    FILETIME ftDateA, ftDateB;

    // Compare versions.
    auto hrA = a->GetInstallationVersion(bstrVersionA.GetAddress());
    auto hrB = b->GetInstallationVersion(bstrVersionB.GetAddress());
    if (SUCCEEDED(hrA) && SUCCEEDED(hrB))
    {
        if (m_helper)
        {
            hrA = m_helper->ParseVersion(bstrVersionA, &ullVersionA);
            hrB = m_helper->ParseVersion(bstrVersionB, &ullVersionB);
            if (SUCCEEDED(hrA) && SUCCEEDED(hrB))
            {
                if (ullVersionA != ullVersionB)
                {
                    return ullVersionA < ullVersionB;
                }
            }
            else
            {
                // a is less than b if we can't parse version for a but can for b.
                return SUCCEEDED(hrB);
            }
        }
        else
        {
            // If ISetupHelper is not available we have only legacy products, or very early pre-releases of VS2017.
            // For version 10.0 and newer either should lexicographically sort correctly.
            return less(wstring(bstrVersionA), wstring(bstrVersionB));
        }
    }
    else
    {
        // a is less than b if we can't get version for a but can for b.
        return SUCCEEDED(hrB);
    }

    // Compare dates.
    hrA = a->GetInstallDate(&ftDateA);
    hrB = b->GetInstallDate(&ftDateB);
    if (SUCCEEDED(hrA) && SUCCEEDED(hrB))
    {
        auto result = ::CompareFileTime(&ftDateA, &ftDateB);
        if (0 == result)
        {
            auto message = ResourceManager::GetString(IDS_E_UNEXPECTEDDATE);
            throw win32_error(E_UNEXPECTED, message);
        }

        return 0 > result;
    }
    else
    {
        // a is less than b if we can't get date for a but can for b.
        return SUCCEEDED(hrB);
    }
}

vector<ISetupInstancePtr> InstanceSelector::Select(_In_opt_ IEnumSetupInstances* pEnum) const
{
    vector<ISetupInstancePtr> instances;
    if (pEnum)
    {
        HRESULT hr = S_OK;
        unsigned long celtFetched = 0;
        ISetupInstance* pInstances[1] = {};

        do
        {
            celtFetched = 0;

            hr = pEnum->Next(1, pInstances, &celtFetched);
            if (SUCCEEDED(hr) && celtFetched)
            {
                ISetupInstancePtr instance(pInstances[0], false);
                if (IsMatch(instance))
                {
                    instances.push_back(instance);
                }
            }
        } while (SUCCEEDED(hr) && celtFetched);
    }

    if (m_args.get_Legacy() && m_provider.HasLegacyInstances())
    {
        vector<LPCWSTR> versions =
        {
            L"14.0",
            L"12.0",
            L"11.0",
            L"10.0",
        };

        for (const auto version : versions)
        {
            ISetupInstancePtr instance;

            // Currently only support version checks.
            if (m_provider.TryGetLegacyInstance(version, &instance) && IsVersionMatch(instance))
            {
                instances.push_back(instance);
            }
        }
    }

    if (1 < instances.size())
    {
        if (m_args.get_Latest() || m_args.get_Sort())
        {
            sort(instances.begin(), instances.end(), [&](const ISetupInstancePtr& a, const ISetupInstancePtr& b) -> bool
            {
                // Reverse the sort with addition of -sort using existing algorithm.
                return !Less(a, b);
            });

            if (m_args.get_Latest())
            {
                return vector<ISetupInstancePtr>
                {
                    instances.front(),
                };
            }
        }
    }

    return instances;
}

bool InstanceSelector::IsMatch(_In_ ISetupInstance* pInstance) const
{
    _ASSERT(pInstance);

    HRESULT hr = S_OK;
    ISetupInstance2Ptr instance;
    ISetupInstanceCatalogPtr catalog;

    hr = pInstance->QueryInterface(&instance);
    if (FAILED(hr))
    {
        // Only VS products were released before ISetupInstance2 was released, so if no workload requirements assume it matches.
        return m_args.get_Requires().empty();
    }

    hr = pInstance->QueryInterface(&catalog);
    if (SUCCEEDED(hr))
    {
        VARIANT_BOOL fPrerelease = VARIANT_FALSE;

        hr = catalog->IsPrerelease(&fPrerelease);
        if (SUCCEEDED(hr) && fPrerelease && !m_args.get_Prerelease())
        {
            return false;
        }
    }

    if (!IsProductMatch(instance))
    {
        return false;
    }

    if (!IsVersionMatch(instance))
    {
        return false;
    }

    if (!IsWorkloadMatch(instance))
    {
        return false;
    }

    return true;
}

bool InstanceSelector::IsProductMatch(_In_ ISetupInstance2* pInstance) const
{
    _ASSERT(pInstance);

    ISetupPackageReferencePtr product;

    auto hr = pInstance->GetProduct(&product);
    if (FAILED(hr) || !product)
    {
        // Should always have a product so no match.
        return false;
    }

    // Asterisk on command line will clear the array to find any products.
    const auto& products = m_args.get_Products();
    if (products.empty())
    {
        return true;
    }

    const auto productId = GetId(product);
    if (productId.empty())
    {
        return false;
    }

    const auto ci_equal_productId = bind(ci_equal(), productId, _1);
    const auto it = find_if(products.begin(), products.end(), ci_equal_productId);
    if (it == products.end())
    {
        return false;
    }

    return true;
}

bool InstanceSelector::IsWorkloadMatch(_In_ ISetupInstance2* pInstance) const
{
    _ASSERT(pInstance);

    // Create copies and erase elements as found.
    auto literals = m_args.get_Requires();
    auto literals_count = literals.size();

    auto patterns = m_args.get_RequiresPattern();
    auto patterns_count = patterns.size();

    if (literals.empty() && patterns.empty())
    {
        // No workloads required matches every instance.
        return true;
    }

    LPSAFEARRAY psa = NULL;
    auto hr = pInstance->GetPackages(&psa);
    if (FAILED(hr))
    {
        return false;
    }

    SafeArray<ISetupPackageReference*> packages(psa);
    for (const auto package : packages.Elements())
    {
        auto id = GetId(package);

        for (auto it = literals.cbegin(); it != literals.cend(); ++it)
        {
            if (s_comparer(id, *it))
            {
                literals.erase(it);
                goto next;
            }
        }

        for (auto it = patterns.cbegin(); it != patterns.cend(); ++it)
        {
            if (regex_match(id, *it))
            {
                patterns.erase(it);
                goto next;
            }
        }

    next: continue;
    }

    if (m_args.get_RequiresAny())
    {
        return literals.size() < literals_count
            || patterns.size() < patterns_count;
    }

    return literals.empty() && patterns.empty();
}

bool InstanceSelector::IsVersionMatch(_In_ ISetupInstance* pInstance) const
{
    _ASSERT(pInstance);

    if (!HasVersionRange())
    {
        return true;
    }

    bstr_t bstrInstallationVersion;
    ULONGLONG ullInstallationVersion = 0;

    auto hr = pInstance->GetInstallationVersion(bstrInstallationVersion.GetAddress());
    if (FAILED(hr))
    {
        return false;
    }

    hr = m_helper->ParseVersion(bstrInstallationVersion, &ullInstallationVersion);
    if (FAILED(hr))
    {
        return false;
    }

    return m_ullMinimumVersion <= ullInstallationVersion && ullInstallationVersion <= m_ullMaximumVersion;
}

wstring InstanceSelector::GetId(_In_ ISetupPackageReference* pPackageReference)
{
    _ASSERT(pPackageReference);

    bstr_t bstrId;

    auto hr = pPackageReference->GetId(bstrId.GetAddress());
    if (FAILED(hr))
    {
        // Exceptional since Id is required on all package references.
        throw win32_error(hr);
    }

    return wstring(bstrId);
}
