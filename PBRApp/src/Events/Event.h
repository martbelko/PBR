#pragma once

#include <string>

#include "Core/Base.h"

#define EVENT_CLASS_TYPE(type) static Event::Type GetStaticType() { return type; }\
                               virtual Event::Type GetEventType() const override { return GetStaticType(); }\
                               virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return static_cast<int>(category); }

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

	virtual Type GetEventType() const = 0;
	virtual const char* GetName() const = 0;
	virtual int GetCategoryFlags() const = 0;
	virtual std::string ToString() const { return GetName(); }

	bool IsInCategory(Category category) { return GetCategoryFlags() & static_cast<int>(category); }
};

class EventDispatcher
{
public:
	EventDispatcher(Event& event)
		: mEvent(event)
	{
	}

	template<typename T, typename F>
	bool Dispatch(const F& func)
	{
		if (mEvent.GetEventType() == T::GetStaticType())
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
	return os << e.ToString();
}
