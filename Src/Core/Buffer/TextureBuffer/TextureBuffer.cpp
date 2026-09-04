#include"TextureBuffer.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>
#include"Helper/BufferHelper/BufferHelper.hpp"
#include"Core/VulkanDevice/VulkanDevice.hpp"
#include"Helper/ImageHelper/ImageHelper.hpp"
#include"Core/Buffer/CommandBuffer/CommandBuffer.hpp"

TextureBuffer::TextureBuffer() :m_textureImage(nullptr), m_textureImageMemory(nullptr), m_textureImageView(nullptr), m_textureSampler(nullptr)
{

}

void TextureBuffer::createTextureImage(const VulkanDevice& vulkanDevice, const CommandBuffer& classCommandBuffer)
{
	int texWidth, texHeight, texChannls;
	stbi_uc* pixels = stbi_load("Resources/Textures/texture.jpg", &texWidth, &texHeight, &texChannls, STBI_rgb_alpha);
	vk::DeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels)
	{
		throw std::runtime_error("failed to load texture image!");
	}

	auto [stagingBuffer, stagingBufferMemory] = BufferHelper::createBuffer(vulkanDevice, imageSize, vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	void* data = stagingBufferMemory.mapMemory(0, imageSize);
	memcpy(data, pixels, imageSize);
	stagingBufferMemory.unmapMemory();

	stbi_image_free(pixels);

	std::tie(m_textureImage, m_textureImageMemory) = ImageHelper::createImage(vulkanDevice, texWidth, texHeight,
		vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		vk::MemoryPropertyFlagBits::eDeviceLocal);

	vk::raii::CommandBuffer commandBuffer = BufferHelper::beginSingleTimeCommands(vulkanDevice.getLogicalDevice(), classCommandBuffer.getCommandPool());
	ImageHelper::transitionImageLayout(commandBuffer, m_textureImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
	ImageHelper::copyBufferToImage(commandBuffer, stagingBuffer, m_textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	ImageHelper::transitionImageLayout(commandBuffer, m_textureImage, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
	BufferHelper::endSingleTimeCommands(std::move(commandBuffer), vulkanDevice.getQueue());

	createTextureImageView(vulkanDevice.getLogicalDevice());
	createTextureSampler(vulkanDevice);
}

void TextureBuffer::createTextureImageView(const vk::raii::Device& device)
{
	m_textureImageView = ImageHelper::createImageView(device, *m_textureImage, vk::Format::eR8G8B8A8Srgb,vk::ImageAspectFlagBits::eColor);
}

void TextureBuffer::createTextureSampler(const VulkanDevice& vulkanDevice)
{
	const auto& device = vulkanDevice.getLogicalDevice();
	const auto& physicalDevice = vulkanDevice.getPhysicalDevice();

	vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();
	vk::SamplerCreateInfo samplerInfo{
		.magFilter = vk::Filter::eLinear,
		.minFilter = vk::Filter::eLinear,
		.mipmapMode = vk::SamplerMipmapMode::eLinear,
		.addressModeU = vk::SamplerAddressMode::eRepeat,
		.addressModeV = vk::SamplerAddressMode::eRepeat,
		.addressModeW = vk::SamplerAddressMode::eRepeat,
		.anisotropyEnable = vk::True,
		.maxAnisotropy = properties.limits.maxSamplerAnisotropy,
		.compareEnable = vk::False,
		.compareOp = vk::CompareOp::eAlways
	};

	m_textureSampler = vk::raii::Sampler(device, samplerInfo);
}

const vk::raii::Sampler& TextureBuffer::getTextureSampler() const
{
	return m_textureSampler;
}

const vk::raii::ImageView& TextureBuffer::getTextureImageView() const
{
	return m_textureImageView;
}