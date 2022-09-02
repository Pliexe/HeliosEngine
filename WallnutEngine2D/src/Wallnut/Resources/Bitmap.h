/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Wallnut/Core.h"

namespace Wallnut {
	class Graphics;
	class Size2D;
	class WALLNUT_API Bitmap {
	private:
		std::shared_ptr<ID2D1Bitmap> bitmap;
		const wchar_t* path = NULL;
	public:
		Bitmap() = delete;
		Bitmap(const wchar_t* path);

		operator ID2D1Bitmap* () { return bitmap.get(); }
		inline bool isNull() {
			return bitmap.get() == nullptr;
		}
	};
}