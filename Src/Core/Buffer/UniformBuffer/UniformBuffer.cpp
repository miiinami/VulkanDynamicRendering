#include"UniformBuffer.hpp"
#include"Helper/BufferHelper/BufferHelper.hpp"
#include<glm/gtc/matrix_transform.hpp>
#include<chrono>

UniformBuffer::UniformBuffer()
{
	m_MVPuniformBuffers.reserve(MAX_FRAMES_IN_FLIGHT);
	m_MVPuniformBuffersMemory.reserve(MAX_FRAMES_IN_FLIGHT);
	m_MVPuniformBuffersMapped.reserve(MAX_FRAMES_IN_FLIGHT);
}

void UniformBuffer::createUniformBuffers(const VulkanDevice& vulkanDevice, const vk::raii::CommandPool& commandPool)
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vk::DeviceSize bufferSize = sizeof(MVPMat);
		auto [buffer, bufferMem] = BufferHelper::createBuffer(vulkanDevice, bufferSize,
			vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		m_MVPuniformBuffers.emplace_back(std::move(buffer));
		m_MVPuniformBuffersMemory.emplace_back(std::move(bufferMem));
		m_MVPuniformBuffersMapped.emplace_back(m_MVPuniformBuffersMemory.back().mapMemory(0, bufferSize));
	}
}

void UniformBuffer::updateMVPMat(const vk::Extent2D& swapChainExtent, const uint32_t& currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	MVPMat mvp{};
	mvp.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	mvp.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	mvp.proj = glm::perspective(glm::radians(45.0f), static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height), 0.1f, 10.f);
	mvp.proj[1][1] *= -1;

	memcpy(m_MVPuniformBuffersMapped[currentImage], &mvp, sizeof(mvp));
}

const std::vector<vk::raii::Buffer>& UniformBuffer::getBuffers() const
{
	return m_MVPuniformBuffers;
}