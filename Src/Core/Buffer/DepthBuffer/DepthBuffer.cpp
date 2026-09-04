#include"DepthBuffer.hpp"
#include"Helper/BufferHelper/BufferHelper.hpp"
#include"Helper/ImageHelper/ImageHelper.hpp"
#include"Core/VulkanDevice/VulkanDevice.hpp"

DepthBuffer::DepthBuffer() :m_depthImage(nullptr), m_depthImageMemory(nullptr), m_depthImageView(nullptr)
{

}

void DepthBuffer::createDepthResources(const VulkanDevice& vulkanDevice,const vk::Extent2D& extent)
{
	vk::Format depthFormat = findDepthFormat(vulkanDevice.getPhysicalDevice());

	std::tie(m_depthImage, m_depthImageMemory) = ImageHelper::createImage(vulkanDevice, extent.width, extent.height, depthFormat,
		vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal);

	m_depthImageView = ImageHelper::createImageView(vulkanDevice.getLogicalDevice(), m_depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
}

vk::Format DepthBuffer::findDepthFormat(const vk::raii::PhysicalDevice& physicalDevice)
{
	return BufferHelper::findSupportedFormat(physicalDevice, { vk::Format::eD32Sfloat,vk::Format::eD32SfloatS8Uint,vk::Format::eD24UnormS8Uint }, 
		vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

const vk::raii::Image& DepthBuffer::getDepthImage() const
{
	return m_depthImage;
}

const vk::raii::ImageView& DepthBuffer::getDepthImageView() const
{
	return m_depthImageView;
}