#pragma once
#include<vulkan/vulkan_raii.hpp>
#include<GLFW/glfw3.h>

class VulkanSwapChain
{
public:
	VulkanSwapChain();
	~VulkanSwapChain() {};

	void createSwapChain(const vk::raii::PhysicalDevice& physicaldevice, const vk::raii::Device& device, const vk::raii::SurfaceKHR& surface, GLFWwindow* window);
	void createImageView(const vk::raii::Device& device);
private:
	vk::raii::SwapchainKHR m_swapchain;
	std::vector<vk::Image> m_swapChainImages;
	std::vector<vk::raii::ImageView> m_swapChainImageViews;
	vk::SurfaceFormatKHR m_swapChainSurfaceFormat;
	vk::Extent2D m_swapChainExtent;

	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
	uint32_t chooseSwapMinImageCount(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities);
};