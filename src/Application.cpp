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
	mWindow->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

	mImGuiLayer = new ImGUILayer();
	mImGuiLayer->OnAttach();

	Layer* mainLayer = new MainLayer();
	mainLayer->OnAttach();

	mLayerStack.PushLayer(mainLayer);
	mLayerStack.PushLayer(mImGuiLayer);
}

void Application::Run()
{
	while (!mWindow->ShouldClose())
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

		mWindow->OnUpdate();
	}
}

Application::~Application()
{
	delete mWindow;
}

void Application::OnEvent(Event& e)
{
	for (auto it = mLayerStack.rbegin(); it != mLayerStack.rend(); ++it)
	{
		if (e.handled)
			break;
		(*it)->OnEvent(e);
	}
}

Application* Application::sInstance = nullptr;
