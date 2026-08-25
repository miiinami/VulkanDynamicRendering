#pragma once
#include<vulkan/vulkan_raii.hpp>
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>


class VulkanSurface
{
public:
	VulkanSurface();
	~VulkanSurface() {};

	const vk::raii::SurfaceKHR& getSurface() const;

	void createSurface(const vk::raii::Instance& instance, GLFWwindow* window);
private:
	vk::raii::SurfaceKHR m_surface;
};