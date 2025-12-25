#pragma once

#include "pch.h"

namespace Helios
{
    // Abusing C++20

    template<typename E>
    struct enable_bitmask_operators : std::false_type {};

    template<typename E>
    constexpr bool is_bitmask_enum_v = enable_bitmask_operators<E>::value;

    template<typename E>
    requires is_bitmask_enum_v<E>
    constexpr E operator|(E lhs, E rhs) {
        using U = std::underlying_type_t<E>;
        return static_cast<E>(static_cast<U>(lhs) | static_cast<U>(rhs));
    }

    template<typename E>
    requires is_bitmask_enum_v<E>
    constexpr E operator&(E lhs, E rhs) {
        using U = std::underlying_type_t<E>;
        return static_cast<E>(static_cast<U>(lhs) & static_cast<U>(rhs));
    }

    template<typename E>
    requires is_bitmask_enum_v<E>
    constexpr E operator^(E lhs, E rhs) {
        using U = std::underlying_type_t<E>;
        return static_cast<E>(static_cast<U>(lhs) ^ static_cast<U>(rhs));
    }

    template<typename E>
    requires is_bitmask_enum_v<E>
    constexpr E operator~(E lhs) {
        using U = std::underlying_type_t<E>;
        return static_cast<E>(~static_cast<U>(lhs));
    }

    template<typename E>
    requires is_bitmask_enum_v<E>
    constexpr E& operator|=(E& lhs, E rhs) {
        return lhs = lhs | rhs;
    }

    template<typename E>
    requires is_bitmask_enum_v<E>
    constexpr E& operator&=(E& lhs, E rhs) {
        return lhs = lhs & rhs;
    }

    template<typename E>
    requires is_bitmask_enum_v<E>
    constexpr bool any(E e) {
        using U = std::underlying_type_t<E>;
        return static_cast<U>(e) != 0;
    }

    template<typename E>
    requires is_bitmask_enum_v<E>
    constexpr bool none(E e) {
        using U = std::underlying_type_t<E>;
        return static_cast<U>(e) == 0;
    }

    #define ENABLE_BITMASK_OPERATORS(typename) \
    template<> \
    struct enable_bitmask_operators<typename> : std::true_type {};
}
