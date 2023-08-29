#include "pch.h"
#include "KeyboardClass.h"

KeyboardClass::KeyboardClass() {
	memset(this->keyStates, false, sizeof(this->keyStates));
}
bool KeyboardClass::keyIsPressed(const unsigned char keyCode) {
	return this->keyStates[keyCode];
}
bool KeyboardClass::keyBufferIsEmpty() {
	return this->keyBuffer.empty();
}
bool KeyboardClass::charBufferIsEmpty() {
	return this->charBuffer.empty();
}
KeyboardEvent KeyboardClass::readKey() {
	if (this->keyBuffer.empty()) {
		return KeyboardEvent();
	}
	KeyboardEvent event = this->keyBuffer.front();
	this->keyBuffer.pop();
	return event;
}
unsigned char KeyboardClass::readChar() {
	if (this->charBuffer.empty()) {
		return 0u;
	}
	char event = this->charBuffer.front();
	this->charBuffer.pop();
	return event;
}
void KeyboardClass::onKeyPressed(const unsigned char key, LPARAM lparam) {
	this->keyStates[key] = true;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::PRESS, key, lparam));
}
void KeyboardClass::onKeyReleased(const unsigned char key, LPARAM lparam) {
	this->keyStates[key] = false;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::RELEASE, key, lparam));
}
void KeyboardClass::onChar(const unsigned char key) {
	this->charBuffer.push(key);
}

void KeyboardClass::enableAutoRepeatKeys() {
	this->autoRepeatKeys = true;
}

void KeyboardClass::disableAutoRepeatKeys() {
	this->autoRepeatKeys = false;
}

void KeyboardClass::enableAutoRepeatChars() {
	this->autoRepeatChars = true;
}

void KeyboardClass::disableAutoRepeatChars() {
	this->autoRepeatChars = false;
}

bool KeyboardClass::isKeysAutoRepeat() {
	return this->autoRepeatKeys;
}

bool KeyboardClass::isCharsAutoRepeat() {
	return this->autoRepeatChars;
}