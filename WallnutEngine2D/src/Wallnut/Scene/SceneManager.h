/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Scene.h"
#include "Wallnut/Core.h"
#include "Wallnut/Graphics/Graphics.h"

namespace Wallnut {
	class Scene;
	class Camera;
	class WALLNUT_API SceneManager
	{
	private:

		static std::map<std::wstring, Scene*> scenes;
		static Scene* currentScene;
		static Scene* loadQueue;

		//inline Scene* getCurrentScene() const { return currentScene; }

		static void CheckQueue();

	public:

		SceneManager() = delete;
		SceneManager(SceneManager const&) = delete;
		void operator=(SceneManager const&) = delete;

		static void Render(Graphics& graphics);
		static void Update();

		static Scene& AddScene(std::wstring name, Scene* scene);
		static Scene& AddScene(std::wstring name, std::function<void(Scene&)> callback);
		static Scene& AddScene(std::wstring name);

		static bool LoadScene(std::wstring name);

		friend class Application;
		friend class GameObject;
		friend class Transform;

		friend class GameEngine;
	};
}

