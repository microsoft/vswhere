// <copyright file="JsonFormatterTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(JsonFormatterTests)
{
public:
    TEST_METHOD(Write_Instance)
    {
        CommandArgs args;
        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
            { L"InstallDate", L"2017-02-23T01:22:35Z"}
        };

        JsonFormatter sut;
        sut.Write(args, console, &instance);

        auto expected =
            L"[\n"
            L"  {\n"
            L"    \"instanceId\": \"a1b2c3\",\n"
            L"    \"installDate\": \"2017-02-23T01:22:35Z\",\n"
            L"    \"installationName\": \"test\"\n"
            L"  }\n"
            L"]\n";

        Assert::AreEqual(expected, console);
    }

    TEST_METHOD(Write_Instances)
    {
        CommandArgs args;
        TestConsole console(args);
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

        JsonFormatter sut;
        sut.Write(args, console, instances);

        auto expected =
            L"[\n"
            L"  {\n"
            L"    \"instanceId\": \"a1b2c3\",\n"
            L"    \"installationName\": \"test\"\n"
            L"  },\n"
            L"  {\n"
            L"    \"instanceId\": \"b1c2d3\",\n"
            L"    \"installationPath\": \"C:\\\\ShouldNotExist\",\n"
            L"    \"installationVersion\": \"1.2.3.4\"\n"
            L"  }\n"
            L"]\n";

        Assert::AreEqual(expected, console);
    }

    TEST_METHOD(Write_No_Instances)
    {
        CommandArgs args;
        TestConsole console(args);
        vector<ISetupInstancePtr> instances;

        JsonFormatter sut;
        sut.Write(args, console, instances);

        auto expected =
            L"[]\n";

        Assert::AreEqual(expected, console);
    }
};
