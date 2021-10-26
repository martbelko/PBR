#pragma once

#include <string>

#include <glm/glm.hpp>

#include "Base.h"
#include "Application.h"
#include "Layer.h"

#include "Renderer/Camera.h"

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
	Window& mWindow = Application::Get().getWindow();
	Camera mCamera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	float lastX;
	float lastY;

	bool mFirstMouse = true;
	bool mShowCursor = false;

	Ref<Shader> pbrShader = nullptr;
	Ref<Shader> equirectangularToCubemapShader = nullptr;
	Ref<Shader> irradianceShader = nullptr;
	Ref<Shader> prefilterShader = nullptr;
	Ref<Shader> brdfShader = nullptr;
	Ref<Shader> backgroundShader = nullptr;

	unsigned int envCubemap;
	unsigned int hdrTexture;
	unsigned int captureFBO;
	unsigned int irradianceMap;
	unsigned int prefilterMap;
	unsigned int brdfLUTTexture;

	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 2.5;

	// lights
	// ------
	glm::vec3 lightPositions[4] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};
	glm::vec3 lightColors[4] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(0.0f, 1000.0f, 0.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};
};