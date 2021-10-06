#include "Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

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

	sInitialized = true;
}

void Window::onUpdate() const
{
	glfwSwapBuffers(mWindow);
	glfwPollEvents();
}

void Window::close() const
{
	glfwDestroyWindow(mWindow);
	glfwTerminate();
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

void Window::processInput()
{
	if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(mWindow, true);
}

bool Window::sInitialized = false;
