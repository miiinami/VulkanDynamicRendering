#include"GLFWwindowClass.hpp"

GLFWWindowClass::GLFWWindowClass() :m_window(nullptr)
{
}

GLFWwindow* const GLFWWindowClass::getGLFWwindow() const
{
	return m_window;
}

void GLFWWindowClass::Init()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void GLFWWindowClass::CleanUp()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}