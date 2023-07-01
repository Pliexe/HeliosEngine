#pragma once
#include "Relationship.h"
#include "Helios/Translation/Quaternion.h"
#include "Helios/Translation/Vector.h"
#include "Helios/Translation/Matrix.h"
#include "../Enums.h"

namespace Helios
{
	struct HELIOS_API TransformComponent
	{
		Vector3		Position = Vector3::Zero();
		Quaternion	Rotation = Quaternion::Identity();
		Vector3		Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const Vector3& position) : Position(position) { }
		TransformComponent(const Vector3& position, const Vector3& rotation) : Position(position), Rotation(rotation) { }
		TransformComponent(const Vector3& position, const Quaternion& rotation, const Vector3& scale) : Position(position), Rotation(rotation), Scale(scale) { }

		TransformComponent GetWorldTransform(RelationshipComponent relationship, entt::registry& registry)
		{
			if (relationship.parent_handle != entt::null)
			{
				Vector3 position = Position;
				Quaternion rotation = Rotation;
				Vector3 scale = Scale;

				auto parentTransform = registry.get<TransformComponent>(relationship.parent_handle);

				position = parentTransform.Rotation * position + parentTransform.Position;
				rotation = parentTransform.Rotation * rotation;
				scale = parentTransform.Scale * scale;

				return TransformComponent(position, rotation, scale);
			}
			else return *this;
		}

		Vector3 GetWorldPosition(RelationshipComponent relationship, entt::registry& registry)
		{
			if (relationship.parent_handle != entt::null)
				GetWorldTransform(relationship, registry).Position;
			else return Position;
		}

		Quaternion GetWorldRotation(RelationshipComponent relationship, entt::registry& registry)
		{
			if (relationship.parent_handle != entt::null)
			{
				auto parentTransform = registry.get<TransformComponent>(relationship.parent_handle);
				return GetWorldRotation(relationship, registry) * parentTransform.Rotation;
			}
			else return Rotation;
		}

		Vector3 GetWorldScale(RelationshipComponent relationship, entt::registry& registry)
		{
			if (relationship.parent_handle != entt::null)
			{
				auto parentTransform = registry.get<TransformComponent>(relationship.parent_handle);
				return GetWorldScale(relationship, registry) * parentTransform.Scale;
			}
			else return Scale;
		}

		void SetWorldPosition(const Vector3& position, RelationshipComponent relationship, entt::registry& registry)
		{
			if (relationship.parent_handle != entt::null)
			{
				auto parentTransform = registry.get<TransformComponent>(relationship.parent_handle);
				Position = Quaternion::Inverse(parentTransform.Rotation) * (position - parentTransform.Position);
			}
			else Position = position;
		}

		void SetWorldRotation(const Quaternion& rotation, RelationshipComponent relationship, entt::registry& registry)
		{
			if (relationship.parent_handle != entt::null)
			{
				auto parentTransform = registry.get<TransformComponent>(relationship.parent_handle);
				Rotation = Quaternion::Inverse(parentTransform.Rotation) * rotation;
			}
			else Rotation = rotation;
		}

		void SetWorldScale(const Vector3& scale, RelationshipComponent relationship, entt::registry& registry)
		{
			if (relationship.parent_handle != entt::null)
			{
				auto parentTransform = registry.get<TransformComponent>(relationship.parent_handle);
				Scale = parentTransform.Scale / scale;
			}
			else Scale = scale;
		}

		void SetWorldTransform(const TransformComponent& transform, RelationshipComponent relationship, entt::registry& registry)
		{
			if (relationship.parent_handle != entt::null)
			{
				auto parentTransform = registry.get<TransformComponent>(relationship.parent_handle);
				Position = Quaternion::Inverse(parentTransform.Rotation) * (transform.Position - parentTransform.Position);
				Rotation = Quaternion::Inverse(parentTransform.Rotation) * transform.Rotation;
				Scale = parentTransform.Scale / transform.Scale;
			}
			else {
				Position = transform.Position;
				Rotation = transform.Rotation;
				Scale = transform.Scale;
			}
		}

		inline void SetLocalPosition(const Vector3& position) { Position = position; }
		inline void SetLocalRotation(const Quaternion& rotation) { Rotation = rotation; }
		inline void SetLocalScale(const Vector3& scale) { Scale = scale; }
		inline void SetLocalTransform(const TransformComponent& transform) { Position = transform.Position; Rotation = transform.Rotation; Scale = transform.Scale; }

		void Rotate(const Vector3& axis, float angle)
		{
			Rotation = Quaternion::FromAxisAngle(axis, angle) * Rotation;
		}

		void Rotate(const Vector3& euler)
		{
			Rotation *= Quaternion::FromEuler(euler);
		}

		void Rotate(const Quaternion& quat)
		{
			Rotation *= quat;
		}

		void RotateRads(const Vector3& euler)
		{
			Rotation = Quaternion::FromEulerRads(euler) * Rotation;
		}

		void Translate(const Vector3& translation)
		{
			Position += translation;
		}


		inline Vector3 Forward() { return Rotation.forward(); }
		//Vector3 forward() { return rotation * Vector3::Forward(); }
		inline Vector3 Right() { return Rotation * Vector3::Right(); }
		inline Vector3 Up() { return Rotation.up(); }
		void RotateAround(const Vector3& target, const Vector3& axis, float angle)
		{
			Position = Quaternion::FromAxisAngle(axis, angle) * (Position - target) + target;
			Rotation = Quaternion::FromAxisAngle(axis, angle) * Rotation;
		}

		static inline Matrix4x4 ToModelMatrix(TransformComponent transform) { return Matrix4x4::TranslationColumn(transform.Position) * Matrix4x4::RotationColumn(transform.Rotation) * Matrix4x4::Scale(transform.Scale); }
		inline Matrix4x4 ToRowModelMatrix(TransformComponent transform) { return Matrix4x4::TranslationRow(transform.Position) * Matrix4x4::RotationRow(transform.Rotation) * Matrix4x4::Scale(transform.Scale); }
		//inline Matrix4x4 GetModelMatrix() const { return Matrix4x4::TranslationColumn(Position) * Matrix4x4::RotationColumn(RotationRow) * Matrix4x4::Scale(Scale); }

		//inline Matrix4x4 GetModelMatrix() const { return Matrix4x4::TranslationColumn(Position) * Matrix4x4::RotationColumn(Rotation) * Matrix4x4::Scale(Scale); }
		Matrix4x4 GetModelMatrix() const
		{
			return (
				Matrix4x4::Translation(Position) *
				Matrix4x4::Scale(Scale) *
				Matrix4x4::Rotation(Rotation)
			);
		}

		inline Matrix4x4 GetRowModelMatrix() const { return Matrix4x4::TranslationRow(Position) * Matrix4x4::RotationRow(Rotation) * Matrix4x4::Scale(Scale); }
		Matrix4x4 GetWorldMatrix(RelationshipComponent relationship, entt::registry& registry)
		{
			if (relationship.parent_handle != entt::null)
			{
				auto parentTransform = registry.get<TransformComponent>(relationship.parent_handle);
				return parentTransform.GetWorldMatrix(relationship, registry) * GetModelMatrix();
			}
			else return GetModelMatrix();
		}
		Matrix4x4 GetWorldRowMatrix(RelationshipComponent relationship, entt::registry& registry)
		{
			if (relationship.parent_handle != entt::null)
			{
				auto parentTransform = registry.get<TransformComponent>(relationship.parent_handle);
				return parentTransform.GetWorldRowMatrix(relationship, registry) * GetRowModelMatrix();
			}
			else return GetRowModelMatrix();
		}

		Matrix4x4 GetWorldMatrixLight(RelationshipComponent relationship, entt::registry& registry)
		{
			if (relationship.parent_handle != entt::null)
				return TransformComponent::ToModelMatrix(GetWorldTransform(relationship, registry));
			else return GetModelMatrix();
		}
		Matrix4x4 GetWorldRowMatrixLight(RelationshipComponent relationship, entt::registry& registry)
		{
			if (relationship.parent_handle != entt::null)
				return TransformComponent::ToRowModelMatrix(GetWorldTransform(relationship, registry));
			else return GetRowModelMatrix();
		}

		/*inline Matrix4x4 GetTransform() const { return Matrix4x4::TranslationColumn(Position) * Matrix4x4::RotationColumn(RotationRow) * Matrix4x4::Scale(Scale); }
		inline Matrix4x4 GetRowTransform() const { return Matrix4x4::TranslationRow(Position) * Matrix4x4::RotationRow(RotationRow) * Matrix4x4::Scale(Scale); }*/
	};

	struct HELIOS_API Transform2DComponent
	{
		Vector2	position = { 0.0f, 0.0f };
		Vector2 rotation = { 0.0f, 0.0f };
		Vector2		size = { 50.0f, 50.0f };

		Transform2DComponent() = default;
		Transform2DComponent(const Transform2DComponent&) = default;
		Transform2DComponent(const Vector2& position, const Vector2& size = { 50.0f, 50.0f }) : position(position), size(size) { }
	};


}
