#include"VertexBuffer.hpp"
#include"Core/VulkanDevice/VulkanDevice.hpp"
#include"Helper/BufferHelper/BufferHelper.hpp"

vk::VertexInputBindingDescription Vertex::getBindingDescription()
{
	return {
		.binding = 0,
		.stride = sizeof(Vertex),
		.inputRate = vk::VertexInputRate::eVertex
	};
}

std::array<vk::VertexInputAttributeDescription, 3> Vertex::getAttributeDescription()
{
	return {
		{
			{.location = 0, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(Vertex, pos)},
			{.location = 1, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(Vertex, color)},
			{.location = 2, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = offsetof(Vertex, texCoord)}
		}
	};
}


VertexBuffer::VertexBuffer() :m_vertexBuffer(nullptr), m_vertexBufferMemory(nullptr), m_indexBuffer(nullptr), m_indexBufferMemory(nullptr)
{

}

void VertexBuffer::createVertexBuffer(const VulkanDevice& vulkanDevice, const vk::raii::CommandPool& commandPool)
{
	const auto& device = vulkanDevice.getLogicalDevice();
	const auto& physicalDevice = vulkanDevice.getPhysicalDevice();
	vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	auto [stagingBuffer, stagingBufferMemory] = BufferHelper::createBuffer(vulkanDevice, bufferSize,
		vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	void* dataStaging = stagingBufferMemory.mapMemory(0, bufferSize);
	memcpy(dataStaging, vertices.data(), static_cast<size_t>(bufferSize));
	stagingBufferMemory.unmapMemory();

	std::tie(m_vertexBuffer, m_vertexBufferMemory) = BufferHelper::createBuffer(vulkanDevice, bufferSize,
		vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

	BufferHelper::copyBuffer(vulkanDevice, commandPool, stagingBuffer, m_vertexBuffer, bufferSize);
}

void VertexBuffer::createIndexBuffer(const VulkanDevice& vulkanDevice, const vk::raii::CommandPool& commandPool)
{
	const auto& device = vulkanDevice.getLogicalDevice();
	const auto& physicalDevice = vulkanDevice.getPhysicalDevice();
	vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	auto [stagingBuffer, stagingBufferMemory] = BufferHelper::createBuffer(vulkanDevice, bufferSize,
		vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	void* dataStaging = stagingBufferMemory.mapMemory(0, bufferSize);
	memcpy(dataStaging, indices.data(), static_cast<size_t>(bufferSize));
	stagingBufferMemory.unmapMemory();

	std::tie(m_indexBuffer, m_indexBufferMemory) = BufferHelper::createBuffer(vulkanDevice, bufferSize,
		vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

	BufferHelper::copyBuffer(vulkanDevice, commandPool, stagingBuffer, m_indexBuffer, bufferSize);
}

const vk::raii::Buffer& VertexBuffer::getVertexBuffer() const
{
	return m_vertexBuffer;
}

const vk::raii::Buffer& VertexBuffer::getIndexBuffer() const
{
	return m_indexBuffer;
}