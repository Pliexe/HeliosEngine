#pragma once
#include "pch.h"
#include "Helios/Scripting/Native/NativeScript.h"
#include <Helios/Utils/DynamicLibrary.h>

namespace Helios
{
	class RustScript : public NativeScript
	{
	public:
		RustScript() = delete;
		RustScript(std::string scriptName, DynamicLibrary& rustLibrary);
		~RustScript() = default;
	private:
		struct RustScriptData {
			uint32_t entityId;
		};

		std::string m_scriptName;
		void (*m_onCreate)() = nullptr;
		void (*m_onStart)() = nullptr;
		void (*m_onDestroy)() = nullptr;
		void (*m_onUpdate)() = nullptr;
	};
}