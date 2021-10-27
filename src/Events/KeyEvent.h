#pragma once

#include <sstream>

#include "Events/Event.h"
#include "KeyCodes.h"

class KeyEvent : public Event
{
public:
	KeyCode GetKeyCode() const { return mKeyCode; }

	EVENT_CLASS_CATEGORY(static_cast<int>(Category::Keyboard) | static_cast<int>(Category::Input))
protected:
	KeyEvent(const KeyCode keycode)
		: mKeyCode(keycode) {}

	KeyCode mKeyCode;
};

class KeyPressedEvent : public KeyEvent
{
public:
	KeyPressedEvent(const KeyCode keycode, const uint16_t repeatCount)
		: KeyEvent(keycode), mRepeatCount(repeatCount) {}

	uint16_t GetRepeatCount() const { return mRepeatCount; }

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "KeyPressedEvent: " << static_cast<int>(mKeyCode) << " (" << mRepeatCount << " repeats)";
		return ss.str();
	}

	EVENT_CLASS_TYPE(Type::KeyPressed)
private:
	uint16_t mRepeatCount;
};

class KeyReleasedEvent : public KeyEvent
{
public:
	KeyReleasedEvent(const KeyCode keycode)
		: KeyEvent(keycode) {}

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "KeyReleasedEvent: " << static_cast<int>(mKeyCode);
		return ss.str();
	}

	EVENT_CLASS_TYPE(Type::KeyReleased)
};

class KeyTypedEvent : public KeyEvent
{
public:
	KeyTypedEvent(const KeyCode keycode)
		: KeyEvent(keycode) {}

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "KeyTypedEvent: " << static_cast<int>(mKeyCode);
		return ss.str();
	}

	EVENT_CLASS_TYPE(Type::KeyTyped)
};
