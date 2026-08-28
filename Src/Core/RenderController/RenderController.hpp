#pragma once
#include<vulkan/vulkan_raii.hpp>

class VulkanDevice;
class VulkanSwapChain;
class CommandBuffer;


class RenderController
{
public:
	uint32_t frameIndex;

	RenderController();
	~RenderController() {};

	void drawFrame(const VulkanDevice& vulkanDevice, const VulkanSwapChain& vulkanSwapChain, CommandBuffer& classCommandBuffer, const vk::raii::Pipeline& graphicsPipeline);
	void createSyncObjects(const vk::raii::Device& device, const size_t& ImagesCount, const uint32_t& MAX_FRAMES_IN_FLIGHT);
private:
	std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
	std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
	std::vector<vk::raii::Fence> inFlightFences;
};