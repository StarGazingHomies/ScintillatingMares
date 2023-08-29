#include "pch.h"
#include "MouseClass.h"

void MouseClass::onLeftPressed(int x, int y) {
	this->leftIsDown = true;
	MouseEvent event(MouseEvent::EventType::LPRESS, x, y);
	this->eventBuffer.push(event);
}

void MouseClass::onLeftReleased(int x, int y) {
	this->leftIsDown = false;
	MouseEvent event(MouseEvent::EventType::LRELEASE, x, y);
	this->eventBuffer.push(event);
}

void MouseClass::onRightPressed(int x, int y) {
	this->rightIsDown = true;
	MouseEvent event(MouseEvent::EventType::RPRESS, x, y);
	this->eventBuffer.push(event);
}

void MouseClass::onRightReleased(int x, int y) {
	this->rightIsDown = false;
	MouseEvent event(MouseEvent::EventType::RRELEASE, x, y);
	this->eventBuffer.push(event);
}

void MouseClass::onMiddlePressed(int x, int y) {
	this->middleIsDown = true;
	MouseEvent event(MouseEvent::EventType::MPRESS, x, y);
	this->eventBuffer.push(event);
}

void MouseClass::onMiddleReleased(int x, int y) {
	this->middleIsDown = false;
	MouseEvent event(MouseEvent::EventType::MRELEASE, x, y);
	this->eventBuffer.push(event);
}

void MouseClass::onWheelUp(int x, int y) {
	MouseEvent event(MouseEvent::EventType::WHEEL_UP, x, y);
	this->eventBuffer.push(event);
}

void MouseClass::onWheelDown(int x, int y) {
	MouseEvent event(MouseEvent::EventType::WHEEL_DOWN, x, y);
	this->eventBuffer.push(event);
}
void MouseClass::onMouseMove(int x, int y) {
	this->x = x;
	this->y = y;
	MouseEvent event(MouseEvent::EventType::MOVE, x, y);
	this->eventBuffer.push(event);
}

MouseEvent MouseClass::readEvent() {
	if (this->eventBuffer.empty()) {
		return MouseEvent();
	}
	MouseEvent event = this->eventBuffer.front();
	this->eventBuffer.pop();
	return event;
}