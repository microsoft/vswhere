// <copyright file="LegacyInstance.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

template <class T>
inline HRESULT NotImplemented(_Out_opt_ T* p)
{
    if (!p)
    {
        return E_POINTER;
    }

    *p = NULL;
    return E_NOTFOUND;
}

template<>
inline HRESULT NotImplemented(_Out_opt_ LPFILETIME pft)
{
    if (!pft)
    {
        return E_POINTER;
    }

    ::SecureZeroMemory(pft, sizeof(FILETIME));
    return E_NOTFOUND;
}

STDMETHODIMP LegacyInstance::GetInstanceId(
    _Out_ BSTR* pbstrInstanceId
)
{
    if (!pbstrInstanceId)
    {
        return E_POINTER;
    }

    *pbstrInstanceId = NULL;

    // Let exceptions throw since we're not a "true" COM object.
    wstring instanceId(L"VisualStudio.");
    instanceId += m_version;

    *pbstrInstanceId = ::SysAllocString(instanceId.c_str());
    if (!pbstrInstanceId)
    {
        return E_OUTOFMEMORY;
    }

    return S_OK;
}

STDMETHODIMP LegacyInstance::GetInstallDate(
    _Out_ LPFILETIME pInstallDate
)
{
    return NotImplemented(pInstallDate);
}

STDMETHODIMP LegacyInstance::GetInstallationName(
    _Out_ BSTR* pbstrInstallationName
)
{
    return NotImplemented(pbstrInstallationName);
}

STDMETHODIMP LegacyInstance::GetInstallationPath(
    _Out_ BSTR* pbstrInstallationPath
)
{
    if (!pbstrInstallationPath)
    {
        return E_POINTER;
    }

    *pbstrInstallationPath = ::SysAllocString(m_path.c_str());
    if (!pbstrInstallationPath)
    {
        return E_OUTOFMEMORY;
    }

    return S_OK;
}

STDMETHODIMP LegacyInstance::GetInstallationVersion(
    _Out_ BSTR* pbstrInstallationVersion
)
{
    if (!pbstrInstallationVersion)
    {
        return E_POINTER;
    }

    *pbstrInstallationVersion = ::SysAllocString(m_version.c_str());
    if (!pbstrInstallationVersion)
    {
        return E_OUTOFMEMORY;
    }

    return S_OK;
}

STDMETHODIMP LegacyInstance::GetDisplayName(
    _In_ LCID lcid,
    _Out_ BSTR* pbstrDisplayName
)
{
    return NotImplemented(pbstrDisplayName);
}

STDMETHODIMP LegacyInstance::GetDescription(
    _In_ LCID lcid,
    _Out_ BSTR* pbstrDescription
)
{
    return NotImplemented(pbstrDescription);
}

STDMETHODIMP LegacyInstance::ResolvePath(
    _In_opt_z_ LPCOLESTR pwszRelativePath,
    _Out_ BSTR* pbstrAbsolutePath
)
{
    return NotImplemented(pbstrAbsolutePath);
}
