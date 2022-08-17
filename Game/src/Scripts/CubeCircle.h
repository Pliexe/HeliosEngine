#pragma once

#include <Wallnut.h>
#include <Wallnut/GameObjects/ObjectBehaviour.h>

#include <cmath>

using namespace Wallnut;

class CubeCircle : public ObjectBehaviour {
public:

	long float time = 0.0f;
	float radius = 40.0f;
	float speed = 15.0f;

	void Update() override {

		time += Time::deltaTime() * speed;

		auto vec = Vector2D(sin(time), cos(time)) * radius;

		std::cout << "Vec: x: " << vec.x() << ", y: " << vec.y() << std::endl;

		transform->setPosition(vec);
	}

};