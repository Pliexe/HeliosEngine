/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"

namespace Helios {
	class HELIOS_API Time {
	private:

		static __int64 m_firstFrame;
		static __int64 m_lastFrame;
		static float m_deltaTime;
		static double PCFreq;

		static const void Init() {
			LARGE_INTEGER li;
			if (!QueryPerformanceFrequency(&li))
			{
				MessageBox(NULL, L"Failed to track PC frequency", L"CRITICAL", MB_ICONERROR);
				exit(-9000);
			}
			PCFreq = double(li.QuadPart);
			m_firstFrame = li.QuadPart;
		}

		static const void frameUpdate() {
			static LARGE_INTEGER li;
			QueryPerformanceCounter(&li);
			m_deltaTime = float(li.QuadPart - m_lastFrame) / PCFreq;
			m_lastFrame = li.QuadPart;
		}

	public:

		static const float deltaTime() { return m_deltaTime; }
		static const float getFPS() { return CLOCKS_PER_SEC / (m_deltaTime * 1000.0f); }
		static const __int64 lastFrame() { return m_lastFrame; }
		static const long float passedTime() { return (m_lastFrame - m_firstFrame) / PCFreq; }
		static const unsigned long long currentTimeMicroseconds() { return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); }
		
		friend class Application;
		extern friend class EngineScene;
	};
}


