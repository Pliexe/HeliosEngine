#pragma once
#include "pch.h"

#include <cstdint>
#include <intrin.h>

#include "Time.h"

namespace Helios
{

	class HELIOS_API Profiler
	{
	public:

		struct CpuProfileResult
		{
			const char* Name;
			const char* ThreadName;
			unsigned long long StartTime, EndTime;

			CpuProfileResult(const char* name, const char* thread_name, unsigned long long startTime, unsigned long long endTime) : Name(name), ThreadName(thread_name), StartTime(startTime), EndTime(endTime) { }
		};

		struct FrameProfile
		{
			unsigned long long frameTime;
			std::deque<CpuProfileResult> results;

			FrameProfile(unsigned long long frameTime) : frameTime(frameTime) {}
		};

		static void BeginFrame()
		{
			if (s_ProfileEnabled)
				s_ProfileResults.emplace_back(Time::currentTimeMicroseconds());
		}

		static void EndFrame()
		{
			if (s_ProfileEnabled)
			{
				auto tmp = s_ProfileResults.back();
				tmp.frameTime = Time::currentTimeMicroseconds() - s_ProfileResults.back().frameTime;
				s_ProfileResults[s_ProfileResults.size()-1] = tmp;
			}
			if (s_toggleQueue)
			{
				s_ProfileEnabled ^= 1; s_toggleQueue = false;
			}
		}

		static void BeginProfile(const char* name, const char* threadName = "Main Thread")
		{
			if (s_ProfileEnabled)
				s_ProfileResults.back().results.emplace_back(
				name, threadName,
				Time::currentTimeMicroseconds(), 0
			);
		}

		static void EndProfile()
		{
			if (s_ProfileEnabled)
				s_ProfileResults.back().results.back().EndTime = Time::currentTimeMicroseconds();
		}

		static void StartProfiling()
		{
			if (!s_ProfileEnabled)
				s_toggleQueue = true;
		}

		static void StopProfiling()
		{
			if(s_ProfileEnabled)
				s_toggleQueue = true;
		}

		static void ToggleProfiling()
		{
			s_toggleQueue = true;
		}

		inline static bool isProfiling() { return s_ProfileEnabled; }

		inline static std::vector<FrameProfile>& getResults() { return s_ProfileResults; }
		inline static void clear() { s_ProfileEnabled = false; s_ProfileResults.clear(); }
	private:
		inline static bool s_toggleQueue = false;
		inline static bool s_ProfileEnabled = false;
		inline static std::vector<FrameProfile> s_ProfileResults;
	};
}

#ifdef HL_PROFILE

	#define HL_PROFILE_FRAME_BEGIN() ::Helios::Profiler::BeginFrame()
	#define HL_PROFILE_FRAME_END() ::Helios::Profiler::EndFrame()
	#define HL_PROFILE_START_PROFILING() ::Helios::Profiler::StartProfiling()
	#define HL_PROFILE_STOP_PROFILING() ::Helios::Profiler::StopProfiling()
	#define HL_PROFILE_TOGGLE_PROFILING() ::Helios::Profiler::ToggleProfiling()
	#define HL_PROFILE_BEGIN() ::Helios::Profiler::BeginFrame()
	#define HL_PROFILE_END() ::Helios::Profiler::EndFrame()

#else

	#define HL_PROFILE_FRAME_BEGIN()
	#define HL_PROFILE_FRAME_END()
	#define HL_PROFILE_START_PROFILING()
	#define HL_PROFILE_STOP_PROFILING()
	#define HL_PROFILE_TOGGLE_PROFILING()
	#define HL_PROFILE_BEGIN()
	#define HL_PROFILE_END()

#endif
