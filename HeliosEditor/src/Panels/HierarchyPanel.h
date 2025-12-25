#pragma once
#include "BasePanel.h"

namespace Helios {
	class HierarchyPanel : public Editor::IPanel
	{
	public:
		HierarchyPanel() { m_title = "Hierarchy"; m_custom_begin = true; }

		virtual void OnUpdate() override;
	};
}
