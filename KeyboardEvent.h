#pragma once
class KeyboardEvent
{
public:
	enum EventType {
		PRESS,
		RELEASE,
		INVALID
	};

	KeyboardEvent();
	KeyboardEvent(const EventType type, const unsigned char key);
	bool isPress();
	bool isRelease();
	bool isValid();
	unsigned char getKeyCode();

private:
	EventType type;
	unsigned char key;
};

