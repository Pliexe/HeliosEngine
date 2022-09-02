/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "Camera.h"
#include "Wallnut/Translation/Vector2D.h"
#include "Wallnut/Application.h"

Wallnut::Vector2D Wallnut::Camera::ScreenToWorldPoint(Point mousePosition)
{
    /*float tmp = (Wallnut::Application::instance->baseCanvas - (Wallnut::Application::instance->baseCanvas - (Wallnut::Application::instance->clientWidth + Wallnut::Application::instance->clientHeight))) / Wallnut::Application::instance->baseCanvas;
    return mousePosition * tmp + transform->getPosition();*/
    return { 0, 0 };
}