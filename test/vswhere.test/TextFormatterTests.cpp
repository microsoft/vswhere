// <copyright file="TextFormatterTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(TextFormatterTests)
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
        };

        TextFormatter sut;
        sut.Write(args, console, &instance);

        auto expected =
            L"instanceId: a1b2c3\n"
            L"installationName: test\n";

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

        TextFormatter sut;
        sut.Write(args, console, instances);

        auto expected =
            L"instanceId: a1b2c3\n"
            L"installationName: test\n"
            L"\n"
            L"instanceId: b1c2d3\n"
            L"installationPath: C:\\ShouldNotExist\n"
            L"installationVersion: 1.2.3.4\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_ProductId)
        TEST_WORKITEM(104)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_ProductId)
    {
        CommandArgs args;
        TestConsole console(args);

        TestPackageReference product =
        {
            { L"Id", L"Microsoft.VisualStudio.Product.Enterprise" },
        };

        TestInstance::MapType properties =
        {
            { L"InstanceId", L"a1b2c3" },
        };

        TestInstance instance(&product, {}, properties);

        TextFormatter sut;
        sut.Write(args, console, &instance);

        auto expected =
            L"instanceId: a1b2c3\n"
            L"productId: Microsoft.VisualStudio.Product.Enterprise\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_ProductPath)
        TEST_WORKITEM(104)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_ProductPath)
    {
        CommandArgs args;
        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationPath", L"C:\\ShouldNotExist" },
            { L"ProductPath", L"Common7\\IDE\\devenv.exe" },
        };

        TextFormatter sut;
        sut.Write(args, console, &instance);

        auto expected =
            L"instanceId: a1b2c3\n"
            L"installationPath: C:\\ShouldNotExist\n"
            L"productPath: C:\\ShouldNotExist\\Common7\\IDE\\devenv.exe\n";

        Assert::AreEqual(expected, console);
    }
};
