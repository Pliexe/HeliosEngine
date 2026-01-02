#pragma once

#include "pch.h"

namespace Helios
{
    template <typename T, std::size_t S, bool IsVector> struct TVector;

    template<typename T, std::size_t S>
    TVector<T, S, true> operator+(const TVector<T, S, true>& a, const TVector<T, S, true>& b) {
        return [&]<std::size_t... I>(std::index_sequence<I...>) {
            return TVector<T, S, true>{ (a.data[I] + b.data[I])... };
        }(std::make_index_sequence<S>{});
    }

    template<typename T, std::size_t S>
    TVector<T, S, true> operator- (const TVector<T, S, true>& a, const TVector<T, S, true>& b) 
    {
        return [&]<std::size_t... I>(std::index_sequence<I...>) {
            return TVector<T, S, true> { (a.data[I] - b.data[I])... };
        }(std::make_index_sequence<S>{});
    }

    template<typename T, std::size_t S>
    TVector<T, S, true> operator* (const TVector<T, S, true>& a, const float scale) 
    {
        return [&]<std::size_t... I>(std::index_sequence<I...>) {
            return TVector<T, S, true> { (a.data[I] * scale)... };
        }(std::make_index_sequence<S>{});
    }

    template<typename T, std::size_t S>
    TVector<T, S, true> operator* (const float& scale, const TVector<T, S, true> a) 
    {
        return [&]<std::size_t... I>(std::index_sequence<I...>) {
            return TVector<T, S, true> { (a.data[I] * scale)... };
        }(std::make_index_sequence<S>{});
    }

    template<typename T, std::size_t S>
    TVector<T, S, true> operator/ (const TVector<T, S, true>& a, const float scale) 
    {
        return [&]<std::size_t... I>(std::index_sequence<I...>) {
            return TVector<T, S, true> { (a.data[I] / scale)... };
        }(std::make_index_sequence<S>{});
    }

    template <typename Derived, std::size_t S, bool IsVector>
    struct TVectorOps
    {
        Derived& self() { return static_cast<Derived&>(*this); }
        const Derived& self() const { return static_cast<const Derived&>(*this); }

        bool operator== (const Derived& other) const { return std::equal(self().data, self().data + S, other.data); }
        bool operator!= (const Derived& other) const { return !(*this == other); }
        
        Derived& operator=(const Derived& other) { return self() = other; }

        Derived operator+() const { return self(); }
        Derived operator-() const { return -self(); }

        Derived& operator+= (const Derived& other)
        {
            auto& self = static_cast<Derived&>(*this);
            
            ([&]<std::size_t... I>(std::index_sequence<I...>) {
                ((self.data[I] += other.data[I]), ...);
                return self;
            })(std::make_index_sequence<S>{});

            return self;
        }

        Derived& operator-= (const Derived& other)
        {
            auto& self = static_cast<Derived&>(*this);
            
            ([&]<std::size_t... I>(std::index_sequence<I...>) {
                ((self.data[I] -= other.data[I]), ...);
                return self;
            })(std::make_index_sequence<S>{});

            return self;
        }

        Derived& operator*= (const float scale)
        {
            auto& self = static_cast<Derived&>(*this);
            
            ([&]<std::size_t... I>(std::index_sequence<I...>) {
                ((self.data[I] *= scale), ...);
                return self;
            })(std::make_index_sequence<S>{});

            return self;
        }

        Derived& operator/= (const float scale)
        {
            auto& self = static_cast<Derived&>(*this);
            
            ([&]<std::size_t... I>(std::index_sequence<I...>) {
                ((self.data[I] /= scale), ...);
                return self;
            })(std::make_index_sequence<S>{});

            return self;
        }

        float magnitude() const requires IsVector { return std::sqrt(sqr_magnitude()); }
        float sqr_magnitude() const requires IsVector { return Dot(self(), self()); }

        void normalize() requires IsVector { *this = static_cast<const Derived&>(*this) / magnitude(); }
        Derived normalized() const requires IsVector { return static_cast<const Derived&>(*this) / magnitude(); }
        
        static inline float Magnitude(const Derived& vector) requires IsVector { return vector.magnitude(); }
        static inline float Distance(const Derived& lhs, const Derived& rhs) requires IsVector { return (lhs - rhs).magnitude(); }
        static inline float SqrDistance(const Derived& lhs, const Derived& rhs) requires IsVector { return (lhs - rhs).sqr_magnitude(); }
        static inline float Dot(const Derived& lhs, const Derived& rhs) requires IsVector { return [&]<std::size_t... I>(std::index_sequence<I...>) { return ((lhs.data[I] * rhs.data[I]) + ...); } (std::make_index_sequence<S>{}); }
        static inline Derived Scale(const Derived& lhs, const Derived& rhs) requires IsVector { return [&]<std::size_t... I>(std::index_sequence<I...>) { return Derived { (lhs.data[I] * rhs.data[I])... }; } (std::make_index_sequence<S>{}); }
        static inline Derived Divide(const Derived& lhs, const Derived& rhs) requires IsVector { return [&]<std::size_t... I>(std::index_sequence<I...>) { return Derived { (lhs.data[I] / rhs.data[I])... }; } (std::make_index_sequence<S>{}); }
        
        static inline Derived Normalize(const Derived& vector) requires IsVector { return vector.normalized(); }

        static inline Derived MoveTowards(const Derived& lhs, const Derived& rhs, float maxDistanceDelta) requires IsVector { return lhs + (rhs - lhs).normalized() * maxDistanceDelta; }
        static inline Derived LerpUnclamped(const Derived& lhs, const Derived& rhs, float t) requires IsVector { return lhs + (rhs - lhs) * t; }
        static inline Derived Lerp(const Derived& lhs, const Derived& rhs, float t) requires IsVector { return LerpUnclamped(lhs, rhs, std::clamp(t, 0.0f, 1.0f)); }
        static inline Derived Reflect(const Derived& vector, const Derived& normal) requires IsVector { return vector - normal * 2.0f * Dot(vector, normal); }

        static inline Derived Max(const Derived& lhs, const Derived& rhs) requires IsVector { return [&]<std::size_t... I>(std::index_sequence<I...>) { return Derived { (std::max(lhs.data[I], rhs.data[I]))... }; } (std::make_index_sequence<S>{}); };
        static inline Derived Min(const Derived& lhs, const Derived& rhs) requires IsVector { return [&]<std::size_t... I>(std::index_sequence<I...>) { return Derived { (std::min(lhs.data[I], rhs.data[I]))... }; } (std::make_index_sequence<S>{}); };
        static inline Derived ClampMagnitude(const Derived& vector, float maxLength) requires IsVector { return [&]<std::size_t... I>(std::index_sequence<I...>) { return Derived { (std::clamp(vector.data[I], -maxLength, maxLength))... }; } (std::make_index_sequence<S>{}); }
        static inline Derived Clamp(const Derived& vector, const Derived& min, const Derived& max) requires IsVector { return [&]<std::size_t... I>(std::index_sequence<I...>) { return Derived { (std::clamp(vector.data[I], min.data[I], max.data[I]))... }; } (std::make_index_sequence<S>{}); }

        static inline Derived Project(const Derived& a, const Derived& n) requires IsVector { return n * (Dot(a, n) / Dot(n, n)); }
        static inline Derived ProjectNormalized(const Derived& a, const Derived& n) requires IsVector { return n * Dot(a, n); }
        static inline Derived ProjectOnPlane(const Derived& pointOnPlane, const Derived& planeNormal, const Derived& point) requires IsVector { 
            return point - Project(point - pointOnPlane, planeNormal); 
        }

        static inline Derived ProjectRayOnPlane(const Derived& rayOrigin, const Derived& rayDirection, const Derived& planeNormal, const Derived& planePoint) requires IsVector { return rayOrigin + rayDirection * (Dot(rayOrigin - planePoint, planeNormal) / Dot(rayDirection, planeNormal)); }

        static inline bool IsNaN(const Derived& vector) requires IsVector { return [&]<std::size_t... I>(std::index_sequence<I...>) { return ((std::isnan(vector.data[I])) || ...); } (std::make_index_sequence<S>{}); }
        static inline bool IsInfinity(const Derived& vector) requires IsVector { return [&]<std::size_t... I>(std::index_sequence<I...>) { return ((std::isinf(vector.data[I])) || ...); } (std::make_index_sequence<S>{}); }
        static inline bool IsFinite(const Derived& vector) requires IsVector { return [&]<std::size_t... I>(std::index_sequence<I...>) { return ((std::isfinite(vector.data[I])) && ...); } (std::make_index_sequence<S>{}); }
        
        // Ik this is not ops but who can stop me?
        inline std::string to_string() const requires IsVector 
        {
            // why did i have to put 50k () so i can do simple I < S - 1
            
            return [&]<std::size_t... I>(std::index_sequence<I...>) {
                if constexpr (IsVector)
                {
                    return "Vector" + std::to_string(S) + "(" + ((std::to_string(self().data[I]) + (I < S - 1 ? ", " : "")) + ...) + ")";
                }
                else
                {
                    return "(" + ((std::to_string(self().data[I]) + (I < S - 1 ? ", " : "")) + ...) + ")";
                }
            }(std::make_index_sequence<S>{});
        }
    };
}