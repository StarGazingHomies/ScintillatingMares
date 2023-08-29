#include "pch.h"
#include "Window.h"

bool Window::Init(HINSTANCE hInstance, std::string title, std::string windowClass, int width, int height) {
	this->hInstance = hInstance;
	this->windowTitle = title;
	this->windowTitleWide = StringConverter::StringToWide(title);
	this->windowClass = windowClass;
	this->windowClassWide = StringConverter::StringToWide(windowClass);
	this->width = width;
	this->height = height;

	this->

}