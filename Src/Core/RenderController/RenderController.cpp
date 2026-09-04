#include"RenderController.hpp"
#include"Core/VulkanDevice/VulkanDevice.hpp"
#include"Core/SwapChain/VulkanSwapChain.hpp"
#include"Core/Buffer/CommandBuffer/CommandBuffer.hpp"
#include"Core/Buffer/VertexBuffer/VertexBuffer.hpp"
#include"Core/Buffer/UniformBuffer/UniformBuffer.hpp"
#include"Core/Buffer/DepthBuffer/DepthBuffer.hpp"
#include<iostream>
#include<exception>

RenderController::RenderController() :frameIndex(0)
{

};

void RenderController::createSyncObjects(const vk::raii::Device& device, const size_t& ImagesCount)
{
	assert(presentCompleteSemaphores.empty() && renderFinishedSemaphores.empty() && inFlightFences.empty());

	for (size_t i = 0; i < ImagesCount; i++)
	{
		renderFinishedSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		presentCompleteSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
		inFlightFences.emplace_back(device, vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled });
	}
}

void RenderController::drawFrame(const VulkanDevice& vulkanDevice, VulkanSwapChain& vulkanSwapChain,
	CommandBuffer& classCommandBuffer, const GraphicsPipeline& classGraphicsPipeline,
	bool& framebufferResized, const vk::raii::SurfaceKHR& surface,
	GLFWwindow* window, const VertexBuffer& classVertexBuffer,UniformBuffer& classUniformBuffer, const DepthBuffer& depthBuffer)
{
	const vk::raii::Device& device = vulkanDevice.getLogicalDevice();

	auto fenceResult = device.waitForFences(*inFlightFences[frameIndex], vk::True, UINT64_MAX);
	if (fenceResult != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to wait for fence!");
	}

	auto [result, imageIndex] = vulkanSwapChain.getSwapChain().acquireNextImage(UINT64_MAX, *presentCompleteSemaphores[frameIndex], nullptr);
	if (result == vk::Result::eErrorOutOfDateKHR)
	{
		vulkanSwapChain.recreateSwapChain(vulkanDevice.getPhysicalDevice(), vulkanDevice.getLogicalDevice(), surface, window);
		return;
	}

	if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
	{
		assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	classUniformBuffer.updateMVPMat(vulkanSwapChain.getSwapChainExtent(), frameIndex);

	device.resetFences(*inFlightFences[frameIndex]);

	classCommandBuffer.getCommandBuffers()[frameIndex].reset();
	classCommandBuffer.recordCommandBuffer(frameIndex, vulkanSwapChain, imageIndex, classGraphicsPipeline, classVertexBuffer, depthBuffer);

	//vulkanDevice.getQueue().waitIdle();

	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	const vk::SubmitInfo submitInfo{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &*presentCompleteSemaphores[frameIndex],
		.pWaitDstStageMask = &waitDestinationStageMask,
		.commandBufferCount = 1,
		.pCommandBuffers = &*classCommandBuffer.getCommandBuffers()[frameIndex],
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &*renderFinishedSemaphores[imageIndex]
	};

	vulkanDevice.getQueue().submit(submitInfo, *inFlightFences[frameIndex]);

	const vk::PresentInfoKHR presentInfoKHR{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &*renderFinishedSemaphores[imageIndex],
		.swapchainCount = 1,
		.pSwapchains = &*vulkanSwapChain.getSwapChain(),
		.pImageIndices = &imageIndex
	};
	try
	{
		result = vulkanDevice.getQueue().presentKHR(presentInfoKHR);
	}
	catch (const std::exception& e)
	{
		if ((result == vk::Result::eSuboptimalKHR) || (result == vk::Result::eErrorOutOfDateKHR) || framebufferResized)
		{
			framebufferResized = false;
			vulkanSwapChain.recreateSwapChain(vulkanDevice.getPhysicalDevice(), vulkanDevice.getLogicalDevice(), surface, window);
		}
		else
		{
			assert(result == vk::Result::eSuccess);
		}
	}
	
	frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}