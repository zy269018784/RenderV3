#pragma once
#ifdef OPENGL_RENDER
#include <GLFW/glfw3.h>
namespace Render {
	void error_callback(int error, const char* description);
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

	void WindowRefreshCallback2(GLFWwindow* window);

	void WindowSizeCallback2(GLFWwindow* window, int32_t res_x, int32_t res_y);

	void KeyCallback2(GLFWwindow* window, int key, int scancode, int action, int mods);
}
#endif