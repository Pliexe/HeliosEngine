/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "Wallnut/Graphics/Graphics.h"
#include "Wallnut/GameObjects/GameObject.h"
#include "Wallnut/Core.h"

namespace Wallnut {
	class Transform;
	class GameObject;
	class WALLNUT_API ObjectComponent
	{
	protected:

		virtual void Render(Graphics& graphics) { };
		virtual void Update() { };
		virtual void Init(Graphics& graphics) { };

		GameObject* gameObject = NULL;
		Transform* transform = NULL;

		//const char* type = typeid(this).name();

	public:

		friend class Application;
		friend class SceneManager;
		friend class GameObject;
		friend class Transform;
	};

	/*class AsyncObjectComponent
	{
	private:


		friend class Application;
	};*/
}
