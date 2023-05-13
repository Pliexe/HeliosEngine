#include "Material.h"

#include "Helios/Core/Asserts.h"
#include "Helios/Graphics/Graphics.h"

#include "Platform/Direct3D11/D3D11Material.h"

namespace Helios
{
	Ref<Material> Helios::Material::Create(Filter filter, Type type)
	{
		switch (Graphics::GetAPI())
		{
		case Graphics::API::Direct3D11:
			return CreateRef<D3D11Material>(filter, type);
		case Graphics::API::OpenGL:
			HELIOS_ASSERT(false, "OpenGL is not supported yet!");
			return nullptr;
		default:
			HELIOS_ASSERT(false, "Unknown DepricatedGraphics API!");
			return nullptr;
		}
	}
}
