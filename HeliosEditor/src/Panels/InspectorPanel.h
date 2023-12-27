#pragma once

#include "BasePanel.h"
#include <Helios/Scene/Entity.h>
#include "pch.h"
#include "entt.hpp"

#include <any>

namespace Helios {
	class InspectorPanel : public Editor::IPanel
	{
	public:
		enum class SelectedType {
			None,
			Entity
		};
	private:

		std::any handle;
		SelectedType type = SelectedType::None;
		bool focus_next_name_input = false;

		static InspectorPanel* instance;

	public:

		static InspectorPanel& GetInstance() { return *instance; }
		void CheckIfValid()
		{
			if (SelectedType::Entity == type) if (SelectedType::Entity == type)
			{
				if (!std::any_cast<Entity>(handle).IsValid())
				{
					Reset();
				}
			}
		}

		const std::any GetHandle() { CheckIfValid(); return handle; }
		const SelectedType GetType() { CheckIfValid(); return type; }

		static void Reset() { instance->handle = nullptr; instance->type = SelectedType::None; }

		template <typename T>
		typename std::enable_if<std::is_same<T, Entity>::value, bool>::type
		operator == (T anything) {
			if (type != SelectedType::Entity) return false;
			return std::any_cast<T>(handle).GetHandle() == anything.GetHandle();
		}

		template <typename type>
		bool operator != (type anything) {
			return *this != anything;
		}

		template <typename T>
		typename std::enable_if<std::is_same<T, Entity>::value, InspectorPanel&>::type
		operator << (T entity) {
			this->type = SelectedType::Entity;
			this->handle = entity;
			return *this;
		}

		void FocusRename(Entity entity) {
			this->type = SelectedType::Entity;
			this->handle = entity;
			focus_next_name_input = true;
		}

		static void FocusMain()
		{
			ImGui::SetWindowFocus("Inspector###inspector_main");
		}

		InspectorPanel(std::string id = "main") { if (instance == nullptr) instance = this; title = "Inspector###inspector_" + id; }
		virtual void OnUpdate() override;
	};
}