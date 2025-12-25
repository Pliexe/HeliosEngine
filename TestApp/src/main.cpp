#include <iostream>

#include <Helios/Core/UUID.h>
#include <Helios.h>
#include "Utils.h"

Helios::Application *Helios::CreateApplication(int argc, char **argv)
{
	return nullptr;
}

int main(int, char **)
{
    std::cout << "Starting UnitTests:" << std::endl;

    // CRITICAL_TEST(Helios::UUID() != Helios::UUID());

    Helios::Quaternion q;
    q.x = 0;
    q.y = 0;
    q.z = 0;
    q.w = 1;

    auto forward = q.forward();
    auto right = q.right();
    auto up = q.up();

    std::cout << "Forward: " << forward.to_string() << std::endl;
    std::cout << "Right: " << right.to_string() << std::endl;
    std::cout << "Up: " << up.to_string() << std::endl;

    assert(forward == Helios::Vector3(0, 0, 1));
    assert(right == Helios::Vector3(1, 0,  0));
    assert(up == Helios::Vector3(0, 1,  0));

    std::cout << "Tests passed!" << std::endl;

    return 0;
}