#pragma once
#include<vulkan/vulkan_raii.hpp>

class VulkanDevice;

class BufferHelper
{
public:
	static std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> createBuffer(const VulkanDevice& vulkanDevice, const vk::DeviceSize& size, 
		const vk::BufferUsageFlags& usage, const vk::MemoryPropertyFlags& properties);

	static uint32_t findMemoryType(const vk::raii::PhysicalDevice& physicalDevice, const uint32_t& typeFilter, const vk::MemoryPropertyFlags& properties);

	static void copyBuffer(const VulkanDevice& vulkanDevice, const vk::raii::CommandPool& commandPool, vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, const vk::DeviceSize& size);

	static vk::raii::CommandBuffer beginSingleTimeCommands(const vk::raii::Device& device, const vk::raii::CommandPool& commandPool);

	static void endSingleTimeCommands(vk::raii::CommandBuffer&& commandBuffer, const vk::raii::Queue& queue);

	static vk::Format findSupportedFormat(const vk::raii::PhysicalDevice& physicalDevice, const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
};