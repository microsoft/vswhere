// <copyright file="ModuleTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// Wrap something cheap and easy to create.
_COM_SMARTPTR_TYPEDEF(IBindCtx, __uuidof(IBindCtx));

TEST_CLASS(ModuleTests)
{
public:
    TEST_METHOD(FromIUnknown_NULL)
    {
        Module sut;
        sut.FromIUnknown(NULL);

        Assert::AreEqual<size_t>(0, sut.get_Path().length());
        Assert::AreEqual<size_t>(0, sut.get_FileVersion().length());
    }

    TEST_METHOD(FromIUnknown_Not_NULL)
    {
        IBindCtxPtr binder;
        Assert::AreEqual(S_OK, ::CreateBindCtx(0, &binder));

        Module sut;
        sut.FromIUnknown(binder);

        Assert::AreNotEqual<size_t>(0, sut.get_Path().length());
        Assert::AreNotEqual<size_t>(0, sut.get_FileVersion().length());
    }
};
