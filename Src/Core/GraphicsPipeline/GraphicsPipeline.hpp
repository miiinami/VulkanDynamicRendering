#pragma once
#include<vulkan/vulkan_raii.hpp>
#include<vector>

class GraphicsPipeline
{
public:
	GraphicsPipeline();
	~GraphicsPipeline() {};

	void createGraphicsPipeline(const vk::raii::Device& device, const vk::SurfaceFormatKHR& swapChainSurfaceFormat);

	const vk::raii::Pipeline& getGraphicsPipeline() const;

private:
	vk::raii::PipelineLayout m_pipelineLayout;
	vk::raii::Pipeline m_graphicsPipeline;

	[[nodiscard]] vk::raii::ShaderModule createShaderModule(const vk::raii::Device& device, const std::vector<char>& codefile);
};