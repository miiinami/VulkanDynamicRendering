#pragma once
#include<vulkan/vulkan_raii.hpp>
#include<tuple>

class VulkanDevice;

class ImageHelper
{
public:
	ImageHelper() = default;
	~ImageHelper() = default;

	static std::pair<vk::raii::Image, vk::raii::DeviceMemory> createImage(const VulkanDevice& vulkanDevice, const uint32_t& width, const uint32_t& height,
		const vk::Format& format, const vk::ImageTiling& tiling, const vk::ImageUsageFlags& usage, const vk::MemoryPropertyFlags& properties);

	static void transitionImageLayout(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Image& image, vk::ImageLayout&& oldLayout, vk::ImageLayout&& newLayout);

	static void copyBufferToImage(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Buffer& buffer, vk::raii::Image& image, const uint32_t& width, const uint32_t& height);

	static vk::raii::ImageView createImageView(const vk::raii::Device& device, const vk::Image& image, const vk::Format& format, const vk::ImageAspectFlags aspectFlags);
};