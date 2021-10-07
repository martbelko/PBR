#include "Application.h"

#include <iostream>
#include <cassert>

#include <glad/glad.h>

#include "Input.h"
#include "Window.h"
#include "Events/Event.h"

Application::Application(const std::string& name)
{
	if (sInstance != nullptr)
		assert(false);

	sInstance = this;
	mWindow = new Window(name, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	mWindow->setEventCallback(OnEvent);
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

void Application::OnEvent(Event& e)
{
	std::cout << e << '\n';
}

Application* Application::sInstance = nullptr;
