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

#include "Core/Base.h"
#include "Core/Input.h"
#include "Core/Window.h"
#include "Core/Timestep.h"
#include "Core/Application.h"

#include "Events/Event.h"
#include "Events/WindowEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"
#include "Renderer/Camera.h"

#include "AtomLoader.h"

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

static uint32_t LoadCubemap(const std::vector<std::string>& faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

static void UploadDataToGPU(const Ref<Shader>& shader, const AtomLoader& loader)
{
	struct Sphere
	{
		float radius;
		float transparency = 0.0f;
		float reflection = 0.0f;
		float _unused = 0.0f;
		glm::vec4 position;
		glm::vec4 color;
	};

	const auto& atoms = loader.GetAtoms();
	const auto& atomTemplates = loader.GetAtomTemplates();
	std::vector<Sphere> spheres;
	spheres.reserve(atoms.size());
	for (const Atom& atom : atoms)
	{
		const AtomTemplate* t = atom.atomTemplate;
		Sphere sphere;
		sphere.position = glm::vec4(atom.position, 0.0f);
		sphere.color = glm::vec4(t->color, 1.0f);
		sphere.radius = t->radius;
		spheres.push_back(std::move(sphere));
	}

	GLuint ssbo;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, spheres.size() * sizeof(Sphere), spheres.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

	shader->SetInt("uSpheresCount", spheres.size());

	/*float spheres[] = {
		// Radius Transparency Reflect Unused | Position | Surface color
		1.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -4.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 0.0f, 2.0f, 2.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 0.0f , -2.0f, -2.0f, -1.0f, 0.0f, 0.33f, 0.33f, 0.33f, 1.0f
	};

	GLuint ssbo;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(spheres), spheres, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);*/
}

void MainLayer::OnAttach()
{
	mWindow.DisableCursor();

	glEnable(GL_DEPTH_TEST);

	mRaytraceShader->Bind();
	mRaytraceShader->SetFloat3("uLightPosition", glm::vec3(5.0f, 5.0f, 5.0f));

	std::vector<std::string> faces = {
		"assets/textures/skybox/right.jpg",
		"assets/textures/skybox/left.jpg",
		"assets/textures/skybox/top.jpg",
		"assets/textures/skybox/bottom.jpg",
		"assets/textures/skybox/front.jpg",
		"assets/textures/skybox/back.jpg"
	};
	mCubemap = LoadCubemap(faces);

	AtomLoader loader("assets/data/1cqw.pdb", "assets/data/test.xml");
	UploadDataToGPU(mRaytraceShader, loader);
}

void MainLayer::OnUpdate(Timestep ts)
{
	mLastTs = ts;
	ProcessInput(ts);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto[width, height] = mWindow.GetSize();
	glm::mat4 projection = glm::perspective(glm::radians(mCamera.GetZoom()), (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 view = mCamera.GetViewMatrix();
	glm::mat4 projview = projection * view;

	mRaytraceShader->SetFloat("uNear", 0.1f);
	mRaytraceShader->SetFloat("uFar", 100.0f);
	mRaytraceShader->SetMat4("uInvProjView", glm::inverse(projview));

	mRaytraceShader->SetInt("uCubemap", 0);
	glBindTextureUnit(0, mCubemap);

	Quad::Render();
}

void MainLayer::OnImGuiRender()
{
	ImGui::Begin("Options");
	{
		float cameraSpeed = mCamera.GetSpeed();
		float cameraSens = mCamera.GetMouseSensitivity();
		static int depth = 3;
		if (ImGui::DragFloat("Camera speed", &cameraSpeed, 0.1f, 0.01f, 1000.0f))
			mCamera.SetSpeed(cameraSpeed);
		if (ImGui::DragFloat("Camera mouse sensitivity", &cameraSens, 0.1f, 0.01f, 1000.0f))
			mCamera.SetMouseSensitivity(cameraSens);
	}
	ImGui::End();

	if (ImGui::Begin("Status"))
	{
		ImGui::Text("Frame time: %f ms", mLastTs.GetMilliseconds());
		ImGui::Text("FPS: %f", 1.0f / mLastTs);
	}
	ImGui::End();
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
	dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(MainLayer::OnMouseMoved));
	dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(MainLayer::OnKeyPressed));
	dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(MainLayer::OnMouseScrolled));
	dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(MainLayer::OnWindowResize));
}

bool MainLayer::OnWindowResize(WindowResizeEvent& e)
{
	glViewport(0, 0, e.GetWidth(), e.GetHeight());
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
	if (e.GetKeyCode() == KeyCode::Escape)
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
