#pragma once
#include "StringConverter.h"
#include <Windows.h>
class Logger
{
public:
	static void Log(std::string message);
	static void Log(HRESULT hr, std::string message);
	static void Log(HRESULT hr, std::wstring message);
	static void Log(DWORD hr, std::string message);
};

