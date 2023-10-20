#pragma once
#include "pch.h"

#include "ApplicationStyle.h"
#include "AssetRegistry.h"
#include "LoadResources.h"
#include "ProjectManager.h"
#include "Helios/Core/Application.h"
#include "Helios/Graphics/Framebuffer.h"
#include "Helios/Resources/Texture.h"
#include "Panels/BasePanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/InspectorPanel.h"
#include "Helios/Graphics/GizmosRenderer.h"
#include <Platform/Windows/Win32GraphicalWindow.h>

#include "Icons.h"
#include "Panels/GameViewPanel.h"
#include "Panels/ScenePanel.h"


enum class EditorMode
{
	Editor,
	Paused,
	Playing
};


namespace Helios
{
	static Ref<Framebuffer> gameFrame;

	class HeliosEditor : public Application
	{
		bool isGameSceneActive = true;
		bool m_barebones = false;

		bool show_profiler_window = false;

		bool show_gizmos = false;

		GizmosRenderer::Tool current_tool = GizmosRenderer::Tool::None;

		struct GraphVertices
		{
			Vector2 pos;
			Color col;
		};

		enum class RSState
		{
			Normal,
			Wireframe,
		};

		InspectorPanel inspector;
		HierarchyPanel hierarchy;
		ScenePanel scenePanel;
		GameViewPanel gameViewPanel;

		std::vector<Editor::IPanel*> panels;

		RSState currentRSState = RSState::Normal;
	public:

#pragma region Icons
		inline static Ref<Texture2D> ICON_FOLDER = nullptr;
		inline static Ref<Texture2D> ICON_FOLDER_EMPTY = nullptr;
		inline static Ref<Texture2D> ICON_FILE_UNKNOWN = nullptr;
		inline static Ref<Texture2D> ICON_FILE_TXT = nullptr;
		inline static Ref<Texture2D> ICON_FILE_C = nullptr;
		inline static Ref<Texture2D> ICON_FILE_H = nullptr;
		inline static Ref<Texture2D> ICON_FILE_CPP = nullptr;
		inline static Ref<Texture2D> ICON_FILE_HPP = nullptr;
		inline static Ref<Texture2D> ICON_FILE_FONT = nullptr;
		inline static Ref<Texture2D> ICON_FILE_SCENE = nullptr;
		inline static Ref<Texture2D> ICON_FILE_IMAGE = nullptr;

		inline static Ref<Texture2D> ICON_PLAY_WHITE = nullptr;
		inline static Ref<Texture2D> ICON_PAUSE_WHITE = nullptr;
		inline static Ref<Texture2D> ICON_STOP_WHITE = nullptr;


		inline static Ref<Texture2D> ICON_UI_REFRESH_WHITE = nullptr;

		inline static Ref<Texture2D> ICON_UI_GROUP_TILES_WHITE = nullptr;
		inline static Ref<Texture2D> ICON_UI_GROUP_LIST_WHITE = nullptr;
		inline static Ref<Texture2D> ICON_UI_GROUP_ICONS_WHITE = nullptr;

		inline static Ref<Texture2D> ICON_UI_REFRESH_BLACK = nullptr;

		inline static Ref<Texture2D> ICON_UI_GROUP_TILES_BLACK = nullptr;
		inline static Ref<Texture2D> ICON_UI_GROUP_LIST_BLACK = nullptr;
		inline static Ref<Texture2D> ICON_UI_GROUP_ICONS_BLACK = nullptr;
		
#pragma endregion

		explicit HeliosEditor(const Specifications& specs);

		void OnUpdate() override;
		void OnRender() override;
		void OnGUI();

		friend class ScenePanel;
		friend class GameViewPanel;
	};
}
