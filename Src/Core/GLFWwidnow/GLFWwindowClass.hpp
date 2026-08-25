#pragma once
#include<vulkan/vulkan_raii.hpp>
#include<GLFW/glfw3.h>

class GLFWWindowClass 
{
public:
	GLFWWindowClass();
	~GLFWWindowClass() {};

	const uint32_t WIDTH = 1600;
	const uint32_t HEIGHT = 900;

	void Init();
	void CleanUp();
	GLFWwindow* const getGLFWwindow() const;
private:
	GLFWwindow* m_window;
};