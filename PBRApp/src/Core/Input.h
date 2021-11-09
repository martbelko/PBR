#pragma once

#include "KeyCodes.h"
#include "MouseCodes.h"

class Input
{
public:
	Input() = delete;

	static bool IsKeyPressed(KeyCode keyCode);
	static bool IsKeyPressed(MouseCode mouseCode);
};
