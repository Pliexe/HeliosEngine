#include "Material.h"

#include "Helios/Core/Asserts.h"
#include "Helios/Graphics/Graphics.h"

namespace Helios
{
	Ref<MaterialInstance> MaterialInstance::Create(Filter filter, Type type)
	{
		return CreateRef<MaterialInstance>();
	}
}
