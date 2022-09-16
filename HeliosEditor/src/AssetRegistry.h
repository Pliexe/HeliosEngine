#pragma once

#include <mutex>
#include <chrono>
#include <future>
#include <filesystem>

#include "Helios/Core/Asserts.h"

#include "ProjectManager.h"
#include "Helios/Resources/Texture.h"
#include "imgui.h"

namespace Helios
{
	class AssetRegistry
	{
	private:
		static std::unordered_map<std::string, Ref<Texture2D>> s_Textures;

		static void RegisterIfExists(std::filesystem::path file)
		{
			auto ext = file.extension().string();

			if (ext == ".png")
			{
				if (std::filesystem::exists(file))
				{
					if (s_Textures.find(file.string()) == s_Textures.end())
					{
						try {
							s_Textures[file.string()] = Texture2D::Create(file.string());
						}
						catch (HeliosExceptin ex)
						{
							ex.what(false);
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

				for (auto& [path, tex] : s_Textures)
				{
					if (ImGui::ImageButton(tex->GetTextureID(), ImVec2(50, 50)))
					{
						s_onTextureSelected(tex);
						ImGui::CloseCurrentPopup();
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
			for (auto& [path, _] : s_Textures)
			{
				if(true) {}
				/*if (!std::filesystem::exists(path))
				{
					s_Textures.erase(path);
				}*/
			}
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

				for (auto& [path, tex] : s_Textures)
				{
					ImGui::Image(tex->GetTextureID(), ImVec2(50, 50));
				}

				ImGui::EndChild();

			}
			ImGui::End();
		}
	};

}