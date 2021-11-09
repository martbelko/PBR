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

	void OnUpdate() const;

	void SetEventCallback(const EventCallbackFn& callback) { mEventCallback = callback; }

	void SetShouldClose(bool shouldClose = true) const;
	bool ShouldClose() const;

	void EnableCursor() const;
	void DisableCursor() const;

	void SetTitle(const std::string& title);
	void SetWidth(int width) const;
	void SetHeight(int height) const;
	void SetSize(int width, int height) const;

	const std::string& GetTitle() const;
	std::pair<int, int> GetSize() const;
	int GetWidth() const;
	int GetHeight() const;

	GLFWwindow* GetNativeWindow() const { return mWindow; }
private:
	GLFWwindow* mWindow = nullptr;
	std::string mTitle;

	EventCallbackFn mEventCallback;
private:
	static bool sInitialized;
};
