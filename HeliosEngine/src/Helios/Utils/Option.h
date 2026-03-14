#pragma once

#include "Helios/Core/Panic.h"
#include "pch.h"
#include <source_location>
#include <string_view>
#include <type_traits>
#include <utility>

namespace Helios
{
    struct NoneType {};
    inline constexpr NoneType None{};
    
    template <typename T>
    struct Option
    {
    public:
        using value_type = T;

        Option() = default;
        Option(const T& value) : m_value(value) {}
        Option(T&& value) : m_value(std::move(value)) {}

        Option(NoneType) : m_value(std::nullopt) {}

        template<typename U>
        Option(NoneType, U&&) : m_value(std::nullopt) {}

        T expect(const std::string_view message, std::source_location location = std::source_location::current()) const { if (!m_value) panic_builder{location}(message); return *m_value; }

        T& unwrap(std::source_location location = std::source_location::current()) { if (!m_value) panic_builder{location}("Option is not set!"); return *m_value; }
        const T& unwrap(std::source_location location = std::source_location::current()) const { if (!m_value) panic_builder{location}("Option is not set!"); return *m_value; }
        const T unwrap_or(const T& default_value) const { if (!m_value) return default_value; return *m_value; }
        const T unwrap_or_else(auto&& default_value) const { 
            if (m_value) 
                return *m_value;
            
            return std::invoke(std::forward<decltype(default_value)>(default_value));
        }

        bool is_some() const { return m_value; }
        bool is_none() const { return !m_value; }

        auto map(auto&& func) const { 
            using U = std::invoke_result_t<decltype(func), const T&>;
            
            if (!m_value) return Option<U>(None);
            
            return Option<U>(std::invoke(func, *m_value));
        }
    
        auto map_or(auto&& default_value, auto&& func) const
        {
            using ResultType = std::invoke_result_t<decltype(func), const T&>;
            using DefaultType = decltype(default_value);

            static_assert(std::is_convertible_v<DefaultType, ResultType>, "default_value must be convertible to the return type of func");

            if (m_value) return std::invoke(func, *m_value);

            return default_value;
        }

        auto map_or_else(auto&& default_value, auto&& func) const
        {
            using ResultType = std::invoke_result_t<decltype(func), const T&>;
            using DefaultType = std::invoke_result_t<decltype(default_value)>;

            static_assert(std::is_convertible_v<DefaultType, ResultType>, "default_value must be convertible to the return type of func");

            if (m_value) return std::invoke(func, *m_value);

            return std::invoke(default_value);
        }

        auto and_then(auto&& func) const {
            using Func = decltype(func);
            using U = std::invoke_result_t<Func, const value_type&>;

            static_assert(std::is_base_of_v<Option<typename U::value_type>, U>, "func must return an Option");

            if (!m_value) return Option<typename U::value_type>(None);
            return std::invoke(std::forward<Func>(func), *m_value);
        }

        auto or_else(auto&& func) const {
            using Func = decltype(func);
            using U = std::invoke_result_t<Func, const value_type&>;

            static_assert(std::is_base_of_v<Option<typename U::value_type>, U>, "func must return an Option");

            if (!m_value) return std::invoke(std::forward<Func>(func));
            return *this;
        }

        auto filter(auto&& predicate) const {
            if (!m_value || !std::invoke(predicate, *m_value)) return Option<T>(None);
            return *this;
        }

        template<typename U, typename = void>
        struct has_value_type : std::false_type {};

        template<typename U>
        struct has_value_type<U, std::void_t<typename U::value_type>> : std::true_type {};

        auto flatten() const {
            static_assert(has_value_type<T>::value, "flatten can only be called on an Option of an Option");
            
            using U = T::value_type;
            static_assert(std::is_base_of_v<Option<U>, T>, "flatten can only be called on an Option of an Option");

            // if (!m_value) return Option<U>(None);
            
            return *m_value;
        }

    private:
        std::optional<T> m_value;
    };

    template <typename T>
    auto Some(T&& value) { return Option<T>(std::forward<T>(value)); }

    template <typename T>
    inline constexpr Option<T> none() { return Option<T>(None); }
}