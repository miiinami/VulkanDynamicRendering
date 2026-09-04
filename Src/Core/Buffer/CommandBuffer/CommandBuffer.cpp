#include"CommandBuffer.hpp"
#include"Core/SwapChain/VulkanSwapChain.hpp"
#include"Core/Buffer/VertexBuffer/VertexBuffer.hpp"
#include"Core/GraphicsPipeline/GraphicsPipeline.hpp"
#include"Core/Buffer/DepthBuffer/DepthBuffer.hpp"

CommandBuffer::CommandBuffer() :m_commandPool(nullptr)
{

}

//void CommandBuffer::createAboutCommand(const vk::raii::Device& device, const uint32_t& queueIndex)
//{
//	createCommandPool(device, queueIndex);
//	createCommandBuffer(device);
//}

void CommandBuffer::createCommandPool(const vk::raii::Device& device, const uint32_t& queueIndex)
{
	vk::CommandPoolCreateInfo poolInfo{
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = queueIndex
	};

	m_commandPool = vk::raii::CommandPool(device, poolInfo);
}

void CommandBuffer::createCommandBuffers(const vk::raii::Device& device)
{
	vk::CommandBufferAllocateInfo allocInfo{
		.commandPool = m_commandPool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = MAX_FRAMES_IN_FLIGHT
	};

	m_commandBuffers = vk::raii::CommandBuffers(device, allocInfo);
}

void CommandBuffer::recordCommandBuffer(const uint32_t& frameIndex, const VulkanSwapChain& swapChain, 
	const uint32_t& imageIndex, const GraphicsPipeline& classGraphicsPipeline, 
	const VertexBuffer& classVertexBuffer, const DepthBuffer& depthBuffer)
{
	auto& commandBuffer = m_commandBuffers[frameIndex];
	auto& swapchainImages = swapChain.getSwapChainImages();
	auto& swapchainExtent = swapChain.getSwapChainExtent();
	commandBuffer.begin({});

	transition_image_layout(
		frameIndex,
		swapchainImages[imageIndex],
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eColorAttachmentOptimal,
		{},
		vk::AccessFlagBits2::eColorAttachmentWrite,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::ImageAspectFlagBits::eColor
	);

	transition_image_layout(
		frameIndex,
		depthBuffer.getDepthImage(),
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eDepthAttachmentOptimal,
		vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
		vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
		vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
		vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
		vk::ImageAspectFlagBits::eDepth
	);

	vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
	vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0);

	vk::RenderingAttachmentInfo colorAttachmentInto{
		.imageView = swapChain.getSwapChainImageViews()[imageIndex],
		.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eStore,
		.clearValue = clearColor
	};

	vk::RenderingAttachmentInfo depthAttachmentInto{
	.imageView = depthBuffer.getDepthImageView(),
	.imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
	.loadOp = vk::AttachmentLoadOp::eClear,
	.storeOp = vk::AttachmentStoreOp::eDontCare,
	.clearValue = clearDepth
	};

	vk::RenderingInfo renderingInfo{
		.renderArea = {.offset = {0,0},.extent = swapchainExtent},
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorAttachmentInto,
		.pDepthAttachment = &depthAttachmentInto
	};

	commandBuffer.beginRendering(renderingInfo);

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *classGraphicsPipeline.getGraphicsPipeline());

	commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f,
		static_cast<float>(swapchainExtent.width), static_cast<float>(swapchainExtent.height),
		0.0f, 1.0f));

	commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapchainExtent));

	const auto& vertexBuffer = classVertexBuffer.getVertexBuffer();
	const auto& indexBuffer = classVertexBuffer.getIndexBuffer();

	commandBuffer.bindVertexBuffers(0, *vertexBuffer, { 0 });
	commandBuffer.bindIndexBuffer(*indexBuffer, 0, vk::IndexType::eUint16);
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, classGraphicsPipeline.getDescriptorSetLayout(), 0, *classGraphicsPipeline.getDescriptorSets()[frameIndex], nullptr);

	commandBuffer.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

	commandBuffer.endRendering();

	transition_image_layout(
		frameIndex,
		swapchainImages[imageIndex],
		vk::ImageLayout::eColorAttachmentOptimal,
		vk::ImageLayout::ePresentSrcKHR,
		vk::AccessFlagBits2::eColorAttachmentWrite,
		{},
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::PipelineStageFlagBits2::eBottomOfPipe,
		vk::ImageAspectFlagBits::eColor
	);

	commandBuffer.end();
}

void CommandBuffer::transition_image_layout(const uint32_t& frameIndex, const vk::Image& image, const vk::ImageLayout& old_layout, const vk::ImageLayout& new_layout, const vk::AccessFlags2& src__access_mask, const vk::AccessFlags2& dst__access_mask, const vk::PipelineStageFlags2& src_stage_mask, const vk::PipelineStageFlags2& dst_stage_mask, const vk::ImageAspectFlags image_aspect_flags)
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
		.image = image,
		.subresourceRange = {
			.aspectMask = image_aspect_flags,
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

const vk::raii::CommandPool& CommandBuffer::getCommandPool() const
{
	return m_commandPool;
}

const std::vector<vk::raii::CommandBuffer>& CommandBuffer::getCommandBuffers() const
{
	return m_commandBuffers;
}