/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/HeliosEngine/blob/master/LICENSE.txt
 */
#include "Time.h"

namespace Helios
{
#pragma region Static Variables
    static std::chrono::high_resolution_clock::time_point FIRST_FRAME;
    static std::chrono::high_resolution_clock::time_point LAST_FRAME;
    static std::chrono::duration<float> DELTA_TIME;
#pragma endregion

#pragma region Private Functions

    void Time::internal_init()
    {
        FIRST_FRAME = std::chrono::high_resolution_clock::now();
        LAST_FRAME = FIRST_FRAME;
        DELTA_TIME = std::chrono::duration<float>(0.0f);
    }

    void Time::internal_frame_update()
    {
        auto now = std::chrono::high_resolution_clock::now();
        DELTA_TIME = now - LAST_FRAME;
        LAST_FRAME = now;
    }

#pragma endregion

#pragma region Public Functions
    float Time::DeltaTime() { return DELTA_TIME.count(); }
    float Time::FramesPerSecond() { return 1.0f / DELTA_TIME.count(); }
    std::chrono::high_resolution_clock::time_point Time::LastFrame() { return LAST_FRAME; }
    double Time::PassedTime() { return std::chrono::duration<double>(LAST_FRAME - FIRST_FRAME).count(); }
    unsigned long long Time::CurrentTimeMicroseconds() { return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); }
#pragma endregion
}

#pragma region C Interface
extern "C"
{
    HELIOS_EXPORT float helios_time_delta_time() { return Helios::Time::DeltaTime(); }
    HELIOS_EXPORT float helios_time_frames_per_second() { return Helios::Time::FramesPerSecond(); }
    // HELIOS_EXPORT std::chrono::high_resolution_clock::time_point Helios_Time_LastFrame() { return Helios::Time::LastFrame(); }
    HELIOS_EXPORT double helios_time_passed_time() { return Helios::Time::PassedTime(); }
    HELIOS_EXPORT unsigned long long helios_time_current_time_microseconds() { return Helios::Time::CurrentTimeMicroseconds(); }
}
#pragma endregion