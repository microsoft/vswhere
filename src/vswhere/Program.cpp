// <copyright file="Program.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

void WriteLogo(_In_ const CommandArgs& args, _In_ wostream& out);

int wmain(_In_ int argc, _In_ LPCWSTR argv[])
{
    CommandArgs args;
    wostream& out = wcout;

    try
    {
        CoInitializer init;

        args.Parse(argc, argv);
        if (args.get_Help())
        {
            WriteLogo(args, out);
            args.Usage(out);

            return ERROR_SUCCESS;
        }

        ISetupConfigurationPtr query;
        IEnumSetupInstancesPtr e;

        auto hr = query.CreateInstance(__uuidof(SetupConfiguration));
        if (FAILED(hr))
        {
            if (REGDB_E_CLASSNOTREG == hr)
            {
                WriteLogo(args, out);
                return ERROR_SUCCESS;
            }
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

        // Attempt to get the ISetupHelper.
        ISetupHelperPtr helper;
        query->QueryInterface(&helper);

        InstanceSelector selector(args, helper);
        auto instances = selector.Select(e);

        // Create the formatter and optionally show the logo.
        auto formatter = Formatter::Create(args.get_Format());
        if (formatter->ShowLogo())
        {
            WriteLogo(args, out);
        }

        formatter->Write(out, instances);
        return ERROR_SUCCESS;
    }
    catch (const system_error& ex)
    {
        out << ResourceManager::GetString(IDS_ERROR) << L" " << hex << showbase << ex.code().value() << L": " << ex.what() << endl;
        return ex.code().value();
    }
    catch (const exception& ex)
    {
        out << ResourceManager::GetString(IDS_ERROR) << L": " << ex.what() << endl;
    }
    catch (...)
    {
        out << ResourceManager::GetString(IDS_ERROR) << L": " << ResourceManager::GetString(IDS_E_UNKNOWN) << endl;
    }

    return E_FAIL;
}

void WriteLogo(_In_ const CommandArgs& args, _In_ wostream& out)
{
    if (args.get_Logo())
    {
        out << ResourceManager::FormatString(IDS_PROGRAMINFO, FILE_VERSION_STRINGW) << endl;
        out << ResourceManager::GetString(IDS_COPYRIGHT) << endl;
        out << endl;
    }
}
