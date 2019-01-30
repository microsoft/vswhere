// <copyright file="UtilitiesTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(UtilitiesTests)
{
public:
    TEST_METHOD(ci_equal_theory)
    {
        vector<tuple<wstring, wstring, bool>> data =
        {
            { L"a", L"a", true },
            { L"a", L"A", true },
            { L"a", L"b", false },
            { L"b", L"a", false },
            { L"foo", L"foobar", false },
            { L"foobar", L"foo", false },
        };

        ci_equal sut;
        for (const auto& item : data)
        {
            wstring lhs, rhs;
            bool expected;

            tie(lhs, rhs, expected) = item;
            auto actual = sut(lhs, rhs);

            Assert::AreEqual(expected, actual, format(L"ci_equal(%ls, %ls)", lhs.c_str(), rhs.c_str()).c_str());
        }
    }

    TEST_METHOD(ci_less_theory)
    {
        vector<tuple<wstring, wstring, bool>> data =
        {
            { L"a", L"a", false },
            { L"a", L"A", false },
            { L"a", L"b", true },
            { L"b", L"a", false },
            { L"foo", L"foobar", true },
            { L"foobar", L"foo", false },
        };

        ci_less sut;
        for (const auto& item : data)
        {
            wstring lhs, rhs;
            bool expected;

            tie(lhs, rhs, expected) = item;
            auto actual = sut(lhs, rhs);

            Assert::AreEqual(expected, actual, format(L"ci_less(%ls, %ls)", lhs.c_str(), rhs.c_str()).c_str());
        }
    }
};
