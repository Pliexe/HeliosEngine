#pragma once

#include "pch.h"

namespace Helios
{
    class BundleException : public std::exception
    {
    public:
        BundleException(const std::string& message) {}
        
        const char* what() const noexcept override
        {
            return (m_Message).c_str();
        }
    private:
        std::string m_Message;
    };

    class BundleFileException : public BundleException
    {
    public:
        BundleFileException(const std::string& message) : BundleException("BundleFileException: " + message) {}
    };

    class BundleBadFileException : public BundleFileException
    {
    public:
        BundleBadFileException(const std::string& message) : BundleFileException("BundleBadFileException: " + message) {}
    };
}