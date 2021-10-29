#pragma once

#include <string>

#include <glm/glm.hpp>

#include "Base.h"
#include "Application.h"
#include "Layer.h"

#include "Renderer/Camera.h"
#include "Renderer/Shader.h"

class Window;
class Event;
class Shader;

class WindowResizeEvent;
class MouseScrolledEvent;
class MouseMovedEvent;
class KeyPressedEvent;

class MainLayer : public Layer
{
public:
	MainLayer() = default;

	MainLayer(const MainLayer&) = delete;
	MainLayer(MainLayer&&) = delete;

	MainLayer& operator=(const MainLayer&) = delete;
	MainLayer& operator=(MainLayer&&) = delete;

	virtual void OnAttach() override;
	//virtual void OnDetach() override;
	virtual void OnUpdate(Timestep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Event& e) override;
private:
	void ProcessInput(Timestep timestep);

	bool OnWindowResize(WindowResizeEvent& e);

	bool OnMouseMoved(MouseMovedEvent& e);
	bool OnMouseScrolled(MouseScrolledEvent& e);

	bool OnKeyPressed(KeyPressedEvent& e);
private:
	Window& mWindow = Application::Get().GetWindow();
	Camera mCamera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	float lastX;
	float lastY;

	bool mFirstMouse = true;
	bool mShowCursor = false;

	Ref<Shader> mRaytraceShader = Shader::CreateFromFile("assets/shaders/Raytrace.vert", "assets/shaders/Raytrace.frag");

	glm::vec3 mSpherePos = glm::vec3(0.0f, 0.0f, -5.0f);
	float mSphereRadius = 1.0f;
};
