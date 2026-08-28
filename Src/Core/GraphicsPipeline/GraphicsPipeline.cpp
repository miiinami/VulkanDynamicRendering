#include"GraphicsPipeline.hpp"
#include"Helper/ReadResources/ReadResources.hpp"

GraphicsPipeline::GraphicsPipeline():m_pipelineLayout(nullptr),m_graphicsPipeline(nullptr)
{

}

void GraphicsPipeline::createGraphicsPipeline(const vk::raii::Device& device, const vk::SurfaceFormatKHR& swapChainSurfaceFormat)
{
	vk::raii::ShaderModule vertexShaderModule = createShaderModule(device, ReadResources::readShaderFile("Shader/vertexshader.spv"));
	vk::raii::ShaderModule fragmentShaderModule = createShaderModule(device, ReadResources::readShaderFile("Shader/fragmentshader.spv"));

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eVertex, .module = vertexShaderModule,  .pName = "main" };
	vk::PipelineShaderStageCreateInfo fragShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eFragment, .module = fragmentShaderModule,  .pName = "main" };

	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo,fragShaderStageInfo };

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
		.topology = vk::PrimitiveTopology::eTriangleList
	};

	vk::PipelineViewportStateCreateInfo viewportState{
		.viewportCount = 1u,
		.scissorCount = 1u
	};

	vk::PipelineRasterizationStateCreateInfo rasterizer{
		.depthClampEnable = vk::False,
		.rasterizerDiscardEnable = vk::False,
		.polygonMode = vk::PolygonMode::eFill,
		.cullMode = vk::CullModeFlagBits::eBack,
		.frontFace = vk::FrontFace::eClockwise,
		.depthBiasEnable = vk::False,
		.lineWidth = 1.0f
	};

	vk::PipelineMultisampleStateCreateInfo multisampling{
		.rasterizationSamples = vk::SampleCountFlagBits::e1,
		.sampleShadingEnable = vk::False
	};

	vk::PipelineColorBlendAttachmentState colorBlendAttachment{
		.blendEnable = vk::False,
		.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
	};
	vk::PipelineColorBlendStateCreateInfo colorBlending{
		.logicOpEnable = vk::False,
		.logicOp = vk::LogicOp::eCopy,
		.attachmentCount = 1u,
		.pAttachments = &colorBlendAttachment
	};

	std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport,vk::DynamicState::eScissor };
	vk::PipelineDynamicStateCreateInfo dynamicState{
		.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
		.pDynamicStates = dynamicStates.data()
	};

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
		.setLayoutCount = 0u,
		.pushConstantRangeCount = 0u
	};

	m_pipelineLayout = vk::raii::PipelineLayout(device, pipelineLayoutInfo);

	vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain =
	{
		{
		.stageCount = 2,
		.pStages = shaderStages,
		.pVertexInputState = &vertexInputInfo,
		.pInputAssemblyState = &inputAssembly,
		.pViewportState = &viewportState,
		.pRasterizationState = &rasterizer,
		.pMultisampleState = &multisampling,
		.pColorBlendState = &colorBlending,
		.pDynamicState = &dynamicState,
		.layout = m_pipelineLayout,
		.renderPass = nullptr
		},
		{
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &swapChainSurfaceFormat.format
		}
	};

	m_graphicsPipeline = vk::raii::Pipeline(device, nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());
}

vk::raii::ShaderModule GraphicsPipeline::createShaderModule(const vk::raii::Device& device, const std::vector<char>& codefile)
{
	vk::ShaderModuleCreateInfo createInfo{
		.codeSize = codefile.size() * sizeof(char),
		.pCode = reinterpret_cast<const uint32_t*>(codefile.data())
	};

	vk::raii::ShaderModule shaderModule(device, createInfo);

	return shaderModule;
}

const vk::raii::Pipeline& GraphicsPipeline::getGraphicsPipeline() const
{
	return m_graphicsPipeline;
}