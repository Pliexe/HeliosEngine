//#include "pch.h"

#include <Helios/EditorGUI.h>
#include <Helios/Translation//Vector2D.h>
#include <Helios/Resources/Brushes/SolidBrush.h>

//using namespace HeliosEditor;
//
//void EditorGUI::Color(ID2D1SolidColorBrush* brush)
//{
//	auto color = brush->GetColor();
//	float c[4] = { color.r, color.g, color.b, color.a };
//	if (ImGui::ColorEdit4("Color", c))
//		brush->SetColor({ c[0], c[1], c[2], c[3] });
//}
//
//void EditorGUI::Color(Helios::SolidBrush& brush)
//{
//	auto color = brush->GetColor();
//	float c[4] = { color.r, color.g, color.b, color.a };
//	if (ImGui::ColorEdit4("Color", c))
//		brush->SetColor({ c[0], c[1], c[2], c[3] });
//}
//
//void EditorGUI::DragVector2D(const char* label, Helios::Vector2D* vec)
//{
//	float c[2] = { vec->x(), vec->y() };
//	if (ImGui::DragFloat2(label, c))
//		*vec = { c[0], c[1] };
//}
//
//void EditorGUI::DragVector2D(const char* label, Helios::Vector2D& vec)
//{
//	float c[2] = { vec.x(), vec.y() };
//	if (ImGui::DragFloat2(label, c))
//		vec.setXY(c[0], c[1]);
//}
//
//void EditorGUI::DragFloat(const char* label, float* v, float v_speed, float v_min, float v_max)
//{
//	ImGui::DragFloat(label, v, v_speed, v_min, v_max);
//}