#include "pch.h"
#include "KeyboardEvent.h"

KeyboardEvent::KeyboardEvent() {
	this->type = EventType::INVALID;
	this->key = 0;
}

KeyboardEvent::KeyboardEvent(const EventType type, const unsigned char key, const LPARAM lparam) {
	this->type = type;
	this->key = key;
	this->lparam = lparam;
}

bool KeyboardEvent::isPress() const {
	return this->type == EventType::PRESS;
}

bool KeyboardEvent::isRelease() const {
	return this->type == EventType::RELEASE;
}

bool KeyboardEvent::isValid() const {
	return this->type != EventType::INVALID;
}

unsigned char KeyboardEvent::getKeyCode() const {
	return this->key;
}

std::wstring KeyboardEvent::getKeyName() const {
	WCHAR keyName[64];
	GetKeyNameTextW(this->lparam, keyName, sizeof(keyName)/sizeof(wchar_t));
	return std::wstring(keyName);
}

std::wstring KeyboardEvent::toString() const {
	switch (this->type) {
	case EventType::PRESS: {
		std::wstring repr = L"Key ";
		repr += getKeyName();
		repr += L" is pressed";
		return repr;
	}
	case EventType::RELEASE: {
		std::wstring repr = L"Key ";
		repr += getKeyName();
		repr += L" is released";
		return repr;
	}
	case EventType::INVALID: {
		return L"Invalid keyboard event";
	}
	}
}