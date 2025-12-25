#pragma once
#include "pch.h"
#include "Helios/Core/Base.h"
#include "BehaviourHandle.h"
#include "ScriptServiceEvent.h"

#include "ScriptingService.h"
#include "BehaviourStore.h"

namespace Helios
{
	namespace Scripting
	{				
		HELIOS_API ServiceResult RegisterService(std::string_view service_name, const ServiceDescriptor &desc);
		HELIOS_API ServiceResult UnregisterService(std::string_view service_name);

		HELIOS_API ServiceResult StartService(std::string_view service_name);
		HELIOS_API ServiceResult StopService(std::string_view service_name);
		HELIOS_API ServiceResult ReloadService(std::string_view service_name);
		HELIOS_API ServiceResult ReloadService(std::string_view service_name, std::function<bool()> callback);
		HELIOS_API ServiceResult ReloadAllServices();

		HELIOS_API BehaviourStore& GetBehaviourStore(std::string_view service_name);
		HELIOS_API ScriptingService& GetService(std::string_view service_name);

		void OnEvent(Event& event);

		ServiceResult ShutdownScriptManager(); // no need to export since its for internal use

		ServiceStatus HELIOS_API GetServiceStatus(std::string_view service_name);

		HELIOS_API const std::vector<std::string>& GetRegisteredServices();

		void UpdateServices(); // Call every frame
		void UpdateEditorServices(); // Call every frame

		enum class ServiceData
		{
			SHARED_LIBRARY_PATH = BIT(10),
		};

		void SetServiceDataUnsafe(std::string_view service_name, ServiceData data, void* value);
		void* GetServiceDataUnsafe(std::string_view service_name, ServiceData data);

		template <typename T>
		void SetServiceData(std::string_view service_name, ServiceData data, T value) { SetServiceDataUnsafe(service_name, data, static_cast<void*>(value)); }
		template <typename T>
		T GetServiceData(std::string_view service_name, ServiceData data) { return static_cast<T>(GetServiceDataUnsafe(service_name, data)); }
	}
}
