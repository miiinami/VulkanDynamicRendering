#pragma once
#include<vulkan/vulkan_raii.hpp>
#include<vector>

class VulkanDevice;
class TextureBuffer;

class GraphicsPipeline
{
public:
	GraphicsPipeline();
	~GraphicsPipeline() {};

	void createGraphicsPipeline(const VulkanDevice& vulkanDevice, const vk::SurfaceFormatKHR& swapChainSurfaceFormat);
	void createDescriptorPool(const vk::raii::Device& device);
	void createDescriptorSets(const vk::raii::Device& device, const std::vector<vk::raii::Buffer>& uniformBuffers, const TextureBuffer& textureBuffer);

	const vk::raii::PipelineLayout& getDescriptorSetLayout() const;
	const vk::raii::Pipeline& getGraphicsPipeline() const;
	const std::vector<vk::raii::DescriptorSet>& getDescriptorSets() const;

private:
	vk::raii::PipelineLayout m_pipelineLayout;
	vk::raii::DescriptorSetLayout m_descriptorSetLayout;
	vk::raii::Pipeline m_graphicsPipeline;
	vk::raii::DescriptorPool m_descriptorPool;
	std::vector<vk::raii::DescriptorSet> m_descriptorSets;

	void createDescriptorSetLayout(const vk::raii::Device& device);
	_NODISCARD/*[[nodiscard]]*/ vk::raii::ShaderModule createShaderModule(const vk::raii::Device& device, const std::vector<char>& codefile);
};