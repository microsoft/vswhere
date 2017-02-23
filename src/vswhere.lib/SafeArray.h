// <copyright file="SafeArray.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

// Simple container for a single-dimension SAFEARRAY.
template <class InterfaceType>
class SafeArray
{
public:
    SafeArray(_In_ const LPSAFEARRAY psa) :
        m_psa(psa)
    {
        Lock();
    }

    SafeArray(_In_ const SafeArray& obj) :
        m_psa(obj.m_psa)
    {
        Lock();
    }

    ~SafeArray()
    {
        Unlock();
    }

    const std::vector<InterfaceType>& Elements() const
    {
        return m_interfaces;
    }

private:
    void Lock()
    {
        if (m_psa)
        {
            auto hr = ::SafeArrayLock(m_psa);
            if (FAILED(hr))
            {
                throw win32_error(hr);
            }

            auto pvData = (InterfaceType*)m_psa->pvData;
            auto celt = m_psa->rgsabound[0].cElements;

            m_interfaces.assign(pvData, pvData + celt);
        }
    }

    void Unlock()
    {
        if (m_psa)
        {
            ::SafeArrayUnlock(m_psa);
        }
    }

    LPSAFEARRAY m_psa;
    std::vector<InterfaceType> m_interfaces;
};
