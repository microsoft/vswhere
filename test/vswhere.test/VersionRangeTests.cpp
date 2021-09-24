// <copyright file="VersionRangeTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(VersionRangeTests)
{
public:

    TEST_METHOD(ParseVersion_pwszVersion_Null)
    {
        auto pHelper = new VersionRange;
        ISetupHelperPtr sut(pHelper, false);

        #pragma warning(suppress: 6387) // Expected to pass invalid arg
        Assert::AreEqual(E_INVALIDARG, sut->ParseVersion(NULL, NULL));
    }

    TEST_METHOD(ParseVersion_pullVersion_Null)
    {
        auto pHelper = new VersionRange;
        ISetupHelperPtr sut(pHelper, false);

        #pragma warning(suppress: 6387) // Expected to pass invalid arg
        Assert::AreEqual(E_POINTER, sut->ParseVersion(L"1.0", NULL));
    }

    TEST_METHOD(ParseVersion_Major)
    {
        auto pHelper = new VersionRange;
        ISetupHelperPtr sut(pHelper, false);

        ULONGLONG ullVersion = 0;
        Assert::AreEqual(S_OK, sut->ParseVersion(L"1", &ullVersion));
        Assert::AreEqual(281474976710656ULL, ullVersion);
    }

    TEST_METHOD(ParseVersion_MajorMinor)
    {
        auto pHelper = new VersionRange;
        ISetupHelperPtr sut(pHelper, false);

        ULONGLONG ullVersion = 0;
        Assert::AreEqual(S_OK, sut->ParseVersion(L"1.2", &ullVersion));
        Assert::AreEqual(281483566645248ULL, ullVersion);
    }

    TEST_METHOD(ParseVersion_MajorMinorBuild)
    {
        auto pHelper = new VersionRange;
        ISetupHelperPtr sut(pHelper, false);

        ULONGLONG ullVersion = 0;
        Assert::AreEqual(S_OK, sut->ParseVersion(L"1.2.3", &ullVersion));
        Assert::AreEqual(281483566841856ULL, ullVersion);
    }

    TEST_METHOD(ParseVersion_MajorMinorBuildRevision)
    {
        auto pHelper = new VersionRange;
        ISetupHelperPtr sut(pHelper, false);

        ULONGLONG ullVersion = 0;
        Assert::AreEqual(S_OK, sut->ParseVersion(L"1.2.3.4", &ullVersion));
        Assert::AreEqual(281483566841860ULL, ullVersion);
    }

    TEST_METHOD(ParseVersion_Too_Many_Fields)
    {
        auto pHelper = new VersionRange;
        ISetupHelperPtr sut(pHelper, false);

        ULONGLONG ullVersion = 0;
        Assert::AreEqual(E_INVALIDARG, sut->ParseVersion(L"1.2.3.4.5", &ullVersion));
        Assert::AreEqual(0ULL, ullVersion);
    }

    TEST_METHOD(ParseVersion_Empty_Field)
    {
        auto pHelper = new VersionRange;
        ISetupHelperPtr sut(pHelper, false);

        ULONGLONG ullVersion = 0;
        Assert::AreEqual(E_INVALIDARG, sut->ParseVersion(L"1..0", &ullVersion));
        Assert::AreEqual(0ULL, ullVersion);
    }

    TEST_METHOD(ParseVersion_Overflow)
    {
        auto pHelper = new VersionRange;
        ISetupHelperPtr sut(pHelper, false);

        ULONGLONG ullVersion = 0;
        Assert::AreEqual(E_INVALIDARG, sut->ParseVersion(L"65536", &ullVersion));
    }

    TEST_METHOD(ParseVersion_NaN)
    {
        auto pHelper = new VersionRange;
        ISetupHelperPtr sut(pHelper, false);

        ULONGLONG ullVersion = 0;
        Assert::AreEqual(E_INVALIDARG, sut->ParseVersion(L"NaN", &ullVersion));
    }

    TEST_METHOD(ParseVersion_only_Spaces)
    {
        auto pHelper = new VersionRange;
        ISetupHelperPtr sut(pHelper, false);

        ULONGLONG ullVersion = 0;
        Assert::AreEqual(E_INVALIDARG, sut->ParseVersion(L"   ", &ullVersion));
    }

    TEST_METHOD(ParseVersion_with_Spaces)
    {
        auto pHelper = new VersionRange;
        ISetupHelperPtr sut(pHelper, false);

        ULONGLONG ullVersion = 0;
        Assert::AreEqual(S_OK, sut->ParseVersion(L" 1.0 ", &ullVersion));
        Assert::AreEqual(281474976710656ULL, ullVersion);
    }

    TEST_METHOD(ParseVersionRange_pwszVersionRange_Null)
    {
        auto pHelper = new VersionRange;
        ISetupHelperPtr sut(pHelper, false);

        #pragma warning(suppress: 6387) // Expected to pass invalid arg
        Assert::AreEqual(E_INVALIDARG, sut->ParseVersionRange(NULL, NULL, NULL));
    }

    TEST_METHOD(ParseVersionRange_pullMinVersion_Null)
    {
        auto pHelper = new VersionRange;
        ISetupHelperPtr sut(pHelper, false);

        #pragma warning(suppress: 6387) // Expected to pass invalid arg
        Assert::AreEqual(E_POINTER, sut->ParseVersionRange(L"[1.0,)", NULL, NULL));
    }

    TEST_METHOD(ParseVersionRange_pullMaxVersion_Null)
    {
        auto pHelper = new VersionRange;
        ISetupHelperPtr sut(pHelper, false);

        ULONGLONG ullMinVersion = 0;

        #pragma warning(suppress: 6387) // Expected to pass invalid arg
        Assert::AreEqual(E_POINTER, sut->ParseVersionRange(L"[1.0,)", &ullMinVersion, NULL));
    }

    TEST_METHOD(ParseVersionRange)
    {
        typedef tuple<UINT, HRESULT, LPCWSTR, ULONGLONG, ULONGLONG> theory_datum;
        typedef vector<theory_datum> theory_data;

        auto pHelper = new VersionRange;
        ISetupHelperPtr sut(pHelper, false);

        theory_data data =
        {
            make_tuple(__LINE__, S_OK,            L"1.0",                       281474976710656,    18446744073709551615),
            make_tuple(__LINE__, S_OK,            L"[1.0]",                     281474976710656,    281474976710656),
            make_tuple(__LINE__, E_INVALIDARG,    L"[1.0)",                     0,                  0),
            make_tuple(__LINE__, S_OK,            L"[1.0,)",                    281474976710656,    18446744073709551614),
            make_tuple(__LINE__, S_OK,            L"[1.0,2.0)",                 281474976710656,    562949953421311),
            make_tuple(__LINE__, S_OK,            L"[1,2)",                     281474976710656,    562949953421311),
            make_tuple(__LINE__, S_OK,            L"[1,2]",                     281474976710656,    562949953421312),
            make_tuple(__LINE__, S_OK,            L"(1.0,2.0]",                 281474976710657,    562949953421312),
            make_tuple(__LINE__, S_OK,            L"[1.0",                      281474976710656,    18446744073709551615),
            make_tuple(__LINE__, S_OK,            L",2.0)",                     0,                  562949953421311),
            make_tuple(__LINE__, E_INVALIDARG,    L"NaN",                       0,                  0),
            make_tuple(__LINE__, E_INVALIDARG,    L"[1,2,3)",                   0,                  0),
            make_tuple(__LINE__, E_INVALIDARG,    L"",                          0,                  0),
            make_tuple(__LINE__, E_INVALIDARG,    L"1[,2)",                     0,                  0),
            make_tuple(__LINE__, E_INVALIDARG,    L"[1,)2",                     0,                  0),
            make_tuple(__LINE__, E_INVALIDARG,    L"(65535.65535.65535.65535]", 0,                  0),
            make_tuple(__LINE__, E_INVALIDARG,    L"[0,0)",                     0,                  0),
            make_tuple(__LINE__, E_INVALIDARG,    L"[2,1)",                     0,                  0),
            make_tuple(__LINE__, E_INVALIDARG,    L"(1.65535.65535.65535,2.0)", 0,                  0),
            make_tuple(__LINE__, S_OK,            L"[1.0, )",                   281474976710656,    18446744073709551614),
            make_tuple(__LINE__, S_OK,            L" [ 1.0 , ) ",               281474976710656,    18446744073709551614),
            make_tuple(__LINE__, E_INVALIDARG,    L" [ 1. 0 , ) ",              0,                  0),
        };

        UINT uiLine;
        HRESULT hrActual;
        HRESULT hrExpected;
        LPCWSTR pwszVersionRange;
        ULONGLONG ullMinVersionActual;
        ULONGLONG ullMinVersionExpected;
        ULONGLONG ullMaxVersionActual;
        ULONGLONG ullMaxVersionExpected;

        for (const auto& datum : data)
        {
            tie(uiLine, hrExpected, pwszVersionRange, ullMinVersionExpected, ullMaxVersionExpected) = datum;

            hrActual = sut->ParseVersionRange(pwszVersionRange, &ullMinVersionActual, &ullMaxVersionActual);
            Assert::AreEqual(hrExpected, hrActual, format(L"Unexpected result parsing '%ls' from line %d, expected: 0x%08x, actual: 0x%08x", pwszVersionRange, uiLine, hrExpected, hrActual).c_str());
            Assert::AreEqual(ullMinVersionExpected, ullMinVersionActual, format(L"Unexpected minimum version parsing '%ls'", pwszVersionRange).c_str());
            Assert::AreEqual(ullMaxVersionExpected, ullMaxVersionActual, format(L"Unexpected maximum version parsing '%ls'", pwszVersionRange).c_str());
        }
    }
};
