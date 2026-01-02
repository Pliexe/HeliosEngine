#include <iostream>

#include <Helios/Core/UUID.h>
#include <Helios.h>
#include "Helios/Math/VectorImpl.h"
#include "Utils.h"

Helios::Application *Helios::CreateApplication(int argc, char **argv)
{
	return nullptr;
}

namespace std
{
    template <typename T, std::size_t S, bool IsVector>
    inline std::string to_string(const Helios::TVector<T, S, IsVector>& vector) {
        std::ostringstream ss;
        ss << vector.to_string();
        return ss.str();
    }

    inline std::string to_string(const Helios::UUID& uuid) {
        std::ostringstream ss;
        ss << uuid.toString();
        return ss.str();
    }
}

int main(int, char **)
{
    TITLE("Starting UnitTests");

    TITLE("UUID Tests");

    CRITICAL_TEST(Helios::UUID() != Helios::UUID());
    
    Helios::UUID uuid = Helios::UUID();
    Helios::UUID uuid2 = Helios::UUID();
    Helios::UUID uuid3 = Helios::UUID(uuid);

    std::cout << "UUID: " << uuid.toString() << std::endl;
    std::cout << "UUID2: " << uuid2.toString() << std::endl;
    std::cout << "UUID3: " << uuid3.toString() << std::endl;

    CRITICAL_TEST_EQUALS(uuid, uuid);
    CRITICAL_TEST_NOT_EQUALS(uuid, uuid2);
    CRITICAL_TEST_EQUALS(uuid, uuid3);

    uint8_t data[16];
    std::memcpy(data, (uint8_t*)uuid, 16);

    Helios::UUID uuid4 = Helios::UUID::fromString(uuid.toString());
    CRITICAL_TEST(uuid4 == uuid);

    TITLE("Quaternion Tests");

    Helios::Quaternion q;
    q.x = 0;
    q.y = 0;
    q.z = 0;
    q.w = 1;

    CRITICAL_TEST(q.forward() == Helios::Vector3(0, 0, 1));
    CRITICAL_TEST(q.right() == Helios::Vector3(1, 0,  0));
    CRITICAL_TEST(q.up() == Helios::Vector3(0, 1,  0));

    TITLE("Vector Tests");

    TITLE("Vector2 Equality Tests");

    CRITICAL_TEST(Helios::Vector2(1, 0) == Helios::Vector2(1, 0));
    CRITICAL_TEST(Helios::Vector2(0, 1) == Helios::Vector2(0, 1));
    CRITICAL_TEST(Helios::Vector2(-1, 0) == Helios::Vector2(-1, 0));
    CRITICAL_TEST(Helios::Vector2(0, -1) == Helios::Vector2(0, -1));

    TITLE("Vector3 Equality Tests");
    
    CRITICAL_TEST(Helios::Vector3(1, 0, 0) == Helios::Vector3(1, 0, 0));
    CRITICAL_TEST(Helios::Vector3(0, 1, 0) == Helios::Vector3(0, 1, 0));
    CRITICAL_TEST(Helios::Vector3(0, 0, 1) == Helios::Vector3(0, 0, 1));
    CRITICAL_TEST(Helios::Vector3(-1, 0, 0) == Helios::Vector3(-1, 0, 0));
    CRITICAL_TEST(Helios::Vector3(0, -1, 0) == Helios::Vector3(0, -1, 0));
    CRITICAL_TEST(Helios::Vector3(0, 0, -1) == Helios::Vector3(0, 0, -1));

    TITLE("Vector4 Equality Tests");

    CRITICAL_TEST(Helios::Vector4(1, 0, 0, 0) == Helios::Vector4(1, 0, 0, 0));
    CRITICAL_TEST(Helios::Vector4(0, 1, 0, 0) == Helios::Vector4(0, 1, 0, 0));
    CRITICAL_TEST(Helios::Vector4(0, 0, 1, 0) == Helios::Vector4(0, 0, 1, 0));
    CRITICAL_TEST(Helios::Vector4(0, 0, 0, 1) == Helios::Vector4(0, 0, 0, 1));
    CRITICAL_TEST(Helios::Vector4(-1, 0, 0, 0) == Helios::Vector4(-1, 0, 0, 0));
    CRITICAL_TEST(Helios::Vector4(0, -1, 0, 0) == Helios::Vector4(0, -1, 0, 0));
    CRITICAL_TEST(Helios::Vector4(0, 0, -1, 0) == Helios::Vector4(0, 0, -1, 0));
    CRITICAL_TEST(Helios::Vector4(0, 0, 0, -1) == Helios::Vector4(0, 0, 0, -1));

    TITLE("Vector3 Constants Tests");

    CRITICAL_TEST_EQUALS(Helios::Vector3::Forward(), Helios::Vector3(0, 0, 1));
    CRITICAL_TEST_EQUALS(Helios::Vector3::Back(), Helios::Vector3(0, 0, -1));
    CRITICAL_TEST_EQUALS(Helios::Vector3::Right(), Helios::Vector3(1, 0, 0));
    CRITICAL_TEST_EQUALS(Helios::Vector3::Up(), Helios::Vector3(0, 1, 0));
    CRITICAL_TEST_EQUALS(Helios::Vector3::Left(), Helios::Vector3(-1, 0, 0));
    CRITICAL_TEST_EQUALS(Helios::Vector3::Down(), Helios::Vector3(0, -1, 0));
    CRITICAL_TEST_EQUALS(Helios::Vector3::One(), Helios::Vector3(1, 1, 1));
    CRITICAL_TEST_EQUALS(Helios::Vector3::Zero(), Helios::Vector3(0, 0, 0));

    TITLE("Vector2 Constants Tests");

    CRITICAL_TEST_EQUALS(Helios::Vector2::Up(), Helios::Vector2(0, 1));
    CRITICAL_TEST_EQUALS(Helios::Vector2::Down(), Helios::Vector2(0, -1));
    CRITICAL_TEST_EQUALS(Helios::Vector2::Right(), Helios::Vector2(1, 0));
    CRITICAL_TEST_EQUALS(Helios::Vector2::Left(), Helios::Vector2(-1, 0));
    CRITICAL_TEST_EQUALS(Helios::Vector2::One(), Helios::Vector2(1, 1));
    CRITICAL_TEST_EQUALS(Helios::Vector2::Zero(), Helios::Vector2(0, 0));

    {
        TITLE("Vector operations Tests");

        auto v1 = Helios::Vector3(1, 0, 0);
        auto v2 = Helios::Vector3(0, 1, 0);
        auto v3 = Helios::Vector3(10, 5, 15);
        auto v4 = 5.f;
        auto v5 = Helios::Vector3(2, 0, 0);
        
        CRITICAL_TEST_EQUALS(v1 + v2, Helios::Vector3(1, 1, 0));
        CRITICAL_TEST_EQUALS(v1 - v2, Helios::Vector3(1, -1, 0));
        CRITICAL_TEST_EQUALS(v1 * v4, Helios::Vector3(5, 0, 0));
        CRITICAL_TEST_EQUALS(v1 / v4, Helios::Vector3(0.2f, 0, 0));

        CRITICAL_TEST_EQUALS(v3 + v3, Helios::Vector3(20, 10, 30));
        CRITICAL_TEST_EQUALS(v3 - v3, Helios::Vector3(0, 0, 0));
        CRITICAL_TEST_EQUALS(v3 * v4, Helios::Vector3(50, 25, 75));
        CRITICAL_TEST_EQUALS(v3 / v4, Helios::Vector3(2, 1, 3));

        CRITICAL_TEST_EQUALS(v1.magnitude(), 1);
        CRITICAL_TEST_EQUALS(v2.magnitude(), 1);
        CRITICAL_TEST_EQUALS(v3.magnitude(), std::sqrt(15.f * 15.f + 5.f * 5.f + 10.f * 10.f));

        CRITICAL_TEST_EQUALS(v1.normalized(), v1 / v1.magnitude());
        CRITICAL_TEST_EQUALS(v2.normalized(), v2 / v2.magnitude());
        CRITICAL_TEST_EQUALS(v3.normalized(), v3 / v3.magnitude());

        CRITICAL_TEST_EQUALS(Helios::Vector3::Distance(v1, v2), (v1 - v2).magnitude());
        CRITICAL_TEST_EQUALS(Helios::Vector3::Distance(v1, v3), (v1 - v3).magnitude());
        CRITICAL_TEST_EQUALS(Helios::Vector3::Distance(v2, v3), (v2 - v3).magnitude());

        CRITICAL_TEST_EQUALS(Helios::Vector3::Dot(v1, v2), 0);
        CRITICAL_TEST_EQUALS(Helios::Vector3::Dot(v1, v3), 10);
        CRITICAL_TEST_EQUALS(Helios::Vector3::Dot(v2, v3), 5);
        CRITICAL_TEST_EQUALS(Helios::Vector3::Dot(v3, v3), 15.f * 15.f + 5.f * 5.f + 10.f * 10.f);
        
        CRITICAL_TEST_EQUALS(Helios::Vector3::Scale(v1, v2), Helios::Vector3(0, 0, 0));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Scale(v1, v3), Helios::Vector3(10, 0, 0));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Scale(v2, v3), Helios::Vector3(0, 5, 0));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Scale(v3, v3), Helios::Vector3(v3.x * v3.x, v3.y * v3.y, v3.z * v3.z));

        CRITICAL_TEST_EQUALS(Helios::Vector3::Divide(v1, v3), Helios::Vector3(v1.x / v3.x, v1.y / v3.y, v1.z / v3.z));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Divide(v2, v3), Helios::Vector3(v2.x / v3.x, v2.y / v3.y, v2.z / v3.z));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Divide(v3, v3), Helios::Vector3(v3.x / v3.x, v3.y / v3.y, v3.z / v3.z));

        CRITICAL_TEST_EQUALS(Helios::Vector3::MoveTowards(v1, v5, 0.5f), Helios::Vector3(1 + 0.5f, 0, 0));
        CRITICAL_TEST_EQUALS(Helios::Vector3::MoveTowards(v1, v5, 1.5f), Helios::Vector3(1 + 1.5f, 0, 0));
        CRITICAL_TEST_EQUALS(Helios::Vector3::MoveTowards(v1, v5, 2.5f), Helios::Vector3(1 + 2.5f, 0, 0));

        CRITICAL_TEST_EQUALS(Helios::Vector3::Lerp(v1, v2, 0.5f), Helios::Vector3(0.5f, 0.5f, 0));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Lerp(v1, v2, 1.5f), v2);
        CRITICAL_TEST_EQUALS(Helios::Vector3::Lerp(v1, v2, 2.5f), v2);
        CRITICAL_TEST_EQUALS(Helios::Vector3::Lerp(v1, v3, 0.5f), Helios::Vector3(5.5f, 2.5f, 7.5f));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Lerp(v1, v3, 1.5f), v3);
        CRITICAL_TEST_EQUALS(Helios::Vector3::Lerp(v1, v3, 2.5f), v3);

        CRITICAL_TEST_EQUALS(Helios::Vector3::LerpUnclamped(v1, v2, 0.5f), Helios::Vector3(0.5f, 0.5f, 0));
        CRITICAL_TEST_EQUALS(Helios::Vector3::LerpUnclamped(v1, v2, 1.5f), v2 + (v2 - v1) * 0.5f);
        CRITICAL_TEST_EQUALS(Helios::Vector3::LerpUnclamped(v1, v2, 2.5f), v2 + (v2 - v1) * 1.5f);
        CRITICAL_TEST_EQUALS(Helios::Vector3::LerpUnclamped(v1, v2, 3.5f), v2 + (v2 - v1) * 2.5f);
        CRITICAL_TEST_EQUALS(Helios::Vector3::LerpUnclamped(v1, v3, 0.5f), Helios::Vector3(5.5f, 2.5f, 7.5f));
        CRITICAL_TEST_EQUALS(Helios::Vector3::LerpUnclamped(v1, v3, 1.5f), v3 + (v3 - v1) * 0.5f);
        CRITICAL_TEST_EQUALS(Helios::Vector3::LerpUnclamped(v1, v3, 2.5f), v3 + (v3 - v1) * 1.5f);
        CRITICAL_TEST_EQUALS(Helios::Vector3::LerpUnclamped(v1, v3, 3.5f), v3 + (v3 - v1) * 2.5f);

        CRITICAL_TEST_EQUALS(Helios::Vector3::Reflect(v1, v2), v1 - v2 * 2.f * Helios::Vector3::Dot(v1, v2));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Reflect(v1, v3), v1 - v3 * 2.f * Helios::Vector3::Dot(v1, v3));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Reflect(v2, v3), v2 - v3 * 2.f * Helios::Vector3::Dot(v2, v3));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Reflect(v3, v3), v3 - v3 * 2.f * Helios::Vector3::Dot(v3, v3));

        CRITICAL_TEST_EQUALS(Helios::Vector3::Max(v1, v2), Helios::Vector3(1, 1, 0));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Max(v1, v3), v3);
        CRITICAL_TEST_EQUALS(Helios::Vector3::Max(v2, v3), v3);
        CRITICAL_TEST_EQUALS(Helios::Vector3::Max(v3, v3), v3);

        CRITICAL_TEST_EQUALS(Helios::Vector3::Min(v1, v2), Helios::Vector3(0, 0, 0));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Min(v1, v3), Helios::Vector3(1, 0, 0));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Min(v2, v3), Helios::Vector3(0, 1, 0));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Min(v3, v3), v3);

        CRITICAL_TEST_EQUALS(Helios::Vector3::Clamp(v1, v2, v3), Helios::Vector3(1, 1, 0));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Clamp(v2, v1, v3), Helios::Vector3(1, 1, 0));

        CRITICAL_TEST_EQUALS(Helios::Vector3::ClampMagnitude(v1, 1), v1);
        CRITICAL_TEST_EQUALS(Helios::Vector3::ClampMagnitude(v2, 1), v2);
        CRITICAL_TEST_EQUALS(Helios::Vector3::ClampMagnitude(v3, 1), Helios::Vector3(1, 1, 1));
        CRITICAL_TEST_EQUALS(Helios::Vector3::ClampMagnitude(v3, 2), Helios::Vector3(2, 2, 2));
        CRITICAL_TEST_EQUALS(Helios::Vector3::ClampMagnitude(v3, 15), v3);

        CRITICAL_TEST_EQUALS(Helios::Vector3::Project(v1, v2), Helios::Vector3(0, 0, 0));
        CRITICAL_TEST_APPROX(Helios::Vector3::Project(v1, v3), Helios::Vector3(0.285714f, 0.142857f, 0.428571f));
        CRITICAL_TEST_APPROX(Helios::Vector3::Project(v2, v3), Helios::Vector3(0.142857f, 0.071429f, 0.214286f));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Project(v3, v3), v3);

        CRITICAL_TEST_APPROX(Helios::Vector3::ProjectOnPlane(v1, v2, v3), Helios::Vector3(10, 0, 15));
        CRITICAL_TEST_APPROX(Helios::Vector3::ProjectOnPlane(v1, v3, v3), Helios::Vector3(0.2857142857, 0.1428571429, 0.4285714286));
        CRITICAL_TEST_APPROX(Helios::Vector3::ProjectOnPlane(v2, v3, v3), Helios::Vector3(0.1428571429, 0.0714285714, 0.2142857143));
        CRITICAL_TEST_APPROX(Helios::Vector3::ProjectOnPlane(v3, v3, v3), v3);

        auto rayOrigin = Helios::Vector3(1,2,10);
        auto rayDirection = Helios::Vector3(0,0,-1);
        auto planeNormal = Helios::Vector3(0,0,1);
        auto planePoint = Helios::Vector3(0,0,0);

        CRITICAL_TEST_APPROX(
            Helios::Vector3::ProjectRayOnPlane(rayOrigin, rayDirection, planeNormal, planePoint),
            Helios::Vector3(1,2,20)
        );

        rayOrigin = Helios::Vector3(5,10,3);
        rayDirection = Helios::Vector3(0,-1,0);
        planeNormal = Helios::Vector3(0,1,0);
        planePoint = Helios::Vector3(0,0,0);

        CRITICAL_TEST_APPROX(
            Helios::Vector3::ProjectRayOnPlane(rayOrigin, rayDirection, planeNormal, planePoint),
            Helios::Vector3(5,20,3)
        );

        rayOrigin = Helios::Vector3(0,0,0);
        rayDirection = Helios::Vector3(1,1,1);
        planeNormal = Helios::Vector3(0,0,1);
        planePoint = Helios::Vector3(0,0,5);

        CRITICAL_TEST_APPROX(
            Helios::Vector3::ProjectRayOnPlane(rayOrigin, rayDirection, planeNormal, planePoint),
            Helios::Vector3(-5,-5,-5)
        );

        rayOrigin = Helios::Vector3(1,2,5);
        rayDirection = Helios::Vector3(1,0,0);
        planeNormal = Helios::Vector3(0,0,1);
        planePoint = Helios::Vector3(0,0,5);

        Helios::Vector3 result;

        CRITICAL_TEST_EQUALS(
            (result = Helios::Vector3::ProjectRayOnPlane(rayOrigin, rayDirection, planeNormal, planePoint), true),
            Helios::Vector3::IsNaN(result)
        );


        TITLE("Vector3 operations Tests");

        CRITICAL_TEST_EQUALS(Helios::Vector3::Angle(v1, v2), std::atan2(v2.y, v2.x) - std::atan2(v1.y, v1.x));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Angle(v1, v3), std::atan2(Helios::Vector3::Cross(v1, v3).magnitude(), Helios::Vector3::Dot(v1, v3)));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Angle(v2, v3), std::atan2(Helios::Vector3::Cross(v2, v3).magnitude(), Helios::Vector3::Dot(v2, v3)));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Angle(v3, v3), 0);

        CRITICAL_TEST_EQUALS(Helios::Vector3::Cross(v1, v2), Helios::Vector3(0, 0, 1));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Cross(v1, v3), Helios::Vector3(0, -15, 5));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Cross(v2, v3), Helios::Vector3(15, 0, -10));
        CRITICAL_TEST_EQUALS(Helios::Vector3::Cross(v3, v3), Helios::Vector3(0, 0, 0));
        
    }

    return 0;
}