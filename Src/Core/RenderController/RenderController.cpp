#include"RenderController.hpp"
#include"Core/VulkanDevice/VulkanDevice.hpp"
#include"Core/SwapChain/VulkanSwapChain.hpp"
#include"Core/Buffer/CommandBuffer/CommandBuffer.hpp"
#include<iostream>

RenderController::RenderController() :frameIndex(0)
{

};

void RenderController::createSyncObjects(const vk::raii::Device& device, const size_t& ImagesCount, const uint32_t& MAX_FRAMES_IN_FLIGHT)
{
	for (size_t i = 0; i < ImagesCount; i++)
	{
		renderFinishedSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
	}
	for (size_t i = 0; i < static_cast<size_t>(MAX_FRAMES_IN_FLIGHT); i++)
	{
		presentCompleteSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
		inFlightFences.emplace_back(device, vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled });
	}
}

void RenderController::drawFrame(const VulkanDevice& vulkanDevice, const VulkanSwapChain& vulkanSwapChain,CommandBuffer& classCommandBuffer, const vk::raii::Pipeline& graphicsPipeline)
{
	const vk::raii::Device& device = vulkanDevice.getLogicalDevice();
	const vk::raii::Queue& queue = vulkanDevice.getQueue();
	const vk::raii::SwapchainKHR& swapChain = vulkanSwapChain.getSwapChain();
	const std::vector<vk::raii::CommandBuffer>& commandBuffers = classCommandBuffer.getCommandBuffers();

	auto fenceResult = device.waitForFences(*inFlightFences[frameIndex], vk::True, UINT64_MAX);
	if (fenceResult != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to wait for fence!");
	}
	device.resetFences(*inFlightFences[frameIndex]);

	auto [result, imageIndex] = swapChain.acquireNextImage(UINT64_MAX, *presentCompleteSemaphores[frameIndex], nullptr);

	classCommandBuffer.recordCommandBuffer(frameIndex, vulkanSwapChain, imageIndex, graphicsPipeline);

	queue.waitIdle();

	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	const vk::SubmitInfo submitInfo{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &*presentCompleteSemaphores[frameIndex],
		.pWaitDstStageMask = &waitDestinationStageMask,
		.commandBufferCount = 1,
		.pCommandBuffers = &*commandBuffers[frameIndex],
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &*renderFinishedSemaphores[frameIndex]
	};
	queue.submit(submitInfo, *inFlightFences[frameIndex]);

	const vk::PresentInfoKHR presentInfoKHR{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &*renderFinishedSemaphores[frameIndex],
		.swapchainCount = 1,
		.pSwapchains = &*swapChain,
		.pImageIndices = &imageIndex
	};

	result = queue.presentKHR(presentInfoKHR);
	switch (result)
	{
	case vk::Result::eSuccess:
		break;
	case vk::Result::eSuboptimalKHR:
		std::cout << "vk::Queue::presentKHR returned vk::Result::eSuboptimalKHR !\n";
		break;
	default:
		break;        // an unexpected result is returned!
	}
	frameIndex = (frameIndex + 1) % classCommandBuffer.MAX_FRAMES_IN_FLIGHT;
}