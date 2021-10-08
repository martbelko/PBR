#include "Application.h"

#include <iostream>
#include <cassert>

#include <glad/glad.h>

#include "Base.h"
#include "Input.h"
#include "Window.h"

#include "Events/Event.h"
#include "Events/WindowEvent.h"

#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"

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
	Ref<Shader> shader = Shader::CreateFromFile("assets/shaders/FlatColorVS.glsl", "assets/shaders/FlatColorPS.glsl");

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};

	BufferLayout layout = {
		{ ShaderDataType::Float3, "Position" }
	};
	Ref<VertexBuffer> vb = VertexBuffer::Create(vertices, sizeof(vertices));
	vb->setLayout(layout);

	Ref<VertexArray> va = VertexArray::Create();
	va->addVertexBuffer(vb);

	while (!mWindow->shouldClose())
	{
		if (Input::IsKeyPressed(KeyCode::Escape))
			mWindow->setShouldClose(true);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader->bind();
		va->bind();
		glDrawArrays(GL_TRIANGLES, 0, 3);

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
