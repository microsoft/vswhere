// <copyright file="CommandArgsTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(CommandArgsTests)
{
public:
    TEST_METHOD(Parse_Empty_Defaults)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe");

        Assert::AreEqual(L"vswhere.exe", args.get_ApplicationPath().c_str());
        Assert::IsFalse(args.get_All());
        Assert::AreEqual<size_t>(0, args.get_Version().length());
        Assert::IsFalse(args.get_Latest());
        Assert::IsFalse(args.get_Legacy());
        Assert::AreEqual<size_t>(0, args.get_Path().length());
        Assert::IsFalse(args.get_Prerelease());
        Assert::AreEqual(L"text", args.get_Format().c_str());
        Assert::AreEqual<size_t>(0, args.get_Property().length());
        Assert::IsFalse(args.get_Help());
        Assert::IsFalse(args.get_UTF8());
        Assert::IsTrue(args.get_Logo());
        Assert::IsTrue(args.get_Color());

        auto& products = args.get_Products();
        Assert::AreEqual<size_t>(3, products.size());
        Assert::IsFalse(find(products.begin(), products.end(), L"Microsoft.VisualStudio.Product.Enterprise") == products.end());
        Assert::IsFalse(find(products.begin(), products.end(), L"Microsoft.VisualStudio.Product.Professional") == products.end());
        Assert::IsFalse(find(products.begin(), products.end(), L"Microsoft.VisualStudio.Product.Community") == products.end());

        auto& requires = args.get_Requires();
        Assert::AreEqual<size_t>(0, requires.size());
    }

    TEST_METHOD(Parse_All)
    {
        CommandArgs args;
        Assert::IsFalse(args.get_All());

        args.Parse(L"vswhere.exe -all");
        Assert::IsTrue(args.get_All());
    }

    TEST_METHOD(Parse_Products_Empty)
    {
        CommandArgs args;

        Assert::ExpectException<win32_error>([&] { args.Parse(L"vswhere.exe -products"); });
    }

    TEST_METHOD(Parse_Products)
    {
        CommandArgs args;

        args.Parse(L"vswhere.exe -products A B");

        auto& products = args.get_Products();
        Assert::AreEqual<size_t>(2, products.size());
        Assert::IsFalse(find(products.begin(), products.end(), L"A") == products.end());
        Assert::IsFalse(find(products.begin(), products.end(), L"B") == products.end());
    }

    TEST_METHOD(Parse_Requires_Empty)
    {
        CommandArgs args;

        Assert::ExpectException<win32_error>([&] { args.Parse(L"vswhere.exe -requires"); });
    }

    TEST_METHOD(Parse_Requires)
    {
        CommandArgs args;

        args.Parse(L"vswhere.exe -requires A B");

        auto& requires = args.get_Requires();
        Assert::AreEqual<size_t>(2, requires.size());
        Assert::IsFalse(find(requires.begin(), requires.end(), L"A") == requires.end());
        Assert::IsFalse(find(requires.begin(), requires.end(), L"B") == requires.end());
    }

    TEST_METHOD(Parse_Version_EOL)
    {
        CommandArgs args;

        Assert::ExpectException<win32_error>([&] { args.Parse(L"vswhere.exe -version"); });
    }

    TEST_METHOD(Parse_Version_Missing)
    {
        CommandArgs args;

        Assert::ExpectException<win32_error>([&] { args.Parse(L"vswhere.exe -version -latest"); });
    }

    TEST_METHOD(Parse_Version)
    {
        CommandArgs args;
        Assert::AreEqual(L"", args.get_Version().c_str());

        args.Parse(L"vswhere.exe -version \"[15.0,16.0)\"");
        Assert::AreEqual(L"[15.0,16.0)", args.get_Version().c_str());
    }

    TEST_METHOD(Parse_Version_Param_Next)
    {
        CommandArgs args;

        Assert::ExpectException<win32_error>([&] { args.Parse(L"vswhere.exe -version -nologo"); });
    }

    TEST_METHOD(Parse_Latest)
    {
        CommandArgs args;
        Assert::IsFalse(args.get_Latest());

        args.Parse(L"vswhere.exe -latest");
        Assert::IsTrue(args.get_Latest());
    }

    TEST_METHOD(Parse_Prerelease)
    {
        CommandArgs args;
        Assert::IsFalse(args.get_Prerelease());

        args.Parse(L"vswhere.exe -prerelease");
        Assert::IsTrue(args.get_Prerelease());
    }

    TEST_METHOD(Parse_Format_EOL)
    {
        CommandArgs args;

        Assert::ExpectException<win32_error>([&] { args.Parse(L"vswhere.exe -format"); });
    }

    TEST_METHOD(Parse_Format_Missing)
    {
        CommandArgs args;

        Assert::ExpectException<win32_error>([&] { args.Parse(L"vswhere.exe -format -latest"); });
    }

    TEST_METHOD(Parse_Format)
    {
        CommandArgs args;
        Assert::AreEqual(L"text", args.get_Format().c_str());

        args.Parse(L"vswhere.exe -format json");
        Assert::AreEqual(L"json", args.get_Format().c_str());
    }

    TEST_METHOD(Parse_Format_Invalid)
    {
        CommandArgs args;

        Assert::ExpectException<win32_error>([&] { args.Parse(L"vswhere.exe -format invalid"); });
    }

    TEST_METHOD(Parse_NoColor)
    {
        CommandArgs args;
        Assert::IsTrue(args.get_Color());

        args.Parse(L"vswhere.exe -nocolor");
        Assert::IsFalse(args.get_Color());
    }

    TEST_METHOD(Parse_NoLogo)
    {
        CommandArgs args;
        Assert::IsTrue(args.get_Logo());

        args.Parse(L"vswhere.exe -nologo");
        Assert::IsFalse(args.get_Logo());
    }

    TEST_METHOD(Parse_Help_Short)
    {
        CommandArgs args;
        Assert::IsFalse(args.get_Help());

        args.Parse(L"vswhere.exe -h");
        Assert::IsTrue(args.get_Help());
    }

    TEST_METHOD(Parse_Help_Question)
    {
        CommandArgs args;
        Assert::IsFalse(args.get_Help());

        args.Parse(L"vswhere.exe -h");
        Assert::IsTrue(args.get_Help());
    }

    TEST_METHOD(Parse_Help_Long)
    {
        CommandArgs args;
        Assert::IsFalse(args.get_Help());

        args.Parse(L"vswhere.exe -help");
        Assert::IsTrue(args.get_Help());
    }

    TEST_METHOD(Parse_Unknown_Parameter)
    {
        CommandArgs args;

        Assert::ExpectException<win32_error>([&] { args.Parse(L"vswhere.exe -unknown"); });
    }

    TEST_METHOD(Parse_Unknown_Argument)
    {
        CommandArgs args;

        Assert::ExpectException<win32_error>([&] { args.Parse(L"vswhere.exe unknown"); });
    }

    TEST_METHOD(Parse_Property)
    {
        CommandArgs args;
        Assert::AreEqual<size_t>(0, args.get_Property().length());
        Assert::AreEqual(L"text", args.get_Format().c_str());

        args.Parse(L"vswhere.exe -property test");
        Assert::AreEqual(L"test", args.get_Property().c_str());
        Assert::AreEqual(L"value", args.get_Format().c_str());
    }

    TEST_METHOD(Parse_Property_Explicit_Format)
    {
        CommandArgs args;
        Assert::AreEqual<size_t>(0, args.get_Property().length());
        Assert::AreEqual(L"text", args.get_Format().c_str());

        args.Parse(L"vswhere.exe -property test -format text");
        Assert::AreEqual(L"test", args.get_Property().c_str());
        Assert::AreEqual(L"text", args.get_Format().c_str());
    }

    TEST_METHOD(Parse_Products_Asterisk)
    {
        CommandArgs args;
        Assert::AreEqual<size_t>(3, args.get_Products().size());

        args.Parse(L"vswhere.exe -products *");
        Assert::IsTrue(args.get_Products().empty());
    }

    TEST_METHOD(Parse_Products_Asterisk_Multiple)
    {
        CommandArgs args;
        Assert::AreEqual<size_t>(3, args.get_Products().size());

        args.Parse(L"vswhere.exe -products A * B");
        Assert::IsTrue(args.get_Products().empty());
    }

    TEST_METHOD(Parse_Legacy)
    {
        CommandArgs args;
        Assert::IsFalse(args.get_Legacy());

        args.Parse(L"vswhere.exe -legacy");
        Assert::IsTrue(args.get_Legacy());
    }

    TEST_METHOD(Parse_Legacy_Products_Throws)
    {
        CommandArgs args;
        Assert::IsFalse(args.get_Legacy());

        Assert::ExpectException<win32_error>([&] { args.Parse(L"vswhere.exe -legacy -products A"); });
    }

    TEST_METHOD(Parse_Legacy_Products_Asterisk)
    {
        CommandArgs args;
        Assert::IsFalse(args.get_Legacy());

        args.Parse(L"vswhere.exe -legacy -products *");
        Assert::IsTrue(args.get_Legacy());
    }

    TEST_METHOD(Parse_Legacy_Requires_Throws)
    {
        CommandArgs args;
        Assert::IsFalse(args.get_Legacy());

        Assert::ExpectException<win32_error>([&] { args.Parse(L"vswhere.exe -legacy -requires A"); });
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Parse_UTF8)
        TEST_WORKITEM(146)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Parse_UTF8)
    {
        CommandArgs args;
        Assert::IsFalse(args.get_UTF8());

        args.Parse(L"vswhere.exe -utf8");
        Assert::IsTrue(args.get_UTF8());
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Find_Exclusive_With_Property)
        TEST_WORKITEM(162)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Find_Exclusive_With_Property)
    {
        CommandArgs args;

        Assert::ExpectException<win32_error>([&] { args.Parse(L"vswhere.exe -property any -find *"); });
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Parse_Path)
        TEST_WORKITEM(191)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Parse_Path)
    {
        CommandArgs args;
        Assert::IsTrue(args.get_Path().empty());

        args.Parse(L"vswhere.exe -path .");
        Assert::AreEqual(L".", args.get_Path().c_str());
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Parse_Path_Requires_Argument)
        TEST_WORKITEM(191)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Parse_Path_Requires_Argument)
    {
        CommandArgs args;
        Assert::IsTrue(args.get_Path().empty());

        Assert::ExpectException<win32_error>([&] { args.Parse(L"vswhere.exe -path"); });
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Parse_Path_Incompatible_Other_Selection_Options)
        TEST_WORKITEM(191)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Parse_Path_Incompatible_Other_Selection_Options)
    {
        vector<wstring> data =
        {
            { L"-all" },
            { L"-prerelease" },
            { L"-products *" },
            { L"-requires Microsoft.VisualStudio.Component.MSBuild" },
            { L"-requiresAny" },
            { L"-version [15.0,16.0)" },
            { L"-latest" },
            { L"-legacy" },
        };

        for (const auto& item : data)
        {
            wstring commandLine = L"vswhere.exe -path . ";
            commandLine += item;

            CommandArgs args;
            Assert::ExpectException<win32_error>([&] { args.Parse(commandLine.c_str()); });
        }
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Parse_Path_Compatible_Output_Options_Find)
        TEST_WORKITEM(191)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Parse_Path_Compatible_Output_Options_Find)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe -path . -sort -format json -find **\\msbuild.exe -nologo -utf8");

        Assert::AreEqual(L".", args.get_Path().c_str());
        Assert::IsTrue(args.get_Sort());
        Assert::AreEqual(L"json", args.get_Format().c_str());
        Assert::AreEqual(L"**\\msbuild.exe", args.get_Find().c_str());
        Assert::IsFalse(args.get_Logo());
        Assert::IsTrue(args.get_UTF8());
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Parse_Path_Compatible_Output_Options_Property)
        TEST_WORKITEM(191)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Parse_Path_Compatible_Output_Options_Property)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe -path . -sort -format json -property catalog_productDisplayName -nologo -utf8");

        Assert::AreEqual(L".", args.get_Path().c_str());
        Assert::IsTrue(args.get_Sort());
        Assert::AreEqual(L"json", args.get_Format().c_str());
        Assert::AreEqual(L"catalog_productDisplayName", args.get_Property().c_str());
        Assert::IsFalse(args.get_Logo());
        Assert::IsTrue(args.get_UTF8());
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Parse_Requires_Patterns)
        TEST_WORKITEM(276)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Parse_Requires_Patterns)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe -requires foo ba* qux");

        const auto& literals = args.get_Requires();
        const auto& patterns = args.get_RequiresPattern();

        Assert::AreEqual(1, count(literals.cbegin(), literals.cend(), wstring(L"foo")));
        Assert::AreEqual(1, count(literals.cbegin(), literals.cend(), wstring(L"qux")));
        Assert::AreEqual<size_t>(1, patterns.size());
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(ParseRegex_Theory)
        TEST_WORKITEM(276)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(ParseRegex_Theory)
    {
        const wstring id = L"Foo.Bar";
        vector<tuple<wstring, bool>> data =
        {
            { L"Foo.Bar", true },
            { L"Foo.*", true },
            { L"*.Bar", true },
            { L"F*R", true },
            { L"foo?bar", true },
            { L"f??", false },
            { L"f??.??r", true },
            { L"*", true },
            { L".*", false },
            { L"?", false },
            { L"Baz", false },
            { L"*baz", false },
            { L"foo.bar*", true },
        };

        for (const auto& item : data)
        {
            wstring pattern;
            bool expected;

            tie(pattern, expected) = item;
            auto re = CommandArgs::ParseRegex(pattern);
            bool actual = regex_match(id, re);

            Assert::AreEqual(expected, actual, format(L"\"%ls\" =~ /%ls/", id.c_str(), pattern.c_str()).c_str());
        }
    }
};
