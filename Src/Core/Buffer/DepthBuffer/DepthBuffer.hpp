#pragma once
#include<vulkan/vulkan_raii.hpp>

class VulkanDevice;

class DepthBuffer
{
public:
	DepthBuffer();
	~DepthBuffer() {};

	void createDepthResources(const VulkanDevice& vulkanDevice, const vk::Extent2D& extent);

	const vk::raii::Image& getDepthImage() const;
	const vk::raii::ImageView& getDepthImageView() const;

private:
	vk::raii::Image m_depthImage;
	vk::raii::DeviceMemory m_depthImageMemory;
	vk::raii::ImageView m_depthImageView;

	vk::Format findDepthFormat(const vk::raii::PhysicalDevice& physicalDevice);
};