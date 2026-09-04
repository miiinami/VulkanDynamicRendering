#include"BufferHelper.hpp"
#include"Core/VulkanDevice/VulkanDevice.hpp"

std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> BufferHelper::createBuffer(const VulkanDevice& vulkanDevice, const vk::DeviceSize& size,
	const vk::BufferUsageFlags& usage, const vk::MemoryPropertyFlags& properties)
{
	const auto& device = vulkanDevice.getLogicalDevice();
	const auto& physicalDevice = vulkanDevice.getPhysicalDevice();

	vk::BufferCreateInfo bufferInfo{ .size = size, .usage = usage, .sharingMode = vk::SharingMode::eExclusive };
	vk::raii::Buffer buffer = vk::raii::Buffer(device, bufferInfo);
	vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();
	vk::MemoryAllocateInfo allocInfo{ .allocationSize = memRequirements.size, .memoryTypeIndex = findMemoryType(physicalDevice,memRequirements.memoryTypeBits, properties) };
	vk::raii::DeviceMemory bufferMemory = vk::raii::DeviceMemory(device, allocInfo);
	buffer.bindMemory(*bufferMemory, 0);
	return { std::move(buffer), std::move(bufferMemory) };
}

uint32_t BufferHelper::findMemoryType(const vk::raii::PhysicalDevice& physicalDevice, const uint32_t& typeFilter, const vk::MemoryPropertyFlags& properties)
{
	vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void BufferHelper::copyBuffer(const VulkanDevice& vulkanDevice, const vk::raii::CommandPool& commandPool,
	vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, const vk::DeviceSize& size)
{
	const auto& device = vulkanDevice.getLogicalDevice();
	const auto& queue = vulkanDevice.getQueue();

	vk::raii::CommandBuffer commandCopyBuffer = beginSingleTimeCommands(device, commandPool);
	commandCopyBuffer.copyBuffer(*srcBuffer, *dstBuffer, vk::BufferCopy(0, 0, size));
	endSingleTimeCommands(std::move(commandCopyBuffer), queue);
}

vk::raii::CommandBuffer BufferHelper::beginSingleTimeCommands(const vk::raii::Device& device, const vk::raii::CommandPool& commandPool)
{
	vk::CommandBufferAllocateInfo allocInfo{
	.commandPool = commandPool,
	.level = vk::CommandBufferLevel::ePrimary,
	.commandBufferCount = 1
	};
	vk::raii::CommandBuffer commandBuffer = std::move(device.allocateCommandBuffers(allocInfo).front());

	commandBuffer.begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

	return std::move(commandBuffer);
}

void BufferHelper::endSingleTimeCommands(vk::raii::CommandBuffer&& commandBuffer, const vk::raii::Queue& queue)
{
	commandBuffer.end();

	queue.submit(vk::SubmitInfo{ .commandBufferCount = 1,.pCommandBuffers = &*commandBuffer }, nullptr);
	queue.waitIdle();
}

vk::Format BufferHelper::findSupportedFormat(const vk::raii::PhysicalDevice& physicalDevice, const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
	for (const auto& format : candidates)
	{
		vk::FormatProperties props = physicalDevice.getFormatProperties(format);

		if (((tiling == vk::ImageTiling::eLinear) && ((props.linearTilingFeatures & features) == features)) ||
			((tiling == vk::ImageTiling::eOptimal) && ((props.optimalTilingFeatures & features) == features)))
		{
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}