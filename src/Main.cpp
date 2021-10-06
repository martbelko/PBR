#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Window.h"

int main()
{
	Window wnd("PBR App", 1280, 720);
	wnd.onUpdate();
	wnd.close();

	return 0;
}
