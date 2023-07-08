#pragma once
#include "pch.h"

#include "Scene.h"

namespace Helios
{
	class HELIOS_API SceneSerializer
	{
	private:
		std::shared_ptr<Scene> m_scene;
	public:
		SceneSerializer(const Ref<Scene>& scene);
		
		void SerializeEditor(const std::string& filepath) const;
		bool DeserializeEditor(const std::string& filepath) const;
	};

	
}
