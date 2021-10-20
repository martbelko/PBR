#include "Application.h"

#include <algorithm>
#include <iostream>
#include <cassert>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include "Base.h"
#include "Input.h"
#include "Window.h"
#include "Timestep.h"

#include "Events/Event.h"
#include "Events/WindowEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"
#include "Renderer/Camera.h"

class Sphere
{
public:
	Sphere(const glm::vec3& color)
		: mColor(color)
	{
		if (sVA == nullptr)
			sVA = GenerateSphereVA();
	}

	void render()
	{
		sVA->bind();
		glDrawElements(GL_TRIANGLE_STRIP, sVA->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, 0);
	}

	void setColor(const glm::vec3& color) { mColor = color; }
	const glm::vec3& getColor() const { return mColor; }
private:
	static Ref<VertexArray> GenerateSphereVA()
	{
		Ref<VertexArray> va = VertexArray::Create();

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		constexpr unsigned int X_SEGMENTS = 64;
		constexpr unsigned int Y_SEGMENTS = 64;
		constexpr float PI = 3.14159265359f;
		for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		{
			for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y * (X_SEGMENTS + 1) + x);
				}
			}

			oddRow = !oddRow;
		}

		std::vector<float> data;
		for (unsigned int i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
		}

		va = VertexArray::Create();

		BufferLayout layout = {
			{ ShaderDataType::Float3 },
			{ ShaderDataType::Float3 },
			{ ShaderDataType::Float2 }
		};

		auto vb = VertexBuffer::Create(data.data(), data.size() * sizeof(float));
		vb->setLayout(layout);

		auto ib = IndexBuffer::Create(indices.data(), indices.size());

		va->addVertexBuffer(vb);
		va->setIndexBuffer(ib);

		return va;
	}
private:
	glm::vec3 mColor;
private:
	static Ref<VertexArray> sVA;
};

class Cube
{
public:
	Cube(const glm::vec3& color)
		: mColor(color)
	{
		if (sVA == nullptr)
			sVA = GenerateVA();
	}

	void render()
	{
		sVA->bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void setColor(const glm::vec3& color) { mColor = color; }
	const glm::vec3& getColor() const { return mColor; }
private:
	static Ref<VertexArray> GenerateVA()
	{
		float vertices[] = {
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		auto va = VertexArray::Create();

		BufferLayout layout = {
			{ ShaderDataType::Float3 }
		};

		auto vb = VertexBuffer::Create(vertices, sizeof(vertices));
		vb->setLayout(layout);
		va->addVertexBuffer(vb);

		return va;
	}
private:
	glm::vec3 mColor;
private:
	static Ref<VertexArray> sVA;
};

unsigned int loadCubemap(const std::vector<std::string>& faces)
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

Ref<VertexArray> Sphere::sVA = nullptr;
Ref<VertexArray> Cube::sVA = nullptr;

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

	Ref<Shader> pbrShader = Shader::CreateFromFile("assets/shaders/PBRVS.glsl", "assets/shaders/PBRPS.glsl");
	Ref<Shader> flatShader = Shader::CreateFromFile("assets/shaders/FlatColorVS.glsl", "assets/shaders/FlatColorPS.glsl");
	Ref<Shader> skyboxShader = Shader::CreateFromFile("assets/shaders/Skybox.vert", "assets/shaders/Skybox.frag");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::vector<std::string> faces = {
		"assets/textures/skybox/right.jpg",
		"assets/textures/skybox/left.jpg",
		"assets/textures/skybox/top.jpg",
		"assets/textures/skybox/bottom.jpg",
		"assets/textures/skybox/front.jpg",
		"assets/textures/skybox/back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	while (!mWindow->shouldClose())
	{
		float timeMs = static_cast<float>(glfwGetTime()) * 1000.0f; // TODO: Use chrono instead of glfwGetTime()
		Timestep timestep = timeMs - mLastFrameTime;
		mLastFrameTime = timeMs;

		processInput(timestep);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(mCamera.getZoom()),
			(float)mWindow->getWidth() / (float)mWindow->getHeigt(), 0.1f, 1000.0f);
		glm::mat4 view = mCamera.getViewMatrix();

		pbrShader->bind();

		pbrShader->setMat4("projection", projection);

		pbrShader->setMat4("view", view);
		pbrShader->setFloat3("camPos", mCamera.getPosition());

		pbrShader->setFloat3("albedo", glm::vec3(0.5f, 0.0f, 0.0f));
		pbrShader->setFloat("ao", 1.0f);

		glm::vec3 lightPositions[] = {
			glm::vec3(-10.0f,  10.0f, 10.0f),
			glm::vec3(10.0f,  10.0f, 10.0f),
			glm::vec3(-10.0f, -10.0f, 10.0f),
			glm::vec3(10.0f, -10.0f, 10.0f),
		};

		glm::vec3 lightColors[] = {
			glm::vec3(1000.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1000.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1000.0f),
			glm::vec3(100.0f, 100.0f, 100.0f)
		};

		for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
		{
			glm::vec3 newPos = lightPositions[i];
			pbrShader->setFloat3("lightPositions[" + std::to_string(i) + "]", newPos);
			pbrShader->setFloat3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
		}

		int nrRows = 10;
		int nrColumns = 10;
		float spacing = 2.5f;

		glm::mat4 model = glm::mat4(1.0f);
		for (int row = 0; row < nrRows; ++row)
		{
			pbrShader->setFloat("metallic", (float)row / (float)nrRows);
			for (int col = 0; col < nrColumns; ++col)
			{
				// we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
				// on direct lighting.
				pbrShader->setFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(
					(col - (nrColumns / 2)) * spacing,
					(row - (nrRows / 2)) * spacing,
					0.0f
				));

				pbrShader->setMat4("model", model);
				Sphere sphere(glm::vec3(1.0f, 1.0f, 1.0f));
				sphere.render();
			}
		}

		flatShader->bind();
		flatShader->setMat4("projection", projection);
		flatShader->setMat4("view", view);
		for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
		{
			glm::vec3 newPos = lightPositions[i];
			model = glm::mat4(1.0f);
			model = glm::translate(model, newPos);
			flatShader->setMat4("model", model);

			glm::vec4 color = glm::vec4(lightColors[i], 1.0f);
			flatShader->setFloat4("aColor", color);

			Sphere sphere(glm::vec3(1.0f, 1.0f, 1.0f));
			sphere.render();
		}

		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader->bind();
		view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
		skyboxShader->setMat4("uView", view);
		skyboxShader->setMat4("uProjection", projection);
		skyboxShader->setInt("uSkybox", 0);
		// skybox cube
		Cube cube(glm::vec4(1.0f));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		cube.render();
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		mWindow->onUpdate();
	}
}

Application::~Application()
{
	delete mWindow;
}

void Application::processInput(Timestep ts)
{
	if (Input::IsKeyPressed(KeyCode::Escape))
		mWindow->setShouldClose(true);

	if (Input::IsKeyPressed(KeyCode::W))
		mCamera.processKeyboard(Camera::Direction::Forward, ts.getSeconds());
	else if (Input::IsKeyPressed(KeyCode::S))
		mCamera.processKeyboard(Camera::Direction::Backward, ts.getSeconds());
	if (Input::IsKeyPressed(KeyCode::A))
		mCamera.processKeyboard(Camera::Direction::Left, ts.getSeconds());
	else if (Input::IsKeyPressed(KeyCode::D))
		mCamera.processKeyboard(Camera::Direction::Right, ts.getSeconds());
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
