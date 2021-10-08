#include "Application.h"

#include <iostream>
#include <cassert>

#include <glad/glad.h>

#include "Core.h"
#include "Input.h"
#include "Window.h"

#include "Events/Event.h"
#include "Events/WindowEvent.h"

Application::Application(const std::string& name)
{
	if (sInstance != nullptr)
		assert(false);

	sInstance = this;
	mWindow = new Window(name, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	mWindow->setEventCallback(BIND_EVENT_FN(Application::onEvent));
}

void Application::run() const
{
	while (!mWindow->shouldClose())
	{
		if (Input::IsKeyPressed(KeyCode::Escape))
			mWindow->setShouldClose(true);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		mWindow->onUpdate();
	}
}

Application::~Application()
{
	delete mWindow;
}

void Application::onEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::onWindowResize));
	std::cout << e << '\n';
}

bool Application::onWindowResize(WindowResizeEvent& e)
{
	glViewport(0, 0, e.getWidth(), e.getHeight());
	return true;
}

Application* Application::sInstance = nullptr;
