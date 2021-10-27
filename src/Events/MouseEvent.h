#pragma once

#include <sstream>

#include "Events/Event.h"
#include "MouseCodes.h"

class MouseMovedEvent : public Event
{
public:
	MouseMovedEvent(const float x, const float y)
		: mMouseX(x), mMouseY(y) {}

	float GetX() const { return mMouseX; }
	float GetY() const { return mMouseY; }

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "MouseMovedEvent: " << mMouseX << ", " << mMouseY;
		return ss.str();
	}

	EVENT_CLASS_TYPE(Type::MouseMoved)
	EVENT_CLASS_CATEGORY(static_cast<int>(Category::Mouse) | static_cast<int>(Category::Input))
private:
	float mMouseX, mMouseY;
};

class MouseScrolledEvent : public Event
{
public:
	MouseScrolledEvent(const float xOffset, const float yOffset)
		: mXOffset(xOffset), mYOffset(yOffset) {}

	float GetXOffset() const { return mXOffset; }
	float GetYOffset() const { return mYOffset; }

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "MouseScrolledEvent: " << mXOffset << ", " << mYOffset;
		return ss.str();
	}

	EVENT_CLASS_TYPE(Type::MouseScrolled)
	EVENT_CLASS_CATEGORY(static_cast<int>(Category::Mouse) | static_cast<int>(Category::Input))
private:
	float mXOffset, mYOffset;
};

class MouseButtonEvent : public Event
{
public:
	MouseCode GetButton() const { return mButton; }

	EVENT_CLASS_CATEGORY(static_cast<int>(Category::Mouse) |
		static_cast<int>(Category::Input) |
		static_cast<int>(Category::MouseButton))
protected:
	MouseButtonEvent(const MouseCode button)
		: mButton(button) {}

	MouseCode mButton;
};

class MouseButtonPressedEvent : public MouseButtonEvent
{
public:
	MouseButtonPressedEvent(const MouseCode button)
		: MouseButtonEvent(button) {}

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "MouseButtonPressedEvent: " << static_cast<int>(mButton);
		return ss.str();
	}

	EVENT_CLASS_TYPE(Type::MouseButtonPressed)
};

class MouseButtonReleasedEvent : public MouseButtonEvent
{
public:
	MouseButtonReleasedEvent(const MouseCode button)
		: MouseButtonEvent(button) {}

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "MouseButtonReleasedEvent: " << static_cast<int>(mButton);
		return ss.str();
	}

	EVENT_CLASS_TYPE(Type::MouseButtonReleased)
};
