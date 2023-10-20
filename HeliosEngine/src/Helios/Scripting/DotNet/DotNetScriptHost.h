#pragma once

#include "pch.h"

//#include "hostfxr.h"

#include "Helios/Scene/Entity.h"

namespace Helios
{
	class IScriptHandler
	{
	public:
		virtual void OnCreate() = 0;
		virtual void Start() = 0;
		virtual void Update() = 0;
	};

	class IScriptHost
	{
	public:
		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;

		virtual void CreateScriptHandler(Entity entity) = 0;
	};

	class DotNetScriptHandler : public IScriptHandler
	{
	public:
		void OnCreate() override
		{
			
		}

		void Start() override
		{
			
		}

		void Update() override
		{

		}
	private:

	};

	class DotNetScriptHost : public IScriptHost
	{
	public:
		void Initialize() override
		{
		}
		void Shutdown() override
		{
		}

		void CreateScriptHandler(Entity entity) override
		{
			//entity.AddComponent<ScriptComponent>(std::make_unique<DotNetScriptHandler>());
		}
	private:
	};
}
