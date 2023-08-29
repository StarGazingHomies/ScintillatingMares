#pragma once

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
	EventType type;
	int x;
	int y;
	MouseEvent();
	MouseEvent(const EventType type, const int x, const int y);
	MousePoint getPos();
};

