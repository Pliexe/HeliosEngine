#include "Graphics.h"
#include "GPURenderPass.h"
#include "Helios/Core/Exceptions.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"
#include <memory>

namespace Helios
{
//     Ref<GPURenderPass> GPURenderPass::Create(GPURenderPassSpecification initSpecs)
//     {
//         switch (Graphics::GetAPI())
//         {
// #ifdef HELIOS_PLATFORM_WINDOWS
//         // case Graphics::API::Direct3D11: 
// #endif // HELIOS_PLATFORM_WINDOWS
//         case Graphics::API::Vulkan: return CreateRef<Internal::VkRenderPass>(std::static_pointer_cast<Internal::VkGraphicsDevice>(Graphics::GetMainDevice()), initSpecs);
//         default: break;
//         }

//         HL_EXCEPTION(true, "No Graphics API selected!");
//         return nullptr;
//     }
}