#pragma once
//#include "pch.h"

// #include "Helios/Core/EntryPoint.h"
#include "ApplicationStyle.h"
#include "AssetRegistry.h"
#include "LoadResources.h"
#include "ProjectManager.h"
#include "Helios/Core/Application.h"
#include "Helios/Resources/Texture.h"
#include "Panels/BasePanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/InspectorPanel.h"
#include "Helios/Scripting/Rust/RustScriptsManager.h"

#include "Icons.h"
#include "Helios/Physics/Physics2D.h"
#include "Panels/GameViewPanel.h"
#include "Panels/ScenePanel.h"
#include "BuildSystem/BuildTask.h"
#include "BuildSystem/Steps/NativeDotNet.h"

#include <FileWatch.hpp>


enum class EditorMode
{
	Editor,
	Paused,
	Playing
};


namespace Helios
{
	static EditorMode currentMode = EditorMode::Editor;
	class HeliosEditor : public Application
	{
		BuildTask m_buildDotNetScriptsTask = BuildTask("DotNetScripts", {
			BuildStep { .name = "DotNet", .action = EditorDotNetBuildAction<true> }
		});
		
		Unique<RustScriptsManager> rustScriptsManager = nullptr;

		Unique<filewatch::FileWatch<std::string>> watch;

		bool isGameSceneActive = true;
		bool m_barebones = false;
		bool m_perFrameDebug = false;

		bool show_profiler_window = false;

		bool show_gizmos = false;

		bool rebuild_dotnet = false;

		// GizmosRenderer::Tool current_tool = GizmosRenderer::Tool::None;

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
		void RebuildNextFrame() { rebuild_dotnet = true; }

		static Ref<Framebuffer> GetGameFrame() { return static_cast<HeliosEditor&>(GetInstance()).gameFrame; };

		Ref<Framebuffer> gameFrame;

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
		inline static Ref<Texture2D> ICON_FILE_OBJECT = nullptr;
		inline static Ref<Texture2D> ICON_FILE_RUST = nullptr;
		inline static Ref<Texture2D> ICON_FILE_CSHARP = nullptr;

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

		~HeliosEditor()
		{
			ICON_FOLDER.reset();
			ICON_FOLDER_EMPTY.reset();
			ICON_FILE_UNKNOWN.reset();
			ICON_FILE_TXT.reset();
			ICON_FILE_C.reset();
			ICON_FILE_H.reset();
			ICON_FILE_CPP.reset();
			ICON_FILE_HPP.reset();
			ICON_FILE_FONT.reset();
			ICON_FILE_SCENE.reset();
			ICON_FILE_IMAGE.reset();
			ICON_FILE_OBJECT.reset();
			ICON_FILE_RUST.reset();
			ICON_FILE_CSHARP.reset();

			ICON_PLAY_WHITE.reset();
			ICON_PAUSE_WHITE.reset();
			ICON_STOP_WHITE.reset();


			ICON_UI_REFRESH_WHITE.reset();

			ICON_UI_GROUP_TILES_WHITE.reset();
			ICON_UI_GROUP_LIST_WHITE.reset();
			ICON_UI_GROUP_ICONS_WHITE.reset();

			ICON_UI_REFRESH_BLACK.reset();

			ICON_UI_GROUP_TILES_BLACK.reset();
			ICON_UI_GROUP_LIST_BLACK.reset();
			ICON_UI_GROUP_ICONS_BLACK.reset();
		}
		
#pragma endregion

		static void OnEnterPlaymode()
		{
			SceneRegistry::Backup();
			Physics2D::Init(Physics2D::ImplementationType::Box2D);
		}

		static void OnExitPlaymode()
		{
			Physics2D::Shutdown();
			SceneRegistry::Restore();
		}

		explicit HeliosEditor(const Specifications& specs);
		bool BuildDotNet();
		void RebuildCS();

		void OnUpdate() override;
		void OnRender(NativeCommandList& cmdList) override;
		void OnGUI();

		friend class ScenePanel;
		friend class GameViewPanel;
	};
}
