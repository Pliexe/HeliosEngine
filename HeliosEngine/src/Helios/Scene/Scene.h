/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"
#include "Components.h"

namespace Helios {
	class GameObject;
	class SceneManager;
	class Camera;
	class HELIOS_API Scene
	{
	private:


		entt::registry m_components;
		static entt::registry s_components;

		std::function<void(Scene&)> initCallback;

		entt::entity primaryCamera = entt::null;

	public:

		Scene();
		~Scene();

		inline Components::Camera& GetPrimaryCamera();
		inline bool IsPrimaryCamera(entt::entity other) const { return other == primaryCamera; }
		inline bool IsPrimaryCameraSet() const { return entt::null != primaryCamera; }
		inline void SetPrimaryCamera(GameObject& obj);
		inline void ResetPrimaryCamera();

		void OnUpdateRuntime();
		void OnUpdateEditor(Components::Transform cameraTransform, Components::Camera cameraPropeties);

		void Init() { initCallback(*this); }

		void Unload();

		static void Serialize(const std::string& filepath, WeakRef<Scene>& scene);
		static void Deserialize(const std::string& filepath);

		/*static void SerializeBinary(std::filesystem::path path, Scene& scene);
		static void DeserializeBinary(std::filesystem::path path, Scene& scene);*/

		friend class SceneManager;
		friend class GameObject;
		friend class Transform;

		extern friend class GameEngine;
		extern friend class HierarchyPanel;
	};


}