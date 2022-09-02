#pragma once

//#ifdef WALLNUT_EDITOR
//void OnGUI() override {
//	using namespace WallnutEditor;
//
//	if (ImGui::CollapsingHeader("Ellipse")) {
//		auto size = transform->getSize();
//		EditorGUI::DragFloat("Stroke", &stroke, 0.0f, 0.0f, ((size.width() < size.height()) ? size.height() : size.width()) / 2.0f);
//
//		switch (m_brush->getType())
//		{
//		case Brush::BrushType::Solid:
//			EditorGUI::Color((ID2D1SolidColorBrush*)(ID2D1Brush*)(*m_brush));
//			break;
//		}
//	}
//}
//#endif // WALLNUT_EDITOR