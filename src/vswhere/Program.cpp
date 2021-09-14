// <copyright file="Program.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

void GetEnumerator(_In_ const CommandArgs& args, _In_ ISetupConfigurationPtr& query, _In_ IEnumSetupInstancesPtr& e);
wstring GetFullPath(_In_ const wstring& path);
void WriteLogo(_In_ const CommandArgs& args, _In_ Console& console, _In_ Module& module);

int wmain(_In_ int argc, _In_ LPCWSTR argv[])
{
    CommandArgs args;
    Console console(args);
    Module queryModule;

    console.Initialize();
    try
    {
        CoInitializer init;

        // Create the query object early to print version in logo.
        ISetupConfigurationPtr query;
        auto hr = query.CreateInstance(__uuidof(SetupConfiguration));
        if (FAILED(hr))
        {
            if (REGDB_E_CLASSNOTREG != hr)
            {
                throw win32_error(hr);
            }
        }

        // Try to get information about the query module for later.
        queryModule.FromIUnknown(static_cast<IUnknown*>(query));

        args.Parse(argc, argv);
        if (args.get_Help())
        {
            WriteLogo(args, console, queryModule);
            args.Usage(console);

            return ERROR_SUCCESS;
        }

        // Attempt to get the ISetupHelper.
        ISetupHelperPtr helper;
        if (query)
        {
            query->QueryInterface(&helper);
        }
        // Fall back to a copy of the current implementation.
        else
        {
            helper.Attach(new VersionRange);
        }

        vector<ISetupInstancePtr> instances;
        if (args.get_Path().empty())
        {
            IEnumSetupInstancesPtr e;
            GetEnumerator(args, query, e);

            InstanceSelector selector(args, helper);
            instances = std::move(selector.Select(e));
        }
        else
        {
            auto path = GetFullPath(args.get_Path());

            ISetupInstancePtr instance;
            hr = query->GetInstanceForPath(path.c_str(), &instance);
            if (SUCCEEDED(hr))
            {
                instances.push_back(instance);
            }
        }

        // Create the formatter and optionally show the logo.
        auto formatter = Formatter::Create(args.get_Format(), args, console);
        if (formatter->ShowLogo())
        {
            WriteLogo(args, console, queryModule);
        }

        if (args.get_Find().length())
        {
            formatter->WriteFiles(instances);
        }
        else
        {
            formatter->Write(instances);
        }

        return ERROR_SUCCESS;
    }
    catch (const system_error& ex)
    {
        const auto code = ex.code().value();
        if (ERROR_INVALID_PARAMETER == code)
        {
            WriteLogo(args, console, queryModule);
        }

        console.Write(L"%ls 0x%x: ", ResourceManager::GetString(IDS_ERROR).c_str(), code);

        const auto* err = dynamic_cast<const win32_error*>(&ex);
        if (err)
        {
            console.WriteLine(L"%ls", err->wwhat());
        }
        else
        {
            console.WriteLine(L"%hs", ex.what());
        }

        return ex.code().value();
    }
    catch (const exception& ex)
    {
        console.WriteLine(L"%ls: %hs", ResourceManager::GetString(IDS_ERROR).c_str(), ex.what());
    }
    catch (...)
    {
        console.WriteLine(L"%ls: %ls", ResourceManager::GetString(IDS_ERROR).c_str(), ResourceManager::GetString(IDS_E_UNKNOWN).c_str());
    }

    return E_FAIL;
}

void GetEnumerator(_In_ const CommandArgs& args, _In_ ISetupConfigurationPtr& query, _In_ IEnumSetupInstancesPtr& e)
{
    if (!query)
    {
        return;
    }

    // If all instances are requested, try to get the proper enumerator; otherwise, fall back to original enumerator.
    if (args.get_All())
    {
        ISetupConfiguration2Ptr query2;

        auto hr = query->QueryInterface(&query2);
        if (SUCCEEDED(hr))
        {
            hr = query2->EnumAllInstances(&e);
            if (FAILED(hr))
            {
                throw win32_error(hr);
            }
        }
    }

    if (!e)
    {
        auto hr = query->EnumInstances(&e);
        if (FAILED(hr))
        {
            throw win32_error(hr);
        }
    }
}

wstring GetFullPath(_In_ const wstring& path)
{
    DWORD ret = 0;
    wstring fullPath;

    for (;;)
    {
        ret = ::GetFullPathNameW(path.c_str(), fullPath.capacity(), const_cast<LPWSTR>(fullPath.c_str()), NULL);

        if (ret == 0)
        {
            throw win32_error();
        }
        // If buffer too small, return value contains required character count including terminating null.
        else if (ret >= fullPath.capacity())
        {
            fullPath.resize(ret, L'\0');
        }
        else
        {
            break;
        }
    }

    return fullPath;
}

void WriteLogo(_In_ const CommandArgs& args, _In_ Console& console, _In_ Module& module)
{
    if (args.get_Logo())
    {
        const auto version = module.get_FileVersion();
        const auto nID = version.empty() ? IDS_PROGRAMINFO : IDS_PROGRAMINFOEX;

        console.WriteLine(ResourceManager::FormatString(nID, NBGV_INFORMATIONAL_VERSION, version.c_str()));
        console.WriteLine(ResourceManager::GetString(IDS_COPYRIGHT));
        console.WriteLine();
    }
}
