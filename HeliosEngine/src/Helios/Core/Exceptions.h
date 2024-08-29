#pragma once

#include "Base.h"
#include "Logger.h"
#include "Helios/Utils/ShowMessage.h"

namespace Helios {
	class HELIOS_API HeliosException : public std::exception
	{
	private:
		const char* file;
		const char* function;
		int line;
		const std::string message;
	protected:
		virtual const char* ExceptionName() const { return "HeliosException"; }
	public:
		HeliosException(const char* message, const char* function, const char* file, int line) : std::exception(), message(message)
		{
			this->function = function;
			this->file = file;
			this->line = line;
		}

		HeliosException(std::string message, const char* function, const char* file, int line) : std::exception(), message(message)
		{
			this->function = function;
			this->file = file;
			this->line = line;
		}

		const std::string& GetMessage() const { return message; }

		std::string what() {
			return (
				message
				#ifdef HL_DEBUG
				+ "\n\nIn file: " + file + "\nAt line: " + std::to_string(line) + "\nIn function: " + function
				#endif
			);
		}
	};
}

#define HL_EXCEPTION(condition, message) { if(condition) throw Helios::HeliosException(message, __FUNCTION__, __FILE__, __LINE__); }

#define HL_EXCEPTION_RETRY(condition, message) { retry: try { if(!(condition)) throw Helios::HeliosException(message, __FUNCTION__, __FILE__, __LINE__); } catch(Helios::HeliosException ex) { switch(Helios::ShowMessage("An Critical Error Occured!", ex.what(), Helios::Message::Flags::IconError | Helios::Message::Flags::AbortRetryIgnore)) { case Helios::Message::Result::Retry: goto retry; break; case Helios::Message::Result::Ok:case Helios::Message::Result::Abort: abort(); } } }

#ifdef HELIOS_PLATFORM_WINDOWS

#define HL_EXCEPTION_HR(condition, message, hr) HL_EXCEPTION(condition, std::string(message) + "\nReason: " + GetLastErrorAsString(hr))

#endif