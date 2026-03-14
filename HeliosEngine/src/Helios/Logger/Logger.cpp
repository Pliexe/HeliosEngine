#include "Logger.h"
#include "Helios/Events/Events.h"

using namespace Helios;

namespace {
    static std::function<void(Event&)> s_Callback;
}

void Helios::Logger::SetCallback(std::function<void(Event&)> callback)
{
    s_Callback = callback;
}

void Helios::Logger::Submit(LogLevel level, std::string_view message)
{
    LogEvent event(level, message);
    s_Callback(event);
}