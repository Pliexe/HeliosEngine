#pragma once

#include "Helios/Utils/ShowMessage.h"
#include "pch.h"
#include <source_location>

namespace Helios
{
    struct panic_builder // NOLINT(readability-identifier-naming)
    {
        std::source_location loc;
        
        panic_builder(std::source_location loc) : loc(loc) {}

        // template <typename... Args>
        // [[noreturn]] void operator()(const std::format_string<Args...> fmt, Args&&... args) const {
        //     auto message = std::format(fmt, std::forward<Args>(args)...);
        //     std::cerr << std::format("{} panicked '{}' at {}:{}:{}\n", loc.function_name(), message, loc.file_name(), loc.line(), loc.column());
        //     ShowMessage("Panic!", std::format("At: {}:{}:{}\nIn: {}\nReason: {}\n", loc.file_name(), loc.line(), loc.column(), loc.function_name(), message), Message::Flags::IconError | Message::Flags::Ok);
        //     std::abort();
        // }

        template <typename... Args>
        [[noreturn]] void operator()(const std::format_string<Args...> fmt, Args&&... args) const {
            operator()(std::format(fmt, std::forward<Args>(args)...));
        }

        [[noreturn]] void operator()(const std::string_view message) const {
            std::cerr << std::format("{} panicked '{}' at {}:{}:{}\n", loc.function_name(), message, loc.file_name(), loc.line(), loc.column());
            ShowMessage("Panic!", std::format("At: {}:{}:{}\nIn: {}\nReason: {}\n", loc.file_name(), loc.line(), loc.column(), loc.function_name(), message), Message::Flags::IconError | Message::Flags::Ok);
            std::abort();
        }
    };
    
    template <typename... Args>
    struct panic // NOLINT(readability-identifier-naming)
    {
        panic(const std::format_string<Args...> fmt, Args&&... args, std::source_location loc = std::source_location::current()) {
            panic_builder{loc}(fmt, std::forward<Args>(args)...);
        }
    };

    template <typename... Args>
    panic(const std::format_string<Args...> fmt, Args&&... args) -> panic<Args...>;
}