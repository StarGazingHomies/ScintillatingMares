#pragma once
#include <queue>
#include "KeyboardEvent.h"
class KeyboardClass
{
public:
	KeyboardClass();
	bool keyIsPressed(const unsigned char keyCode) const;
	bool keyBufferIsEmpty() const;
	bool charBufferIsEmpty() const;
	KeyboardEvent readKey();
	unsigned char readChar();
	void onKeyPressed(const unsigned char key, LPARAM lparam);
	void onKeyReleased(const unsigned char key, LPARAM lparam);
	void onChar(const unsigned char key);
	void enableAutoRepeatKeys();
	void disableAutoRepeatKeys();
	void enableAutoRepeatChars();
	void disableAutoRepeatChars();
	bool isKeysAutoRepeat() const;
	bool isCharsAutoRepeat() const;
private:
	bool autoRepeatKeys = false;
	bool autoRepeatChars = false;
	bool keyStates[256];
	std::queue<KeyboardEvent> keyBuffer;
	std::queue<unsigned char> charBuffer;
};

