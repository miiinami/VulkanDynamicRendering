#include"CommandBuffer.hpp"
#include"Core/SwapChain/VulkanSwapChain.hpp"

CommandBuffer::CommandBuffer() :m_commandPool(nullptr)
{

}

void CommandBuffer::createAboutCommand(const vk::raii::Device& device, const uint32_t& queueIndex)
{
	createCommandPool(device, queueIndex);
	createCommandBuffer(device);
}

void CommandBuffer::createCommandPool(const vk::raii::Device& device, const uint32_t& queueIndex)
{
	vk::CommandPoolCreateInfo poolInfo{
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = queueIndex
	};

	m_commandPool = vk::raii::CommandPool(device, poolInfo);
}

void CommandBuffer::createCommandBuffer(const vk::raii::Device& device)
{
	vk::CommandBufferAllocateInfo allocInfo{
		.commandPool = m_commandPool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = MAX_FRAMES_IN_FLIGHT
	};

	m_commandBuffers = vk::raii::CommandBuffers(device, allocInfo);
}

void CommandBuffer::recordCommandBuffer(const uint32_t& frameIndex, const VulkanSwapChain& swapChain, const uint32_t& imageIndex, const vk::raii::Pipeline& graphicsPipeline)
{
	auto& commandBuffer = m_commandBuffers[frameIndex];
	auto& swapchainImages = swapChain.getSwapChainImages();
	auto& swapchainExtent = swapChain.getSwapChainExtent();
	commandBuffer.begin({});

	transition_image_layout(
		frameIndex,
		swapchainImages,
		imageIndex,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eColorAttachmentOptimal,
		{},
		vk::AccessFlagBits2::eColorAttachmentWrite,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput
	);

	vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
	vk::RenderingAttachmentInfo attachmentInto{
		.imageView = swapChain.getSwapChainImageViews()[imageIndex],
		.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eStore,
		.clearValue = clearColor
	};

	vk::RenderingInfo renderingInfo{
		.renderArea = {.offset = {0,0},.extent = swapchainExtent},
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &attachmentInto
	};

	commandBuffer.beginRendering(renderingInfo);

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *graphicsPipeline);

	commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f,
		static_cast<float>(swapchainExtent.width), static_cast<float>(swapchainExtent.height),
		0.0f, 1.0f));

	commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapchainExtent));

	commandBuffer.draw(3, 1, 0, 0);

	commandBuffer.endRendering();

	transition_image_layout(
		frameIndex,
		swapchainImages,
		imageIndex,
		vk::ImageLayout::eColorAttachmentOptimal,
		vk::ImageLayout::ePresentSrcKHR,
		vk::AccessFlagBits2::eColorAttachmentWrite,
		{},
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::PipelineStageFlagBits2::eBottomOfPipe
	);

	commandBuffer.end();
}

void CommandBuffer::transition_image_layout(const uint32_t& frameIndex, const std::vector<vk::Image>& swapChainImages, const uint32_t& imageIndex, const vk::ImageLayout& old_layout, const vk::ImageLayout& new_layout, const vk::AccessFlags2& src__access_mask, const vk::AccessFlags2& dst__access_mask, const vk::PipelineStageFlagBits2& src_stage_mask, const vk::PipelineStageFlagBits2& dst_stage_mask)
{
	auto& commandBuffer = m_commandBuffers[frameIndex];

	vk::ImageMemoryBarrier2 barrier{
		.srcStageMask = src_stage_mask,
		.srcAccessMask = src__access_mask,
		.dstStageMask = dst_stage_mask,
		.dstAccessMask = dst__access_mask,
		.oldLayout = old_layout,
		.newLayout = new_layout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = swapChainImages[imageIndex],
		.subresourceRange = {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

	vk::DependencyInfo dependencyInfo{
		.dependencyFlags = {},
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier
	};

	commandBuffer.pipelineBarrier2(dependencyInfo);
}

const std::vector<vk::raii::CommandBuffer>& CommandBuffer::getCommandBuffers() const
{
	return m_commandBuffers;
}