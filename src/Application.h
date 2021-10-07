#pragma once

#include <string>

class Window;
class Event;

class Application
{
public:
	Application(const std::string& name);
	~Application();

	Application(const Application&) = delete;
	Application(Application&&) = delete;

	Application& operator=(const Application&) = delete;
	Application& operator=(Application&&) = delete;

	void run() const;

	Window& getWindow() const { return *mWindow; }
public:
	static Application& Get() { return *sInstance; }
private:
	void OnEvent(Event& event);
private:
	Window* mWindow;
private:
	static Application* sInstance;

	static constexpr int DEFAULT_WIDTH = 1280;
	static constexpr int DEFAULT_HEIGHT = 720;
};