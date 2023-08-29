#pragma once

#include <string>

class KeyboardEvent
{
public:
	enum class EventType {
		PRESS,
		RELEASE,
		INVALID
	};

	KeyboardEvent();
	KeyboardEvent(const EventType type, const unsigned char key, const LPARAM lparam);
	bool isPress();
	bool isRelease();
	bool isValid();
	unsigned char getKeyCode();
	std::wstring getKeyName();
	std::wstring toString();

private:
	EventType type;
	unsigned char key;
	LPARAM lparam;
};

