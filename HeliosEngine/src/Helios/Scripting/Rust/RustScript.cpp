#include "RustScript.h"

namespace Helios
{
	RustScript::RustScript(const std::string& scriptName, DynamicLibrary& rustLibrary)
	{
		m_scriptName = scriptName;

		m_onCreate = rustLibrary.GetSymbol<void>("onCreate");
		m_onStart = rustLibrary.GetSymbol<void>("onStart");
		m_onUpdate = rustLibrary.GetSymbol<void>("onUpdate");
		m_onDestroy = rustLibrary.GetSymbol<void>("onDestroy");

		if (m_onCreate) m_onCreate();
	}

}
