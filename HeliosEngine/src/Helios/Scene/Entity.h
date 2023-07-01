/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"
#include "Helios/Core/Asserts.h"
#include "Helios/Translation/Quaternion.h"
#include "Components.h"
#include "Helios/Scene/Scene.h"
#include "Helios/Scene/SceneRegistry.h"

namespace Helios {

	
	class Camera;
	class DepricatedGraphics;

	class HELIOS_API Entity
	{
	public:

		//GameObject() = default;
		Entity() { }
		Entity(const Entity& other) = default;
		Entity(entt::entity entity_id, Ref<Scene> scene)
		{
			m_scene = scene.get();
			m_entityHandle = entity_id;
		}
		Entity(entt::entity entity_id, Scene* scene)
		{
			HL_CORE_ASSERT_WITH_MSG(entity_id != entt::null, "entity handle is invalid!");
			m_scene = scene;
			m_entityHandle = entity_id;
		}

		template <typename... T>
		bool HasComponent()
		{
			HL_CORE_ASSERT_WITH_MSG(m_entityHandle != entt::null, "entity handle is invalid!")
			//HL_CORE_ASSERT_WITH_MSG(!m_scene.expired(), "Scene does not exist anymore!")
			return m_scene->m_components.template any_of<T...>(m_entityHandle);
		}

		template <typename T, typename... Args>
		T& AddComponent(Args &&...args)
		{
			HL_CORE_ASSERT_WITH_MSG(!HasComponent<T>(), "GameObject arleady has this component!");
			//HL_CORE_ASSERT_WITH_MSG(!m_scene.expired(), "Scene does not exist anymore!")
			T& comp = m_scene->m_components.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
			return comp;
		}

		template <typename T, typename... Args>
		T& AddOrReplaceComponent(Args &&...args)
		{
			T& comp = m_scene->m_components.emplace_or_replace<T>(m_entityHandle, std::forward<Args>(args)...);
			return comp;
		}

		template <typename T>
		void RemoveComponent()
		{
			HL_CORE_ASSERT_WITH_MSG(HasComponent<T>(), "GameObject does not have the component!");
			//HL_CORE_ASSERT_WITH_MSG(!m_scene.expired(), "Scene does not exist anymore!");
			m_scene->m_components.remove<T>(m_entityHandle);
		}

		template <typename... T>
		decltype(auto) GetComponent()
		{
			HL_CORE_ASSERT_WITH_MSG(HasComponent<T...>(), "GameObject does not have the component!");
			//HL_CORE_ASSERT_WITH_MSG(!m_scene.expired(), "Scene does not exist anymore!");
			return m_scene->m_components.template get<T...>(m_entityHandle);
		}

		operator entt::entity() const { return m_entityHandle; }
		bool operator==(const Entity& other) const { return m_entityHandle == other.m_entityHandle && m_scene == other.m_scene; }
		bool operator==(const entt::entity& other) const { return m_entityHandle == other; }
		bool operator==(const Scene* other) const { return m_scene == other; }
		bool operator==(const Ref<Scene> other) const { return m_scene == other.get(); }
		bool operator==(const WeakRef<Scene> other) const { return m_scene == other.lock().get(); }

		inline bool isGlobal() { return HasComponent<GlobalObjectComponent>(); }
		void MakeGlobal();
		void Destroy();
		inline bool IsValid() { return m_entityHandle != entt::null && m_scene->m_components.valid(m_entityHandle); }
		inline bool IsNotNull() { return m_entityHandle != entt::null; }
		inline bool IsNull() { return m_entityHandle == entt::null; }

		Scene* GetScene() { return m_scene; }
		entt::entity GetHandle() { return m_entityHandle; }

	private:

		Scene* m_scene;
		entt::entity m_entityHandle{ entt::null };
		
	public:

#pragma region GameObject Creation Helpers

		Entity& CreateMainCamera(Vector2 position = Vector2(0.0f, 0.0f));
		Entity& CreateCamera(Vector2 position = Vector2(0.0f, 0.0f));

#pragma endregion

		void ResetParent();
		void SetParent(Entity& object);
		void SetParent(entt::entity& object);

		inline std::string& GetName() {
			return GetComponent<InfoComponent>().name;
		}

		inline Transform2DComponent GetTransform() {
			return GetComponent<Transform2DComponent>();
		}

		Entity InstantiateObject();
		Entity InstantiateObject(std::string name);
		Entity InstantiateObject(Entity& parent);
		Entity InstantiateObject(std::string name, Entity& parent);
		
		// Serbia - Novi Sad has the worst healthcare personal that doesn't respect the laws and all and kill people by their actions
		
		friend struct Transform2DComponent;

		friend class DepricatedApplication;
		friend class SceneManager;
		friend class Scene;
		friend void SerializeObject(YAML::Emitter& out, Entity& o);
		friend class Transform;
		
		extern friend class GameEngine;
		extern friend class InspectorPanel;
	};

	class Transform
	{
	public:
		Transform() = default;
		Transform(const Transform& other) = default;
		Transform(Entity& gameObject) : m_GameObject(gameObject), m_transform(gameObject.GetComponent<TransformComponent>()), m_relationship(gameObject.GetComponent<RelationshipComponent>()) { }
		Transform(entt::entity entity, Scene* m_scene) : m_GameObject(Entity(entity, m_scene)), m_transform(m_GameObject.GetComponent<TransformComponent>()), m_relationship(m_GameObject.GetComponent<RelationshipComponent>()) { }
		Transform(entt::entity entity, TransformComponent transform_component, RelationshipComponent relationship_component, Scene* m_scene) : m_GameObject(Entity(entity, m_scene)), m_transform(transform_component), m_relationship(relationship_component) { }

		inline TransformComponent GetWorldTransform() { return m_transform.GetWorldTransform(m_relationship, m_GameObject.m_scene->m_components); }

		inline TransformComponent GetWorldTransformCache()
		{
			if(m_relationship.HasParent())
			{
				if (m_GameObject.m_scene->m_worldTransformCache.contains(m_GameObject.m_entityHandle)) return m_GameObject.m_scene->m_worldTransformCache[m_GameObject.m_entityHandle];

				TransformComponent worldTransform = m_transform;
				TransformComponent parentTransform = (
					m_GameObject.m_scene->m_worldTransformCache.contains(m_relationship.parent_handle) ?
					m_GameObject.m_scene->m_worldTransformCache[m_relationship.parent_handle] :
					Transform(m_relationship.parent_handle, m_GameObject.m_scene).GetWorldTransformCache()
				);

				worldTransform.Position = parentTransform.Rotation * worldTransform.Position + parentTransform.Position;
				worldTransform.Rotation = parentTransform.Rotation * worldTransform.Rotation;
				worldTransform.Scale = parentTransform.Scale * worldTransform.Scale;


				m_GameObject.m_scene->m_worldTransformCache[m_GameObject.m_entityHandle] = worldTransform;
				return worldTransform;
			}
			
			if(m_relationship.HasChild()) m_GameObject.m_scene->m_worldTransformCache[m_GameObject.m_entityHandle] = m_transform;
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

		inline Vector3 GetWorldPosition() { return m_transform.GetWorldPosition(m_relationship, m_GameObject.m_scene->m_components); }
		inline Quaternion GetWorldRotation() { return m_transform.GetWorldRotation(m_relationship, m_GameObject.m_scene->m_components); }
		inline Vector3 GetWorldScale() { return m_transform.GetWorldScale(m_relationship, m_GameObject.m_scene->m_components); }

		inline Vector3 GetLocalPosition() {	return m_transform.Position; }
		inline Quaternion GetLocalRotation() { return m_transform.Rotation; }
		inline Vector3 GetLocalScale() { return m_transform.Scale; }

		inline Entity GetGameObject() { return m_GameObject; }
		inline TransformComponent& GetTransformComponent() { return m_transform; }

		inline void SetWorldPosition(Vector3 position) { m_transform.SetWorldPosition(position, m_relationship, m_GameObject.m_scene->m_components); }
		inline void SetWorldRotation(Quaternion rotation) { m_transform.SetWorldRotation(rotation, m_relationship, m_GameObject.m_scene->m_components); }
		inline void SetWorldScale(Vector3 scale) { m_transform.SetWorldScale(scale, m_relationship, m_GameObject.m_scene->m_components); }
		inline void SetWorldTransform(Vector3 position, Quaternion rotation, Vector3 scale) { m_transform.SetWorldTransform({ position, rotation, scale }, m_relationship, m_GameObject.m_scene->m_components); }

		inline void SetLocalPosition(Vector3 position) { m_transform.SetLocalPosition(position); }
		inline void SetLocalRotation(Quaternion rotation) { m_transform.SetLocalRotation(rotation); }
		inline void SetLocalScale(Vector3 scale) { m_transform.SetLocalScale(scale); }
		inline void SetLocalTransform(Vector3 position, Quaternion rotation, Vector3 scale) { m_transform.SetLocalTransform({ position, rotation, scale }); }

		inline void Translate(Vector3 translation) { m_transform.Translate(translation); }

		inline void Rotate(Quaternion rotation) { m_transform.Rotate(rotation); }
		inline void Rotate(Vector3 euler) { m_transform.Rotate(euler); }

		inline Vector3 Forward() { return m_transform.Forward(); }
		inline Vector3 Right() { return m_transform.Right(); }
		void RotateAround(const Vector3& target, const Vector3& axis, float angle) { return m_transform.RotateAround(target, axis, angle); }

	private:
		Entity m_GameObject;
		TransformComponent& m_transform;
		RelationshipComponent& m_relationship;
	};
}

