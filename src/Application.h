#pragma once

#include <string>

#include <glm/glm.hpp>

#include "Timestep.h"

#include "Renderer/Camera.h"

class Window;
class Event;

class WindowResizeEvent;
class MouseScrolledEvent;
class MouseMovedEvent;
class KeyPressedEvent;

class Application
{
public:
	Application(const std::string& name);
	~Application();

	Application(const Application&) = delete;
	Application(Application&&) = delete;

	Application& operator=(const Application&) = delete;
	Application& operator=(Application&&) = delete;

	void run();

	Window& getWindow() const { return *mWindow; }
public:
	static Application& Get() { return *sInstance; }
private:
	void processInput(Timestep timestep);

	void onEvent(Event& e);

	bool onWindowResize(WindowResizeEvent& e);

	bool onMouseMoved(MouseMovedEvent& e);
	bool onMouseScrolled(MouseScrolledEvent& e);
private:
	Window* mWindow;
	float mLastFrameTime = 0.0f;

	Camera mCamera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	float lastX = DEFAULT_WIDTH / 2.0f;
	float lastY = DEFAULT_HEIGHT / 2.0f;

	bool mSimple = false;
private:
	static Application* sInstance;

	static constexpr int DEFAULT_WIDTH = 1280;
	static constexpr int DEFAULT_HEIGHT = 720;
};