#include"GLFWwindowClass.hpp"

GLFWWindowClass::GLFWWindowClass() :framebufferResized(false), m_window(nullptr)
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
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

	glfwSetWindowUserPointer(m_window, this);
	glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
}

void GLFWWindowClass::CleanUp()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void GLFWWindowClass::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<GLFWWindowClass*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
}