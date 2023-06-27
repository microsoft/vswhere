// <copyright file="GlobTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#ifdef _DEBUG
#define ASSERT_PATTERN
#endif

TEST_CLASS(GlobTests)
{
public:
    BEGIN_TEST_METHOD_ATTRIBUTE(No_Pattern)
        TEST_WORKITEM(162)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(No_Pattern)
    {
        Glob sut(L"C:\\ShouldNotExist", L"MSBuild\\15.0\\Bin\\MSBuild.exe");
        Assert::AreEqual(L"C:\\ShouldNotExist\\MSBuild\\15.0\\Bin", sut.Root().c_str());

#ifdef ASSERT_PATTERN
        Assert::AreEqual(L"^\\\\MSBuild\\.exe$", sut.Pattern().c_str());
#endif

        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\15.0\\Bin\\MSBuild.exe"));
        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\MSBuild.exe"));
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Zero_Width_Globstar)
        TEST_WORKITEM(162)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Zero_Width_Globstar)
    {
        Glob sut(L"C:\\ShouldNotExist", L"MSBuild\\**\\MSBuild.exe");
        Assert::AreEqual(L"C:\\ShouldNotExist\\MSBuild", sut.Root().c_str());

#ifdef ASSERT_PATTERN
        Assert::AreEqual(L"^\\\\(.*\\\\)?MSBuild\\.exe$", sut.Pattern().c_str());
#endif

        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\amd64\\MSBuild.exe"));
        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\MSBuild.exe"));
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Multiple_Globstars)
        TEST_WORKITEM(162)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Multiple_Globstars)
    {
        Glob sut(L"C:\\ShouldNotExist", L"MSBuild\\**\\Bin\\**\\MSBuild.exe");
        Assert::AreEqual(L"C:\\ShouldNotExist\\MSBuild", sut.Root().c_str());

#ifdef ASSERT_PATTERN
        Assert::AreEqual(L"^\\\\(.*\\\\)?Bin\\\\(.*\\\\)?MSBuild\\.exe$", sut.Pattern().c_str());
#endif

        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\MSBuild.exe"));
        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\amd64\\MSBuild.exe"));
        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\MSBuild.exe"));
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Matches_Subdirectory)
        TEST_WORKITEM(162)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Matches_Subdirectory)
    {
        Glob sut(L"C:\\ShouldNotExist", L"MSBuild\\**\\Bin\\*\\MSBuild.exe");
        Assert::AreEqual(L"C:\\ShouldNotExist\\MSBuild", sut.Root().c_str());

#ifdef ASSERT_PATTERN
        Assert::AreEqual(L"^\\\\(.*\\\\)?Bin\\\\[^\\]*\\\\MSBuild\\.exe$", sut.Pattern().c_str());
#endif

        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\MSBuild.exe"));
        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\MSBuild.exe"));
        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\amd64\\MSBuild.exe"));
        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\MSBuild.exe"));
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Matches_Question_Mark)
        TEST_WORKITEM(162)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Matches_Question_Mark)
    {
        Glob sut(L"C:\\ShouldNotExist", L"MSBuild\\Current\\Bin\\MSBuild.ex?");
        Assert::AreEqual(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin", sut.Root().c_str());

#ifdef ASSERT_PATTERN
        Assert::AreEqual(L"^\\\\MSBuild\\.ex[^\\]$", sut.Pattern().c_str());
#endif

        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\MSBuild.exe"));
        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\MSBuild.ex"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\MSBuild.exe"));
        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\MSBuild.ex\\"));
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Escapes)
        TEST_WORKITEM(162)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Escapes)
    {
        Glob sut(L"C:\\ShouldNotExist", L"VC\\Tools\\MSVC\\14.*\\bin\\**\\cl.exe");
        Assert::AreEqual(L"C:\\ShouldNotExist\\VC\\Tools\\MSVC", sut.Root().c_str());

#ifdef ASSERT_PATTERN
        Assert::AreEqual(L"^\\\\14\\.[^\\]*\\\\bin\\\\(.*\\\\)?cl\\.exe$", sut.Pattern().c_str());
#endif

        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\VC\\Tools\\MSVC\\15.0\\bin\\cl.exe"));
        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\VC\\Tools\\MSVC\\14\\bin\\cl.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\VC\\Tools\\MSVC\\14.16.27023\\bin\\cl.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\VC\\Tools\\MSVC\\14.16.27023\\bin\\Hostx64\\x86\\cl.exe"));
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Case_Insensitive)
        TEST_WORKITEM(162)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Case_Insensitive)
    {
        Glob sut(L"C:\\ShouldNotExist", L"msbuild\\**\\msbuild.exe");
        Assert::AreEqual(L"C:\\ShouldNotExist\\msbuild", sut.Root().c_str());

#ifdef ASSERT_PATTERN
        Assert::AreEqual(L"^\\\\(.*\\\\)?msbuild\\.exe$", sut.Pattern().c_str());
#endif

        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\amd64\\MSBuild.exe"));
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Leading_Globstar_Invalid)
        TEST_WORKITEM(162)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Leading_Globstar_Invalid)
    {
        try
        {
            Glob(L"C:\\ShouldNotExist", L"**msbuild.exe");
        }
        catch (const win32_error& ex)
        {
            Assert::AreEqual<int>(ERROR_INVALID_PARAMETER, ex.code().value());
            return;
        }

        Assert::Fail(L"No exception thrown");
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Trailing_Globstar_Invalid)
        TEST_WORKITEM(162)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Trailing_Globstar_Invalid)
    {
        try
        {
            Glob(L"C:\\ShouldNotExist", L"msbuild**");
        }
        catch (const win32_error& ex)
        {
            Assert::AreEqual<int>(ERROR_INVALID_PARAMETER, ex.code().value());
            return;
        }

        Assert::Fail(L"No exception thrown");
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Trailing_Globstar)
        TEST_WORKITEM(162)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Trailing_Globstar)
    {
        Glob sut(L"C:\\ShouldNotExist", L"MSBuild\\**");
        Assert::AreEqual(L"C:\\ShouldNotExist\\MSBuild", sut.Root().c_str());

#ifdef ASSERT_PATTERN
        Assert::AreEqual(L"^\\\\.*$", sut.Pattern().c_str());
#endif

        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\amd64\\MSBuild.exe"));
        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\MSBuild.exe"));
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Leading_Backslash)
        TEST_WORKITEM(171)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Leading_Backslash)
    {
        Glob sut(L"C:\\ShouldNotExist", L"\\MSBuild\\**\\MSBuild.exe");
        Assert::AreEqual(L"C:\\ShouldNotExist\\MSBuild", sut.Root().c_str());

#ifdef ASSERT_PATTERN
        Assert::AreEqual(L"^\\\\(.*\\\\)?MSBuild\\.exe$", sut.Pattern().c_str());
#endif

        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\amd64\\MSBuild.exe"));
        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\MSBuild.exe"));
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Leading_Double_Backslashes)
        TEST_WORKITEM(171)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Leading_Double_Backslashes)
    {
        Glob sut(L"C:\\ShouldNotExist", L"\\\\MSBuild\\**\\MSBuild.exe");
        Assert::AreEqual(L"C:\\ShouldNotExist\\MSBuild", sut.Root().c_str());

#ifdef ASSERT_PATTERN
        Assert::AreEqual(L"^\\\\(.*\\\\)?MSBuild\\.exe$", sut.Pattern().c_str());
#endif

        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\amd64\\MSBuild.exe"));
        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\MSBuild.exe"));
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Double_Backslashes_Later)
        TEST_WORKITEM(171)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Double_Backslashes_Later)
    {
        Glob sut(L"C:\\ShouldNotExist", L"MSBuild\\**\\Bin\\\\MSBuild.exe");
        Assert::AreEqual(L"C:\\ShouldNotExist\\MSBuild", sut.Root().c_str());

#ifdef ASSERT_PATTERN
        Assert::AreEqual(L"^\\\\(.*\\\\)?Bin\\\\MSBuild\\.exe$", sut.Pattern().c_str());
#endif

        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\MSBuild.exe"));
        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\MSBuild.exe"));
        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\amd64\\MSBuild.exe"));
        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\MSBuild.exe"));
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Parent_Directory)
        TEST_WORKITEM(171)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Parent_Directory)
    {
        try
        {
            Glob(L"C:\\ShouldNotExist", L"..\\**\\msbuild.exe");
        }
        catch (const win32_error& ex)
        {
            Assert::AreEqual<int>(ERROR_INVALID_PARAMETER, ex.code().value());
            return;
        }

        Assert::Fail(L"No exception thrown");
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Parent_Directory_Later)
        TEST_WORKITEM(171)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Parent_Directory_Later)
    {
        try
        {
            Glob(L"C:\\ShouldNotExist", L"**\\..\\msbuild.exe");
        }
        catch (const win32_error& ex)
        {
            Assert::AreEqual<int>(ERROR_INVALID_PARAMETER, ex.code().value());
            return;
        }

        Assert::Fail(L"No exception thrown");
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Current_Directory)
        TEST_WORKITEM(171)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Current_Directory)
    {
        Glob sut(L"C:\\ShouldNotExist", L".\\**\\msbuild.exe");
        Assert::AreEqual(L"C:\\ShouldNotExist", sut.Root().c_str());

#ifdef ASSERT_PATTERN
        Assert::AreEqual(L"^\\\\(.*\\\\)?msbuild\\.exe$", sut.Pattern().c_str());
#endif

        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\amd64\\MSBuild.exe"));
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Current_Directory_Later)
        TEST_WORKITEM(171)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Current_Directory_Later)
    {
        Glob sut(L"C:\\ShouldNotExist", L"**\\.\\msbuild.exe");
        Assert::AreEqual(L"C:\\ShouldNotExist", sut.Root().c_str());

#ifdef ASSERT_PATTERN
        Assert::AreEqual(L"^\\\\(.*\\\\)?msbuild\\.exe$", sut.Pattern().c_str());
#endif

        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\MSBuild.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\MSBuild\\Current\\Bin\\amd64\\MSBuild.exe"));
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Ignores_Trailing_Separator)
        TEST_WORKITEM(290)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Ignores_Trailing_Separator)
    {
        Glob sut(L"C:\\ShouldNotExist\\", L"**\\cl.exe");

        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\VC\\Tools\\MSVC\\14.36.32532\\bin\\Hostx64\\x64\\cl.exe"));
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Issue290)
        TEST_WORKITEM(290)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Issue290)
    {
        Glob sut(L"C:\\ShouldNotExist", L"**\\cl.exe");

#ifdef ASSERT_PATTERN
        Assert::AreEqual(L"^\\\\(.*\\\\)?cl\\.exe$", sut.Pattern().c_str());
#endif


        Assert::IsFalse(sut.Match(L"C:\\ShouldNotExist\\Common7\\IDE\\CommonExtensions\\Microsoft\\TeamFoundation\\Team Explorer\\Git\\usr\\bin\\getfacl.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\VC\\Tools\\MSVC\\14.36.32532\\bin\\Hostx64\\arm64\\cl.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\VC\\Tools\\MSVC\\14.36.32532\\bin\\Hostx64\\x64\\cl.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\VC\\Tools\\MSVC\\14.36.32532\\bin\\Hostx86\\arm\\cl.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\VC\\Tools\\MSVC\\14.36.32532\\bin\\Hostx86\\x86\\cl.exe"));
        
        // Assert the reason the last directory specifier is optional: to make sure we find cl.exe in the root directory.
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\cl.exe"));
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Issue290_With_Wildcard_Filename)
        TEST_WORKITEM(290)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Issue290_With_Wildcard_Filename)
    {
        Glob sut(L"C:\\ShouldNotExist", L"**\\*cl.exe");

#ifdef ASSERT_PATTERN
        Assert::AreEqual(L"^\\\\(.*\\\\)?[^\\]*cl\\.exe$", sut.Pattern().c_str());
#endif


        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\Common7\\IDE\\CommonExtensions\\Microsoft\\TeamFoundation\\Team Explorer\\Git\\usr\\bin\\getfacl.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\VC\\Tools\\MSVC\\14.36.32532\\bin\\Hostx64\\arm64\\cl.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\VC\\Tools\\MSVC\\14.36.32532\\bin\\Hostx64\\x64\\cl.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\VC\\Tools\\MSVC\\14.36.32532\\bin\\Hostx86\\arm\\cl.exe"));
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\VC\\Tools\\MSVC\\14.36.32532\\bin\\Hostx86\\x86\\cl.exe"));
        
        // Assert the reason the last directory specifier is optional: to make sure we find cl.exe in the root directory.
        Assert::IsTrue(sut.Match(L"C:\\ShouldNotExist\\cl.exe"));
    }
};
