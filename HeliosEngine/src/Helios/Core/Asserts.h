#pragma once

#include "Base.h"
#include "Logger.h"
#include <string>
#include "Helios/Utils/Conversions.h"

#ifdef HELIOS_DEBUG

#define HL_ASSERT_C(condition, message) if(!(condition)) { HL_MESSAGE((std::string("Helios Assertion Failed!\n\nIn file: ") + std::string(__FILE__) + "\nAt line: " + std::to_string(__LINE__) + "\nIn function: " + std::string(__FUNCTION__) + "\n\nMessage: " + message ).c_str()); HL_DEBUGBREAK(); }
#define HL_ASSERT_NM(condition) assert(condition)
#define GET_ASSERT_MACRO(_1, _2, NAME, ...) NAME
#define HL_ASSERT(...) GET_ASSERT_MACRO(__VA_ARGS__, HL_ASSERT_C, HL_ASSERT_NM)(__VA_ARGS__)

#else

#define HL_ASSERT_C(condition, message)
#define HL_ASSERT_NM(condition)
#define HL_ASSERT(...)

#endif // HELIOS_DEBUG

#ifdef HELIOS_DEBUG
#define HL_PANIC(message) { Helios::ShowMessage("Helios Panic!", "\nReason: " + message + "\n\nIn file: " + __FILE__ + "\nAt line: " + std::to_string(__LINE__) + "\nIn function: " + __FUNCTION__, Message::IconError); HL_DEBUGBREAK(); std::abort(); }
#else
#define HL_PANIC(message) { Helios::ShowMessage("Helios Panic!", message, Message::IconError); HL_DEBUGBREAK(); std::abort(); }
#endif 
