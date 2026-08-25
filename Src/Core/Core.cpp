#include"Core.hpp"
#include<iostream>
#include<stdexcept>
#include<cstdlib>

void Core::InitVulkan()
{
	vulkanInstance.CreateInstance(glfwWindow.getGLFWwindow());
#ifndef NDEBUG
	debugMessenger.setupDebugMessenger(vulkanInstance.enableValidationLayers, vulkanInstance.getInstance());
#endif // !NDEBUG
	vulkanSurface.createSurface(vulkanInstance.getInstance(), glfwWindow.getGLFWwindow());
	vulkanDevice.createDevice(vulkanInstance.getInstance(), vulkanSurface.getSurface());
	vulkanSwapChain.createSwapChain(vulkanDevice.getPhysicalDevice(), vulkanDevice.getLogicalDevice(),
		vulkanSurface.getSurface(), glfwWindow.getGLFWwindow());
	vulkanSwapChain.createImageView(vulkanDevice.getLogicalDevice());

}

void Core::Init()
{
	glfwWindow.Init();
	InitVulkan();
}

bool Core::WindowShouldClose()
{
	return glfwWindowShouldClose(glfwWindow.getGLFWwindow());
}

//void Core::MainLoop()
//{
//	while (!glfwWindowShouldClose(glfwWindow.getGLFWwindow()))
//	{
//		glfwPollEvents();
//	}
//}

void Core::CleanUp()
{
	glfwWindow.CleanUp();
}