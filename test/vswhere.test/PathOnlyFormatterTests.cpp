// <copyright file="TextFormatterTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(PathOnlyFormatterTests)
{
public:
    TEST_METHOD(Write_Instance)
    {
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
        };

        PathOnlyFormatter sut;
        wostringstream ostr;

        sut.Write(ostr, &instance);

        auto actual = ostr.str();
        auto expected =
            L"";

        Assert::AreEqual(expected, actual.c_str());
    }

    TEST_METHOD(Write_Instances)
    {
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

        PathOnlyFormatter sut;
        wostringstream ostr;

        sut.Write(ostr, instances);

        auto actual = ostr.str();
        auto expected =
            L"C:\\ShouldNotExist";

        Assert::AreEqual(expected, actual.c_str());
    }
};
