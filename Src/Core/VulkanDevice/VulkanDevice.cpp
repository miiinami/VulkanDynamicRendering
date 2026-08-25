#include"VulkanDevice.hpp"
#include<stdexcept>
#include<map>
#include<cassert>

VulkanDevice::VulkanDevice() :m_physicalDevice(nullptr), m_device(nullptr), m_queue(nullptr)
{

}

const vk::raii::PhysicalDevice& VulkanDevice::getPhysicalDevice() const
{
	return m_physicalDevice;
}

const vk::raii::Device& VulkanDevice::getLogicalDevice() const
{
	return m_device;
}

bool VulkanDevice::isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice)
{
	bool supportsVulkan1_3 = physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

	auto queueFamilies = physicalDevice.getQueueFamilyProperties();
	bool supportsGraphics = std::ranges::any_of(queueFamilies, [](auto const& qfp)
		{
			return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
		});

	std::vector<const char*> requiredDeviceExtension =
	{
		vk::KHRSwapchainExtensionName
	};

	auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
	bool supportsAllRequiredExtensions = std::ranges::all_of(
		requiredDeviceExtension, [&availableDeviceExtensions](auto const& requireDeviceExtension)
		{
			return std::ranges::any_of(availableDeviceExtensions,
				[&requireDeviceExtension](auto const& availableDeviceExtension)
				{
					return strcmp(availableDeviceExtension.extensionName, requireDeviceExtension) == 0;
				});
		}
	);
	auto features = physicalDevice.template getFeatures2<
		vk::PhysicalDeviceFeatures2,
		vk::PhysicalDeviceVulkan11Features,
		vk::PhysicalDeviceVulkan13Features,
		vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();

	bool supportsRequiredFeatures = features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
		features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
		features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

	return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
}

void VulkanDevice::pickPhysicalDevice(const vk::raii::Instance& instance)
{
	std::vector<vk::raii::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();

	if (physicalDevices.empty())
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	auto const devIter = std::ranges::find_if(physicalDevices, [&](auto const& physicalDevice) { return isDeviceSuitable(physicalDevice); });
	if (devIter == physicalDevices.end())
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
	m_physicalDevice = *devIter;
}

void VulkanDevice::createLogicalDevice(const vk::raii::SurfaceKHR& surface)
{
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_physicalDevice.getQueueFamilyProperties();

	uint32_t queueIndex = ~0;
	for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++)
	{
		if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) && m_physicalDevice.getSurfaceSupportKHR(qfpIndex, *surface))
		{
			queueIndex = qfpIndex;
			break;
		}
	}
	if (queueIndex == ~0)
	{
		throw std::runtime_error("Could not find a queue for graphics and present -> terminating");
	}

	float queuePriority = 0.5f;
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
		.queueFamilyIndex = queueIndex,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority
	};

	vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
		vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain = {
			{}, {.shaderDrawParameters = true},
			{.dynamicRendering = true}, {.extendedDynamicState = true}
	};

	std::vector<const char*> requiredDeviceExtension =
	{
		vk::KHRSwapchainExtensionName
	};

	vk::DeviceCreateInfo deviceCreateInfo{
		.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &deviceQueueCreateInfo,
		.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtension.size()),
		.ppEnabledExtensionNames = requiredDeviceExtension.data()
	};

	m_device = vk::raii::Device(m_physicalDevice, deviceCreateInfo);
	m_queue = vk::raii::Queue(m_device, queueIndex, 0);
}

void VulkanDevice::createDevice(const vk::raii::Instance& instance, const vk::raii::SurfaceKHR& surface)
{
	pickPhysicalDevice(instance);
	createLogicalDevice(surface);
}