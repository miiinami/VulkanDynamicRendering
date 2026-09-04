#pragma once
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<vulkan/vulkan_raii.hpp>
#include<glm/glm.hpp>
#include<vector>

struct MVPMat
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

class VulkanDevice;

class UniformBuffer
{
public:
	UniformBuffer();
	~UniformBuffer() {};

	void createUniformBuffers(const VulkanDevice& vulkanDevice, const vk::raii::CommandPool& commandPool);
	void updateMVPMat(const vk::Extent2D& swapChainExtent, const uint32_t& currentImage);

	const std::vector<vk::raii::Buffer>& getBuffers() const;

private:
	std::vector<vk::raii::Buffer> m_MVPuniformBuffers;
	std::vector<vk::raii::DeviceMemory> m_MVPuniformBuffersMemory;
	std::vector<void*> m_MVPuniformBuffersMapped;
};