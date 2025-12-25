#pragma once

#include <cstdint>
#include <memory>

using uint8  = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;
using int8   = std::int8_t;
using int16  = std::int16_t;
using int32  = std::int32_t;
using int64  = std::int64_t;

// TODO: Might change later
typedef std::size_t size_t;

// * std::shared_ptr wrapper
namespace Helios {

    template <typename T>
    using Unique = std::unique_ptr<T>;
    template <typename T, typename ... Args>
    constexpr Unique<T> CreateUnique(Args&& ... args) { return std::make_unique<T>(std::forward<Args>(args)...); }
    
// #define ENABLE_DEBUG_REF
#ifdef ENABLE_DEBUG_REF
    template<typename T>
    class DebugSharedPtr : public std::shared_ptr<T>
    {
    public:
        using std::shared_ptr<T>::shared_ptr;

        DebugSharedPtr() : std::shared_ptr<T>() { log("Default Constructed"); }
        DebugSharedPtr(std::nullptr_t) : std::shared_ptr<T>(nullptr) { log("Null Constructed"); }

        DebugSharedPtr(const std::shared_ptr<T>& p, const char* n = "<unknown>")
            : std::shared_ptr<T>(p), name(n)
        {
            log("Created");
        }

        DebugSharedPtr(const DebugSharedPtr& other) : std::shared_ptr<T>(other), name(other.name)
        {
            log("Copied");
        }

        DebugSharedPtr(DebugSharedPtr&& other) noexcept
            : std::shared_ptr<T>(std::move(other)), name(other.name)
        {
            log("Moved");
            other.name = "<moved-from>";
        }

        ~DebugSharedPtr() { log("Destroyed"); }

        DebugSharedPtr& operator=(const DebugSharedPtr& other)
        {
            std::shared_ptr<T>::operator=(other);
            name = other.name;
            log("Assigned");
            return *this;
        }

        DebugSharedPtr& operator=(DebugSharedPtr&& other) noexcept
        {
            std::shared_ptr<T>::operator=(std::move(other));
            name = other.name;
            other.name = "<moved-from>";
            log("Move Assigned");
            return *this;
        }

        template<typename U>
        DebugSharedPtr<U> static_pointer_cast() const
        {
            return DebugSharedPtr<U>(std::static_pointer_cast<U>(*this), name);
        }

        size_t use_count() const { return std::shared_ptr<T>::use_count(); }

    private:
        void log(const char* action) const
        {
            std::cout << "[DebugSharedPtr] " << action << " " << name
                    << ", count=" << use_count() << std::endl;
        }

        const char* name = "<unknown>";
    };

    template <typename T>
    using Ref = DebugSharedPtr<T>;

    template <typename T, typename ... Args>
    Ref<T> CreateRef(Args&& ... args)
    {
        auto sp = std::make_shared<T>(std::forward<Args>(args)...);
        return Ref<T>(sp, typeid(T).name());
    }


#else
    
    template <typename T>
    using Ref = std::shared_ptr<T>;
    template <typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args) { return std::make_shared<T>(std::forward<Args>(args)...); }

#endif

    template <typename T>
    using WeakRef = std::weak_ptr<T>;
}
#ifdef ENABLE_DEBUG_REF
namespace std
{
    // template<typename U, typename T>
    // Helios::DebugRef<U> static_pointer_cast(const Helios::DebugRef<T>& ref)
    // {
    //     return Helios::DebugRef<U>(std::static_pointer_cast<U>(ref.getShared()), typeid(U).name());
    // }
}
#endif