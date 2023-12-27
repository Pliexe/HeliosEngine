/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Base.h"

namespace Helios {
    class HELIOS_API Time {
    private:
        inline static std::chrono::high_resolution_clock::time_point m_firstFrame;
        inline static std::chrono::high_resolution_clock::time_point m_lastFrame;
        inline static std::chrono::duration<float> m_deltaTime;

        static void Init() {
            m_firstFrame = std::chrono::high_resolution_clock::now();
            frameUpdate();
        }

        static void frameUpdate() {
            auto now = std::chrono::high_resolution_clock::now();
            m_deltaTime = now - m_lastFrame;
            m_lastFrame = now;
        }

    public:
        static float deltaTime() { return m_deltaTime.count(); }
        static float getFPS() { return 1.0f / m_deltaTime.count(); }
        static std::chrono::high_resolution_clock::time_point lastFrame() { return m_lastFrame; }
        static double passedTime() { return std::chrono::duration<double>(m_lastFrame - m_firstFrame).count(); }
        static unsigned long long currentTimeMicroseconds() { return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); }

        friend class Application;
    };
}
