/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"

namespace Helios {
	class DepricatedGraphics;
	class Size;
	class HELIOS_API Bitmap {
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