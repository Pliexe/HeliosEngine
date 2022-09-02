/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "Wallnut/GameObjects/ObjectComponent.h"
#include "Wallnut/Resources/Bitmap.h"

namespace Wallnut {
	class SpriteComponent : public ObjectComponent {
	private:
		std::unique_ptr<Bitmap> bitmap;
		float opacity;

		void Render(Graphics& g) override {
			if (!bitmap->isNull()) {
				g.getRenderTarget()->DrawBitmap(*bitmap, transform->getScreenPosition(), opacity);
			}
		}

	public:
		SpriteComponent() = delete;
		SpriteComponent(Bitmap image, float opacity = 1.0f) {
			this->opacity = opacity;
			bitmap = std::make_unique<Bitmap>(image);
		}

		friend class SceneManager;
	};
}