#pragma once

#include <Wallnut.h>
#include <Wallnut/GameObjects/ObjectBehaviour.h>

using namespace Wallnut;

class CameraMovement : public ObjectBehaviour {
public:

	float speed = 155.0f;

	void Update() override {
		auto position = transform->getPosition();
		if (InputManager::IsKeyPressedDown(WN_KEY_A))
			position += (Vector2D::left() * speed * Time::deltaTime());
		if (InputManager::IsKeyPressedDown(WN_KEY_D))
			position += (Vector2D::right() * speed * Time::deltaTime());
		if (InputManager::IsKeyPressedDown(WN_KEY_W))
			position += (Vector2D::up() * speed * Time::deltaTime());
		if (InputManager::IsKeyPressedDown(WN_KEY_S))
			position += (Vector2D::down() * speed * Time::deltaTime());

		transform->setPosition(position);
	}

};