// <copyright file="InstanceSelectorTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(InstanceSelectorTests)
{
public:
    TEST_METHOD(Select_No_Product)
    {
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
        };

        TestEnumInstances instances =
        {
            &instance,
        };

        CommandArgs args;
        args.Parse(L"vswhere.exe");

        InstanceSelector sut(args);
        auto selected = sut.Select(&instances);

        Assert::AreEqual<size_t>(0, selected.size());
    }

    TEST_METHOD(Select_Default_Product)
    {
        TestPackageReference product =
        {
            { L"Id", L"Microsoft.VisualStudio.Product.Enterprise" },
        };

        TestInstance::MapType properties =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
        };

        TestInstance instance(&product, {}, properties);
        TestEnumInstances instances =
        {
            &instance,
        };

        CommandArgs args;
        args.Parse(L"vswhere.exe");

        InstanceSelector sut(args);
        auto selected = sut.Select(&instances);

        Assert::AreEqual<size_t>(1, selected.size());
    }

    TEST_METHOD(Select_Other_Product)
    {
        TestPackageReference product =
        {
            { L"Id", L"Microsoft.VisualStudio.Product.BuildTools" },
        };

        TestInstance::MapType properties =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
        };

        TestInstance instance(&product, {}, properties);
        TestEnumInstances instances =
        {
            &instance,
        };

        CommandArgs args;
        args.Parse(L"vswhere.exe -products microsoft.visualstudio.product.buildtools");

        InstanceSelector sut(args);
        auto selected = sut.Select(&instances);

        Assert::AreEqual<size_t>(1, selected.size());
    }

    TEST_METHOD(Select_No_Version)
    {
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
        };

        TestEnumInstances instances =
        {
            &instance,
        };

        CommandArgs args;
        args.Parse(L"vswhere.exe -version [1.0,2.0)");

        TestHelper helper(MAKEVERSION(1, 0, 0, 0), MAKEVERSION(1, USHRT_MAX, USHRT_MAX, USHRT_MAX));

        InstanceSelector sut(args, &helper);
        auto selected = sut.Select(&instances);

        Assert::AreEqual<size_t>(0, selected.size());
    }

    TEST_METHOD(Select_No_Workload)
    {
        TestPackageReference product =
        {
            { L"Id", L"Microsoft.VisualStudio.Product.Enterprise" },
        };

        TestPackageReference managedDesktop = { { L"Id", L"Microsoft.VisualStudio.Workload.ManagedDesktop" } };
        TestPackageReference nativeDesktop = { { L"Id", L"Microsoft.VisualStudio.Workload.NativeDesktop" } };
        vector<TestInstance::ElementType> packages =
        {
            &managedDesktop,
            &nativeDesktop,
        };

        TestInstance::MapType properties =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
        };

        TestInstance instance(&product, packages, properties);
        TestEnumInstances instances =
        {
            &instance,
        };

        CommandArgs args;
        args.Parse(L"vswhere.exe -requires microsoft.visualstudio.workload.azure");

        InstanceSelector sut(args);
        auto selected = sut.Select(&instances);

        Assert::AreEqual<size_t>(0, selected.size());
    }

    TEST_METHOD(Select_Requires_Workload)
    {
        TestPackageReference product =
        {
            { L"Id", L"Microsoft.VisualStudio.Product.Enterprise" },
        };

        TestPackageReference managedDesktop = { { L"Id", L"Microsoft.VisualStudio.Workload.ManagedDesktop" } };
        TestPackageReference nativeDesktop = { { L"Id", L"Microsoft.VisualStudio.Workload.NativeDesktop" } };
        vector<TestInstance::ElementType> packages =
        {
            &managedDesktop,
            &nativeDesktop,
        };

        TestInstance::MapType properties =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
        };

        TestInstance instance(&product, packages, properties);
        TestEnumInstances instances =
        {
            &instance,
        };

        CommandArgs args;
        args.Parse(L"vswhere.exe -requires microsoft.visualstudio.workload.nativedesktop");

        InstanceSelector sut(args);
        auto selected = sut.Select(&instances);

        Assert::AreEqual<size_t>(1, selected.size());
    }

    TEST_METHOD(Select_Invalid_Version_Range)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe -version invalid");

        TestHelper helper(0, 0);

        Assert::ExpectException<win32_error>([&] { InstanceSelector(args, &helper); });
    }

    TEST_METHOD(Less_No_Version_A)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe");

        TestInstance a =
        {
        };

        TestInstance b =
        {
            { L"InstallationVersion", L"2.0" },
        };

        TestHelper helper(0, 0);

        InstanceSelector sut(args, &helper);
        Assert::IsTrue(sut.Less(&a, &b));
    }

    TEST_METHOD(Less_No_Version_B)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe");

        TestInstance a =
        {
            { L"InstallationVersion", L"1.0" },
        };

        TestInstance b =
        {
        };

        TestHelper helper(0, 0);

        InstanceSelector sut(args, &helper);
        Assert::IsFalse(sut.Less(&a, &b));
    }

    TEST_METHOD(Less_Version_A)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe");

        TestInstance a =
        {
            { L"InstallationVersion", L"1.0" },
        };

        TestInstance b =
        {
            { L"InstallationVersion", L"2.0" },
        };

        TestHelper helper(0, 0);

        InstanceSelector sut(args, &helper);
        Assert::IsTrue(sut.Less(&a, &b));
    }

    TEST_METHOD(Less_Version_B)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe");

        TestInstance a =
        {
            { L"InstallationVersion", L"2.0" },
        };

        TestInstance b =
        {
            { L"InstallationVersion", L"1.0" },
        };

        TestHelper helper(0, 0);

        InstanceSelector sut(args, &helper);
        Assert::IsFalse(sut.Less(&a, &b));
    }

    TEST_METHOD(Less_No_Date_A)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe");

        TestInstance a =
        {
            { L"InstallationVersion", L"2.0" },
        };

        TestInstance b =
        {
            { L"InstallationVersion", L"2.0" },
            { L"InstallDate", L"2017-02-23T22:00:00Z" },
        };

        TestHelper helper(0, 0);

        InstanceSelector sut(args, &helper);
        Assert::IsTrue(sut.Less(&a, &b));
    }

    TEST_METHOD(Less_No_Date_B)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe");

        TestInstance a =
        {
            { L"InstallationVersion", L"2.0" },
            { L"InstallDate", L"2017-02-23T22:00:00Z" },
        };

        TestInstance b =
        {
            { L"InstallationVersion", L"2.0" },
        };

        TestHelper helper(0, 0);

        InstanceSelector sut(args, &helper);
        Assert::IsFalse(sut.Less(&a, &b));
    }

    TEST_METHOD(Less_Date_A)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe");

        TestInstance a =
        {
            { L"InstallationVersion", L"2.0" },
            { L"InstallDate", L"2017-02-23T22:00:00Z" },
        };

        TestInstance b =
        {
            { L"InstallationVersion", L"2.0" },
            { L"InstallDate", L"2017-02-23T23:00:00Z" },
        };

        TestHelper helper(0, 0);

        InstanceSelector sut(args, &helper);
        Assert::IsTrue(sut.Less(&a, &b));
    }

    TEST_METHOD(Less_Date_B)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe");

        TestInstance a =
        {
            { L"InstallationVersion", L"2.0" },
            { L"InstallDate", L"2017-02-23T23:00:00Z" },
        };

        TestInstance b =
        {
            { L"InstallationVersion", L"2.0" },
            { L"InstallDate", L"2017-02-23T22:00:00Z" },
        };

        TestHelper helper(0, 0);

        InstanceSelector sut(args, &helper);
        Assert::IsFalse(sut.Less(&a, &b));
    }

    TEST_METHOD(Less_Dates_Equal)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe");

        TestInstance a =
        {
            { L"InstallationVersion", L"2.0" },
            { L"InstallDate", L"2017-02-23T22:00:00Z" },
        };

        TestInstance b =
        {
            { L"InstallationVersion", L"2.0" },
            { L"InstallDate", L"2017-02-23T22:00:00Z" },
        };

        TestHelper helper(0, 0);

        InstanceSelector sut(args, &helper);
        Assert::ExpectException<win32_error>([&] { sut.Less(&a, &b); });
    }
};
