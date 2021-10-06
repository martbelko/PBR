#include "Application.h"

#include <cassert>

#include <glad/glad.h>

#include "Input.h"

Application::Application(const std::string& name)
{
	if (sInstance != nullptr)
		assert(false);

	sInstance = this;
	mWindow = new Window(name, DEFAULT_WIDTH, DEFAULT_HEIGHT);
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

Application* Application::sInstance = nullptr;
