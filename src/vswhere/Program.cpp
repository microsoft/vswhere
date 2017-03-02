// <copyright file="Program.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

void GetEnumerator(_In_ const CommandArgs& args, _In_ ISetupConfigurationPtr& query, _In_ IEnumSetupInstancesPtr& e);
void WriteLogo(_In_ const CommandArgs& args, _In_ Console& console);

int wmain(_In_ int argc, _In_ LPCWSTR argv[])
{
    CommandArgs args;
    Console console(args);

    console.Initialize();
    try
    {
        CoInitializer init;

        args.Parse(argc, argv);
        if (args.get_Help())
        {
            WriteLogo(args, console);
            args.Usage(console);

            return ERROR_SUCCESS;
        }

        ISetupConfigurationPtr query;
        IEnumSetupInstancesPtr e;

        GetEnumerator(args, query, e);

        // Attempt to get the ISetupHelper.
        ISetupHelperPtr helper;
        if (query)
        {
            query->QueryInterface(&helper);
        }

        InstanceSelector selector(args, helper);
        auto instances = selector.Select(e);

        // Create the formatter and optionally show the logo.
        auto formatter = Formatter::Create(args.get_Format());
        if (formatter->ShowLogo())
        {
            WriteLogo(args, console);
        }

        formatter->Write(args, console, instances);
        return ERROR_SUCCESS;
    }
    catch (const system_error& ex)
    {
        const auto code = ex.code().value();
        if (ERROR_INVALID_PARAMETER == code)
        {
            WriteLogo(args, console);
        }

        console.Write(L"%ls 0x%x: ", ResourceManager::GetString(IDS_ERROR).c_str(), code);

        const auto* err = dynamic_cast<const win32_error*>(&ex);
        if (err)
        {
            console.WriteLine(err->wwhat());
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
    auto hr = query.CreateInstance(__uuidof(SetupConfiguration));
    if (FAILED(hr))
    {
        if (REGDB_E_CLASSNOTREG == hr)
        {
            return;
        }

        throw win32_error(hr);
    }

    // If all instances are requested, try to get the proper enumerator; otherwise, fall back to original enumerator.
    if (args.get_All())
    {
        ISetupConfiguration2Ptr query2;

        hr = query->QueryInterface(&query2);
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
        hr = query->EnumInstances(&e);
        if (FAILED(hr))
        {
            throw win32_error(hr);
        }
    }
}

void WriteLogo(_In_ const CommandArgs& args, _In_ Console& console)
{
    if (args.get_Logo())
    {
        console.WriteLine(ResourceManager::FormatString(IDS_PROGRAMINFO, FILE_VERSION_STRINGW));
        console.WriteLine(ResourceManager::GetString(IDS_COPYRIGHT));
        console.WriteLine();
    }
}
