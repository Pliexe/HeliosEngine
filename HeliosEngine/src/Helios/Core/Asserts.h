#pragma once

#include "Base.h"
#include "Logger.h"
#include <string>

#ifdef HELIOS_DEBUG

#define HL_ASSERT_C(condition, message) if(!(condition)) { HL_MESSAGE(("Helios Assertion Failed!\n\nFunction:" + std::string(__FUNCTION__) + "\nLine: " + std::to_string(__LINE__) + "\n\nMessage: " + message ).c_str()); HL_DEBUGBREAK(); }
#define HL_ASSERT_NM(condition) assert(condition)
#define GET_ASSERT_MACRO(_1, _2, NAME, ...) NAME
#define HL_ASSERT(...) GET_ASSERT_MACRO(__VA_ARGS__, HL_ASSERT_C, HL_ASSERT_NM)(__VA_ARGS__)

#else

#define HL_ASSERT_C(condition, message)
#define HL_ASSERT_NM(condition)
#define HL_ASSERT(...)

#endif // HELIOS_DEBUG

namespace Helios {
	class HeliosException : public std::exception
	{
	private:
		const char* file;
		const char* function;
		int line;
	protected:
		virtual const char* ExceptionName() const { return "HeliosException"; }
	public:
		HeliosException(const char* message, const char* function, const char* file, int line) : std::exception(message)
		{
			this->function = function;
			this->file = file;
			this->line = line;
		}

		HeliosException(std::string message, const char* function, const char* file, int line) : std::exception(message.c_str())
		{
			this->function = function;
			this->file = file;
			this->line = line;
		}

		int what(bool retry = true) {
			return MessageBoxA(NULL, (
				std::string("An Critical Error Occured!\n\n") +

#ifdef HL_DEBUG

				"\n\nFile: " + file +
				"\n\nAt line: " + std::to_string(line) +
				"\n\nIn function: " + function +
				"\n\n" +
#endif // HL_DEBUG
				"Message: " + exception::what()
				).c_str(), (std::string("Triggered: ") + ExceptionName()).c_str(), retry ? (MB_ABORTRETRYIGNORE | MB_ICONERROR) : (MB_OK | MB_ICONERROR));
		}
	};
}

#define HL_EXCEPTION(condition, message) { if(condition) throw Helios::HeliosException(std::string(message), __FUNCTION__, __FILE__, __LINE__); }
#define HL_EXCEPTION_HR(condition, message, hr) { if(condition) throw Helios::HeliosException(std::string(message) + "\nReason: " + GetLastErrorAsString(hr) + "\nCode: " + std::to_string(hr), __FUNCTION__, __FILE__, __LINE__); }

#define HL_EXCEPTION_RETRY(condition, message) { retry: try { if(!(condition)) throw Helios::HeliosException(message, __FUNCTION__, __FILE__, __LINE__); } catch(Helios::HeliosException ex) { switch(ex.what()) { case IDRETRY: goto retry; break; case IDOK:case IDABORT: abort(); } } }
#define HL_ASSERT_EXCEPTION(condition, message) { if(!(condition)) throw Helios::HeliosException(message, __FUNCTION__, __FILE__, __LINE__); assert(true); }