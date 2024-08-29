#pragma once

#include <mutex>
#include <chrono>
#include <future>
#include <filesystem>

#include "Helios/Core/Asserts.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Core/Application.h"

#include "ProjectManager.h"
#include "Helios/Resources/Texture.h"
#include "imgui.h"
#include <Helios/Resources/ResourceResolver.h>
#include <Helios/Resources/ResourceRegistry.h>

namespace Helios
{
	class AssetRegistry
	{
	private:
		static std::unordered_map<std::string, Ref<Texture2D>> s_Textures;

		static void RegisterIfExists(std::filesystem::path file)
		{
			auto ext = file.extension().string();

			if (ext == ".png" || ext == ".jpg")
			{
				if (std::filesystem::exists(file))
				{
					if (!ResourceResolver::IsPathRegistered(file))
					{
						try {
							ResourceRegistry::GetResource<Texture2D>(ResourceResolver::RegisterResource(file, LoadFullResource | LoadMetadataFile));
						}
						catch (HeliosException ex)
						{
							Helios::ShowMessage("Error", ex.what(), Message::IconError);
						}
					}
				}
			}
			else if (ext == ".obj")
			{
				if (std::filesystem::exists(file))
				{
					if (!ResourceResolver::IsPathRegistered(file))
					{
						try {
							//ResourceRegistry::GetResource<Mesh>();
							ResourceResolver::RegisterResource(file, LoadFullResource | LoadMetadataFile | LoadAsOneMesh);
						}
						catch (HeliosException ex)
						{
							Helios::ShowMessage("Error", ex.what(), Message::IconError);
						}
					}
				}
			}
		}

		static bool s_CheckForChanges;

		static bool s_RegistryWindowOpen;
		
		static bool s_TextureSelect;
		static std::function<void(Ref<Texture2D>)> s_onTextureSelected;

	public:

		~AssetRegistry()
		{
			s_CheckForChanges = false;
		}

		static std::unordered_map<std::string, Ref<Texture2D>>& GetTextures() { return s_Textures; }

		static Ref<Texture2D> GetTexture(std::string path)
		{
			if (s_Textures.find(path) != s_Textures.end())
				return s_Textures[path];
			else
				return nullptr;
		}

		static inline bool CheckForChangesEnabled() { return s_CheckForChanges; }

		static void Init()
		{
			s_CheckForChanges = true;
			std::thread([]() {
				while (AssetRegistry::CheckForChangesEnabled())
				{
					CheckValidity();
					Rescan();
					std::this_thread::sleep_for(std::chrono::milliseconds(3000));
				}
			}).detach();
		}

		static void OpenTextureSelect(std::function<void(Ref<Texture2D>)> onSelected)
		{
			s_onTextureSelected = onSelected;
			s_TextureSelect = true;
		}

		static void ShowTextureSelect()
		{
			
			if (ImGui::BeginPopupModal("textureSelection"))
			{
				static char search[31] = "";
				ImGui::Text("Search: ");
				ImGui::SameLine();
				ImGui::InputText("##search", search, 30);

				ImGui::BeginChild("items");

				for (auto& [uuid, tex] : ResourceRegistry::GetResources<Texture2D>())
				{
					if (ImGui::ImageButton(tex->GetTextureID(), ImVec2(50, 50)))
					{
						s_onTextureSelected(tex);
						ImGui::CloseCurrentPopup();
						break;
					}
				}

				ImGui::EndChild();

				ImGui::EndPopup();
			}
			if (s_TextureSelect) {
				ImGui::OpenPopup("textureSelection");
				s_TextureSelect = false;
			}
		}

		static void CheckValidity()
		{
			//for (auto& [path, _] : s_Textures)
			//{
			//	if(true) {}
			//	/*if (!std::filesystem::exists(path))
			//	{
			//		s_Textures.erase(path);
			//	}*/
			//}
			for (auto it = s_Textures.cbegin(); it != s_Textures.cend();)
			{
				if (!std::filesystem::exists(it->first))
				{
					s_Textures.erase(it++);
				}
				else
				{
					++it;
				}
			}
		}

		static void Rescan(std::filesystem::path dir = Project::GetAssetsPath())
		{
			for (auto& entry : std::filesystem::recursive_directory_iterator(dir))
			{
				if (entry.is_directory())
					Rescan(entry);
				else RegisterIfExists(entry);
			}
		}

		static void ShowRegistryWindow()
		{
			if (ImGui::Begin("Asset Registry", &s_RegistryWindowOpen))
			{
				static char search[31] = "";
				ImGui::Text("Search: ");
				ImGui::SameLine();
				ImGui::InputText("##search", search, 30);

				ImGui::BeginChild("items");

				auto test = ResourceRegistry::GetResources<Texture2D>();

				for (auto& [uuid, tex] : ResourceRegistry::GetResources<Texture2D>())
				{
					ImGui::Image(tex->GetTextureID(), ImVec2(50, 50));
				}

				ImGui::EndChild();

			}
			ImGui::End();
		}
	};

}