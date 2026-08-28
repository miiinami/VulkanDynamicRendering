#pragma once
#include<vulkan/vulkan_raii.hpp>
#include<vector>

class VulkanSwapChain;

class CommandBuffer
{
public:
	CommandBuffer();
	~CommandBuffer() {};

public:
	const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

	void createAboutCommand(const vk::raii::Device& device, const uint32_t& queueIndex);
	void recordCommandBuffer(const uint32_t& frameIndex, const VulkanSwapChain& swapChain, const uint32_t& imageIndex, const vk::raii::Pipeline& graphicsPipeline);

	const std::vector<vk::raii::CommandBuffer>& getCommandBuffers() const;

private:
	vk::raii::CommandPool m_commandPool;
	std::vector<vk::raii::CommandBuffer> m_commandBuffers;

	void createCommandPool(const vk::raii::Device& device, const uint32_t& queueIndex);
	void createCommandBuffer(const vk::raii::Device& device);
	void transition_image_layout(
		const uint32_t& frameIndex,
		const std::vector<vk::Image>& swapChainImages,
		const uint32_t& imageIndex,
		const vk::ImageLayout& old_layout,
		const vk::ImageLayout& new_layout,
		const vk::AccessFlags2& src__access_mask,
		const vk::AccessFlags2& dst__access_mask,
		const vk::PipelineStageFlagBits2& src_stage_mask,
		const vk::PipelineStageFlagBits2& dst_stage_mask);
};