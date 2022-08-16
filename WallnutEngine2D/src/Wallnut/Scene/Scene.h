/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Wallnut/Utils/LinkedList.h"
#include "Wallnut/GameObjects/GameObject.h"

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



	public:
		void Init() { initCallback(*this); }

		void Unload();

		friend class SceneManager;
		friend class GameObject;
		friend class Transform;
	};


}