// <copyright file="InstanceSelector.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using std::placeholders::_1;

const ci_equal InstanceSelector::s_comparer;

InstanceSelector::InstanceSelector(_In_ const CommandArgs& args, _In_opt_ ISetupHelper* pHelper) :
    m_args(args)
{
    // Get the ISetupHelper (if implemented) if a version range is specified.
    auto version = args.get_Version();
    if (!version.empty())
    {
        m_helper = pHelper;
        if (m_helper)
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

vector<ISetupInstancePtr> InstanceSelector::Select(_In_ IEnumSetupInstances* pEnum) const
{
    _ASSERT(pEnum);

    HRESULT hr = S_OK;
    unsigned long celtFetched = 0;
    ISetupInstance* pInstances[1] = {};

    vector<ISetupInstancePtr> instances;
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

    return instances;
}

bool InstanceSelector::IsMatch(_In_ ISetupInstance* pInstance) const
{
    _ASSERT(pInstance);

    HRESULT hr = S_OK;
    ISetupInstance2Ptr instance;

    hr = pInstance->QueryInterface(&instance);
    if (FAILED(hr))
    {
        // Only VS products were released before ISetupInstance2 was released, so if no workload requirements assume it matches.
        return m_args.get_Requires().empty();
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
    if (FAILED(hr))
    {
        // Should always have a product so no match.
        return false;
    }

    const auto productId = GetId(product);
    if (productId.empty())
    {
        return false;
    }

    const auto products = m_args.get_Products();
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

    auto requires = m_args.get_Requires();
    if (requires.empty())
    {
        // No workloads required matches every instance.
        return true;
    }

    // Keep track of which requirements we matched.
    typedef map<wstring, bool, ci_less> MapType;
    MapType found;
    for (const auto require : requires)
    {
        found.emplace(make_pair(require, false));
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

        auto it = found.find(id);
        if (it != found.end())
        {
            it->second = true;
        }
    }

    return all_of(found.begin(), found.end(), [](MapType::const_reference pair) -> bool
    {
        return pair.second;
    });
}

bool InstanceSelector::IsVersionMatch(_In_ ISetupInstance* pInstance) const
{
    _ASSERT(pInstance);

    // m_helper will be NULL if no version range was specified or the interface was not yet implemented.
    if (!m_helper)
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
