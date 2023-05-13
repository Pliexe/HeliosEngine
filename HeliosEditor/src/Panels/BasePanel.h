#pragma once

#include <string>

#include "ImGuiCustomControls.h"

namespace Helios {
	namespace Editor {
		
		class IPanel {
		protected:
			std::string title;
			ImGuiWindowFlags flags = 0;
			bool m_custom_begin = false;
		public:
			inline bool IsCustomBegin() const { return m_custom_begin; }
			inline ImGuiWindowFlags GetFlags() const { return flags; }
			bool m_window_open = true;
			bool destroy_window = false;
			inline std::string GetName() const { return title; }
			virtual void OnUpdate() = 0;
		};

	}
}
