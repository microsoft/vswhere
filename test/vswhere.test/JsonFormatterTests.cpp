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
            { L"InstallDate", L"2017-02-23T01:22:35Z"},
            { L"Description", L"This description contains \"quotes\"." },
        };

        JsonFormatter sut(args, console);
        sut.Write(&instance);

        auto expected =
            L"[\n"
            L"  {\n"
            L"    \"instanceId\": \"a1b2c3\",\n"
            L"    \"installDate\": \"2017-02-23T01:22:35Z\",\n"
            L"    \"installationName\": \"test\",\n"
            L"    \"description\": \"This description contains \\\"quotes\\\".\"\n"
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

        JsonFormatter sut(args, console);
        sut.Write(instances);

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

        JsonFormatter sut(args, console);
        sut.Write(instances);

        auto expected =
            L"[]\n";

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
            { L"InstallDate", L"2017-02-23T01:22:35Z" }
        };

        TestPropertyStore properties =
        {
            { L"campaignId", make_tuple(VT_BSTR, L"abcd1234") },
            { L"nickname", make_tuple(VT_BSTR, L"test") },
        };

        instance.AssignAdditionalProperties(properties);

        JsonFormatter sut(args, console);
        sut.Write(&instance);

        auto expected =
            L"[\n"
            L"  {\n"
            L"    \"instanceId\": \"a1b2c3\",\n"
            L"    \"installDate\": \"2017-02-23T01:22:35Z\",\n"
            L"    \"installationName\": \"test\",\n"
            L"    \"properties\": {\n"
            L"      \"campaignId\": \"abcd1234\",\n"
            L"      \"nickname\": \"test\"\n"
            L"    }\n"
            L"  }\n"
            L"]\n";

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

        JsonFormatter sut(args, console);
        sut.Write(&instance);

        auto expected =
            L"[\n"
            L"  {\n"
            L"    \"properties\": {\n"
            L"      \"campaignId\": \"abcd1234\",\n"
            L"      \"nickname\": \"test\"\n"
            L"    }\n"
            L"  }\n"
            L"]\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_Specified_Property)
        TEST_WORKITEM(104)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_Specified_Property)
    {
        // Verify that "." works as separator since this fits with JSON member syntax.
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

        JsonFormatter sut(args, console);
        sut.Write(&instance);

        auto expected =
            L"[\n"
            L"  {\n"
            L"    \"properties\": {\n"
            L"      \"nickname\": \"test\"\n"
            L"    }\n"
            L"  }\n"
            L"]\n";

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

        JsonFormatter sut(args, console);
        sut.Write(&instance);

        auto expected =
            L"[\n"
            L"  {\n"
            L"    \"properties\": {\n"
            L"      \"nickname\": \"test\"\n"
            L"    }\n"
            L"  }\n"
            L"]\n";

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

        JsonFormatter sut(args, console);
        sut.Write(instances);

        auto expected =
            L"[\n"
            L"  {\n"
            L"    \"instanceId\": \"a1b2c3\",\n"
            L"    \"installationName\": \"test\",\n"
            L"    \"properties\": {\n"
            L"      \"campaignId\": \"abcd1234\",\n"
            L"      \"nickname\": \"test1\"\n"
            L"    }\n"
            L"  },\n"
            L"  {\n"
            L"    \"instanceId\": \"b1c2d3\",\n"
            L"    \"installationName\": \"test\",\n"
            L"    \"properties\": {\n"
            L"      \"campaignId\": \"abcd1234\",\n"
            L"      \"nickname\": \"test2\"\n"
            L"    }\n"
            L"  }\n"
            L"]\n";

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

        JsonFormatter sut(args, console);
        sut.Write(&instance);

        auto expected =
            L"[]\n";

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

        JsonFormatter sut(args, console);
        sut.Write(&instance);

        auto expected =
            L"[\n"
            L"  {\n"
            L"    \"instanceId\": \"a1b2c3\",\n"
            L"    \"installDate\": \"2017-02-23T01:22:35Z\",\n"
            L"    \"installationName\": \"test\",\n"
            L"    \"isPrerelease\": false,\n"
            L"    \"catalog\": {\n"
            L"      \"productLineVersion\": 2017,\n"
            L"      \"productName\": \"Test\",\n"
            L"      \"productSemanticVersion\": \"1.0\"\n"
            L"    },\n"
            L"    \"properties\": {\n"
            L"      \"campaignId\": \"abcd1234\",\n"
            L"      \"nickname\": \"test\"\n"
            L"    }\n"
            L"  }\n"
            L"]\n";

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

        JsonFormatter sut(args, console);
        sut.Write(&instance);

        auto expected =
            L"[\n"
            L"  {\n"
            L"    \"instanceId\": \"a1b2c3\",\n"
            L"    \"state\": 11,\n"
            L"    \"isComplete\": true,\n"
            L"    \"isLaunchable\": false,\n"
            L"    \"isRebootRequired\": true\n"
            L"  }\n"
            L"]\n";

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

        JsonFormatter sut(args, console);
        sut.Write(&instance);

        auto expected =
            L"[\n"
            L"  {\n"
            L"    \"instanceId\": \"a1b2c3\",\n"
            L"    \"state\": 4294967295,\n"
            L"    \"isComplete\": true,\n"
            L"    \"isLaunchable\": true,\n"
            L"    \"isRebootRequired\": false\n"
            L"  }\n"
            L"]\n";

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

        JsonFormatter sut(args, console);
        sut.Write(L"values", L"value", values);

        auto expected =
            L"[\n"
            L"  \"a\",\n"
            L"  \"b\",\n"
            L"  \"c\"\n"
            L"]\n";

        Assert::AreEqual(expected, console);
    }

    TEST_METHOD(Escape)
    {
        vector<tuple<wstring, wstring>> data =
        {
            { L"value", L"value" },
            { L"C:\\ShouldNotExist", L"C:\\\\ShouldNotExist" },
            { L"C:\\ShouldNotExist\\Sub", L"C:\\\\ShouldNotExist\\\\Sub" },
            { L"\\\\ShouldNotExist", L"\\\\\\\\ShouldNotExist" },
            { L"\"value\"", L"\\\"value\\\"" },
            { L"\"C:\\ShouldNotExist\"", L"\\\"C:\\\\ShouldNotExist\\\"" },
        };

        for (const auto& item : data)
        {
            wstring source, expected;

            tie(source, expected) = item;
            auto actual = JsonFormatter::Escape(source);

            Assert::AreEqual(expected.c_str(), actual.c_str());
        }
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

        JsonFormatter sut(args, console);
        sut.Write(instances);

        auto expected =
            L"[\n"
            L"  {\n"
            L"    \"instanceId\": \"a1b2c3\",\n"
            L"    \"installationName\": \"test\",\n"
            L"    \"productId\": \"Microsoft.VisualStudio.Product.Community\",\n"
            L"    \"packages\": [\n"
            L"      {\n"
            L"        \"id\": \"A\",\n"
            L"        \"version\": \"1.0\",\n"
            L"        \"type\": \"Workload\"\n"
            L"      },\n"
            L"      {\n"
            L"        \"id\": \"B\",\n"
            L"        \"version\": \"1.0\",\n"
            L"        \"type\": \"Component\"\n"
            L"      }\n"
            L"    ]\n"
            L"  },\n"
            L"  {\n"
            L"    \"instanceId\": \"b1c2d3\",\n"
            L"    \"installationName\": \"test\",\n"
            L"    \"productId\": \"Microsoft.VisualStudio.Product.Enterprise\",\n"
            L"    \"packages\": [\n"
            L"      {\n"
            L"        \"id\": \"A\",\n"
            L"        \"version\": \"1.0\",\n"
            L"        \"type\": \"Workload\"\n"
            L"      },\n"
            L"      {\n"
            L"        \"id\": \"B\",\n"
            L"        \"version\": \"1.0\",\n"
            L"        \"type\": \"Component\"\n"
            L"      }\n"
            L"    ]\n"
            L"  }\n"
            L"]\n";

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
            { L"InstallDate", L"2017-02-23T01:22:35Z"},
            { L"State", L"4294967295" },
            { L"IsComplete", L"true"},
        };

        JsonFormatter sut(args, console);
        sut.Write(&instance);

        auto expected =
            L"[\n"
            L"  {\n"
            L"    \033[38;2;156;220;254m\"instanceId\"\033[0m: \033[38;2;206;145;120m\"a1b2c3\"\033[0m,\n"
            L"    \033[38;2;156;220;254m\"installDate\"\033[0m: \033[38;2;206;145;120m\"2017-02-23T01:22:35Z\"\033[0m,\n"
            L"    \033[38;2;156;220;254m\"state\"\033[0m: \033[38;2;181;206;168m4294967295\033[0m,\n"
            L"    \033[38;2;156;220;254m\"isComplete\"\033[0m: \033[38;2;86;156;214mtrue\033[0m,\n"
            L"    \033[38;2;156;220;254m\"isRebootRequired\"\033[0m: \033[38;2;86;156;214mfalse\033[0m\n"
            L"  }\n"
            L"]\n";

        Assert::AreEqual(expected, console);
    }
};
