#include "SceneSerializer.h"
#include "Helios/Core/Application.h"
#include "Helios/Core/Asserts.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Resources/ResourceRegistry.h"
#include "Helios/Utils/YamlExtensions.h"

#include "Entity.h"


namespace Helios
{

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
	{
		m_scene = scene;
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		HL_ASSERT(entity.HasComponent<UUIDComponent>(), "Entity has no uuid component!");
		HL_ASSERT(entity.HasComponent<InfoComponent>(), "Entity has no tag component!");

		auto container = entity.GetContainer().lock();

		if (!container) HL_ASSERT(false, "Entity has no container!");

		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID().toString();

		if (entity.HasComponent<InfoComponent>())
		{
			out << YAML::Key << "Info" << YAML::Value << YAML::BeginMap;

			auto& name = entity.GetComponent<InfoComponent>().name;
			out << YAML::Key << "Name" << YAML::Value << name;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Transform" << YAML::Value << YAML::BeginMap;

			out << YAML::Key << "Position" << YAML::Value << transform.Position;
			out << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << transform.Scale;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			auto& camera = entity.GetComponent<CameraComponent>();
			out << YAML::Key << "Camera" << YAML::Value << YAML::BeginMap;

			out << YAML::Key << "Primary" << YAML::Value << camera.isPrimary;
			out << YAML::Key << "Size" << YAML::Value << camera.size;
			out << YAML::Key << "ClearColor" << YAML::Value << camera.clear_color;
			out << YAML::Key << "IsOrthographic" << YAML::Value << camera.ortographic;
			out << YAML::Key << "Near" << YAML::Value << camera.near_z;
			out << YAML::Key << "Far" << YAML::Value << camera.far_z;
			out << YAML::Key << "FOV" << YAML::Value << camera.fov;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<RelationshipComponent>())
		{
			auto& relationship = entity.GetComponent<RelationshipComponent>();
			out << YAML::Key << "Relationship" << YAML::Value << YAML::BeginMap;

			out << YAML::Key << "Parent" << YAML::Value << container->GetEntity(relationship.parent_handle);
			out << YAML::Key << "Top" << YAML::Value << container->GetEntity(relationship.top_handle);

			out << YAML::Key << "Siblings" << YAML::Value << YAML::BeginMap;
			
			out << YAML::Key << "First" << YAML::Value << container->GetEntity(relationship.first_child);
			out << YAML::Key << "Next" << YAML::Value << container->GetEntity(relationship.next_sibling);
			out << YAML::Key << "Previous" << YAML::Value << container->GetEntity(relationship.prev_sibling);

			out << YAML::EndMap;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<DirectionalLightComponent>())
		{
			auto& light = entity.GetComponent<DirectionalLightComponent>();
			out << YAML::Key << "DirectionalLight" << YAML::Value << YAML::BeginMap;

			out << YAML::Key << "Color" << YAML::Value << light.color;
			out << YAML::Key << "Intensity" << YAML::Value << light.intensity;
			
			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			auto& sprite = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "SpriteRenderer" << YAML::Value << YAML::BeginMap;

			out << YAML::Key << "Color" << YAML::Value << sprite.color;
			//out << YAML::Key << "Texture" << YAML::Value << sprite.texture;
			//out << YAML::Key << "TilingFactor" << YAML::Value << sprite.tiling_factor;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<MeshRendererComponent>())
		{
			auto& mesh = entity.GetComponent<MeshRendererComponent>();
			out << YAML::Key << "MeshRenderer" << YAML::Value << YAML::BeginMap;
			out << YAML::Key << "Color" << YAML::Value << mesh.material->Color;
			out << YAML::Key << "Mesh" << YAML::Value << mesh.mesh->GetID();
			//if (mesh.material->texture != nullptr) out << YAML::Key << "Material" << YAML::Value << mesh.material->texture->
			//out << YAML::Key << "Material" << YAML::Value << mesh.material;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<Rigidbody2D>())
		{
			auto& rigidbody = entity.GetComponent<Rigidbody2D>();
			out << YAML::Key << "Rigidbody2D" << YAML::Value << YAML::BeginMap;

			out << YAML::Key << "BodyType" << YAML::Value << (int)rigidbody.type;

			out << YAML::Key << "IsKinematic" << YAML::Value << rigidbody.isKinematic;
			out << YAML::Key << "UseGravity" << YAML::Value << rigidbody.useGravity;
			out << YAML::Key << "Mass" << YAML::Value << rigidbody.mass;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<BoxCollider2D>())
		{
			auto& collider = entity.GetComponent<BoxCollider2D>();
			out << YAML::Key << "BoxCollider2D" << YAML::Value << YAML::BeginMap;
			out << YAML::Key << "Offset" << YAML::Value << collider.offset;
			out << YAML::Key << "Size" << YAML::Value << collider.size;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<CircleCollider2D>())
		{
			auto& collider = entity.GetComponent<CircleCollider2D>();
			out << YAML::Key << "CircleCollider2D" << YAML::Value << YAML::BeginMap;
			out << YAML::Key << "Offset" << YAML::Value << collider.offset;
			out << YAML::Key << "Radius" << YAML::Value << collider.radius;
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::SerializeEditor(const std::string& filepath) const
	{
		YAML::Emitter out;

		out << YAML::BeginMap;

		out << YAML::Key << "SceneName" << YAML::Value << m_scene->GetName();

		// -----------------------------

		out << YAML::Key << "Lighting" << YAML::Value << YAML::BeginMap;

		out << YAML::Key << "Ambient" << YAML::Value << "0,0,0,0";

		out << YAML::EndMap;

		// -----------------------------

		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		m_scene->m_components.each([&](auto entityID)
		{
			Entity entity = { entityID, m_scene };
			SerializeEntity(out, entity);
		});

		out << YAML::EndSeq;

		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void DeserializeEntity(YAML::Node entityNode, Scene* scene)
	{
		Entity entity = scene->CreateEntity();

		UUID uuid = entityNode["Entity"].as<UUID>();

		entity.AddOrReplaceComponent<UUIDComponent>().uuid = uuid;
		scene->m_UUIDMap[uuid] = entity;

		if (entityNode["Info"])
		{
			auto& tagComponent = entity.AddOrReplaceComponent<InfoComponent>();
			tagComponent.name = entityNode["Info"]["Name"].as<std::string>();
		}
		if (entityNode["Transform"])
		{
			auto& transformComponent = entity.AddOrReplaceComponent<TransformComponent>();
			auto transformNode = entityNode["Transform"];
			transformComponent.Position = transformNode["Position"].as<Vector3>();
			transformComponent.Rotation = transformNode["Rotation"].as<Quaternion>();
			transformComponent.Scale = transformNode["Scale"].as<Vector3>();
		}
		if (entityNode["Camera"])
		{
			auto& cameraComponent = entity.AddScopedComponent<CameraComponent>();
			auto cameraNode = entityNode["Camera"];
			cameraComponent.isPrimary = cameraNode["Primary"].as<bool>();
			cameraComponent.size = cameraNode["Size"].as<float>();
			cameraComponent.clear_color = cameraNode["ClearColor"].as<Color>();
			cameraComponent.ortographic = cameraNode["IsOrthographic"].as<bool>();
			cameraComponent.near_z = cameraNode["Near"].as<float>();
			cameraComponent.far_z = cameraNode["Far"].as<float>();
			cameraComponent.fov = cameraNode["FOV"].as<float>();
		}
		if (entityNode["DirectionalLight"])
		{
			auto& lightComponent = entity.AddScopedComponent<DirectionalLightComponent>();
			auto lightNode = entityNode["DirectionalLight"];
			lightComponent.color = lightNode["Color"].as<Color>();
			lightComponent.intensity = lightNode["Intensity"].as<float>();
		}
		if (entityNode["SpriteRenderer"])
		{
			auto& spriteComponent = entity.AddScopedComponent<SpriteRendererComponent>();
			auto spriteNode = entityNode["SpriteRenderer"];
			spriteComponent.color = spriteNode["Color"].as<Color>();
			//spriteComponent.texture = spriteNode["Texture"].as<Texture2D>();
		}
		if (entityNode["MeshRenderer"])
		{
			auto& meshComponent = entity.AddScopedComponent<MeshRendererComponent>();
			auto meshNode = entityNode["MeshRenderer"];
			meshComponent.material = Material::Create(Material::Filter::MinMagMipPoint, Material::Type::Clamp);
			//meshComponent.material = meshNode["Material"].as<Material>();
			meshComponent.material->Color = meshNode["Color"].as<Color>();
			if (meshNode["Texture"]) meshComponent.material->texture = ResourceRegistry::GetResource<Texture2D>(meshNode["Texture"].as<UUID>());
			//meshComponent.mesh = meshNode["Mesh"].as<Mesh>();
			meshComponent.mesh = ResourceRegistry::GetResource<Mesh>(meshNode["Mesh"].as<Helios::UUID>());
		}
		if (entityNode["Rigidbody2D"])
		{
			auto& rigidbodyComponent = entity.AddScopedComponent<Rigidbody2D>();
			auto rigidbodyNode = entityNode["Rigidbody2D"];

			rigidbodyComponent.type = (BodyType)rigidbodyNode["BodyType"].as<int>();

			rigidbodyComponent.isKinematic = rigidbodyNode["IsKinematic"].as<bool>();
			rigidbodyComponent.useGravity = rigidbodyNode["UseGravity"].as<bool>();
			rigidbodyComponent.mass = rigidbodyNode["Mass"].as<float>();
		}
		if (entityNode["BoxCollider2D"])
		{
			auto& colliderComponent = entity.AddScopedComponent<BoxCollider2D>();
			auto colliderNode = entityNode["BoxCollider2D"];
			colliderComponent.offset = colliderNode["Offset"].as<Vector2>();
			colliderComponent.size = colliderNode["Size"].as<Vector2>();
		}
		if (entityNode["CircleCollider2D"])
		{
			auto& colliderComponent = entity.AddScopedComponent<CircleCollider2D>();
			auto colliderNode = entityNode["CircleCollider2D"];
			colliderComponent.offset = colliderNode["Offset"].as<Vector2>();
			colliderComponent.radius = colliderNode["Radius"].as<float>();
		}
	}

	void CheckRelationship(YAML::Node entityNode, Scene* scene)
	{
		if (entityNode["Relationship"])
		{
			Entity entity = scene->GetEntity(entityNode["Entity"].as<UUID>());

			auto relationshipNode = entityNode["Relationship"];

			if (relationshipNode["Parent"] && relationshipNode["Parent"].as<std::string>() != "0") // 0 means no uuid (null)
			{
				UUID parentUUID = relationshipNode["Parent"].as<UUID>();
				Entity parentEntity = scene->GetEntity(parentUUID);
				entity.GetComponent<RelationshipComponent>().parent_handle = parentEntity.GetHandle();
			}

			if (relationshipNode["Top"] && relationshipNode["Top"].as<std::string>() != "0")
			{
				UUID topUUID = relationshipNode["Top"].as<UUID>();
				Entity topEntity = scene->GetEntity(topUUID);
				entity.GetComponent<RelationshipComponent>().top_handle = topEntity.GetHandle();
			}

			if (relationshipNode["Siblings"] && relationshipNode["Siblings"].IsMap())
			{
				auto siblings = relationshipNode["Siblings"];
				if (siblings["First"] && siblings["First"].as<std::string>() != "0")
				{
					UUID firstUUID = siblings["First"].as<UUID>();
					Entity firstEntity = scene->GetEntity(firstUUID);
					entity.GetComponent<RelationshipComponent>().first_child = firstEntity.GetHandle();
				}
				if (siblings["Next"] && siblings["Next"].as<std::string>() != "0")
				{
					UUID nextUUID = siblings["Next"].as<UUID>();
					Entity nextEntity = scene->GetEntity(nextUUID);
					entity.GetComponent<RelationshipComponent>().next_sibling = nextEntity.GetHandle();
				}
				if (siblings["Previous"] && siblings["Previous"].as<std::string>() != "0")
				{
					UUID previousUUID = siblings["Previous"].as<UUID>();
					Entity previousEntity = scene->GetEntity(previousUUID);
					entity.GetComponent<RelationshipComponent>().prev_sibling = previousEntity.GetHandle();
				}
			}
		}
	}

	bool SceneSerializer::DeserializeEditor(const std::string& filepath) const
	{
		YAML::Node data;

		try
		{
			std::ifstream file(conversions::from_utf8_to_utf16(filepath));
			std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			data = YAML::Load(str);
		}
		catch (const std::exception& e)
		{
			HL_EXCEPTION_RETRY(false, "Failed to load scene file: " + filepath);
			return false;
		}

		if (!data["SceneName"])
			return false;

		std::string sceneName = data["SceneName"].as<std::string>();

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				DeserializeEntity(entity, m_scene.get());
			}

			for (auto entity : entities)
			{
				CheckRelationship(entity, m_scene.get());
			}
		}

		return true;
	}
}
