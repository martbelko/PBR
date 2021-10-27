#include "MainLayer.h"

#include <algorithm>
#include <iostream>
#include <cassert>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Base.h"
#include "Input.h"
#include "Window.h"
#include "Timestep.h"
#include "Application.h"

#include "Events/Event.h"
#include "Events/WindowEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"
#include "Renderer/Camera.h"

class Quad
{
public:
	Quad() = delete;

	// Renders a 1x1 XY quad in NDC
	static void Render()
	{
		if (sVA == nullptr)
			sVA = GenerateVA();
		sVA->bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
private:
	static Ref<VertexArray> GenerateVA()
	{
		float quadVertices[] = {
			-1.0f,  1.0f,
			-1.0f, -1.0f,
			 1.0f,  1.0f,
			 1.0f, -1.0f
		};

		auto vb = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
		BufferLayout vbLayout = {
			{ ShaderDataType::Float2 }
		};

		vb->setLayout(vbLayout);

		auto va = VertexArray::Create();
		va = VertexArray::Create();
		va->addVertexBuffer(vb);
		return va;
	}
private:
	static Ref<VertexArray> sVA;
};

Ref<VertexArray> Quad::sVA = nullptr;

void MainLayer::OnAttach()
{
	mWindow.disableCursor();
}

void MainLayer::OnUpdate(Timestep ts)
{
	ProcessInput(ts);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	uint32_t width = mWindow.getWidth();
	uint32_t height = mWindow.getHeigt();
	glm::mat4 projection = glm::perspective(glm::radians(mCamera.getZoom()), (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 view = mCamera.getViewMatrix();
	glm::mat4 projview = projection * view;

	mRaytraceShader->bind();
	mRaytraceShader->setFloat2("uResolution", glm::vec2(width, height));
	mRaytraceShader->setFloat("uNear", 0.1f);
	mRaytraceShader->setFloat("uFar", 100.0f);
	mRaytraceShader->setMat4("uInvProjView", glm::inverse(projview));
	Quad::Render();
}

void MainLayer::OnImGuiRender()
{
}

void MainLayer::ProcessInput(Timestep ts)
{
	if (!mShowCursor)
	{
		if (Input::IsKeyPressed(KeyCode::W))
			mCamera.processKeyboard(Camera::Direction::Forward, ts);
		else if (Input::IsKeyPressed(KeyCode::S))
			mCamera.processKeyboard(Camera::Direction::Backward, ts);
		if (Input::IsKeyPressed(KeyCode::A))
			mCamera.processKeyboard(Camera::Direction::Left, ts);
		else if (Input::IsKeyPressed(KeyCode::D))
			mCamera.processKeyboard(Camera::Direction::Right, ts);
	}
}

void MainLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<MouseMovedEvent>(BIND_EVENT_FN(MainLayer::OnMouseMoved));
	dispatcher.dispatch<KeyPressedEvent>(BIND_EVENT_FN(MainLayer::OnKeyPressed));
	dispatcher.dispatch<MouseScrolledEvent>(BIND_EVENT_FN(MainLayer::OnMouseScrolled));
	dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(MainLayer::OnWindowResize));
}

bool MainLayer::OnWindowResize(WindowResizeEvent& e)
{
	glViewport(0, 0, e.getWidth(), e.getHeight());
	return false;
}

bool MainLayer::OnMouseMoved(MouseMovedEvent& e)
{
	if (!mShowCursor)
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
	}

	return false;
}

bool MainLayer::OnMouseScrolled(MouseScrolledEvent& e)
{
	if (!mShowCursor)
		mCamera.processMouseScroll(e.getYOffset());
	return false;
}

bool MainLayer::OnKeyPressed(KeyPressedEvent& e)
{
	if (e.getKeyCode() == KeyCode::Escape)
	{
		mShowCursor = !mShowCursor;
		mFirstMouse = true;
		if (mShowCursor)
			mWindow.enableCursor();
		else
			mWindow.disableCursor();
	}

	return false;
}