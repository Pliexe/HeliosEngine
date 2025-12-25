#pragma once

#include "Helios/Graphics/CommandList.h"
#include "Helios/Graphics/RenderGraph/RenderResources.h"
#include "Helios/Math/Matrix.h"
#include "Helios/Resources/Image.h"
#include "Helios/Resources/Mesh.h"
#include "Helios/Resources/Material.h"

namespace Helios
{
    struct RenderObject
    {
        Matrix4x4 transform = Matrix4x4::Identity();
        Ref<Mesh> mesh;
        Ref<MaterialInstance> material;
        uint64 layerMask = 0;
    };
    
    struct RenderView
    {
        enum class Family
        {
            Unknown,
            Scene,
            ShadowMap,
        };

        Size size;
        Family family = Family::Unknown;
        Matrix4x4 viewTransform = Matrix4x4::Identity();
        uint64 layerMask = 0;
        uint64 flag = 0;
    };

    struct ViewCulling
    {
        RenderView view;
        std::vector<RenderObject*> objects;
    };

    class RenderContext
    {
    public:
        std::vector<ViewCulling> views;
        RenderResources& resources;

        RenderContext() = delete;
        RenderContext(RenderResources& resources, std::vector<ViewCulling>& views)
            : resources(resources), views(views) {}

        template <typename T>
        T& Get(std::string_view name)
        {
            return GetStorage<T>()[std::string(name)];
        }

        template <typename T>
        bool Has(std::string_view name) const
        {
            return HasStorage<T>() && GetStorage<T>().contains(std::string(name));
        }

        template <typename T>
        bool HasList(std::string_view name) const
        {
            return Has<std::vector<T>>(name);
        }

        template <typename T>
        std::vector<T>& GetList(std::string_view name)
        {
            return Get<std::vector<T>>(name);
        }

        template <typename T>
        void AddToList(std::string_view name, const T& item)
        {
            GetList<T>(name).push_back(item);
        }

    private:
        template <typename T>
        using StorageMap = std::unordered_map<std::string, T>;

        template <typename T>
        StorageMap<T>& GetStorage()
        {
            const size_t key = typeid(T).hash_code();

            auto it = m_allStorage.find(key);
            if (it == m_allStorage.end())
            {
                auto [newIt, _] = m_allStorage.emplace(
                    key,
                    std::make_any<StorageMap<T>>()
                );
                it = newIt;
            }

            return std::any_cast<StorageMap<T>&>(it->second);
        }

        template <typename T>
        const StorageMap<T>& GetStorage() const
        {
            const size_t key = typeid(T).hash_code();
            auto it = m_allStorage.find(key);
            if (it == m_allStorage.end())
            {
                static const StorageMap<T> empty;
                return empty;
            }
            return std::any_cast<const StorageMap<T>&>(it->second);
        }

        template <typename T>
        bool HasStorage() const
        {
            return m_allStorage.contains(typeid(T).hash_code());
        }

        std::unordered_map<size_t, std::any> m_allStorage;
    };
}