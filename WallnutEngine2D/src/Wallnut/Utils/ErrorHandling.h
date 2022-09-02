#pragma once

#include "pch.h"

std::wstring GetLastMessageAsReadable()
{
	DWORD error = GetLastError();
	if (error == 0)
		return L"No error";

	wchar_t* message = nullptr;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&message, 0, NULL);
	std::wstring result = message;
	LocalFree(message);
	return result;
}

std::wstring GetErrorAsReadable(HRESULT error)
{
	if (error == 0)
		return L"No error";

	wchar_t* message = nullptr;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&message, 0, NULL);
	std::wstring result = message;
	LocalFree(message);
	return result;
}