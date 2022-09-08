#pragma once

#include "Base.h"
#include "Logger.h"
#include <string>
#define HL_CORE_ASSERT(condition) if(!(condition)) { HL_DEBUGBREAK(); } 
#define HL_CORE_ASSERT_WITH_MSG(condition, message) if(!(condition)) { HL_MESSAGE(("Helios Assertion Failed!\n\nFunction:" + std::string(__FUNCTION__) + "\nLine: " + std::to_string(__LINE__) + "\n\nMessage: " + message ).c_str()); HL_DEBUGBREAK(); }