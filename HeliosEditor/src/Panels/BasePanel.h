#pragma once

#include <string>

namespace Helios {
	namespace Editor {
		
		class IPanel {
		protected:
			std::string title;
		public:
			inline std::string GetName() const { return title; }
			virtual void OnUpdate() = 0;
		};

	}
}