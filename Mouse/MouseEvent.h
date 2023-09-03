#pragma once

#include <string>

struct MousePoint {
	int x, y;
};

class MouseEvent
{
public:
	enum class EventType {
		LPRESS,
		LRELEASE,
		RPRESS,
		RRELEASE,
		MPRESS,
		MRELEASE,
		WHEEL_UP,
		WHEEL_DOWN,
		MOVE,
		RAW_MOVE,
		INVALID
	};
	MouseEvent();
	MouseEvent(const EventType type, const int x, const int y);
	bool isValid() const;
	MouseEvent::EventType getType() const;
	MousePoint getPos();
	int getPosX() const;
	int getPosY() const;
	std::wstring toString() const;
private:
	MouseEvent::EventType type;
	int x;
	int y;
};

