#include "MainLayer.h"

#include <algorithm>
#include <iostream>
#include <cassert>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

	static void Render()
	{
		if (sVA == nullptr)
			sVA = GenerateVA();
		sVA->Bind();
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

		vb->SetLayout(vbLayout);

		auto va = VertexArray::Create();
		va = VertexArray::Create();
		va->AddVertexBuffer(vb);
		return va;
	}
private:
	static Ref<VertexArray> sVA;
};

Ref<VertexArray> Quad::sVA = nullptr;

void MainLayer::OnAttach()
{
	mWindow.DisableCursor();
}

void MainLayer::OnUpdate(Timestep ts)
{
	ProcessInput(ts);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto[width, height] = mWindow.GetSize();
	glm::mat4 projection = glm::perspective(glm::radians(mCamera.GetZoom()), (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 view = mCamera.GetViewMatrix();
	glm::mat4 projview = projection * view;

	mRaytraceShader->Bind();
	mRaytraceShader->SetFloat2("uResolution", glm::vec2(width, height));
	mRaytraceShader->SetFloat("uNear", 0.1f);
	mRaytraceShader->SetFloat("uFar", 100.0f);
	mRaytraceShader->SetMat4("uInvProjView", glm::inverse(projview));
	Quad::Render();
}

void MainLayer::OnImGuiRender()
{
	if (mShowCursor)
	{
		ImGui::Begin("Option");
		{
			float cameraSpeed = mCamera.GetSpeed();
			float cameraSens = mCamera.GetMouseSensitivity();
			if (ImGui::DragFloat("Camera speed", &cameraSpeed, 0.1f, 0.01f, 1000.0f));
				mCamera.SetSpeed(cameraSpeed);
			if (ImGui::DragFloat("Camera mouse sensitivity", &cameraSens, 0.1f, 0.01f, 1000.0f))
				mCamera.SetMouseSensitivity(cameraSens);
		}
		ImGui::End();
	}
}

void MainLayer::ProcessInput(Timestep ts)
{
	if (!mShowCursor)
	{
		if (Input::IsKeyPressed(KeyCode::W))
			mCamera.ProcessKeyboard(Camera::Direction::Forward, ts);
		else if (Input::IsKeyPressed(KeyCode::S))
			mCamera.ProcessKeyboard(Camera::Direction::Backward, ts);
		if (Input::IsKeyPressed(KeyCode::A))
			mCamera.ProcessKeyboard(Camera::Direction::Left, ts);
		else if (Input::IsKeyPressed(KeyCode::D))
			mCamera.ProcessKeyboard(Camera::Direction::Right, ts);
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
			lastX = e.GetX();
			lastY = e.GetY();

			mFirstMouse = false;
		}

		float xoffset = e.GetX() - lastX;
		float yoffset = lastY - e.GetY(); // reversed since y-coordinates go from bottom to top

		lastX = e.GetX();
		lastY = e.GetY();

		mCamera.ProcessMouseMovement(xoffset, yoffset);
	}

	return false;
}

bool MainLayer::OnMouseScrolled(MouseScrolledEvent& e)
{
	if (!mShowCursor)
		mCamera.ProcessMouseScroll(e.GetYOffset());
	return false;
}

bool MainLayer::OnKeyPressed(KeyPressedEvent& e)
{
	if (e.getKeyCode() == KeyCode::Escape)
	{
		mShowCursor = !mShowCursor;
		mFirstMouse = true;
		if (mShowCursor)
			mWindow.EnableCursor();
		else
			mWindow.DisableCursor();
	}

	return false;
}
