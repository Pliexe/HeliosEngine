#pragma once

#include "Helios/Core/Panic.h"
#include "pch.h"
#include <source_location>
#include <string_view>
#include <type_traits>
#include <utility>

namespace Helios
{    
    template <typename T>
    struct Ok {

        static_assert(!std::is_same_v<T, void>, "Value type cannot be void");

        Ok(const T& value) : value(value) {}
        Ok(T&& value) : value(std::move(value)) {}
        
        T value;
    };
    
    template <typename E>
    struct Err {

        static_assert(!std::is_same_v<E, void>, "Error type cannot be void");

        Err(const E& error) : error(error) {}
        Err(E&& error) : error(std::move(error)) {}
        
        E error;
    };
    
    template <typename T, typename E>
    struct Result
    {
    public:
        using value_type = T;
        using error_type = E;

        Result() = default;

        template <typename U, typename = std::enable_if_t<std::is_constructible_v<T, U>>>
        Result(const U& value) : m_value(value) {}

        template <typename U, typename = std::enable_if_t<std::is_constructible_v<T, U>>>
        Result(U&& value) : m_value(std::move(value)) {}
        
        Result(Ok<T> ok) : m_value(std::move(ok.value)) {}
        Result(Err<E> err) : m_value(std::move(err.error)) {}

        bool is_ok() const { return std::holds_alternative<T>(m_value); }
        bool is_err() const { return std::holds_alternative<E>(m_value); }

        T expect(const std::string_view message, std::source_location location = std::source_location::current()) const { if (auto* v = std::get_if<T>(&m_value)) return *v; panic_builder{location}(message); }
        E expect_err(const std::string_view message, std::source_location location = std::source_location::current()) const { if (auto* v = std::get_if<E>(&m_value)) return *v; panic_builder{location}(message); }

        T& unwrap(std::source_location location = std::source_location::current()) { 
            if (auto* v = std::get_if<T>(&m_value)) 
                return *v;
            
            panic_builder{location}("Result is not set!");
        }
        const T& unwrap(std::source_location location = std::source_location::current()) const { 
            if (auto* v = std::get_if<T>(&m_value)) 
                return *v;
            
            panic_builder{location}("Result is not set!");
        }
        const T unwrap_or(const T& default_value) const { 
            if (auto* v = std::get_if<T>(&m_value)) 
                return *v;
            
            return default_value;
        }
        const T unwrap_or_else(auto&& default_value) const { 
            if (auto* v = std::get_if<T>(&m_value)) 
                return *v;
            
            return std::invoke(std::forward<decltype(default_value)>(default_value));
        }


        auto map(auto&& func) const { 
            using U = std::invoke_result_t<decltype(func), const T&>;
            
            if (auto* v = std::get_if<T>(&m_value)) 
                return Result<U, E>(Ok(std::invoke(func, *v)));
            
            return Result<T, E>(*this);
        }

        auto map_err(auto&& func) const { 
            using U = std::invoke_result_t<decltype(func), const E&>;
            
            if (auto* v = std::get_if<E>(&m_value)) 
                return Result<T, U>(Err(std::invoke(func, *v)));
            
            return Result<T, E>(*this);
        }
    
        auto map_or(auto&& default_value, auto&& func) const
        {
            using ResultType = std::invoke_result_t<decltype(func), const T&>;
            using DefaultType = decltype(default_value);

            static_assert(std::is_convertible_v<DefaultType, ResultType>, "default_value must be convertible to the return type of func");

            if (auto* v = std::get_if<T>(&m_value)) 
                return std::invoke(func, *v);
            
            return default_value;
        }

        auto map_or_else(auto&& default_value, auto&& func) const
        {
            using ResultType = std::invoke_result_t<decltype(func), const T&>;
            using DefaultType = std::invoke_result_t<decltype(default_value)>;

            static_assert(std::is_convertible_v<DefaultType, ResultType>, "default_value must be convertible to the return type of func");

            if (auto* v = std::get_if<T>(&m_value)) 
                return std::invoke(func, *v);
            
            return std::invoke(default_value);
        }

        auto and_then(auto&& func) const {
            using Func = decltype(func);
            using U = std::invoke_result_t<Func, const value_type&>;

            static_assert(std::is_base_of_v<Result<typename U::value_type, E>, U>, "func must return an Result");

            if (auto* v = std::get_if<T>(&m_value)) 
                return std::invoke(func, *v);
            
            return Result<T, E>(*this);
        }

        auto or_else(auto&& func) const {
            using Func = decltype(func);
            using U = std::invoke_result_t<Func, const error_type&>;

            static_assert(std::is_base_of_v<Result<T, typename U::error_type>, U>, "func must return an Result");

            if (auto* v = std::get_if<E>(&m_value)) 
                return std::invoke(func, *v);
            
            return Result<T, E>(*this);
        }

        template<typename U, typename = void>
        struct has_value_type : std::false_type {};

        template<typename U>
        struct has_value_type<U, std::void_t<typename U::value_type>> : std::true_type {};

        auto flatten() const {
            static_assert(has_value_type<T>::value, "flatten can only be called on an Result of an Result");
            
            using U = T::value_type;
            static_assert(std::is_base_of_v<Result<U, E>, T>, "flatten can only be called on an Result of an Result");

            if (auto* v = std::get_if<T>(&m_value)) 
                return *v;
            
            return Result<U, E>(*this);
        }

    private:
        std::variant<T, E> m_value;
    };
}