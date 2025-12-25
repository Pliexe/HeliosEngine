#pragma once
#include <vector>

enum class EditorMode;

namespace Helios
{
	namespace Editor
	{
		class IPanel;
	}

	void DrawToolbar(EditorMode& mode, std::vector<Editor::IPanel*>& panels);
}
