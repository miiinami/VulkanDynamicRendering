#pragma once
#include<vulkan/vulkan_raii.hpp>
#include<GLFW/glfw3.h>
#include<vector>

class VulkanInstance
{
public:
	VulkanInstance();
	~VulkanInstance();

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif // NDEBUG

	const vk::raii::Instance& getInstance() const;
	void CreateInstance(GLFWwindow *window);

private:
	vk::raii::Context m_context;
	vk::raii::Instance m_instance;

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	std::vector<const char*> getRequiredInstanceExtensions(GLFWwindow *window);
};