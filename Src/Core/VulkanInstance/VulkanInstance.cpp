#include"VulkanInstance.hpp"
#include<stdexcept>
#include<string>

VulkanInstance::VulkanInstance():m_instance(nullptr)
{
}

VulkanInstance::~VulkanInstance()
{
}

const vk::raii::Instance& VulkanInstance::getInstance() const
{
	return m_instance;
}

void VulkanInstance::CreateInstance(GLFWwindow* window)
{
	constexpr vk::ApplicationInfo appInfo{
		.pApplicationName = "VulkanDynamicRendering",
		.applicationVersion = VK_MAKE_VERSION(1,0,0),
		.pEngineName = "No Engine",
		.engineVersion = VK_MAKE_VERSION(1,0,0),
		.apiVersion = vk::ApiVersion14
	};

	std::vector<char const*> requiredLayers;
	if (enableValidationLayers)
	{
		requiredLayers.assign(validationLayers.begin(), validationLayers.end());
	}

	auto layerProperties = m_context.enumerateInstanceLayerProperties();
	auto unsupportedLayerIt = std::ranges::find_if(requiredLayers,
		[&layerProperties](auto const& requiredLayer)
		{
			return std::ranges::none_of(layerProperties,
				[&requiredLayer](auto const& layerProperty)
				{
					return strcmp(layerProperty.layerName, requiredLayer) == 0;
				}
			);
		}
	);

	if (unsupportedLayerIt != requiredLayers.end())
	{
		throw std::runtime_error("Require layer not supported" + std::string(*unsupportedLayerIt));
	}

	auto requiredExtensions = getRequiredInstanceExtensions(window);

	auto extensionProperties = m_context.enumerateInstanceExtensionProperties();
	auto unsupportedPropertyIt =
		std::ranges::find_if(requiredExtensions,
			[&extensionProperties](auto const& requiredExtension) 
			{
				return std::ranges::none_of(extensionProperties,
					[requiredExtension](auto const& extensionProperty) { return strcmp(extensionProperty.extensionName, requiredExtension) == 0; });
			});
	if (unsupportedPropertyIt != requiredExtensions.end())
	{
		throw std::runtime_error("Required extension not supported: " + std::string(*unsupportedPropertyIt));
	}

	vk::InstanceCreateInfo createInfo{
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
		.ppEnabledLayerNames = requiredLayers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
		.ppEnabledExtensionNames = requiredExtensions.data()
	};

	m_instance = vk::raii::Instance(m_context, createInfo);
}

std::vector<const char*> VulkanInstance::getRequiredInstanceExtensions(GLFWwindow* window)
{
	uint32_t glfwExtensionCount = 0;
	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	if (enableValidationLayers)
	{
		extensions.push_back(vk::EXTDebugUtilsExtensionName);
	}

	return extensions;
}