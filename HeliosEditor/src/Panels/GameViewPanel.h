#pragma once
#include "BasePanel.h"

namespace Helios
{
	class GameViewPanel : public Editor::IPanel
	{
	public:
		GameViewPanel()
		{
			m_title = "Game View";
			m_flags = ImGuiWindowFlags_MenuBar;
			m_custom_begin = true;
		}

		void OnUpdate() override;
	};	
}
