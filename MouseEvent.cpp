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