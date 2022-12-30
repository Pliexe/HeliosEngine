#pragma once

#include <Helios.h>
#include <Helios/GameObjects/ObjectBehaviour.h>

using namespace Helios;

class CameraMovement : public ObjectBehaviour {
public:

	float sensitivity = 155.0f;

	void Update() override {
		auto position = transform->getPosition();
		if (InputManager::IsKeyPressedDown(WN_KEY_A))
			position += (Vector2::left() * sensitivity * Time::deltaTime());
		if (InputManager::IsKeyPressedDown(WN_KEY_D))
			position += (Vector2::right() * sensitivity * Time::deltaTime());
		if (InputManager::IsKeyPressedDown(WN_KEY_W))
			position += (Vector2::up() * sensitivity * Time::deltaTime());
		if (InputManager::IsKeyPressedDown(WN_KEY_S))
			position += (Vector2::down() * sensitivity * Time::deltaTime());

		transform->setPosition(position);
	}

};