#include "Application.h"

#include <iostream>
#include <cassert>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Base.h"
#include "Input.h"
#include "Window.h"

#include "Events/Event.h"
#include "Events/WindowEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"
#include "Renderer/Camera.h"

Application::Application(const std::string& name)
{
	if (sInstance != nullptr)
		assert(false);

	sInstance = this;
	mWindow = new Window(name, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	mWindow->setEventCallback(BIND_EVENT_FN(Application::onEvent));
}

void Application::run()
{
	mWindow->disableCursor();

	Ref<Shader> shader = Shader::CreateFromFile("assets/shaders/FlatColorVS.glsl", "assets/shaders/FlatColorPS.glsl");

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f
	};

	BufferLayout layout = {
		{ ShaderDataType::Float3, "Position" }
	};

	Ref<VertexBuffer> vb = VertexBuffer::Create(vertices, sizeof(vertices));
	vb->setLayout(layout);

	Ref<VertexArray> va = VertexArray::Create();
	va->addVertexBuffer(vb);

	glEnable(GL_DEPTH_TEST);

	while (!mWindow->shouldClose())
	{
		processInput();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader->bind();
		va->bind();

		glm::mat4 projection = glm::perspective(glm::radians(mCamera.getZoom()),
			(float)mWindow->getWidth() / (float)mWindow->getHeigt(), 0.1f, 1000.0f);
		shader->setMat4("projection", projection);

		glm::mat4 view = mCamera.getViewMatrix();
		shader->setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.5f, 0.5f, -5.0f));
		model = glm::rotate(model, glm::radians(40.0f), glm::vec3(1.0f, 0.3f, -0.5f));
		shader->setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		mWindow->onUpdate();
	}
}

Application::~Application()
{
	delete mWindow;
}

void Application::processInput()
{
	if (Input::IsKeyPressed(KeyCode::Escape))
		mWindow->setShouldClose(true);

	if (Input::IsKeyPressed(KeyCode::W))
		mCamera.processKeyboard(Camera::Direction::Forward, 0.1f);
	else if (Input::IsKeyPressed(KeyCode::S))
		mCamera.processKeyboard(Camera::Direction::Backward, 0.1f);
	else if (Input::IsKeyPressed(KeyCode::A))
		mCamera.processKeyboard(Camera::Direction::Left, 0.1f);
	else if (Input::IsKeyPressed(KeyCode::D))
		mCamera.processKeyboard(Camera::Direction::Right, 0.1f);
}

void Application::onEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::onWindowResize));
	dispatcher.dispatch<MouseMovedEvent>(BIND_EVENT_FN(Application::onMouseMoved));
	dispatcher.dispatch<MouseScrolledEvent>(BIND_EVENT_FN(Application::onMouseScrolled));
}

bool Application::onWindowResize(WindowResizeEvent& e)
{
	glViewport(0, 0, e.getWidth(), e.getHeight());
	return true;
}

bool Application::onMouseMoved(MouseMovedEvent& e)
{
	if (mFirstMouse)
	{
		lastX = e.getX();
		lastY = e.getY();
		mFirstMouse = false;
	}

	float xoffset = e.getX() - lastX;
	float yoffset = lastY - e.getY(); // reversed since y-coordinates go from bottom to top

	lastX = e.getX();
	lastY = e.getY();

	mCamera.processMouseMovement(xoffset, yoffset);
	return true;
}

bool Application::onMouseScrolled(MouseScrolledEvent& e)
{
	mCamera.processMouseScroll(e.getYOffset());
	return true;
}

Application* Application::sInstance = nullptr;
