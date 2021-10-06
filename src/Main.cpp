#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Application.h"

int main()
{
	Application app("Name this");
	app.run();

	return 0;
}
