#include "ScriptManager.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Scene/Entity.h"

// namespace Helios
// {
// 	inline static Scope<DynamicLibrary> s_native_script;
// 	inline static Scope<DynamicLibrary> s_dotnet_native_script;
// 	inline static ManagedLibrary s_dotnet_managed_script; // Change type to ManagedLibrary

// 	inline static std::filesystem::path s_native_script_path;
// 	inline static std::filesystem::path s_dotnet_native_script_path;
// 	inline static std::filesystem::path s_dotnet_managed_script_path;

// 	using ServiceEventCallbackFn = void(ScriptLanguage, ScriptServiceEvent);
// 	using ServiceEventCallback = std::function<ServiceEventCallbackFn>;

// 	struct CallbackHash
// 	{
// 		std::size_t operator()(const ServiceEventCallback &cb) const
// 		{
// 			auto target = cb.target<ServiceEventCallbackFn>();
// 			return std::hash<const ServiceEventCallbackFn *>()(target);
// 		}
// 	};

// 	struct CallbackEqual
// 	{
// 		bool operator()(const ServiceEventCallback &a, const ServiceEventCallback &b) const
// 		{
// 			return a.target<ServiceEventCallbackFn>() == b.target<ServiceEventCallbackFn>();
// 		}
// 	};

// 	std::vector<ScriptManager::ServiceEventCallback> ScriptManager::s_event_callbacks;

// 	inline static ScriptManager::ServiceInterface s_native_script_methods;
// 	inline static ScriptManager::ServiceInterface s_dotnet_managed_script_methods;
// 	inline static ScriptManager::ServiceInterface s_dotnet_native_script_methods;

// 	inline static std::vector<std::string> s_derived_dotnet_native_class_names;
// 	inline static std::vector<std::string> s_derived_native_class_names;
// 	inline static std::vector<std::string> s_derived_dotnet_managed_class_names;

// 	ScriptManager::~ScriptManager()
// 	{
// 		Shutdown();
// 	}

// 	bool ScriptManager::InitService(ScriptLanguage language, const std::filesystem::path &path)
// 	{
// 		switch (language)
// 		{
// 		case ScriptLanguage::Native:
// 			s_native_script_path = path;
// 			break;
// 		case ScriptLanguage::DotNetManaged:
// 		{
// 			s_dotnet_managed_script_path = path;
// 			// if (!s_dotnet_managed_script.Load(path, [](const char *name)
// 			// 								  { std::cout << "Loading " << name << " from managed library" << std::endl; }))
// 			{
// 				ShowMessage("Error", "Failed to load .NET managed script", Message::IconError);
// 				return false;
// 			}

// 			// Get the methods
// 			// s_dotnet_managed_script_methods.method_call = s_dotnet_managed_script.GetMethod<void>("MethodCall");
// 			// s_dotnet_managed_script_methods.batch_method_call = s_dotnet_managed_script.GetMethod<void>("BatchMethodCall");
// 			// s_dotnet_managed_script_methods.create_instance = s_dotnet_managed_script.GetMethod<void>("CreateInstance");
// 			// s_dotnet_managed_script_methods.destroy_instance = s_dotnet_managed_script.GetMethod<void>("DestroyInstance");

// 			// Get the derived class names
// 			auto initialize_fn = s_dotnet_managed_script.GetMethod<void, void (*)(const char *)>("Initialize");

// 			break;
// 		}
// 		case ScriptLanguage::DotNetNative:
// 			s_dotnet_native_script_path = path;
// 			try
// 			{
// 				s_dotnet_native_script = std::make_unique<DynamicLibrary>(path);
// 				auto initFn = s_dotnet_native_script->GetSymbol<void, DynamicLibrarySymbol<void, const char *>>("initialize");
// 				if (initFn)
// 				{
// 					initFn([](const char *class_name)
// 						   {
// 							std::cout << "Found derived class: " << class_name << std::endl;
// 							if (class_name) {
// 								s_derived_dotnet_native_class_names.reserve(s_derived_dotnet_native_class_names.size() + 1);
// 								s_derived_dotnet_native_class_names.push_back(class_name);
// 								} });

// 					auto createFn = s_dotnet_native_script->GetSymbol<ExternalScriptHandle, const char *, Helios::Entity*>("CreateInstance");
// 					auto destroyFn = s_dotnet_native_script->GetSymbol<bool, ExternalScriptHandle>("DestroyInstance");

// 					auto callMethod = s_dotnet_native_script->GetSymbol<void, ExternalScriptHandle, const char *>("MethodCall");
// 					auto batchMethod = s_dotnet_native_script->GetSymbol<void, ExternalScriptHandle *, uint32_t, const char *>("BatchMethodCall");

// 					if (createFn && destroyFn && callMethod && batchMethod)
// 					{
// 						s_dotnet_native_script_methods.create_instance = createFn;
// 						s_dotnet_native_script_methods.destroy_instance = destroyFn;

// 						s_dotnet_native_script_methods.method_call = callMethod;
// 						s_dotnet_native_script_methods.batch_method_call = batchMethod;
// 					}
// 					else
// 					{
// 						ShowMessage("Error", "AAAAAAAAAA Failed to load .NET native script", Message::IconError);
// 						s_dotnet_native_script.reset();
// 						return false;
// 					}
// 				}
// 				else
// 				{
// 					ShowMessage("Error", "Failed to load .NET native script", Message::IconError);
// 					s_dotnet_native_script.reset();
// 					return false;
// 				}

// 				for (const auto &cb : s_event_callbacks)
// 				{
// 					cb(language, ScriptServiceEvent::Started);
// 				}

// 				return true;
// 			}
// 			catch (Helios::HeliosException ex)
// 			{
// 				ShowMessage("Error", "Failed to load .NET native script\n" + ex.what(), Message::IconError);
// 				s_dotnet_native_script.reset();
// 				return false;
// 			}
// 			break;
// 		}
// 		HL_EXCEPTION(true, std::string("The language \"") + to_string(language) + "\" is not supported yet!");
// 	}

// 	bool ScriptManager::ReloadService(ScriptLanguage language, const std::function<bool()> &between_reload_cb)
// 	{
// 		std::filesystem::path path = GetPath(language);

// 		ShutdownService(language);
// 		if (between_reload_cb())
// 			return InitService(language, path);
// 		else
// 			return false;
// 	}

//     bool ScriptManager::ReloadAllServices(const std::function<bool(ScriptLanguage)> &between_reload_cb)
//     {
// 		bool res = true;

// 		if (!GetPath(ScriptLanguage::Native).empty() && ReloadService(ScriptLanguage::Native, [between_reload_cb]() { return between_reload_cb(ScriptLanguage::Native); })) res = false;

// 		if (!GetPath(ScriptLanguage::DotNetManaged).empty() && ReloadService(ScriptLanguage::DotNetManaged, [between_reload_cb]() { return between_reload_cb(ScriptLanguage::DotNetManaged); })) res = false;

// 		if (!GetPath(ScriptLanguage::DotNetNative).empty() && ReloadService(ScriptLanguage::DotNetNative, [between_reload_cb]() { return between_reload_cb(ScriptLanguage::DotNetNative); })) res = false;

// 		return res;
//     }

//     bool ScriptManager::ShutdownService(ScriptLanguage language)
// 	{
// 		for (const auto &cb : s_event_callbacks)
// 		{
// 			cb(language, ScriptServiceEvent::Stopped);
// 		}

// 		switch (language)
// 		{
// 		case ScriptLanguage::Native:
// 			s_native_script_path = "";
// 			s_derived_native_class_names.clear();
// 			s_native_script_methods.clear();
// 			s_native_script.reset();
// 			return true;
// 			break;
// 		case ScriptLanguage::DotNetManaged:
// 			s_dotnet_managed_script_path = "";
// 			s_derived_dotnet_managed_class_names.clear();
// 			s_dotnet_managed_script_methods.clear();
// 			s_dotnet_managed_script.Unload();
// 			return true;
// 			break;
// 		case ScriptLanguage::DotNetNative:
// 			s_dotnet_native_script_path = "";
// 			s_derived_dotnet_native_class_names.clear();
// 			s_dotnet_native_script_methods.clear();
// 			s_dotnet_native_script.reset();
// 			return true;
// 			break;
// 		}
// 		// HL_EXCEPTION(true, std::string("The language \"") + to_string(language) + "\" is not supported yet!");
// 		return false;
// 	}

//     bool ScriptManager::ServiceStatus(ScriptLanguage language)
//     {
// 		switch (language)
// 		{
// 		case ScriptLanguage::Native:
// 			return s_native_script_path != "" && s_native_script != nullptr;
// 		// case ScriptLanguage::DotNetManaged:
// 			// return s_dotnet_managed_script_path != "" && s_dotnet_managed_script.IsLoaded();
// 		case ScriptLanguage::DotNetNative:
// 			return s_dotnet_native_script_path != "" && s_dotnet_native_script != nullptr;
// 		}

//         return false;
//     }

//     const std::vector<std::string> &ScriptManager::GetDerivedClassNames(ScriptLanguage language)
// 	{
// 		static std::vector<std::string> s_empty;

// 		switch (language)
// 		{
// 		case ScriptLanguage::DotNetNative:
// 			return s_derived_dotnet_native_class_names;
// 		default:
// 			return s_empty;
// 		}
// 	}

//     const ScriptManager::ServiceInterface &ScriptManager::GetServiceInterface(ScriptLanguage language)
//     {
// 		switch (language)
// 		{
// 		case ScriptLanguage::Native:
// 			return s_native_script_methods;
// 		case ScriptLanguage::DotNetManaged:
// 			return s_dotnet_managed_script_methods;
// 		case ScriptLanguage::DotNetNative:
// 			return s_dotnet_native_script_methods;
// 		}
// 		HL_EXCEPTION(true, std::string("The language \"") + to_string(language) + "\" is not supported yet!");
//     }

//     bool ScriptManager::CreateScript(Entity entity, ScriptLanguage language, const std::string_view &scriptName, ScriptHandle *outHandle)
// 	{
// 		switch (language)
// 		{
// 		case ScriptLanguage::DotNetNative:
// 		{
// 			if (s_dotnet_native_script == nullptr) return false;

// 			auto ptr = s_dotnet_native_script_methods.create_instance(scriptName.data(), &entity);
// 			if (ptr)
// 			{
// 				*outHandle = ScriptHandle{ .handle = ptr, .language = language };
// 			}
// 			else
// 				return false;

// 			return true;
// 		}
// 		default:
// 			return false;
// 		}

// 		return false;
// 	}

// 	bool ScriptManager::DestroyScript(ScriptHandle handle)
// 	{
// 		if (handle.handle == nullptr) return false;
// 		switch (handle.language)
// 		{
// 		case ScriptLanguage::DotNetNative:
// 		{
// 			return s_dotnet_native_script_methods.destroy_instance(handle.handle);
// 			break;
// 		}
// 		default:
// 			return false;
// 		};

// 		return false;
// 	}

// 	void ScriptManager::RegisterEventCallback(const std::function<void(ScriptLanguage, ScriptServiceEvent)> &callback)
// 	{
// 		s_event_callbacks.push_back(callback);
// 	}

// 	void ScriptManager::UnregisterEventCallback(const std::function<void(ScriptLanguage, ScriptServiceEvent)>& callback)
// 	{
// 	    const void* target_ptr = callback.target<ServiceEventCallbackFn>();

// 	    auto it = std::find_if(s_event_callbacks.begin(), s_event_callbacks.end(),
// 	        [target_ptr](const auto& cb) {
// 	            return cb.target<ServiceEventCallbackFn>() == target_ptr;
// 	        });

// 	    if (it != s_event_callbacks.end())
// 	        s_event_callbacks.erase(it);
// 	}

// 	void ScriptManager::Shutdown()
// 	{
// 		for (const auto &cb : s_event_callbacks)
// 		{
// 			cb(ScriptLanguage::Native, ScriptServiceEvent::Stopped);
// 			cb(ScriptLanguage::DotNetManaged, ScriptServiceEvent::Stopped);
// 			cb(ScriptLanguage::DotNetNative, ScriptServiceEvent::Stopped);
// 		}
// 		s_native_script.reset();
// 		s_dotnet_native_script.reset();
// 		s_dotnet_managed_script.Unload();
// 		s_derived_dotnet_native_class_names.clear();
// 	}

// 	std::filesystem::path ScriptManager::GetPath(const ScriptLanguage &language)
// 	{
// 		switch (language)
// 		{
// 		case ScriptLanguage::Native:
// 			return s_native_script_path;
// 		case ScriptLanguage::DotNetManaged:
// 			return s_dotnet_managed_script_path;
// 		case ScriptLanguage::DotNetNative:
// 			return s_dotnet_native_script_path;
// 		}
// 		HL_EXCEPTION(true, std::string("The language \"") + to_string(language) + "\" is not supported yet!");
// 		return "";
// 	}
// }

using namespace Helios::Scripting;

namespace
{
	struct string_view_hash
	{
		std::size_t operator()(std::string_view sv) const
		{
			return std::hash<std::string_view>{}(sv);
		}
	};

	// DO NOT ACCESS THE KEYS OF THIS MAP DIRECTLY AS THEIR OWNERSHIP IS NOT OWNED
	std::unordered_map<std::string_view, std::unique_ptr<ScriptingService>, string_view_hash> s_services;
	std::unordered_map<std::string_view, std::unique_ptr<BehaviourStore>, string_view_hash> s_behaviour_stores;

	std::vector<std::string> s_registered_services;

	std::mutex s_services_mutex;
}

ServiceResult Helios::Scripting::RegisterService(std::string_view service_name, const ServiceDescriptor &desc)
{
	if (s_services.find(service_name) != s_services.end())
		return ServiceResult::ServiceAlreadyExists;

	std::unique_ptr<ScriptingService> service;

	try
	{
		service = std::make_unique<ScriptingService>(service_name, desc);
	}
	catch (const ServiceResult &result)
	{
		return result;
	}

	s_services[service_name] = std::move(service);
	s_registered_services.push_back(std::string(service_name));

	ScriptingService *ptr = s_services[service_name].get();
	s_behaviour_stores[service_name] = std::make_unique<BehaviourStore>(ptr);
	// No need to load the behaviour store at this point

	std::cout << "[ServiceManager] Registered service: " << service_name << std::endl;

	return ServiceResult::Ok;
}

ServiceResult Helios::Scripting::UnregisterService(std::string_view service_name)
{
	auto it = s_services.find(service_name);
	if (it == s_services.end())
		return ServiceResult::ServiceNotFound;
	if (it->second->GetStatus() != ServiceStatus::Stopped || it->second->GetStatus() != ServiceStatus::NotInitialized)
		return ServiceResult::ServiceRunning;

	s_behaviour_stores[service_name]->OnServiceStop(); // Unload the behaviour store for this service
	s_behaviour_stores.erase(service_name);

	s_registered_services.erase(std::remove(s_registered_services.begin(), s_registered_services.end(), std::string(service_name)), s_registered_services.end());
	s_services.erase(it);

	std::cout << "[ServiceManager] Unregistered service: " << service_name << std::endl;

	return ServiceResult::Ok;
}

ServiceResult Helios::Scripting::StartService(std::string_view service_name)
{
	auto it = s_services.find(service_name);
	if (it == s_services.end())
		return ServiceResult::ServiceNotFound;
	if (it->second->GetStatus() == ServiceStatus::Running)
		return ServiceResult::ServiceRunning;

	auto result = it->second->Start();

	if (ServiceResult::Ok != result)
		return result;

	s_behaviour_stores[service_name]->OnServiceStart();

	std::cout << "[ServiceManager] Started service: " << service_name << std::endl;

	return result;
}

ServiceResult Helios::Scripting::StopService(std::string_view service_name)
{
	auto it = s_services.find(service_name);
	if (it == s_services.end())
		return ServiceResult::ServiceNotFound;
	if (it->second->GetStatus() != ServiceStatus::Running)
		return ServiceResult::ServiceNotRunning;

	s_behaviour_stores[service_name]->OnServiceStop();

	std::cout << "[ServiceManager] Stopped service: " << service_name << std::endl;

	return it->second->Stop();
}

ServiceResult Helios::Scripting::ReloadService(std::string_view service_name)
{
	auto it = s_services.find(service_name);
	if (it == s_services.end())
		return ServiceResult::ServiceNotFound;

	std::cout << "[ServiceManager] Reloading service: " << service_name << std::endl;

	auto result = it->second->Stop();
	if (ServiceResult::Ok != result)
		return result;

	s_behaviour_stores[service_name]->OnServiceStop();

	result = it->second->Start();

	if (ServiceResult::Ok != result)
		return result;

	s_behaviour_stores[service_name]->OnServiceStart();

	std::cout << "[ServiceManager] Reloaded service: " << service_name << std::endl;

	return result;
}

ServiceResult Helios::Scripting::ReloadService(std::string_view service_name, std::function<bool()> callback)
{
	auto it = s_services.find(service_name);
	if (it == s_services.end())
		return ServiceResult::ServiceNotFound;

	std::cout << "[ServiceManager] Reloading service: " << service_name << std::endl;

	auto result = it->second->Stop();
	if (ServiceResult::Ok != result)
		return result;

	s_behaviour_stores[service_name]->OnServiceStop();

	if (!callback())
		return ServiceResult::StoppedDuringCallback;

	result = it->second->Start();

	if (ServiceResult::Ok != result)
		return result;

	s_behaviour_stores[service_name]->OnServiceStart();

	std::cout << "[ServiceManager] Reloaded service: " << service_name << std::endl;

	return result;
}

ServiceResult Helios::Scripting::ReloadAllServices()
{
	if (s_services.empty())
		return ServiceResult::Ok;

	std::cout << "[ServiceManager] Reloading all services" << std::endl;

	ServiceResult result = ServiceResult::Ok;

	for (auto &it : s_services)
	{
		result = Scripting::StopService(it.first);
	}

	for (auto &it : s_services)
	{
		result = Scripting::StartService(it.first);
	}

	std::cout << "[ServiceManager] Reloaded all services" << std::endl;

	return result;
}

BehaviourStore &Helios::Scripting::GetBehaviourStore(std::string_view service_name)
{
	auto it = s_behaviour_stores.find(service_name);
	if (it == s_behaviour_stores.end())
		throw std::runtime_error("Behaviour store not found");
	return *it->second.get();
}

ScriptingService &Helios::Scripting::GetService(std::string_view service_name)
{
	auto it = s_services.find(service_name);
	if (it == s_services.end())
		throw std::runtime_error("Service not found");
	return *it->second.get();
}

void Helios::Scripting::OnEvent(Event &event)
{
	std::lock_guard<std::mutex> lock(s_services_mutex); // In case of for example event from another thread (Physics)
	
	if (!s_behaviour_stores.empty()) { // Needed in case of invalidated iterators
		for (auto &[_, store] : s_behaviour_stores)
			store->OnEvent(event);
	}
}

ServiceResult Helios::Scripting::ShutdownScriptManager()
{
	std::cout << "[ServiceManager] Stopping all services" << std::endl;
	
	for (auto &it : s_services)
	{
		auto result = it.second->Stop();
		if (ServiceResult::Ok != result)
			return result;
	}

	s_services.clear();
	s_behaviour_stores.clear();
	s_registered_services.clear();

	return ServiceResult::Ok;
}

ServiceStatus Helios::Scripting::GetServiceStatus(std::string_view service_name)
{
	auto it = s_services.find(service_name);
	if (it == s_services.end())
		return ServiceStatus::NotFound;

	return it->second->GetStatus();
}

const std::vector<std::string> &Helios::Scripting::GetRegisteredServices()
{
	return s_registered_services;
}

void Helios::Scripting::UpdateServices()
{
	for (auto &it : s_behaviour_stores)
		it.second->OnUpdate();
}

void Helios::Scripting::UpdateEditorServices()
{
	for (auto &it : s_behaviour_stores)
		it.second->OnEditorUpdate();
}
