// <copyright file="Utf8ConsoleTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"
#include <vector>

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(Utf8ConsoleTests)
{
public:
    TEST_METHOD(Output_file)
    {
        WCHAR tempDir[MAX_PATH], tempPath[MAX_PATH];
        memset(tempDir, 0, sizeof(tempDir));
        GetTempPath(MAX_PATH, tempDir);
        GetTempFileName(tempDir, L"utf8console", 0, tempPath);
        FILE *fp;
        auto err = _wfopen_s(&fp, tempPath, L"w+");
        Assert::AreEqual(0, err);
        Assert::IsNotNull(fp);
        try
        {
            CommandArgs args;
            Utf8Console u8console(args, fp);
            u8console.Initialize();
            Console& console = u8console;
            std::vector<wchar_t> wbuf;
            // ascii code
            wbuf.push_back(0x12);
            // 0x80 - 0x7ff code
            wbuf.push_back(0x123);
            // 0x800 - 0xD7FF
            wbuf.push_back(0x1234);
            // 0xE000 - 0xFFFF
            wbuf.push_back(0xE123);
            wbuf.push_back(L'\0');
            console.Write(L"%s", wbuf.data());
            fseek(fp, 0, SEEK_SET);
            char actualmbs[64];
            memset(actualmbs, 0, sizeof(actualmbs));
            fgets(actualmbs, 64, fp);
            wchar_t actualw[64];
            memset(actualw, 0, sizeof(actualw));
            MultiByteToWideChar(CP_UTF8, 0, actualmbs, strlen(actualmbs), actualw, 64);
            Assert::AreEqual(std::wstring(wbuf.data()), std::wstring(actualw));
        }
        catch (...)
        {
            fclose(fp);
            DeleteFile(tempPath);
            throw;
        }
        fclose(fp);
        DeleteFile(tempPath);
    }
};
