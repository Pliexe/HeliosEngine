/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Helios/Core/Base.h"
#include "Helios/Graphics/Graphics.h"

namespace Helios {
	class Scene;
	class Camera;

	class HELIOS_API SceneManager
	{
	private:

		static std::map<std::string, Ref<Scene>> scenes;
		static Ref<Scene> currentScene;
		static Ref<Scene> loadQueue;

		//inline Scene* getCurrentScene() const { return currentScene; }

		static void CheckQueue();

	public:

		inline static const WeakRef<Scene>& GetCurrentScene() { return currentScene; }
		inline static bool HasCurrentScene() { return currentScene != nullptr; }

		SceneManager() = delete;
		SceneManager(SceneManager const&) = delete;
		void operator=(SceneManager const&) = delete;

		static void Render(Graphics& graphics);
		static void Update();

		static const WeakRef<Scene>& AddScene(std::string name, std::function<void(Scene&)> callback);
		static const WeakRef<Scene>& AddScene(std::string name);

		static bool LoadScene(std::string name);

		friend class Application;
		friend class GameObject;
		friend class Transform;

		friend class GameEngine;

	};
}

