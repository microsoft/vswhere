// <copyright file="InstanceSelector.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class InstanceSelector
{
public:
    InstanceSelector(_In_ const CommandArgs& args, _In_opt_ ISetupHelper* pHelper = NULL);
    InstanceSelector(_In_ const InstanceSelector& obj) :
        m_args(obj.m_args),
        m_helper(obj.m_helper),
        m_ullMinimumVersion(obj.m_ullMinimumVersion),
        m_ullMaximumVersion(obj.m_ullMaximumVersion)
    {
    }

    bool Less(const ISetupInstancePtr& a, const ISetupInstancePtr&  b) const;
    std::vector<ISetupInstancePtr> Select(_In_ IEnumSetupInstances* pEnum) const;

private:
    static std::wstring GetId(_In_ ISetupPackageReference* pPackageReference);
    bool IsMatch(_In_ ISetupInstance* pInstance) const;
    bool IsProductMatch(_In_ ISetupInstance2* pInstance) const;
    bool IsWorkloadMatch(_In_ ISetupInstance2* pInstance) const;
    bool IsVersionMatch(_In_ ISetupInstance* pInstance) const;
    bool HasVersionRange() const
    {
        return m_ullMinimumVersion != 0 && m_ullMaximumVersion != 0;
    }

    const CommandArgs& m_args;
    ULONGLONG m_ullMinimumVersion;
    ULONGLONG m_ullMaximumVersion;
    ISetupHelperPtr m_helper;
};
