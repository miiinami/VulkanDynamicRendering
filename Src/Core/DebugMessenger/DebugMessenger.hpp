#pragma once
#include<vulkan/vulkan_raii.hpp>

class DebugMessenger
{
public:
	DebugMessenger();
	~DebugMessenger() {};

	void setupDebugMessenger(const bool& enableValidationLayers,const vk::raii::Instance& instance);
private:
	vk::raii::DebugUtilsMessengerEXT m_debugMessenger;

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
		vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*);
};