#pragma once

#include "Helios/Core/Base.h"
#include "Helios/Events/Event.h"
#include "pch.h"

#define LOG_LEVEL_INFO 0
#define LOG_LEVEL_WARNING 1
#define LOG_LEVEL_ERROR 2

#ifndef __cplusplus
#define LogLevel unsigned int
#endif

#ifdef __cplusplus
namespace Helios::Logger
{
    
    enum class LogLevel
    {
        Info = LOG_LEVEL_INFO,
        Warning = LOG_LEVEL_WARNING,
        Error = LOG_LEVEL_ERROR
    };

    void HELIOS_API SetCallback(std::function<void(Event&)> callback);    
    void HELIOS_API Submit(LogLevel level, std::string_view message);
    
    template<typename... Args>
    void Info(std::format_string<Args...> fmt, Args&&... args)
    {
        Logger::Submit(LogLevel::Info, std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void Warning(std::format_string<Args...> fmt, Args&&... args)
    {
        Logger::Submit(LogLevel::Warning, std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void Error(std::format_string<Args...> fmt, Args&&... args)
    {
        Logger::Submit(LogLevel::Error, std::format(fmt, std::forward<Args>(args)...));
    }
}
#endif