#pragma once
#include<vulkan/vulkan_raii.hpp>

class VulkanDevice;
class CommandBuffer;

class TextureBuffer
{
public:
	TextureBuffer();
	~TextureBuffer() {};

	void createTextureImage(const VulkanDevice& vulkanDevice, const CommandBuffer& classCommandBuffer);

	const vk::raii::Sampler& getTextureSampler() const;
	const vk::raii::ImageView& getTextureImageView() const;

private:
	vk::raii::Image m_textureImage;
	vk::raii::DeviceMemory m_textureImageMemory;
	vk::raii::ImageView m_textureImageView;
	vk::raii::Sampler m_textureSampler;

	void createTextureImageView(const vk::raii::Device& device);
	void createTextureSampler(const VulkanDevice& vulkanDevice);
};