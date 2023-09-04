#include "pch.h"
#include "Logger.h"
#include <comdef.h>

void Logger::Log(std::string message) {
	std::string error_message = "Error: " + message;
	MessageBoxA(NULL, error_message.c_str(), "Error", MB_ICONERROR);
}

void Logger::Log(std::wstring message) {
	std::wstring error_message = L"Error: " + message;
	MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}

void Logger::Log(HRESULT hr, std::string message) {
	_com_error error(hr);
	
	std::wstring error_message = L"Error: " + StringConverter::StringToWide(message) + L"\n" + error.ErrorMessage();
	MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}

void Logger::Log(HRESULT hr, std::wstring message) {
	_com_error error(hr);

	std::wstring error_message = L"Error: " + message + L"\n" + error.ErrorMessage();
	MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}

void Logger::Log(DWORD error, std::string message) {
    LPSTR messageBuffer = nullptr;

    //Ask Win32 to give us the string version of that message ID.
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string error_message(messageBuffer, size);
    LocalFree(messageBuffer);

    MessageBoxA(NULL, ("Error: " + message + "\n" + error_message).c_str(), "Error", MB_ICONERROR);
}

void Logger::Log(pugi::xml_parse_result hr, std::string message) {
	std::string error_message = "Error: " + message + "\n";
	error_message += hr.description();
	MessageBoxA(NULL, error_message.c_str(), "Error", MB_ICONERROR);
}
