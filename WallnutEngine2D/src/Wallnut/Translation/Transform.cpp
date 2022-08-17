/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "Transform.h"
#include "Wallnut/Scene/SceneManager.h"
#include "Wallnut/GameObjects/Components/Camera.h"

using Wallnut::Transform;

D2D1_RECT_F Transform::getRectF() {
	auto posOffset = getPosition();
	auto cam = SceneManager::currentScene->currentCamera;
	if (cam) posOffset = posOffset + cam->gameObject->getTransform().getPosition();
	return D2D1::RectF(posOffset.m_x, posOffset.m_y, posOffset.m_x + size.m_x, posOffset.m_y + size.m_y);
}