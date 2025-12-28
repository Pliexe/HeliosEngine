/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/HeliosEngine/blob/master/HeliosEngine/LICENSE.txt
 */
#pragma once

#include "Helios/Math/Vector.h"
#include "pch.h"
#include "Helios/Core/Base.h"
#include "Helios/Core/Asserts.h"
#include "Helios/Math/Quaternion.h"
#include "Components.h"
#include "Helios/Scene/Scene.h"
#include "Helios/Scene/SceneRegistry.h"
#include "EntityContainer.h"
#include "Helios/Scripting/Behaviour.h"

namespace Helios
{
	class EntityContainer;

	class Camera;
	class DepricatedGraphics;

	class HELIOS_API Entity
	{
	public:
		// GameObject() = default;
		Entity() {}
		Entity(const Entity &other) : m_container(other.m_container), m_entityHandle(other.m_entityHandle) {};
		Entity(entt::entity entity_id, WeakRef<EntityContainer> container)
		{
			// HL_ASSERT(entity_id != entt::null, "entity handle is invalid!");
			m_container = container;
			m_entityHandle = entity_id;
		}

		template <typename... T>
		bool HasComponent()
		{
			HL_ASSERT(m_entityHandle != entt::null, "entity handle is invalid!");
			HL_ASSERT(!m_container.expired(), "Scene does not exist anymore!");
			return m_container.lock()->m_components.template any_of<T...>(m_entityHandle);
		}

		template <typename T, typename... Args>
		T &AddScopedComponent(Args &&...args)
		{
			HL_ASSERT(!HasComponent<T>(), "GameObject arleady has this component!");
			HL_ASSERT(!m_container.expired(), "Scene does not exist anymore!");
			T &comp = m_container.lock()->m_components.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
			return comp;
		}

		template <typename T, typename... Args>
		T &AddComponent(Args &&...args)
		{
			T &comp = m_container.lock()->m_components.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
			return comp;
		}

		template <typename T, typename... Args>
		T &AddOrReplaceComponent(Args &&...args)
		{
			T &comp = m_container.lock()->m_components.emplace_or_replace<T>(m_entityHandle, std::forward<Args>(args)...);
			return comp;
		}

		template <typename T>
		void RemoveComponent()
		{
			HL_ASSERT(HasComponent<T>(), "GameObject does not have the component!");
			// HL_ASSERT(!m_scene.expired(), "Scene does not exist anymore!");
			m_container.lock()->m_components.remove<T>(m_entityHandle);
		}

		template <typename... T>
		decltype(auto) GetComponent()
		{
			HL_ASSERT(HasComponent<T...>(), "GameObject does not have the component!");
			// HL_ASSERT(!m_scene.expired(), "Scene does not exist anymore!");
			return m_container.lock()->m_components.template get<T...>(m_entityHandle);
		}

		operator entt::entity() const { return m_entityHandle; }
		bool operator==(const Entity &other) const { return m_entityHandle == other.m_entityHandle && m_container.lock() == other.m_container.lock(); }
		// bool operator==(const Entity &other) const {
		// 	if (m_entityHandle != other.m_entityHandle) return false;

		// 	auto this_container = m_container.lock();
		// 	auto other_container = other.m_container.lock();

		// 	if (!this_container || !other_container)
		// 		return false;

		// 	return this_container == other_container;
		// }
		bool operator==(const entt::entity &other) const { return m_entityHandle == other; }
		bool operator==(const WeakRef<EntityContainer> other) const { return m_container.lock() == other.lock(); }

		inline bool isGlobal() { return HasComponent<GlobalObjectComponent>(); }
		void MakeGlobal();
		void Destroy();
		inline bool IsValid()
		{
			if (m_container.expired())
				return false;
			return m_entityHandle != entt::null && m_container.lock()->m_components.valid(m_entityHandle);
		}
		inline bool IsNotNull() { return m_entityHandle != entt::null; }
		inline bool IsNull() { return m_entityHandle == entt::null; }

		WeakRef<EntityContainer> &GetContainer() { return m_container; }
		const WeakRef<EntityContainer> &GetContainer() const { return m_container; }
		entt::entity GetHandle() { return m_entityHandle; }
		entt::entity GetHandle() const { return m_entityHandle; }
		UUID GetUUID() { return GetComponent<UUIDComponent>().uuid; }

#pragma region Scripting

		bool HasBehaviours(std::string_view service_name);

		bool AddBehaviour(std::string_view service_name, std::string_view behaviour_name, Scripting::Behaviour *outHandle = nullptr);

		bool RemoveBehaviour(std::string_view service_name, const Scripting::Behaviour& handle);

		std::pair<std::vector<Scripting::Behaviour>::const_iterator, std::vector<Scripting::Behaviour>::const_iterator> GetBehavioursIter(std::string_view service_name);
		const std::vector<Scripting::Behaviour> GetBehaviours(std::string_view service_name) const;

#pragma endregion

	private:
		WeakRef<EntityContainer> m_container;
		entt::entity m_entityHandle{entt::null};

	public:
#pragma region GameObject Creation Helpers

		Entity &CreateMainCamera(Vector2 position = Vector2(0.0f, 0.0f));
		Entity &CreateCamera(Vector2 position = Vector2(0.0f, 0.0f));

#pragma endregion

		void ResetParent();
		void SetParent(Entity &object);
		void SetParent(entt::entity &object);

		inline std::string &GetName()
		{
			return GetComponent<InfoComponent>().name;
		}

		inline Transform2DComponent GetTransform()
		{
			return GetComponent<Transform2DComponent>();
		}

		Entity InstantiateObject();
		Entity InstantiateObject(std::string name);
		Entity InstantiateObject(Entity &parent);
		Entity InstantiateObject(std::string name, Entity &parent);

		// Serbia - Novi Sad has the worst healthcare personal that doesn't respect the laws and all and kill people by their actions

		friend struct Transform2DComponent;

		friend class DepricatedApplication;
		friend class SceneManager;
		friend class Scene;
		friend void SerializeObject(YAML::Emitter &out, Entity &o);
		friend class Transform;

		friend class EntityContainer;
	};

	class Transform
	{
	public:
		Transform() = delete;
		Transform(const Transform &other) = default;
		Transform(Entity &gameObject) : m_GameObject(gameObject), m_transform(gameObject.GetComponent<TransformComponent>()), m_relationship(gameObject.GetComponent<RelationshipComponent>()) {}
		Transform(entt::entity entity, WeakRef<EntityContainer> container) : m_GameObject(Entity(entity, container)), m_transform(m_GameObject.GetComponent<TransformComponent>()), m_relationship(m_GameObject.GetComponent<RelationshipComponent>()) {}
		Transform(entt::entity entity, TransformComponent transform_component, RelationshipComponent relationship_component, WeakRef<EntityContainer> container) : m_GameObject(Entity(entity, container)), m_transform(transform_component), m_relationship(relationship_component) {}

		inline TransformComponent GetWorldTransform() { return m_transform.GetWorldTransform(m_relationship, m_GameObject.m_container.lock()->m_components); }

		inline TransformComponent GetWorldTransformCache()
		{
			if (m_relationship.HasParent())
			{
				if (m_worldTransformCache.contains(m_GameObject.m_entityHandle))
					return m_worldTransformCache[m_GameObject.m_entityHandle];

				TransformComponent worldTransform = m_transform;
				TransformComponent parentTransform = (m_worldTransformCache.contains(m_relationship.parent_handle) ? m_worldTransformCache[m_relationship.parent_handle] : Transform(m_relationship.parent_handle, m_GameObject.m_container).GetWorldTransformCache());

				worldTransform.Position = parentTransform.Rotation * worldTransform.Position + parentTransform.Position;
				worldTransform.Rotation = parentTransform.Rotation * worldTransform.Rotation;
				worldTransform.Scale = FVector3::Scale(parentTransform.Scale, worldTransform.Scale);

				m_worldTransformCache[m_GameObject.m_entityHandle] = worldTransform;
				return worldTransform;
			}

			if (m_relationship.HasChild())
				m_worldTransformCache[m_GameObject.m_entityHandle] = m_transform;
			return m_transform;
		}

		inline Vector3 GetWorldPositionCache()
		{
			return GetWorldTransformCache().Position;
		}

		inline Quaternion GetWorldRotationCache()
		{
			return GetWorldTransformCache().Rotation;
		}

		inline Vector3 GetWorldScaleCache()
		{
			return GetWorldTransformCache().Scale;
		}

		inline Matrix4x4 GetModelMatrix()
		{
			return m_transform.GetModelMatrix();
		}

		inline Matrix4x4 GetModelRowMatrix()
		{
			return m_transform.GetRowModelMatrix();
		}

		inline Matrix4x4 GetWorldMatrix()
		{
			return GetWorldTransformCache().GetModelMatrix();
		}

		inline Matrix4x4 GetWorldRowMatrix()
		{
			return GetWorldTransformCache().GetRowModelMatrix();
		}

		inline Vector3 GetWorldPosition() { return m_transform.GetWorldPosition(m_relationship, m_GameObject.m_container.lock()->m_components); }
		inline Quaternion GetWorldRotation() { return m_transform.GetWorldRotation(m_relationship, m_GameObject.m_container.lock()->m_components); }
		inline Vector3 GetWorldScale() { return m_transform.GetWorldScale(m_relationship, m_GameObject.m_container.lock()->m_components); }

		inline Vector3 GetLocalPosition() { return m_transform.Position; }
		inline Quaternion GetLocalRotation() { return m_transform.Rotation; }
		inline Vector3 GetLocalScale() { return m_transform.Scale; }

		inline Entity GetGameObject() { return m_GameObject; }
		inline TransformComponent &GetTransformComponent() { return m_transform; }

		inline void SetWorldPosition(Vector3 position) { m_transform.SetWorldPosition(position, m_relationship, m_GameObject.m_container.lock()->m_components); }
		inline void SetWorldRotation(Quaternion rotation) { m_transform.SetWorldRotation(rotation, m_relationship, m_GameObject.m_container.lock()->m_components); }
		inline void SetWorldScale(Vector3 scale) { m_transform.SetWorldScale(scale, m_relationship, m_GameObject.m_container.lock()->m_components); }
		inline void SetWorldTransform(Vector3 position, Quaternion rotation, Vector3 scale) { m_transform.SetWorldTransform({position, rotation, scale}, m_relationship, m_GameObject.m_container.lock()->m_components); }

		inline void SetLocalPosition(Vector3 position) { m_transform.SetLocalPosition(position); }
		inline void SetLocalRotation(Quaternion rotation) { m_transform.SetLocalRotation(rotation); }
		inline void SetLocalScale(Vector3 scale) { m_transform.SetLocalScale(scale); }
		inline void SetLocalTransform(Vector3 position, Quaternion rotation, Vector3 scale) { m_transform.SetLocalTransform({position, rotation, scale}); }

		inline void Translate(Vector3 translation) { m_transform.Translate(translation); }

		inline void Rotate(Quaternion rotation) { m_transform.Rotate(rotation); }
		inline void Rotate(Vector3 euler) { m_transform.Rotate(euler); }

		inline Vector3 Forward() { return m_transform.Forward(); }
		inline Vector3 Right() { return m_transform.Right(); }
		void RotateAround(const Vector3 &target, const Vector3 &axis, float angle) { return m_transform.RotateAround(target, axis, angle); }

	private:
		Entity m_GameObject;
		TransformComponent m_transform;
		RelationshipComponent m_relationship;

		inline static std::unordered_map<entt::entity, TransformComponent> m_worldTransformCache;
		friend class Scene;
	};
}

namespace std
{
	template <>
	struct hash<Helios::Entity>
	{
		size_t operator()(const Helios::Entity &entity) const
		{
			auto containerPtr = entity.GetContainer().lock();
			size_t containerHash = std::hash<std::shared_ptr<Helios::EntityContainer>>{}(containerPtr);
			return (static_cast<size_t>(entity.GetHandle()) ^ (containerHash + 0x9e3779b9 + (static_cast<size_t>(entity.GetHandle()) << 6) + (static_cast<size_t>(entity.GetHandle()) >> 2)));
		}
	};
}
