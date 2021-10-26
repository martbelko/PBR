#include "Application.h"

#include <cassert>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Base.h"
#include "Input.h"
#include "Window.h"
#include "Timestep.h"
#include "MainLayer.h"
#include "ImGUILayer.h"

#include "Events/Event.h"

Application::Application(const std::string& name)
{
	if (sInstance != nullptr)
		assert(false);

	sInstance = this;
	mWindow = new Window(name, 1280, 720);
	mWindow->setEventCallback(BIND_EVENT_FN(Application::onEvent));

	mImGuiLayer = new ImGUILayer();
	mImGuiLayer->OnAttach();

	Layer* mainLayer = new MainLayer();
	mainLayer->OnAttach();

	mLayerStack.PushLayer(mainLayer);
	mLayerStack.PushLayer(mImGuiLayer);
}

void Application::run()
{
	while (!mWindow->shouldClose())
	{
		float timeMs = static_cast<float>(glfwGetTime()) * 1000.0f; // TODO: Use chrono instead of glfwGetTime()
		Timestep timestep = timeMs - mLastFrameTime;
		mLastFrameTime = timeMs;

		for (Layer* layer : mLayerStack)
			layer->OnUpdate(timestep);

		mImGuiLayer->Begin();
		{
			for (Layer* layer : mLayerStack)
				layer->OnImGuiRender();
		}
		mImGuiLayer->End();

		mWindow->onUpdate();
	}
}

Application::~Application()
{
	delete mWindow;
}

void Application::onEvent(Event& e)
{
	for (auto it = mLayerStack.rbegin(); it != mLayerStack.rend(); ++it)
	{
		if (e.handled)
			break;
		(*it)->OnEvent(e);
	}
}

Application* Application::sInstance = nullptr;
