#pragma once

#include <Helios.h>
#include <Helios/GameObjects/ObjectBehaviour.h>

#include <cmath>

using namespace Helios;

class CubeCircle : public ObjectBehaviour {
public:

	long float time = 0.0f;
	float radius = 40.0f;
	float speed = 15.0f;

	void Update() override {

		time += Time::deltaTime() * speed;

		auto vec = (Vector2(sin(time), cos(time)) * radius);

		transform->setPosition(vec);
	}

};