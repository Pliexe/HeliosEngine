#pragma once

#include "pch.h"

#include <Helios/Math/Vector.h>
#include <Helios/Math/Quaternion.h>
#include <Helios/Resources/Texture.h>

namespace ImGui {
	
	bool EditVector2(const char* label, float width, Helios::Vector2& vec, float speed = 0.1f, float min = 0.0f, float max = 0.0f);
	bool EditVector3(const char* label, float width, Helios::Vector3& vec, float speed = 0.1f, float min = 0.0f, float max = 0.0f);

	void EditQuanterionEuler(const char* label, float width, Helios::Quaternion& quanterion, float speed = 0.1f, float min = 0.0f, float max = 0.0f);

	void EditQuanterion(const char* label, Helios::Quaternion& quanterion, float speed = 0.1f, float min = 0.0f, float max = 0.0f);

	IMGUI_API void Image(Helios::Texture2D& texture, const ImVec2& image_size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1));
    IMGUI_API void ImageWithBg(Helios::Texture2D& texture, const ImVec2& image_size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));
    IMGUI_API bool ImageButton(const char* str_id, Helios::Texture2D& texture, const ImVec2& image_size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

	IMGUI_API void Image(Helios::Ref<Helios::Texture2D>& texture, const ImVec2& image_size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1));
    IMGUI_API void ImageWithBg(Helios::Ref<Helios::Texture2D>& texture, const ImVec2& image_size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));
    IMGUI_API bool ImageButton(const char* str_id, Helios::Ref<Helios::Texture2D>& texture, const ImVec2& image_size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));
	
}
