#include "Input.h"

#include <GLFW/glfw3.h>

#include "Window.h"
#include "Application.h"

bool Input::IsKeyPressed(KeyCode keyCode)
{
	auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	auto state = glfwGetKey(window, static_cast<int32_t>(keyCode));
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::IsKeyPressed(MouseCode mouseCode)
{
	auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	auto state = glfwGetMouseButton(window, static_cast<int32_t>(mouseCode));
	return state == GLFW_PRESS;
}
