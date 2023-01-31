#pragma once

#include "BasePanel.h"
#include <Helios/Scene/GameObject.h>
#include "pch.h"
#include "entt.hpp"

#include <any>

namespace Helios {
	class InspectorPanel : public Editor::IPanel
	{
	private:
		enum class SelectedType;

		std::any handle;
		SelectedType type = SelectedType::None;
		bool focus_next_name_input = false;

		static InspectorPanel* instance;

	public:

		static InspectorPanel& GetInstance() { return *instance; }

		const std::any GetHandle() { return handle; }
		const SelectedType GetType() { return type; }

		static void Reset() { instance->handle = nullptr; instance->type = SelectedType::None; }

		enum class SelectedType {
			None,
			GameObject
		};

		template <typename T>
		typename std::enable_if<std::is_same<T, entt::entity>::value, bool>::type
		operator == (T anything) {
			if (type != SelectedType::GameObject) return false;
			return std::any_cast<T>(handle) == anything;
		}

		template <typename type>
		bool operator != (type anything) {
			return *this != anything;
		}

		template <typename T>
		typename std::enable_if<std::is_same<T, entt::entity>::value, InspectorPanel&>::type
		operator << (T entity) {
			this->type = SelectedType::GameObject;
			this->handle = entity;
			return *this;
		}

		void FocusRename(entt::entity gameObject) {
			this->type = SelectedType::GameObject;
			this->handle = gameObject;
			focus_next_name_input = true;
		}

		InspectorPanel(std::string id = "main") { if (instance == nullptr) instance = this; title = "Inspector###inspector_" + id; }
		virtual void OnUpdate() override;
	};
}