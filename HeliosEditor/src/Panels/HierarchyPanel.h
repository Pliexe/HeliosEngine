#pragma once
#include "BasePanel.h"

namespace Helios {
	class HierarchyPanel : public Editor::IPanel
	{
	public:
		HierarchyPanel() { title = "Hierarchy"; };

		virtual void OnUpdate() override;
	};
}
