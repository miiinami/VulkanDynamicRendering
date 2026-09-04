#pragma once
#include<vulkan/vulkan_raii.hpp>
#include<vector>

class GraphicsPipeline;
class VulkanSwapChain;
class VertexBuffer;
class DepthBuffer;

class CommandBuffer
{
public:
	CommandBuffer();
	~CommandBuffer() {};

public:
	//void createAboutCommand(const vk::raii::Device& device, const uint32_t& queueIndex);
	void createCommandPool(const vk::raii::Device& device, const uint32_t& queueIndex);
	void createCommandBuffers(const vk::raii::Device& device);

	void recordCommandBuffer(const uint32_t& frameIndex, const VulkanSwapChain& swapChain,
		const uint32_t& imageIndex, const GraphicsPipeline& classGraphicsPipeline,
		const VertexBuffer& classVertexBuffer, const DepthBuffer& depthBuffer);

	const std::vector<vk::raii::CommandBuffer>& getCommandBuffers() const;
	const vk::raii::CommandPool& getCommandPool() const;

private:
	vk::raii::CommandPool m_commandPool;
	std::vector<vk::raii::CommandBuffer> m_commandBuffers;

	void transition_image_layout(
		const uint32_t& frameIndex,
		const vk::Image& image,
		const vk::ImageLayout& old_layout,
		const vk::ImageLayout& new_layout,
		const vk::AccessFlags2& src__access_mask,
		const vk::AccessFlags2& dst__access_mask,
		const vk::PipelineStageFlags2& src_stage_mask,
		const vk::PipelineStageFlags2& dst_stage_mask,
		const vk::ImageAspectFlags image_aspect_flags
	);
};