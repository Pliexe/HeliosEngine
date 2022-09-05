/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "Transform.h"
#include "Helios/Scene/SceneManager.h"
#include "Helios/GameObjects/Components/Camera.h"
#include "Helios/Application.h"

using Helios::Transform;

D2D1_RECT_F Transform::getRectF() {
	auto posOffset = getPosition();
	auto cam = SceneManager::currentScene->currentCamera;
	if (cam) posOffset = posOffset + cam->gameObject->getTransform().getPosition();
	return D2D1::RectF(posOffset.vec2[0], posOffset.vec2[1], posOffset.vec2[0] + size.vec2[0], posOffset.vec2[1] + size.vec2[1]);
}

Helios::Transform::Matrix2x2 Helios::Transform::getScreenPosition() const
{
	auto cam = SceneManager::currentScene->currentCamera;
	if (cam == NULL) throw std::logic_error("Can't get screen position if the Camera is missing!");

	Vector2D positionRelative = getPosition() - cam->gameObject->getTransform().getPosition();
	Vector2D size = getSize();

	auto aspectRatio = (WindowCordinates::getWidth() / WindowCordinates::getHeight());

	static auto viewport = 1000.0f;

	auto scale = (WindowCordinates::getHeight() / viewport);

	return {
		positionRelative.vec2[0] * scale + (WindowCordinates::getWidth() / 2.0f),
		positionRelative.vec2[1] * scale + (WindowCordinates::getHeight() / 2.0f),

		(size.vec2[0] + positionRelative.vec2[0]) * scale + (WindowCordinates::getWidth() / 2.0f),
		(size.vec2[1] + positionRelative.vec2[1]) * scale + (WindowCordinates::getHeight() / 2.0f)
	};
}