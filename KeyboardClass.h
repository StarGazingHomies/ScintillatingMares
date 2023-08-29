#pragma once
#include <queue>
#include "KeyboardEvent.h"
class KeyboardClass
{
public:
	KeyboardClass();
	bool keyIsPressed(const unsigned char keyCode);
	bool keyBufferIsEmpty();
	bool charBufferIsEmpty();
	KeyboardEvent readKey();
	unsigned char readChar();
	void onKeyPressed(const unsigned char key);
	void onKeyReleased(const unsigned char key);
	void onChar(const unsigned char key);
private:
	bool keyStates[256];
	std::queue<KeyboardEvent> keyBuffer;
	std::queue<unsigned char> charBuffer;
};

