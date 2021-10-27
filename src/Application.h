#pragma once

#include <string>

#include <glm/glm.hpp>

#include "Timestep.h"
#include "LayerStack.h"
#include "Window.h"

class Event;
class ImGUILayer;

class Application
{
public:
	Application(const std::string& name);
	~Application();

	Application(const Application&) = delete;
	Application(Application&&) = delete;

	Application& operator=(const Application&) = delete;
	Application& operator=(Application&&) = delete;

	void Run();

	Window& GetWindow() const { return *mWindow; }
public:
	static Application& Get() { return *sInstance; }
private:
	void OnEvent(Event& e);
private:
	Window* mWindow;
	float mLastFrameTime = 0.0f;

	LayerStack mLayerStack;
	ImGUILayer* mImGuiLayer;
private:
	static Application* sInstance;
};