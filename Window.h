#pragma once

#include "Logger.h"

class Window
{
public:
	bool Init(HINSTANCE hInstance, std::string title, std::string windowClass, int width, int height);
	bool ProcessMessage();
	~Window();
private:
	void RegisterWindowClass();
	HWND handle = NULL;
	HINSTANCE hInstance = NULL;
	std::string windowTitle = "";
	std::wstring windowTitleWide = L"";
	std::string windowClass = "";
	std::wstring windowClassWide = L"";
	int width = 0;
	int height = 0;
};

