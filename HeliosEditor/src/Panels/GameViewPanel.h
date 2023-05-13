#pragma once
#include "BasePanel.h"

namespace Helios
{
	class GameViewPanel : public Editor::IPanel
	{
	public:
		GameViewPanel()
		{
			title = "Game View";
			flags = ImGuiWindowFlags_MenuBar;
			m_custom_begin = true;
		}

		void OnUpdate() override;
	};	
}
