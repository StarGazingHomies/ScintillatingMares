#pragma once
#include "StringConverter.h"
#include <Windows.h>
#include "PugiXML/pugixml.hpp"

class Logger
{
public:
	static void Log(std::string message);
	static void Log(std::wstring message);
	static void Log(HRESULT hr, std::string message);
	static void Log(HRESULT hr, std::wstring message);
	static void Log(DWORD hr, std::string message);
	static void Log(pugi::xml_parse_result hr, std::string message);
};

