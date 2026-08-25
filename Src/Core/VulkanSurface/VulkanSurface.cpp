#include"VulkanSurface.hpp"


VulkanSurface::VulkanSurface() :m_surface(nullptr)
{

}

const vk::raii::SurfaceKHR& VulkanSurface::getSurface() const
{
	return m_surface;
}

void VulkanSurface::createSurface(const vk::raii::Instance& instance, GLFWwindow* window)
{
	VkSurfaceKHR _surface;
	if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0)
	{
		throw std::runtime_error("failed to create window surface!");
	}
	m_surface = vk::raii::SurfaceKHR(instance, _surface);
}
