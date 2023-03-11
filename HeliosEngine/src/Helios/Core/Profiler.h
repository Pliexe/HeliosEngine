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
			std::string Name;
			const char* ThreadName;
			unsigned long long StartTime, EndTime;

			CpuProfileResult(const char* name, const char* thread_name, unsigned long long startTime, unsigned long long endTime) : Name(name), ThreadName(thread_name), StartTime(startTime), EndTime(endTime) { }
			CpuProfileResult(std::string name, const char* thread_name, unsigned long long startTime, unsigned long long endTime) : Name(name), ThreadName(thread_name), StartTime(startTime), EndTime(endTime) { }
		};

		struct FrameProfile
		{
			unsigned long long frameTime, startTime;
			std::deque<CpuProfileResult> results;

			FrameProfile(unsigned long long startTime) : startTime(startTime) {}
		};

		static void BeginFrame()
		{
			if (s_ProfileEnabled)
			{
				s_ProfileResults.emplace_back(Time::currentTimeMicroseconds());
			}
		}

		static void EndFrame()
		{
			if (s_ProfileEnabled)
			{
				auto tmp = s_ProfileResults.back();
				tmp.frameTime = Time::currentTimeMicroseconds() - s_ProfileResults.back().startTime;
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
			{
				auto& r = s_ProfileResults.back().results;
				s_profileQueue.emplace_back(r.size());
				r.emplace_back(
					name, threadName,
					Time::currentTimeMicroseconds(), 0
				);
			}
		}

		static void BeginProfile(std::string name, const char* threadName = "Main Thread")
		{
			if (s_ProfileEnabled)
			{
				auto& r = s_ProfileResults.back().results;
				s_profileQueue.emplace_back(r.size());
				r.emplace_back(
					name, threadName,
					Time::currentTimeMicroseconds(), 0
				);
			}
		}

		static void EndProfile()
		{
			if (s_ProfileEnabled)
			{
				s_ProfileResults.back().results[s_profileQueue.back()].EndTime = Time::currentTimeMicroseconds();
				s_profileQueue.pop_back();
			}
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
		inline static std::deque<uint32_t> s_profileQueue;
		inline static uint32_t depth = 0u;
		inline static uint32_t select = 1u;
	};
}

#ifdef HL_PROFILE

	#define HL_PROFILE_FRAME_BEGIN() ::Helios::Profiler::BeginFrame()
	#define HL_PROFILE_FRAME_END() ::Helios::Profiler::EndFrame()
	#define HL_PROFILE_START_PROFILING() ::Helios::Profiler::StartProfiling()
	#define HL_PROFILE_STOP_PROFILING() ::Helios::Profiler::StopProfiling()
	#define HL_PROFILE_TOGGLE_PROFILING() ::Helios::Profiler::ToggleProfiling()
	#define HL_PROFILE_BEGIN ::Helios::Profiler::BeginProfile
	#define HL_PROFILE_END() ::Helios::Profiler::EndProfile()

#else

	#define HL_PROFILE_FRAME_BEGIN()
	#define HL_PROFILE_FRAME_END()
	#define HL_PROFILE_START_PROFILING()
	#define HL_PROFILE_STOP_PROFILING()
	#define HL_PROFILE_TOGGLE_PROFILING()
	#define HL_PROFILE_BEGIN()
	#define HL_PROFILE_END()

#endif
