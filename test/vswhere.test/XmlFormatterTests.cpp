// <copyright file="XmlFormatterTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(XmlFormatterTests)
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
            { L"InstallDate", L"2017-02-23T01:22:35Z" }
        };

        XmlFormatter sut;
        sut.Write(args, console, &instance);

        auto expected =
            L"<?xml version=\"1.0\"?>\n"
            L"<instances>\n"
            L"  <instance>\n"
            L"    <instanceId>a1b2c3</instanceId>\n"
            L"    <installDate>2017-02-23T01:22:35Z</installDate>\n"
            L"    <installationName>test</installationName>\n"
            L"  </instance>\n"
            L"</instances>\n";

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

        XmlFormatter sut;
        sut.Write(args, console, instances);

        auto expected =
            L"<?xml version=\"1.0\"?>\n"
            L"<instances>\n"
            L"  <instance>\n"
            L"    <instanceId>a1b2c3</instanceId>\n"
            L"    <installationName>test</installationName>\n"
            L"  </instance>\n"
            L"  <instance>\n"
            L"    <instanceId>b1c2d3</instanceId>\n"
            L"    <installationPath>C:\\ShouldNotExist</installationPath>\n"
            L"    <installationVersion>1.2.3.4</installationVersion>\n"
            L"  </instance>\n"
            L"</instances>\n";

        Assert::AreEqual(expected, console);
    }

    TEST_METHOD(Write_No_Instances)
    {
        CommandArgs args;
        TestConsole console(args);
        vector<ISetupInstancePtr> instances;

        XmlFormatter sut;
        sut.Write(args, console, instances);

        auto expected =
            L"<?xml version=\"1.0\"?>\n"
            L"<instances>\n"
            L"</instances>\n";

        Assert::AreEqual(expected, console);
    }
};
