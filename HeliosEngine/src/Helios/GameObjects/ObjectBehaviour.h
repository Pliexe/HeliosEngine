/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "ObjectComponent.h"
#include "Helios/Core/Base.h"
#include "Helios/Graphics/DepricatedGraphics.h"

namespace Helios {
	class HELIOS_API ObjectBehaviour : public ObjectComponent
	{
	private:
		void Render(DepricatedGraphics& g) override { }
	protected:
		virtual void Update() override = 0;
	};

}
