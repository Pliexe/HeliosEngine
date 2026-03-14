#pragma once

#include "pch.h"

namespace Helios
{
    template<typename Enum>
    struct EnableBitmaskOperators : std::false_type {};
    
    template<typename Enum>
    requires EnableBitmaskOperators<Enum>::value
    constexpr Enum operator|(Enum lhs, Enum rhs) noexcept {
        using U = std::underlying_type_t<Enum>;
        return static_cast<Enum>(static_cast<U>(lhs) | static_cast<U>(rhs));
    }

    template<typename Enum>
    requires EnableBitmaskOperators<Enum>::value
    constexpr Enum& operator|=(Enum& lhs, Enum rhs) noexcept {
        lhs = lhs | rhs;
        return lhs;
    }

    template<typename Enum>
    requires EnableBitmaskOperators<Enum>::value
    constexpr Enum operator&(Enum lhs, Enum rhs) noexcept {
        using U = std::underlying_type_t<Enum>;
        return static_cast<Enum>(static_cast<U>(lhs) & static_cast<U>(rhs));
    }

    template<typename Enum>
    requires EnableBitmaskOperators<Enum>::value
    constexpr Enum& operator&=(Enum& lhs, Enum rhs) noexcept {
        lhs = lhs & rhs;
        return lhs;
    }

    template<typename Enum>
    requires EnableBitmaskOperators<Enum>::value
    constexpr Enum operator~(Enum e) noexcept {
        using U = std::underlying_type_t<Enum>;
        return static_cast<Enum>(~static_cast<U>(e));
    }
    
    template<typename Enum>
    requires EnableBitmaskOperators<Enum>::value
    constexpr bool has_flag(Enum lhs, Enum rhs) noexcept {
        using U = std::underlying_type_t<Enum>;
        return static_cast<bool>(static_cast<U>(lhs) & static_cast<U>(rhs));
    }
}