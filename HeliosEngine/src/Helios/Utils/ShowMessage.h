#pragma once

#include "Helios/Core/Base.h"
#include "Message.h"

namespace Helios
{
	// Show native message box or use the fallback
	HELIOS_API Helios::Message::Result ShowMessage(const std::string& title, const std::string& message, Message::Flags type = Message::Flags::Ok | Message::Flags::IconInformation);
}