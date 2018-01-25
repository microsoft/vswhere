// <copyright file="Utf8Console.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>
#pragma once

#include "Console.h"
#include "CommandArgs.h"
class Utf8Console :
    public Console
{
public:
    Utf8Console(_In_ const CommandArgs &args, FILE *output) : Console(args)
    {
        m_Output = output;
    }
    Utf8Console(_In_ const Utf8Console& obj) : Console(obj)
    {
        m_Output = obj.m_Output;
    }
    virtual ~Utf8Console()
    {
    }
    virtual void Initialize() noexcept override;
protected:
    virtual void Write(_In_ LPCWSTR wzFormat, va_list args) override;
private:
    FILE * m_Output;
};

