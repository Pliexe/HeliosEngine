#include "RustScript.h"

namespace Helios
{
	RustScript::RustScript(std::string scriptName, DynamicLibrary& rustLibrary)
	{
		m_scriptName = scriptName;

		m_onCreate = reinterpret_cast<void(*)()>(rustLibrary.GetFunction("onCreate"));
		m_onStart = reinterpret_cast<void(*)()>(rustLibrary.GetFunction("onStart"));
		m_onUpdate = reinterpret_cast<void(*)()>(rustLibrary.GetFunction("onUpdate"));
		m_onDestroy = reinterpret_cast<void(*)()>(rustLibrary.GetFunction("onDestroy"));

		if (m_onCreate) m_onCreate();
	}
}
