#include "pch.h"
#include "KeyboardEvent.h"

KeyboardEvent::KeyboardEvent() {
	this->type = EventType::INVALID;
	this->key = 0;
}

KeyboardEvent::KeyboardEvent(const EventType type, const unsigned char key) {
	this->type = type;
	this->key = key;
}

bool KeyboardEvent::isPress() {
	return this->type == EventType::PRESS;
}

bool KeyboardEvent::isRelease() {
	return this->type == EventType::RELEASE;
}

bool KeyboardEvent::isValid() {
	return this->type != EventType::INVALID;
}

unsigned char KeyboardEvent::getKeyCode() {
	return this->key;
}