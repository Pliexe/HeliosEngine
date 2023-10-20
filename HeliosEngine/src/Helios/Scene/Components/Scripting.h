#pragma once
#include "pch.h"

namespace Helios
{
	class IScriptHandler;
	struct HELIOS_API ScriptComponent
	{
		//Scope<IScriptHandler> handler;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
		//ScriptComponent(Scope<IScriptHandler>& handler)
		//{
		//	//this->handler = std::move(handler);
		//}
	};
}
