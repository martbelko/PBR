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

		auto vb = VertexBuffer::Create(data.data(), static_cast<uint32_t>(data.size() * sizeof(float)));
		vb->setLayout(layout);

		auto ib = IndexBuffer::Create(indices.data(), static_cast<uint32_t>(indices.size()));

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

Ref<VertexArray> Sphere::sVA = nullptr;
Ref<VertexArray> Cube::sVA = nullptr;

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

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

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	// set depth function to less than AND equal for skybox depth trick.
	glDepthFunc(GL_LEQUAL);
	// enable seamless cubemap sampling for lower mip levels in the pre-filter map.
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// build and compile shaders
	// -------------------------
	auto pbrShader = Shader::CreateFromFile("assets/shaders/pbr.vert", "assets/shaders/pbr.frag");
	auto equirectangularToCubemapShader = Shader::CreateFromFile("assets/shaders/cubemap.vert", "assets/shaders/equirectangular_to_cubemap.frag");
	auto irradianceShader = Shader::CreateFromFile("assets/shaders/cubemap.vert", "assets/shaders/irradiance_convolution.frag");
	auto prefilterShader = Shader::CreateFromFile("assets/shaders/cubemap.vert", "assets/shaders/prefilter.frag");
	auto brdfShader = Shader::CreateFromFile("assets/shaders/brdf.vert", "assets/shaders/brdf.frag");
	auto backgroundShader = Shader::CreateFromFile("assets/shaders/background.vert", "assets/shaders/background.frag");

	pbrShader->bind();
	pbrShader->setInt("irradianceMap", 0);
	pbrShader->setInt("prefilterMap", 1);
	pbrShader->setInt("brdfLUT", 2);
	pbrShader->setFloat3("albedo", glm::vec3(0.5f, 0.0f, 0.0f));
	pbrShader->setFloat("ao", 1.0f);

	backgroundShader->bind();
	backgroundShader->setInt("environmentMap", 0);

	// lights
	// ------
	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};
	glm::vec3 lightColors[] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(0.0f, 1000.0f, 0.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};
	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 2.5;

	// pbr: setup framebuffer
	// ----------------------
	unsigned int captureFBO;
	glGenFramebuffers(1, &captureFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	uint32_t size = 1024;

	// pbr: load the HDR environment map
	// ---------------------------------
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float* data = stbi_loadf("assets/textures/hdr/newport.hdr", &width, &height, &nrComponents, 0);
	unsigned int hdrTexture;
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		assert(false, "Failed to load HDR image!");
	}

	// pbr: setup cubemap to render to and attach to framebuffer
	// ---------------------------------------------------------
	unsigned int envCubemap;
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
	// ----------------------------------------------------------------------------------------------
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// pbr: convert HDR equirectangular environment map to cubemap equivalent
	// ----------------------------------------------------------------------
	equirectangularToCubemapShader->bind();
	equirectangularToCubemapShader->setInt("equirectangularMap", 0);
	equirectangularToCubemapShader->setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);

	glViewport(0, 0, size, size); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		equirectangularToCubemapShader->setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Cube cube(glm::vec3(1.0f));
		cube.render();
	}

	// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
	// --------------------------------------------------------------------------------
	unsigned int irradianceMap;
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
	// -----------------------------------------------------------------------------
	irradianceShader->bind();
	irradianceShader->setInt("environmentMap", 0);
	irradianceShader->setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		irradianceShader->setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Cube cube(glm::vec3(1.0f));
		cube.render();
	}

	// pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
	// --------------------------------------------------------------------------------
	unsigned int prefilterMap;
	glGenTextures(1, &prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
	// ----------------------------------------------------------------------------------------------------
	prefilterShader->bind();
	prefilterShader->setInt("environmentMap", 0);
	prefilterShader->setMat4("projection", captureProjection);
	glBindTextureUnit(0, envCubemap);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader->setFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader->setMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			Cube cube(glm::vec3(1.0f));
			cube.render();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// pbr: generate a 2D LUT from the BRDF equations used.
	// ----------------------------------------------------
	unsigned int brdfLUTTexture;
	glGenTextures(1, &brdfLUTTexture);

	// pre-allocate enough memory for the LUT texture.
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, size, size, 0, GL_RG, GL_FLOAT, 0);
	// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	glViewport(0, 0, size, size);
	brdfShader->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// initialize static shader uniforms before rendering
	// --------------------------------------------------

	// then before rendering, configure the viewport to the original framebuffer's screen dimensions
	int scrWidth, scrHeight;
	glfwGetFramebufferSize(mWindow->getNativeWindow(), &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);

	// render loop
	// -----------
	while (!mWindow->shouldClose())
	{
		// per-frame time logic
		// --------------------
		float timeMs = static_cast<float>(glfwGetTime()) * 1000.0f; // TODO: Use chrono instead of glfwGetTime()
		Timestep timestep = timeMs - mLastFrameTime;
		mLastFrameTime = timeMs;

		// input
		// -----
		processInput(timestep);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render scene, supplying the convoluted irradiance map to the final shader.
		// ------------------------------------------------------------------------------------------
		glm::mat4 projection = glm::perspective(glm::radians(mCamera.getZoom()), (float)mWindow->getWidth() / (float)mWindow->getHeigt(), 0.1f, 100.0f);
		glm::mat4 view = mCamera.getViewMatrix();

		pbrShader->bind();
		pbrShader->setMat4("view", view);
		pbrShader->setMat4("projection", projection);
		pbrShader->setFloat3("camPos", mCamera.getPosition());

		// bind pre-computed IBL data
		glBindTextureUnit(0, irradianceMap);
		glBindTextureUnit(1, prefilterMap);
		glBindTextureUnit(2, brdfLUTTexture);

		// render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
		glm::mat4 model = glm::mat4(1.0f);
		for (int row = 0; row < nrRows; ++row)
		{
			pbrShader->setFloat("metallic", (float)row / (float)nrRows);
			for (int col = 0; col < nrColumns; ++col)
			{
				// we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
				// on direct lighting.
				pbrShader->setFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(
					(float)(col - (nrColumns / 2)) * spacing,
					(float)(row - (nrRows / 2)) * spacing,
					-2.0f
				));
				pbrShader->setMat4("model", model);
				Sphere sphere(glm::vec3(1.0f));
				sphere.render();
			}
		}

		// render light source (simply re-render sphere at light positions)
		// this looks a bit off as we use the same shader, but it'll make their positions obvious and
		// keeps the codeprint small.
		for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
		{
			glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
			newPos = lightPositions[i];
			pbrShader->setFloat3("lightPositions[" + std::to_string(i) + "]", newPos);
			pbrShader->setFloat3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

			model = glm::mat4(1.0f);
			model = glm::translate(model, newPos);
			model = glm::scale(model, glm::vec3(0.5f));
			pbrShader->setMat4("model", model);
			Sphere sphere(glm::vec3(1.0f));
			sphere.render();
		}

		// render skybox (render as last to prevent overdraw)
		backgroundShader->bind();
		backgroundShader->setMat4("view", view);
		backgroundShader->setMat4("projection", projection);
		glBindTextureUnit(0, envCubemap);
		//glBindTextureUnit(0, irradianceMap); // display irradiance map
		//glBindTextureUnit(0, prefilterMap); // display prefilter map
		Cube cube(glm::vec3(1.0f));
		cube.render();

		// render BRDF map to screen
		// brdfShader->bind();
		// renderQuad();

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
		mCamera.processKeyboard(Camera::Direction::Forward, ts);
	else if (Input::IsKeyPressed(KeyCode::S))
		mCamera.processKeyboard(Camera::Direction::Backward, ts);
	if (Input::IsKeyPressed(KeyCode::A))
		mCamera.processKeyboard(Camera::Direction::Left, ts);
	else if (Input::IsKeyPressed(KeyCode::D))
		mCamera.processKeyboard(Camera::Direction::Right, ts);
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
