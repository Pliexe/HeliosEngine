#pragma once

namespace Helios
{
	class NativeScript
	{
	public:
		virtual ~NativeScript() = default;

		virtual void OnCreate() {}
		virtual void OnStart() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(float deltaTime) {}
	};
}
