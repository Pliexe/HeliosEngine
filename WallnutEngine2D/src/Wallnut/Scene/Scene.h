/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Wallnut/Utils/LinkedList.h"
#include "Wallnut/GameObjects/GameObject.h"

#define SCENE_FILE_VERSION "0.1"

namespace Wallnut {
	class SceneManager;
	class GameObject;
	class Camera;
	
	class WALLNUT_API Scene
	{
	private:

		Camera* currentCamera = NULL;
		std::function<void(Scene&)> initCallback;

		//LinkedList<std::shared_ptr<GameObject>> gameObjects;

	protected:

		void Serialize(const std::string& filepath);
		void Deserialize(const std::string& filepath);

	public:
		void Init() { initCallback(*this); }

		void Unload();

		friend class SceneManager;
		friend class GameObject;
		friend class Transform;

		extern friend class GameEngine;
	};


}