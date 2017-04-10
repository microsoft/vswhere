// <copyright file="Module.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

void Module::FromIUnknown(_In_ const IUnknown* pUnk) noexcept
{
    typedef struct IUnknownVtbl
    {
        HRESULT(STDMETHODCALLTYPE *QueryInterface)(ISetupConfiguration*, REFIID, LPVOID*);
        ULONG(STDMETHODCALLTYPE *AddRef)(ISetupConfiguration*);
        ULONG(STDMETHODCALLTYPE *Release)(ISetupConfiguration*);
    } IUnknownVtbl;

    auto pVtbl = reinterpret_cast<const IUnknownVtbl*>(pUnk);
    ::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCWSTR>(pVtbl->QueryInterface), &m_hModule);
}

const wstring& Module::get_Path() noexcept
{
    if (m_hModule && m_path.empty())
    {
        m_path.resize(MAX_PATH);
        ::GetModuleFileNameW(m_hModule, const_cast<LPWSTR>(m_path.data()), m_path.size());
    }

    return m_path;
}

const wstring& Module::get_FileVersion() noexcept
{
    auto path = get_Path();
    if (path.empty())
    {
        return m_fileVersion;
    }

    DWORD dwHandle = 0;
    DWORD cbVersionInfo = ::GetFileVersionInfoSizeW(path.c_str(), &dwHandle);
    if (!cbVersionInfo)
    {
        return m_fileVersion;
    }

    vector<byte> buffer(cbVersionInfo);
    if (!::GetFileVersionInfoW(path.c_str(), dwHandle, buffer.size(), buffer.data()))
    {
        return m_fileVersion;
    }

    VS_FIXEDFILEINFO* pFileInfo = NULL;
    UINT cbFileInfo = 0;
    if (!::VerQueryValueW(buffer.data(), L"\\", reinterpret_cast<LPVOID*>(&pFileInfo), &cbFileInfo))
    {
        return m_fileVersion;
    }

    auto cch = _scwprintf(
        L"%d.%d.%d.%d",
        (pFileInfo->dwFileVersionMS >> 16) & 0xffff,
        pFileInfo->dwFileVersionMS & 0xffff,
        (pFileInfo->dwFileVersionLS >> 16) & 0xffff,
        pFileInfo->dwFileVersionLS & 0xffff);
    if (!cch)
    {
        return m_fileVersion;
    }

    m_fileVersion.resize(++cch);
    swprintf_s(
        const_cast<LPWSTR>(m_fileVersion.c_str()),
        m_fileVersion.size(),
        L"%d.%d.%d.%d",
        (pFileInfo->dwFileVersionMS >> 16) & 0xffff,
        pFileInfo->dwFileVersionMS & 0xffff,
        (pFileInfo->dwFileVersionLS >> 16) & 0xffff,
        pFileInfo->dwFileVersionLS & 0xffff);

    // Trim terminating null character.
    m_fileVersion.resize(--cch);

    return m_fileVersion;
}
