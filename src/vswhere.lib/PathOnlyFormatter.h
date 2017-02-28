// <copyright file="TextFormatter.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class PathOnlyFormatter :
    public Formatter
{
public:
    static std::unique_ptr<Formatter> Create()
    {
        return std::unique_ptr<PathOnlyFormatter>(new PathOnlyFormatter());
    }

    PathOnlyFormatter() :
        Formatter(),
        m_pathWritten(false)
    {
    }

    PathOnlyFormatter(_In_ const PathOnlyFormatter& obj) :
        Formatter(obj),
        m_pathWritten(obj.m_pathWritten)
    {
    }

    bool ShowLogo() const override
    {
        return false;
    }

protected:
    void StartObject(_In_ std::wostream& out) override;
    void WriteProperty(_In_ std::wostream& out, _In_ const std::wstring& name, _In_ const std::wstring& value) override;
    void EndObject(_In_ std::wostream& out) override;

private:
    bool m_pathWritten;
};
