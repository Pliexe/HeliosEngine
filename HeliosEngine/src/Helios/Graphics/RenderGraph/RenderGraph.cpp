#include "RenderGraph.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Graphics/RenderGraph/RenderContext.h"
#include "Helios/Scene/Components/Renderers.h"
#include "Helios/Scene/SceneRegistry.h"
#include "Helios/Core/Profiler.h"

using namespace Helios;

RenderGraph::RenderGraph(Ref<GraphicsDevice> device)
{
    m_device = device;
    m_resources = CreateRef<RenderResources>(device);
}

void RenderGraph::Execute()
{
    HL_PROFILE_BEGIN("RenderGraph::Execute");

    std::vector<ViewCulling> viewCullings;

    HL_PROFILE_BEGIN("RenderGraph::Execute - Sort");

    std::sort(m_objects.begin(), m_objects.end(), [](const RenderObject& a, const RenderObject& b)
    {
        if (a.material.get() != b.material.get())
            return a.material.get() < b.material.get();
        return a.mesh.get() < b.mesh.get();
    });

    HL_PROFILE_END();

    HL_PROFILE_BEGIN("RenderGraph::Execute - Cull");

    // TODO: Cull views

    for (auto& view : m_views)
    {
        viewCullings.push_back({
            .view = view,
            .objects = {},
        });

        auto& viewCulling = viewCullings.back();

        for (auto& object : m_objects)
        {
            // TODO: Cull objects
            if ((object.layerMask & view.layerMask) == view.layerMask)
            {     
                viewCulling.objects.push_back(&object);
            }
        }
    }

    HL_PROFILE_END();

    HL_PROFILE_BEGIN("RenderGraph::Execute - Render");

    RenderContext context(*m_resources, viewCullings);

    for (auto& pass : m_renderPasses)
    {
        HL_PROFILE_BEGIN("RenderGraph::Execute - (Pass) - " + pass.name);

        pass.callback(context);

        HL_PROFILE_END();
    }

    HL_PROFILE_END();

    HL_PROFILE_BEGIN("RenderGraph::Execute - Cleanup");

    m_resources->Step();
    m_views.clear();
    m_objects.clear();

    HL_PROFILE_END();

    HL_PROFILE_END();
}