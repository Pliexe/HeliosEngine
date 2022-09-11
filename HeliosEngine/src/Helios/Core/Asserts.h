#pragma once

#include "Base.h"
#include "Logger.h"
#include <string>
#define HL_CORE_ASSERT(condition) if(!(condition)) { HL_DEBUGBREAK(); } 
#define HL_CORE_ASSERT_WITH_MSG(condition, message) if(!(condition)) { HL_MESSAGE(("Helios Assertion Failed!\n\nFunction:" + std::string(__FUNCTION__) + "\nLine: " + std::to_string(__LINE__) + "\n\nMessage: " + message ).c_str()); HL_DEBUGBREAK(); }



namespace Helios {
	class HeliosExceptin : public std::exception
	{
	private:
		const char* file;
		const char* function;
		int line;
	protected:
		virtual const char* ExceptionName() const { return "HeliosException"; }
	public:
		HeliosExceptin(const char* message, const char* function, const char* file, int line) : std::exception(message)
		{
			this->function = function;
			this->file = file;
			this->line = line;
		}

		HeliosExceptin(std::string message, const char* function, const char* file, int line) : std::exception(message.c_str())
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

#define HL_EXCEPTION(condition, message) { if(condition) throw Helios::HeliosExceptin(message, __FUNCTION__, __FILE__, __LINE__); }

#define HL_ASSERT_EXCEPTION_RETRY(condition, message) { retry: try { if(!condition) throw Helios::HeliosExceptin(message, __FUNCTION__, __FILE__, __LINE__); } catch(Helios::HeliosExceptin ex) { switch(ex.what()) { case: IDRETRY: goto retry; break; case IDOK:case IDABORT: abort(); } } }
#define HL_ASSERT_EXCEPTION(condition, message) { if(!condition) throw Helios::HeliosExceptin(message, __FUNCTION__, __FILE__, __LINE__); }