#include "Material.h"

#include "Helios/Core/Asserts.h"
#include "Helios/Graphics/Graphics.h"

#include "Platform/DirectX/DirectXMaterial.h"

namespace Helios
{
	Ref<Material> Helios::Material::Create(Filter filter, Type type)
	{
		switch (Graphics::GetAPI())
		{
		case Graphics::API::DirectX:
			return CreateRef<DirectXMaterial>(filter, type);
		case Graphics::API::OpenGL:
			HL_CORE_ASSERT(false, "OpenGL is not supported yet!");
			return nullptr;
		default:
			HL_CORE_ASSERT(false, "Unknown Graphics API!");
			return nullptr;
		}
	}
}
