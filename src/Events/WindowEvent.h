#pragma once

#include <sstream>

#include "Events/Event.h"

class WindowResizeEvent : public Event
{
public:
	WindowResizeEvent(unsigned int width, unsigned int height)
		: m_Width(width), m_Height(height) {}

	unsigned int getWidth() const { return m_Width; }
	unsigned int getHeight() const { return m_Height; }

	std::string toString() const override
	{
		std::stringstream ss;
		ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
		return ss.str();
	}

	EVENT_CLASS_TYPE(Event::Type::WindowResize)
	EVENT_CLASS_CATEGORY(Event::Category::Window)
private:
	unsigned int m_Width, m_Height;
};

class WindowCloseEvent : public Event
{
public:
	WindowCloseEvent() = default;

	EVENT_CLASS_TYPE(Event::Type::WindowClose)
	EVENT_CLASS_CATEGORY(Event::Category::Window)
};
