#pragma once

#include "Helios/Graphics/RenderGraph/RenderPass.h"
#include "Helios/Graphics/RenderGraph/RenderResources.h"

namespace Helios
{        
    class RenderGraph
    {
    public:

        RenderGraph() = delete;
        RenderGraph(const RenderGraph&) = delete;
        RenderGraph& operator=(const RenderGraph&) = delete;

        RenderGraph(Ref<GraphicsDevice> device);

        

        // void Compile();
        void Execute(); // Clears views

        bool HasPass(const std::string& name)
        {
            return m_passNames.contains(name);
        }

        void AddPass(const RenderPass& pass)
        {
            HL_EXCEPTION(m_passNames.contains(pass.name), "Pass with name {} already exists", pass.name);
            m_passNames.insert(pass.name);
            m_renderPasses.push_back(pass);
        }

        void AddPass(const std::string& name, const RenderPassCallback& callback)
        {
            HL_EXCEPTION(name.empty(), "Pass name cannot be empty");
            HL_EXCEPTION(m_passNames.contains(name), "Pass with name {} already exists", name);

            m_passNames.insert(name);
            m_renderPasses.push_back({ name, callback });
        }

        void RemovePass(const std::string& name)
        {
            HL_EXCEPTION(name.empty(), "Pass name cannot be empty");
            HL_EXCEPTION(!m_passNames.contains(name), "Pass with name {} does not exist", name);

            m_passNames.erase(name);
            m_renderPasses.erase(std::remove_if(m_renderPasses.begin(), m_renderPasses.end(), [name](const RenderPass& pass) { return pass.name == name; }));
        }

        void PushView(const RenderView& view)
        {
            if (view.size.width <= 0 || view.size.height <= 0)
            {
                std::cout << "Warning: View has invalid size! Skipping view." << std::endl;
                return;
            }
            
            m_views.push_back(view);
        }

        // from 21ms to 13ms with std::move
        void PushObject(RenderObject&& object)
        {
            m_objects.emplace_back(std::move(object));
        }

    private:
        std::set<std::string> m_passNames;
        std::vector<RenderPass> m_renderPasses;


        std::vector<RenderView> m_views;
        std::vector<RenderObject> m_objects;

        Ref<GraphicsDevice> m_device;
        Ref<RenderResources> m_resources;
    };
}