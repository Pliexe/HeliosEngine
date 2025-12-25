#pragma once

#include "Helios/Core/Exceptions.h"
#include "pch.h"

namespace Helios::Utils
{
    // C++23 Please release already.... (on all platforms)
    template<typename T, typename E>
    class Expected {
        std::variant<T, E> data;

    public:
        Expected(const T& val) : data(val) {}
        Expected(T&& val) : data(std::move(val)) {}
        Expected(const E& err) : data(err) {}
        Expected(E&& err) : data(std::move(err)) {}

        bool has_value() const {
            return std::holds_alternative<T>(data);
        }

        T& value() {
            HL_EXCEPTION(!has_value(), "bad expected access");
            return std::get<T>(data);
        }

        const T& value() const {
            HL_EXCEPTION(!has_value(), "bad expected access");
            return std::get<T>(data);
        }

        E& error() {
            HL_EXCEPTION(has_value(), "no error present");
            return std::get<E>(data);
        }

        const E& error() const {
            HL_EXCEPTION(has_value(), "no error present");
            return std::get<E>(data);
        }

        explicit operator bool() const { return has_value(); }
    };

}