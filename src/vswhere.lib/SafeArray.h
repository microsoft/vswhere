// <copyright file="SafeArray.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

// Simple container for a single-dimension SAFEARRAY.
template <class _Element>
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
        Destroy();
    }

    const std::vector<_Element>& Elements() const
    {
        return m_elements;
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

            auto pvData = (_Element*)m_psa->pvData;
            auto celt = m_psa->rgsabound[0].cElements;

            m_elements.assign(pvData, pvData + celt);
        }
    }

    void Unlock()
    {
        if (m_psa)
        {
            ::SafeArrayUnlock(m_psa);
        }
    }

    void Destroy()
    {
        if (m_psa)
        {
            ::SafeArrayDestroy(m_psa);
        }
    }

    LPSAFEARRAY m_psa;
    std::vector<_Element> m_elements;
};
