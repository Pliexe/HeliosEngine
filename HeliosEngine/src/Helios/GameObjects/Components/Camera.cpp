/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "Camera.h"
#include "Helios/Translation/Vector2D.h"
#include "Helios/Application.h"

Helios::Vector2D Helios::Camera::ScreenToWorldPoint(Point mousePosition)
{
    /*float tmp = (Helios::Application::instance->baseCanvas - (Helios::Application::instance->baseCanvas - (Helios::Application::instance->clientWidth + Helios::Application::instance->clientHeight))) / Helios::Application::instance->baseCanvas;
    return mousePosition * tmp + transform->getPosition();*/
    return { 0, 0 };
}