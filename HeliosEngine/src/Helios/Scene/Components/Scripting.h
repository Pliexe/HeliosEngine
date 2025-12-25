#pragma once

#include "pch.h"

namespace Helios
{
	class IScriptHandler;
	struct ScriptComponent
	{
		std::byte a;
		//Scope<IScriptHandler> handler;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
		//ScriptComponent(Scope<IScriptHandler>& handler)
		//{
		//	//this->handler = std::move(handler);
		//}
	};
}
