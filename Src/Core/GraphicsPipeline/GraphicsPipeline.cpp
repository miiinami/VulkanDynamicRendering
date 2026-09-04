#include"GraphicsPipeline.hpp"
#include"Helper/ReadResources/ReadResources.hpp"
#include"Core/Buffer/VertexBuffer/VertexBuffer.hpp"
#include"Core/Buffer/UniformBuffer/UniformBuffer.hpp"
#include"Core/Buffer/TextureBuffer/TextureBuffer.hpp"
#include"Helper/BufferHelper/BufferHelper.hpp"
#include"Core/VulkanDevice/VulkanDevice.hpp"

GraphicsPipeline::GraphicsPipeline() :m_descriptorSetLayout(nullptr), m_pipelineLayout(nullptr), m_graphicsPipeline(nullptr), m_descriptorPool(nullptr)
{
	m_descriptorSets.reserve(MAX_FRAMES_IN_FLIGHT);
}

void GraphicsPipeline::createGraphicsPipeline(const VulkanDevice& vulkanDevice, const vk::SurfaceFormatKHR& swapChainSurfaceFormat)
{
	const auto& device = vulkanDevice.getLogicalDevice();
	const auto& physicalDevice = vulkanDevice.getPhysicalDevice();

	vk::raii::ShaderModule vertexShaderModule = createShaderModule(device, ReadResources::readShaderFile("Shader/vertexshader.spv"));
	vk::raii::ShaderModule fragmentShaderModule = createShaderModule(device, ReadResources::readShaderFile("Shader/fragmentshader.spv"));

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eVertex, .module = vertexShaderModule,  .pName = "main" };
	vk::PipelineShaderStageCreateInfo fragShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eFragment, .module = fragmentShaderModule,  .pName = "main" };

	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo,fragShaderStageInfo };

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescription = Vertex::getAttributeDescription();

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &bindingDescription,
		.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size()),
		.pVertexAttributeDescriptions = attributeDescription.data()
	};

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
		.frontFace = vk::FrontFace::eCounterClockwise,
		.depthBiasEnable = vk::False,
		.lineWidth = 1.0f
	};

	vk::PipelineMultisampleStateCreateInfo multisampling{
		.rasterizationSamples = vk::SampleCountFlagBits::e1,
		.sampleShadingEnable = vk::False
	};

	vk::PipelineDepthStencilStateCreateInfo depthStencil{
			.depthTestEnable = vk::True,
			.depthWriteEnable = vk::True,
			.depthCompareOp = vk::CompareOp::eLess,
			.depthBoundsTestEnable = vk::False,
			.stencilTestEnable = vk::False
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

	createDescriptorSetLayout(device);

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
		.setLayoutCount = 1,
		.pSetLayouts = &*m_descriptorSetLayout,
		.pushConstantRangeCount = 0
	};

	m_pipelineLayout = vk::raii::PipelineLayout(device, pipelineLayoutInfo);

	vk::Format depthFormat = BufferHelper::findSupportedFormat(physicalDevice, { vk::Format::eD32Sfloat,vk::Format::eD32SfloatS8Uint,vk::Format::eD24UnormS8Uint },
		vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);

	vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain{
		{
		.stageCount = 2,
		.pStages = shaderStages,
		.pVertexInputState = &vertexInputInfo,
		.pInputAssemblyState = &inputAssembly,
		.pViewportState = &viewportState,
		.pRasterizationState = &rasterizer,
		.pMultisampleState = &multisampling,
		.pDepthStencilState = &depthStencil,
		.pColorBlendState = &colorBlending,
		.pDynamicState = &dynamicState,
		.layout = m_pipelineLayout,
		.renderPass = nullptr
		},
		{
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &swapChainSurfaceFormat.format,
		.depthAttachmentFormat = depthFormat 
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

void GraphicsPipeline::createDescriptorSetLayout(const vk::raii::Device& device)
{
	std::array<vk::DescriptorSetLayoutBinding, 2> bindings{
		{
			{
				.binding = 0,
				.descriptorType = vk::DescriptorType::eUniformBuffer,
				.descriptorCount = 1,
				.stageFlags = vk::ShaderStageFlagBits::eVertex
			},
			{
				.binding = 1,
				.descriptorType = vk::DescriptorType::eCombinedImageSampler,
				.descriptorCount = 1,
				.stageFlags = vk::ShaderStageFlagBits::eFragment
			}
		}
	};

	vk::DescriptorSetLayoutCreateInfo layoutInfo{
		.bindingCount = static_cast<uint32_t>(bindings.size()),
		.pBindings = bindings.data()
	};

	m_descriptorSetLayout = vk::raii::DescriptorSetLayout(device, layoutInfo);
}

void GraphicsPipeline::createDescriptorPool(const vk::raii::Device& device)
{
	std::array<vk::DescriptorPoolSize, 2> poolSize{
		{
			{.type = vk::DescriptorType::eUniformBuffer,.descriptorCount = MAX_FRAMES_IN_FLIGHT,},
			{.type = vk::DescriptorType::eCombinedImageSampler,.descriptorCount = MAX_FRAMES_IN_FLIGHT,}
		}
	};

	vk::DescriptorPoolCreateInfo poolInfo{
		.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		.maxSets = MAX_FRAMES_IN_FLIGHT,
		.poolSizeCount = static_cast<uint32_t>(poolSize.size()),
		.pPoolSizes = poolSize.data()
	};

	m_descriptorPool = vk::raii::DescriptorPool(device, poolInfo);
}

void GraphicsPipeline::createDescriptorSets(const vk::raii::Device& device, const std::vector<vk::raii::Buffer>& uniformBuffers, const TextureBuffer& textureBuffer)
{
	std::vector<vk::DescriptorSetLayout> layout(MAX_FRAMES_IN_FLIGHT, *m_descriptorSetLayout);
	vk::DescriptorSetAllocateInfo allocInfo{
		.descriptorPool = m_descriptorPool,
		.descriptorSetCount = static_cast<uint32_t>(layout.size()),
		.pSetLayouts = layout.data()
	};

	m_descriptorSets = device.allocateDescriptorSets(allocInfo);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vk::DescriptorBufferInfo bufferInfo{
			.buffer = uniformBuffers[i],
			.offset = 0,
			.range = sizeof(MVPMat)
		};
		vk::DescriptorImageInfo imageInfo{
			.sampler = textureBuffer.getTextureSampler(),
			.imageView = textureBuffer.getTextureImageView(),
			.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
		};
		std::array<vk::WriteDescriptorSet,2> descriptorWrites{
			{
				{
					.dstSet = m_descriptorSets[i],
					.dstBinding = 0u,
					.dstArrayElement = 0u,
					.descriptorCount = 1u,
					.descriptorType = vk::DescriptorType::eUniformBuffer,
					.pBufferInfo = &bufferInfo
				},
				{
					.dstSet = m_descriptorSets[i],
					.dstBinding = 1u,
					.dstArrayElement = 0u,
					.descriptorCount = 1u,
					.descriptorType = vk::DescriptorType::eCombinedImageSampler,
					.pImageInfo = &imageInfo
				}
			}
		};
		device.updateDescriptorSets(descriptorWrites, {});
	}
}

const vk::raii::Pipeline& GraphicsPipeline::getGraphicsPipeline() const
{
	return m_graphicsPipeline;
}

const vk::raii::PipelineLayout& GraphicsPipeline::getDescriptorSetLayout() const
{
	return m_pipelineLayout;
}

const std::vector<vk::raii::DescriptorSet>& GraphicsPipeline::getDescriptorSets() const
{
	return m_descriptorSets;
}