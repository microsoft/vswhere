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
    TEST_METHOD(NoProduct)
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

    TEST_METHOD(Default_Product)
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

    TEST_METHOD(Other_Product)
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

    TEST_METHOD(NoVersion)
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

    TEST_METHOD(NoWorkload)
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

    TEST_METHOD(Requires_Workload)
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

    TEST_METHOD(InvalidVersionRange)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe -version invalid");

        TestHelper helper(0, 0);

        Assert::ExpectException<win32_error>([&] { InstanceSelector(args, &helper); });
    }
};
