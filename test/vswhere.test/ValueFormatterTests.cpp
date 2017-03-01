// <copyright file="ValueFormatterTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(ValueFormatterTests)
{
public:
    TEST_METHOD(Write_Instance)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe -property instanceId");

        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
        };

        ValueFormatter sut;
        wostringstream ostr;

        sut.Write(args, ostr, &instance);

        auto actual = ostr.str();
        auto expected = L"a1b2c3\n";

        Assert::AreEqual(expected, actual.c_str());
    }

    TEST_METHOD(Write_Instances_Single)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe -property InstallationPath");

        TestInstance instance1 =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
        };

        TestInstance instance2 =
        {
            { L"instanceId", L"b1c2d3" },
            { L"installationPath", L"C:\\ShouldNotExist" },
            { L"installationVersion", L"1.2.3.4" },
        };

        vector<ISetupInstancePtr> instances =
        {
            &instance1,
            &instance2,
        };

        ValueFormatter sut;
        wostringstream ostr;

        sut.Write(args, ostr, instances);

        auto actual = ostr.str();
        auto expected = L"C:\\ShouldNotExist\n";

        Assert::AreEqual(expected, actual.c_str());
    }

    TEST_METHOD(Write_Instances)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe -property instanceid");

        TestInstance instance1 =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
        };

        TestInstance instance2 =
        {
            { L"instanceId", L"b1c2d3" },
            { L"installationPath", L"C:\\ShouldNotExist" },
            { L"installationVersion", L"1.2.3.4" },
        };

        vector<ISetupInstancePtr> instances =
        {
            &instance1,
            &instance2,
        };

        ValueFormatter sut;
        wostringstream ostr;

        sut.Write(args, ostr, instances);

        auto actual = ostr.str();
        auto expected =
            L"a1b2c3\n"
            L"b1c2d3\n";

        Assert::AreEqual(expected, actual.c_str());
    }
};
