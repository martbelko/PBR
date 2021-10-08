#include "Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Events/WindowEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

Window::Window(const std::string& title, int width, int height)
	: mTitle(title)
{
	if (sInitialized)
	{
		std::cout << "There may be only 1 window\n";
		return;
	}

	if (glfwInit() == GLFW_FALSE)
	{
		std::cout << "GLFWInit() failed\n";
		return;
	}

	mWindow = glfwCreateWindow(width, height, mTitle.c_str(), nullptr, nullptr);
	if (mWindow == nullptr)
	{
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		sInitialized = false;
		return;
	}

	glfwMakeContextCurrent(mWindow);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD\n";
		return;
	}

	glfwSetWindowUserPointer(mWindow, this);

	glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* glfwWindow, int width, int height)
	{
		Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);
		WindowResizeEvent event(width, height);
		window.mEventCallback(event);
	});

	glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* glfwWindow)
	{
		Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);
		WindowCloseEvent event;
		window.mEventCallback(event);
	});

	glfwSetKeyCallback(mWindow, [](GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
	{
		Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);

		switch (action)
		{
		case GLFW_PRESS:
		{
			KeyPressedEvent event(static_cast<KeyCode>(key), 0);
			window.mEventCallback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			KeyReleasedEvent event(static_cast<KeyCode>(key));
			window.mEventCallback(event);
			break;
		}
		case GLFW_REPEAT:
		{
			KeyPressedEvent event(static_cast<KeyCode>(key), 1);
			window.mEventCallback(event);
			break;
		}
		}
	});

	glfwSetCharCallback(mWindow, [](GLFWwindow* glfwWindow, unsigned int keycode)
	{
		Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);
		KeyTypedEvent event(static_cast<KeyCode>(keycode));
		window.mEventCallback(event);
	});

	glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* glfwWindow, int button, int action, int mods)
	{
		Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);

		switch (action)
		{
		case GLFW_PRESS:
		{
			MouseButtonPressedEvent event(static_cast<MouseCode>(button));
			window.mEventCallback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
			window.mEventCallback(event);
			break;
		}
		}
	});

	glfwSetScrollCallback(mWindow, [](GLFWwindow* glfwWindow, double xOffset, double yOffset)
	{
		Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);
		MouseScrolledEvent event((float)xOffset, (float)yOffset);
		window.mEventCallback(event);
	});

	glfwSetCursorPosCallback(mWindow, [](GLFWwindow* glfwWindow, double xPos, double yPos)
	{
		Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);
		MouseMovedEvent event((float)xPos, (float)yPos);
		window.mEventCallback(event);
	});

	sInitialized = true;
}

Window::~Window()
{
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

void Window::onUpdate() const
{
	glfwPollEvents();
	glfwSwapBuffers(mWindow);
}

void Window::setShouldClose(bool shoudClose) const
{
	glfwSetWindowShouldClose(mWindow, shoudClose);
}

bool Window::shouldClose() const
{
	return glfwWindowShouldClose(mWindow);
}

void Window::setTitle(const std::string& title)
{
	mTitle = title;
	glfwSetWindowTitle(mWindow, title.data());
}

void Window::setWidth(int width) const
{
	glfwSetWindowSize(mWindow, width, getHeigt());
}

void Window::setHeight(int height) const
{
	glfwSetWindowSize(mWindow, getWidth(), height);
}

void Window::setSize(int width, int height) const
{
	glfwSetWindowSize(mWindow, width, height);
}

const std::string& Window::getTitle() const
{
	return mTitle;
}

std::pair<int, int> Window::getSize() const
{
	int width, height;
	glfwGetWindowSize(mWindow, &width, &height);
	return std::make_pair(width, height);
}

int Window::getWidth() const
{
	auto [width, height] = getSize();
	return width;
}

int Window::getHeigt() const
{
	auto [width, height] = getSize();
	return height;
}

bool Window::sInitialized = false;
