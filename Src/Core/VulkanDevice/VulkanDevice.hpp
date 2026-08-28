#pragma once
#include<vulkan/vulkan_raii.hpp>
#include<vector>

class VulkanDevice
{
public:
	VulkanDevice();
	~VulkanDevice() {};

	void createDevice(const vk::raii::Instance& instance, const vk::raii::SurfaceKHR& surface);
	const vk::raii::PhysicalDevice& getPhysicalDevice() const;
	const vk::raii::Device& getLogicalDevice() const;
	const vk::raii::Queue& getQueue() const;
	const uint32_t& getQueueIndex() const;

private:
	vk::raii::PhysicalDevice m_physicalDevice;
	vk::raii::Device m_device;
	vk::raii::Queue m_queue;
	uint32_t m_queueIndex;

	bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice);
	void pickPhysicalDevice(const vk::raii::Instance& instance);
	void createLogicalDevice(const vk::raii::SurfaceKHR& surface);
};