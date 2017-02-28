// <copyright file="ExceptionsTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(ExceptionsTests)
{
public:
    TEST_METHOD_INITIALIZE(Initialize)
    {
        ::SetThreadLocale(1033);
    }

    TEST_METHOD(win32_error_last_error)
    {
        ::SetLastError(ERROR_NOT_FOUND);

        win32_error sut;
        Assert::AreEqual<int>(ERROR_NOT_FOUND, sut.code().value());
    }

    TEST_METHOD(win32_error_explicit)
    {
        win32_error sut(ERROR_NOT_FOUND);
        Assert::AreEqual<int>(ERROR_NOT_FOUND, sut.code().value());
        Assert::AreEqual("Element not found.\r\n", sut.what());
        Assert::AreEqual(L"Element not found.\r\n", sut.wwhat());
    }

    TEST_METHOD(win32_error_custom_message)
    {
        win32_error sut(ERROR_NOT_FOUND, L"property not found");
        Assert::AreEqual<int>(ERROR_NOT_FOUND, sut.code().value());
        Assert::AreEqual("property not found: Element not found.\r\n", sut.what());
        Assert::AreEqual(L"property not found", sut.wwhat());
    }
};
