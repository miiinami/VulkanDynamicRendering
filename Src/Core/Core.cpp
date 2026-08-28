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

	graphicsPipeline.createGraphicsPipeline(vulkanDevice.getLogicalDevice(), vulkanSwapChain.getSwapChainSurfaceFormat());

	commandBuffer.createAboutCommand(vulkanDevice.getLogicalDevice(), vulkanDevice.getQueueIndex());

	renderController.createSyncObjects(vulkanDevice.getLogicalDevice(), vulkanSwapChain.getSwapChainImages().size(), commandBuffer.MAX_FRAMES_IN_FLIGHT);
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

void Core::drawFrame()
{
	renderController.drawFrame(vulkanDevice, vulkanSwapChain, commandBuffer, graphicsPipeline.getGraphicsPipeline());
}

void Core::deviceWaitIdle()
{
	vulkanDevice.getLogicalDevice().waitIdle();
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