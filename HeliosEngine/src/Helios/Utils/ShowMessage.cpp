#include "ShowMessage.h"
#include "Conversions.h"

#if defined(HELIOS_PLATFORM_WINDOWS)
Helios::Message::Result Helios::ShowMessage(const std::string& title, const std::string& message, Message::Flags type)
{
	UINT uType = NULL;

	if (type & Message::Flags::Ok)
		uType |= MB_OK;
	if (type & Message::Flags::OkCancel)
		uType |= MB_OKCANCEL;
	if (type & Message::Flags::AbortRetryIgnore)
		uType |= MB_ABORTRETRYIGNORE;
	if (type & Message::Flags::YesNoCancel)
		uType |= MB_YESNOCANCEL;
	if (type & Message::Flags::YesNo)
		uType |= MB_YESNO;

	if (type & Message::Flags::IconInformation)
		uType |= MB_ICONINFORMATION;
	if (type & Message::Flags::IconWarning)
		uType |= MB_ICONWARNING;
	if (type & Message::Flags::IconError)
		uType |= MB_ICONERROR;
	if (type & Message::Flags::IconQuestion)
		uType |= MB_ICONQUESTION;

	HRESULT hr = ::MessageBoxW(NULL, conversions::from_utf8_to_utf16(message).c_str(), conversions::from_utf8_to_utf16(title).c_str(), uType);

	switch (hr)
	{
	case IDOK: return Message::Result::Ok;
	case IDCANCEL: return Message::Result::Cancel;
	case IDABORT: return Message::Result::Abort;
	case IDRETRY: return Message::Result::Retry;
	case IDIGNORE: return Message::Result::Ignore;
	case IDYES: return Message::Result::Yes;
	case IDNO: return Message::Result::No;
	default: return Message::Result::None;
	}
}
#elif defined(HELIOS_PLATFORM_LINUX)
Helios::Message::Result Helios::ShowMessage(const std::string& title, const std::string& message, Message::Flags type)
{
	std::cout << message << std::endl;
	return Helios::Message::Result::Ok;
}
#else
Helios::Message::Result Helios::ShowMessage(const std::string& title, const std::string& message, Message::Flags type)
{
	std::cout << message << std::endl;
    return Helios::Message::Result::Ok;
}
#endif