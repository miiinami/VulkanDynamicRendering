#pragma once
#include<vulkan/vulkan_raii.hpp>
#include<GLFW/glfw3.h>

class VulkanDevice;
class VulkanSwapChain;
class CommandBuffer;
class VertexBuffer;
class GraphicsPipeline;
class UniformBuffer;
class DepthBuffer;


class RenderController
{
public:
	uint32_t frameIndex;

	RenderController();
	~RenderController() {};

	void drawFrame(const VulkanDevice& vulkanDevice, VulkanSwapChain& vulkanSwapChain,
		CommandBuffer& classCommandBuffer, const GraphicsPipeline& classGraphicsPipeline,
		bool& framebufferResized, const vk::raii::SurfaceKHR& surface,
		GLFWwindow* window, const VertexBuffer& classVertexBuffer, UniformBuffer& classUniformBuffer, const DepthBuffer& depthBuffer);
	void createSyncObjects(const vk::raii::Device& device, const size_t& ImagesCount);
private:
	std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
	std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
	std::vector<vk::raii::Fence> inFlightFences;
};