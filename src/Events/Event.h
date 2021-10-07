#pragma once

#include <string>

#include "Core.h"

#define EVENT_CLASS_TYPE(type) static Event::Type GetStaticType() { return type; }\
                               virtual Event::Type getEventType() const override { return GetStaticType(); }\
                               virtual const char* getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int getCategoryFlags() const override { return static_cast<int>(category); }

class Event
{
public:
	enum class Type
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum class Category
	{
		None = 0,
		Window = BIT(0),
		Input = BIT(1),
		Keyboard = BIT(2),
		Mouse = BIT(3),
		MouseButton = BIT(4)
	};

	virtual ~Event() = default;

	bool handled = false;

	virtual Type getEventType() const = 0;
	virtual const char* getName() const = 0;
	virtual int getCategoryFlags() const = 0;
	virtual std::string toString() const { return getName(); }

	bool isInCategory(Category category) { return getCategoryFlags() & static_cast<int>(category); }
};

class EventDispatcher
{
public:
	EventDispatcher(Event& event)
		: mEvent(event)
	{
	}

	template<typename T, typename F>
	bool dispatch(const F& func)
	{
		if (mEvent.getEventType() == T::GetStaticType())
		{
			mEvent.handled |= func(static_cast<T&>(mEvent));
			return true;
		}

		return false;
	}
private:
	Event& mEvent;
};

inline std::ostream& operator<<(std::ostream& os, const Event& e)
{
	return os << e.toString();
}
