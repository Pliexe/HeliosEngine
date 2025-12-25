#pragma once
#include "Base.h"
#include "pch.h"

#include <cstdint>

#include "Time.h"

namespace Helios
{
	class HELIOS_API Profiler
	{
	public:

		struct CpuFrameProfile
		{
			unsigned long long startTime, frameTime;
			uint64_t starts_at = 0u;

			CpuFrameProfile(unsigned long long startTime, unsigned long long frameTime, uint64_t startsAt = 0u)
				: startTime(startTime), frameTime(frameTime), starts_at(startsAt) {}
			CpuFrameProfile() : startTime(0), frameTime(0), starts_at(0) {}
		};

		struct CpuTaskProfile
		{
			std::string Name;
			const char* ThreadName;
			unsigned long long StartTime, EndTime;
			int Depth = 0u;

			CpuTaskProfile(const char* name, const char* thread_name, unsigned long long startTime, unsigned long long endTime, int depth) : Name(name), ThreadName(thread_name), StartTime(startTime), EndTime(endTime), Depth(depth) { }
			CpuTaskProfile(std::string name, const char* thread_name, unsigned long long startTime, unsigned long long endTime, int depth) : Name(name), ThreadName(thread_name), StartTime(startTime), EndTime(endTime), Depth(depth) { }
		};

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
				s_tmpFrame = CpuFrameProfile (Time::CurrentTimeMicroseconds(), 0u, s_CpuProfileTasks.size());
			}
		}

		static void EndFrame()
		{
			if (s_ProfileEnabled)
			{
				s_tmpFrame.frameTime = Time::CurrentTimeMicroseconds() - s_tmpFrame.startTime;
				s_CpuProfileFrames.emplace_back(s_tmpFrame);
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
				s_profileQueue.emplace_back(s_CpuProfileTasks.size());
				s_CpuProfileTasks.emplace_back(
					name, threadName,
					Time::CurrentTimeMicroseconds(), 0, s_profileQueue.size()-1
				);
			}
		}

		static void BeginProfile(std::string name, const char* threadName = "Main Thread")
		{
			if (s_ProfileEnabled)
			{
				s_profileQueue.emplace_back(s_CpuProfileTasks.size());
				s_CpuProfileTasks.emplace_back(
					name, threadName,
					Time::CurrentTimeMicroseconds(), 0, s_profileQueue.size()-1
				);
			}
		}

		static void EndProfile()
		{
			if (s_ProfileEnabled)
			{
				s_CpuProfileTasks[s_profileQueue.back()].EndTime = Time::CurrentTimeMicroseconds();
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

		inline static std::vector<CpuFrameProfile>& getFrameResults() { return s_CpuProfileFrames; }
		inline static std::vector<CpuTaskProfile>& getTaskResults() { return s_CpuProfileTasks; }
		inline static void clear() { s_ProfileEnabled = false; s_CpuProfileTasks.clear(); s_CpuProfileFrames.clear(); }
	private:
		inline static bool s_toggleQueue = false;
		inline static bool s_ProfileEnabled = false;
		inline static std::deque<uint32_t> s_profileQueue;
		inline static uint32_t select = 1u;

		inline static std::vector<CpuFrameProfile> s_CpuProfileFrames;
		inline static std::vector<CpuTaskProfile> s_CpuProfileTasks;
		inline static CpuFrameProfile s_tmpFrame;
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
	#define HL_PROFILE_BEGIN
	#define HL_PROFILE_END()

#endif
