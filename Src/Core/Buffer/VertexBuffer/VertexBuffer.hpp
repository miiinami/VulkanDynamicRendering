#pragma once
#include<vulkan/vulkan_raii.hpp>
#include<glm/glm.hpp>
#include<array>
#include<vector>

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	static vk::VertexInputBindingDescription getBindingDescription();

	static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescription();
};

const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

	{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4
};

class VulkanDevice;

class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer() {};

	void createVertexBuffer(const VulkanDevice& vulkanDevice, const vk::raii::CommandPool& commandPool);
	void createIndexBuffer(const VulkanDevice& vulkanDevice, const vk::raii::CommandPool& commandPool);

	const vk::raii::Buffer& getVertexBuffer() const;
	const vk::raii::Buffer& getIndexBuffer() const;
private:
	vk::raii::Buffer m_vertexBuffer;
	vk::raii::DeviceMemory m_vertexBufferMemory;
	vk::raii::Buffer m_indexBuffer;
	vk::raii::DeviceMemory m_indexBufferMemory;
};