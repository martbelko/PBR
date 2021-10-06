#pragma once

#include <string>

struct GLFWwindow; // Forward declaration

class Window
{
public:
	Window(const std::string& title = "Default Window Title", int width = 800, int height = 600);
	~Window() = default;

	Window(const Window&) = delete;
	Window(Window&& window) = delete;

	Window& operator=(const Window&) = delete;
	Window& operator=(Window&& window) = delete;

	void onUpdate() const;

	void close() const;

	void setTitle(const std::string& title);
	void setWidth(int width) const;
	void setHeight(int height) const;
	void setSize(int width, int height) const;

	const std::string& getTitle() const;
	std::pair<int, int> getSize() const;
	int getWidth() const;
	int getHeigt() const;
private:
	GLFWwindow* mWindow = nullptr;
	std::string mTitle;
private:
	static bool sInitialized;
};
