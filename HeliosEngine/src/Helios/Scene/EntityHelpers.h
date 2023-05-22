#pragma once
#include "Entity.h"

namespace Helios
{
	template <typename... Component>
	struct ComponentGroup {};

	template <typename... Component>
	static void  CopyComponentIfExists(ComponentGroup<Component...>, Entity destination, Entity origin)
	{
		CopyComponentIfExists<Component...>(destination, origin);
	}

	template <typename... Component>
	static void CopyComponentIfExists(Entity destination, Entity origin)
	{
		auto x = [&] <typename T> {
			if (origin.HasComponent<T>())
				destination.AddOrReplaceComponent<T>(origin.GetComponent<T>());
		};

		(x.template operator()<Component>(), ...);
	}

}
