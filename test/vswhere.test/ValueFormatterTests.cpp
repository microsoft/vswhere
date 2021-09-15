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
        args.Parse(L"vswhere.exe");

        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
            { L"Description", L"This description contains \"quotes\"." },
        };

        ValueFormatter sut(args, console);
        sut.Write(&instance);

        auto expected = L"a1b2c3\n"
            L"test\n"
            L"This description contains \"quotes\".\n";

        Assert::AreEqual(expected, console);
    }

    TEST_METHOD(Write_Instances_Single)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe -property InstallationPath");

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

        ValueFormatter sut(args, console);
        sut.Write(instances);

        auto expected = L"C:\\ShouldNotExist\n";

        Assert::AreEqual(expected, console);
    }

    TEST_METHOD(Write_Instances)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe -property instanceid");

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

        ValueFormatter sut(args, console);
        sut.Write(instances);

        auto expected =
            L"a1b2c3\n"
            L"b1c2d3\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_With_Properties)
        TEST_WORKITEM(104)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_With_Properties)
    {
        CommandArgs args;
        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
        };

        TestPropertyStore properties =
        {
            { L"campaignId", make_tuple(VT_BSTR, L"abcd1234") },
            { L"nickname", make_tuple(VT_BSTR, L"test") },
        };

        instance.AssignAdditionalProperties(properties);

        ValueFormatter sut(args, console);
        sut.Write(&instance);

        auto expected =
            L"a1b2c3\n"
            L"test\n"
            L"abcd1234\n"
            L"test\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_All_Properties)
        TEST_WORKITEM(104)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_All_Properties)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe -property properties");

        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
            { L"InstallDate", L"2017-02-23T01:22:35Z" }
        };

        TestPropertyStore properties =
        {
            { L"campaignId", make_tuple(VT_BSTR, L"abcd1234") },
            { L"nickname", make_tuple(VT_BSTR, L"test") },
        };

        instance.AssignAdditionalProperties(properties);

        ValueFormatter sut(args, console);
        sut.Write(&instance);

        auto expected =
            L"abcd1234\n"
            L"test\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_Specified_Property)
        TEST_WORKITEM(104)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_Specified_Property)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe -property properties.nickname");

        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
            { L"InstallDate", L"2017-02-23T01:22:35Z" }
        };

        TestPropertyStore properties =
        {
            { L"campaignId", make_tuple(VT_BSTR, L"abcd1234") },
            { L"nickname", make_tuple(VT_BSTR, L"test") },
        };

        instance.AssignAdditionalProperties(properties);

        ValueFormatter sut(args, console);
        sut.Write(&instance);

        auto expected =
            L"test\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_Specified_Short_Property)
        TEST_WORKITEM(104)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_Specified_Short_Property)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe -property nickname");

        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
            { L"InstallDate", L"2017-02-23T01:22:35Z" }
        };

        TestPropertyStore properties =
        {
            { L"campaignId", make_tuple(VT_BSTR, L"abcd1234") },
            { L"nickname", make_tuple(VT_BSTR, L"test") },
        };

        instance.AssignAdditionalProperties(properties);

        ValueFormatter sut(args, console);
        sut.Write(&instance);

        auto expected =
            L"test\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_Multiple_With_Properties)
        TEST_WORKITEM(104)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_Multiple_With_Properties)
    {
        CommandArgs args;
        TestConsole console(args);

        TestInstance instance1 =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
        };
        instance1.AssignAdditionalProperties(TestPropertyStore
        {
            { L"campaignId", make_tuple(VT_BSTR, L"abcd1234") },
            { L"nickname", make_tuple(VT_BSTR, L"test1") },
        });

        TestInstance instance2 =
        {
            { L"InstanceId", L"b1c2d3" },
            { L"InstallationName", L"test" },
        };
        instance2.AssignAdditionalProperties(TestPropertyStore
        {
            { L"campaignId", make_tuple(VT_BSTR, L"abcd1234") },
            { L"nickname", make_tuple(VT_BSTR, L"test2") },
        });

        vector<ISetupInstancePtr> instances =
        {
            &instance1,
            &instance2,
        };

        ValueFormatter sut(args, console);
        sut.Write(instances);

        auto expected =
            L"a1b2c3\n"
            L"test\n"
            L"abcd1234\n"
            L"test1\n"
            L"b1c2d3\n"
            L"test\n"
            L"abcd1234\n"
            L"test2\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Missing_Property_Writes_Empty_Object)
        TEST_WORKITEM(108)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Missing_Property_Writes_Empty_Object)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe -property missing");

        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
            { L"InstallDate", L"2017-02-23T01:22:35Z" }
        };

        TestPropertyStore catalogInfo =
        {
            { L"productName", make_tuple(VT_BSTR, L"Test") },
            { L"productSemanticVersion", make_tuple(VT_BSTR, L"1.0") },
            { L"productLineVersion", make_tuple(VT_I4, L"2017") },
        };

        instance.AssignCatalogProperties(catalogInfo);

        TestPropertyStore properties =
        {
            { L"campaignId", make_tuple(VT_BSTR, L"abcd1234") },
            { L"nickname", make_tuple(VT_BSTR, L"test") },
        };

        instance.AssignAdditionalProperties(properties);

        ValueFormatter sut(args, console);
        sut.Write(&instance);

        auto expected = L"";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_All)
        TEST_WORKITEM(109)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_All)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe");

        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
        };

        TestPropertyStore catalogInfo =
        {
            { L"productName", make_tuple(VT_BSTR, L"Test") },
            { L"productSemanticVersion", make_tuple(VT_BSTR, L"1.0") },
            { L"productLineVersion", make_tuple(VT_I4, L"2017") },
        };

        instance.AssignCatalogProperties(catalogInfo);

        TestPropertyStore properties =
        {
            { L"campaignId", make_tuple(VT_BSTR, L"abcd1234") },
            { L"nickname", make_tuple(VT_BSTR, L"test") },
        };

        instance.AssignAdditionalProperties(properties);

        ValueFormatter sut(args, console);
        sut.Write(&instance);

        auto expected =
            L"a1b2c3\n"
            L"test\n"
            L"0\n"
            L"2017\n"
            L"Test\n"
            L"1.0\n"
            L"abcd1234\n"
            L"test\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_State)
        TEST_WORKITEM(133)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_State)
    {
        CommandArgs args;
        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"State", L"11" },
            { L"IsComplete", L"true"},
            { L"IsLaunchable", L"false" },
        };

        ValueFormatter sut(args, console);
        sut.Write(&instance);

        auto expected =
            L"a1b2c3\n"
            L"11\n"
            L"1\n"
            L"0\n"
            L"1\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_Complete_State)
        TEST_WORKITEM(133)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_Complete_State)
    {
        CommandArgs args;
        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"State", L"4294967295" },
            { L"IsComplete", L"true"},
            { L"IsLaunchable", L"true" },
        };

        ValueFormatter sut(args, console);
        sut.Write(&instance);

        auto expected =
            L"a1b2c3\n"
            L"4294967295\n"
            L"1\n"
            L"1\n"
            L"0\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_Array)
        TEST_WORKITEM(162)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_Array)
    {
        CommandArgs args;
        TestConsole console(args);
        vector<wstring> values =
        {
            L"a",
            L"b",
            L"c",
        };

        ValueFormatter sut(args, console);
        sut.Write(L"values", L"value", values);

        auto expected =
            L"a\n"
            L"b\n"
            L"c\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Include_Packages)
        TEST_WORKITEM(199)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Include_Packages)
    {
        TestPackageReference product1 =
        {
            { L"Id", L"Microsoft.VisualStudio.Product.Community" },
        };

        TestPackageReference a1 =
        {
            { L"Id", L"A" },
            { L"Version", L"1.0" },
            { L"Type", L"Workload"},
        };

        TestPackageReference b1 =
        {
            { L"Id", L"B" },
            { L"Version", L"1.0" },
            { L"Type", L"Component"},
        };

        vector<TestInstance::ElementType> packages1 = { &a1, &b1 };

        TestInstance::MapType properties1 =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
        };

        TestInstance instance1(&product1, packages1, properties1);

        TestPackageReference product2 =
        {
            { L"Id", L"Microsoft.VisualStudio.Product.Enterprise" },
        };

        TestPackageReference a2 =
        {
            { L"Id", L"A" },
            { L"Version", L"1.0" },
            { L"Type", L"Workload"},
        };

        TestPackageReference b2 =
        {
            { L"Id", L"B" },
            { L"Version", L"1.0" },
            { L"Type", L"Component"},
        };

        vector<TestInstance::ElementType> packages2 = { &a2, &b2 };

        TestInstance::MapType properties2 =
        {
            { L"InstanceId", L"b1c2d3" },
            { L"InstallationName", L"test" },
        };

        TestInstance instance2(&product2, packages2, properties2);

        vector<ISetupInstancePtr> instances =
        {
            &instance1,
            &instance2,
        };

        CommandArgs args;
        args.Parse(L"vswhere.exe -include packages");

        TestConsole console(args);

        ValueFormatter sut(args, console);
        sut.Write(instances);

        auto expected =
            L"a1b2c3\n"
            L"test\n"
            L"Microsoft.VisualStudio.Product.Community\n"
            L"b1c2d3\n"
            L"test\n"
            L"Microsoft.VisualStudio.Product.Enterprise\n";

        Assert::AreEqual(expected, console);
    }

    TEST_METHOD(Write_Instance_With_Color)
    {
        CommandArgs args;
        TestConsole console(args);
        console.SetColorSupported(true);

        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"State", L"4294967295" },
            { L"IsComplete", L"true"},
        };

        ValueFormatter sut(args, console);
        sut.Write(&instance);

        auto expected =
            L"a1b2c3\n"
            L"4294967295\n"
            L"1\n"
            L"0\n";

        Assert::AreEqual(expected, console);
    }
};
