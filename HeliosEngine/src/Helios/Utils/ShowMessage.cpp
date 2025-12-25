#include "ShowMessage.h"
#include "Conversions.h"
#include "Helios/Core/Logger.h"

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
#ifdef USE_QT
#include <QtWidgets/QMessageBox>
#endif 

Helios::Message::Result Helios::ShowMessage(const std::string& title, const std::string& message, Message::Flags type)
{
#ifdef USE_QT
		QMessageBox msgBox;
		msgBox.setWindowTitle(title.c_str());
		msgBox.setText(message.c_str());
		QMessageBox::StandardButtons uButtons = QMessageBox::NoButton;
		// QMessageBox::StandardButton buttons = QMessageBox::Ok;
		QMessageBox::Icon uIcon = QMessageBox::NoIcon;

		if (type & Message::Flags::Ok)
			uButtons |= QMessageBox::Ok;
		if (type & Message::Flags::OkCancel)
			uButtons |= QMessageBox::Ok | QMessageBox::Cancel;
		if (type & Message::Flags::AbortRetryIgnore)
			uButtons |= QMessageBox::Abort | QMessageBox::Retry | QMessageBox::Ignore;
		if (type & Message::Flags::YesNoCancel)
			uButtons |= QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;
		if (type & Message::Flags::YesNo)
			uButtons |= QMessageBox::Yes | QMessageBox::No;

		if (type & Message::Flags::IconInformation)
			uIcon = QMessageBox::Information;
		if (type & Message::Flags::IconWarning)
			uIcon = QMessageBox::Warning;
		if (type & Message::Flags::IconError)
			uIcon = QMessageBox::Critical;
		if (type & Message::Flags::IconQuestion)
			uIcon = QMessageBox::Question;

		msgBox.setStandardButtons(uButtons);
		msgBox.setIcon(uIcon);

		int result = msgBox.exec();

		switch (result)
		{
		case QMessageBox::Accepted:
			return Message::Result::Ok;
		case QMessageBox::Rejected:
			return Message::Result::Cancel;
		case QMessageBox::Abort:
			return Message::Result::Abort;
		case QMessageBox::Retry:
			return Message::Result::Retry;
		case QMessageBox::Ignore:
			return Message::Result::Ignore;
		case QMessageBox::Yes:
			return Message::Result::Yes;
		case QMessageBox::No:
			return Message::Result::No;
		default:
			return Message::Result::None;
		}
#elif defined(USE_ZENITY)

    std::string cmd = "zenity --title=\"" + title + "\"";

    if (type & Message::Flags::Ok || type & Message::Flags::OkCancel)
        cmd += " --info";

    if (type & Message::Flags::IconWarning)
        cmd = "zenity --warning --title=\"" + title + "\"";

    if (type & Message::Flags::IconError)
        cmd = "zenity --error --title=\"" + title + "\"";

    if (type & Message::Flags::IconQuestion)
        cmd = "zenity --question --title=\"" + title + "\"";

    std::string escapedMessage = message;
    size_t pos = 0;
	while ((pos = escapedMessage.find("\"", pos)) != std::string::npos) {
		escapedMessage.replace(pos, 1, "\\\"");
		pos += 2;
	}
	
    cmd += " --text=\"" + escapedMessage + "\"";

	std::cout << cmd << std::endl;
	
    int exitCode = std::system(cmd.c_str());

    if (type & Message::Flags::YesNo || type & Message::Flags::YesNoCancel || type & Message::Flags::OkCancel) {
        if (exitCode == 0)
            return Message::Result::Yes;
        else
            return Message::Result::No;
    }

    if (exitCode == 0)
        return Message::Result::Ok;

    return Message::Result::None;

#else
	std::cout << message << std::endl;
	return Helios::Message::Result::Ok;
#endif
}
#else
Helios::Message::Result Helios::ShowMessage(const std::string& title, const std::string& message, Message::Flags type)
{
	std::cout << message << std::endl;
    return Helios::Message::Result::Ok;
}
#endif