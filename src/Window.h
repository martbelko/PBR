#pragma once

#include <functional>

struct GLFWwindow;
class Event;

class Window
{
public:
	using EventCallbackFn = std::function<void(Event&)>;
public:
	Window(const std::string& title = "Default Window Title", int width = 800, int height = 600);
	~Window();

	Window(const Window&) = delete;
	Window(Window&& window) = delete;

	Window& operator=(const Window&) = delete;
	Window& operator=(Window&& window) = delete;

	void onUpdate() const;

	void setEventCallback(const EventCallbackFn& callback) { mEventCallback = callback; }

	void setShouldClose(bool shouldClose = true) const;
	bool shouldClose() const;

	void disableCursor() const;

	void setTitle(const std::string& title);
	void setWidth(int width) const;
	void setHeight(int height) const;
	void setSize(int width, int height) const;

	const std::string& getTitle() const;
	std::pair<int, int> getSize() const;
	int getWidth() const;
	int getHeigt() const;

	GLFWwindow* getNativeWindow() const { return mWindow; }
private:
	GLFWwindow* mWindow = nullptr;
	std::string mTitle;

	EventCallbackFn mEventCallback;
private:
	static bool sInitialized;
};
