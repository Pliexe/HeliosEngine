#pragma once

#include "Components/Basic.h"
#include "Components/Relationship.h"
#include "Components/Lighting.h"
#include "Components/Physics2D.h"
#include "Components/Renderers.h"
#include "Components/Transforms.h"

//https://en.cppreference.com/w/cpp/language/parameter_pack
template <class... Types>
struct pack {};

namespace Helios
{
	template <typename... Component>
	struct ComponentGroup;
	using AllComponents = ComponentGroup<
		//Basic
		InfoComponent,
		CameraComponent,
		//Relationship
		RelationshipComponent,
		//Lighting
		DirectionalLightComponent,
		//Renderers
		SpriteRendererComponent,
		MeshRendererComponent,
		//Transforms
		TransformComponent,
		Transform2DComponent,
		// Physics2D
		Rigidbody2D,
		CircleCollider2D
	>;
}