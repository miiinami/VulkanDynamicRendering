#include"ImageHelper.hpp"
#include"Helper/BufferHelper/BufferHelper.hpp"
#include"Core/VulkanDevice/VulkanDevice.hpp"

std::pair<vk::raii::Image, vk::raii::DeviceMemory> ImageHelper::createImage(const VulkanDevice& vulkanDevice, const uint32_t& width, const uint32_t& height, const vk::Format& format, const vk::ImageTiling& tiling, const vk::ImageUsageFlags& usage, const vk::MemoryPropertyFlags& properties)
{
	const auto& device = vulkanDevice.getLogicalDevice();
	const auto& physicalDevice = vulkanDevice.getPhysicalDevice();

	vk::ImageCreateInfo imageInfo{
		.imageType = vk::ImageType::e2D,
		.format = format,
		.extent = {width,height,1},
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = vk::SampleCountFlagBits::e1,
		.tiling = tiling,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive
	};

	vk::raii::Image image = vk::raii::Image(device, imageInfo);

	vk::MemoryRequirements memRequirements = image.getMemoryRequirements();
	vk::MemoryAllocateInfo allocInfo{
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = BufferHelper::findMemoryType(physicalDevice,memRequirements.memoryTypeBits,properties)
	};

	vk::raii::DeviceMemory imageMemory = vk::raii::DeviceMemory(device, allocInfo);
	image.bindMemory(imageMemory, 0);
	return { std::move(image),std::move(imageMemory) };
}

void ImageHelper::transitionImageLayout(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Image& image, vk::ImageLayout&& oldLayout, vk::ImageLayout&& newLayout)
{
	vk::ImageMemoryBarrier barrier{
		.oldLayout = oldLayout,
		.newLayout = newLayout,
		.srcQueueFamilyIndex = vk::QueueFamilyIgnored,
		.dstQueueFamilyIndex = vk::QueueFamilyIgnored,
		.image = image,
		.subresourceRange = {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.levelCount = 1,
			.layerCount = 1
		}
	};

	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.srcAccessMask = {};
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition!");
	}
	commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, {}, barrier);
}

void ImageHelper::copyBufferToImage(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Buffer& buffer, vk::raii::Image& image, const uint32_t& width, const uint32_t& height)
{
	vk::BufferImageCopy region{
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource = {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1
		},
		.imageOffset = {0,0,0},
		.imageExtent = {width,height,1}
	};
	commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, region);
}

vk::raii::ImageView ImageHelper::createImageView(const vk::raii::Device& device, const vk::Image& image, const vk::Format& format, const vk::ImageAspectFlags aspectFlags)
{
	vk::ImageViewCreateInfo viewInfo{
	.image = image,
	.viewType = vk::ImageViewType::e2D,
	.format = format,
	.subresourceRange = {
			.aspectMask = aspectFlags,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};
	return vk::raii::ImageView(device, viewInfo);
}