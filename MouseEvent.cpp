#include "pch.h"
#include "MouseEvent.h"

MouseEvent::MouseEvent() {
	this->type = EventType::INVALID;
	this->x = 0;
	this->y = 0;
}

MouseEvent::MouseEvent(const EventType type, const int x, const int y) {
	this->type = type;
	this->x = x;
	this->y = y;
}

MousePoint MouseEvent::getPos() {
	return { this->x, this->y };
}

int MouseEvent::getPosX() const {
	return this->x;
}

int MouseEvent::getPosY() const {
	return this->y;
}

bool MouseEvent::isValid() const {
	return this->type != EventType::INVALID;
}

MouseEvent::EventType MouseEvent::getType() const {
	return this->type;
}

std::wstring MouseEvent::toString() const {
	switch (this->type) {
	case EventType::MOVE: {
		std::wstring repr = L"Mouse moved to (";
		repr += std::to_wstring(this->x);
		repr += L", ";
		repr += std::to_wstring(this->y);
		repr += L")";
		return repr;
	}
	case EventType::INVALID: {
		return L"Invalid mouse event";
	}
	case EventType::LPRESS: {
		return L"Mouse left button down";
	}
	case EventType::LRELEASE: {
		return L"Mouse left button up";
	}
	case EventType::RPRESS: {
		return L"Mouse right button down";
	}
	case EventType::RRELEASE: {
		return L"Mouse right button up";
	}
	case EventType::MPRESS: {
		return L"Mouse middle button down";
	}
	case EventType::MRELEASE: {
		return L"Mouse middle button up";
	}
	case EventType::WHEEL_UP: {
		return L"Mouse scroll wheel up";
	}
	case EventType::WHEEL_DOWN: {
		return L"Mouse scroll wheel down";
	}
	default: {
		return L"Unimplemented toString() method for this event type";
	}
	}
}