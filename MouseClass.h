#pragma once

#include <queue>
#include "MouseEvent.h"

class MouseClass
{
public:
	void onLeftPressed(int x, int y);
	void onLeftReleased(int x, int y);
	void onRightPressed(int x, int y);
	void onRightReleased(int x, int y);
	void onMiddlePressed(int x, int y);
	void onMiddleReleased(int x, int y);
	void onWheelUp(int x, int y);
	void onWheelDown(int x, int y);
	void onMouseMove(int x, int y);

	bool isLeftDown() const;
	bool isMiddleDown() const;
	bool isRightDown() const;

	int getPosX() const;
	int getPosY() const;
	MousePoint getPos() const;

	bool eventBufferIsEmpty() const;
	MouseEvent readEvent();

private:
	std::queue<MouseEvent> eventBuffer;
	bool leftIsDown = false;
	bool rightIsDown = false;
	bool middleIsDown = false;
	int x = 0;
	int y = 0;
};

