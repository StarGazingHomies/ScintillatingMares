#include "pch.h"
#include "Logger.h"
#include <comdef.h>

void Logger::Log(std::string message) {
	std::string error_message = "Error: " + message;
	MessageBoxA(NULL, error_message.c_str(), "Error", MB_ICONERROR);
}

void Logger::Log(HRESULT hr, std::string message) {
	_com_error error(hr);
	
	std::wstring error_message = L"Error: " + StringConverter::StringToWide(message) + L"\n" + error.ErrorMessage();
	MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}

void Logger::Log(DWORD error, std::string message) {
    LPSTR messageBuffer = nullptr;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    //Copy the error message into a std::string.
    std::string error_message(messageBuffer, size);
    LocalFree(messageBuffer);

    MessageBoxA(NULL, ("Error: " + message + "\n" + error_message).c_str(), "Error", MB_ICONERROR);
}