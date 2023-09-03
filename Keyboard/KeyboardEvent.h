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
	bool isPress() const;
	bool isRelease() const;
	bool isValid() const;
	unsigned char getKeyCode() const;
	std::wstring getKeyName() const;
	std::wstring toString() const;

private:
	EventType type;
	unsigned char key;
	LPARAM lparam;
};

