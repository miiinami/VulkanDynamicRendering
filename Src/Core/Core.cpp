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

	vulkanSwapChain.createImageViews(vulkanDevice.getLogicalDevice());

	graphicsPipeline.createGraphicsPipeline(vulkanDevice, vulkanSwapChain.getSwapChainSurfaceFormat());

	commandBuffer.createCommandPool(vulkanDevice.getLogicalDevice(), vulkanDevice.getQueueIndex());

	depthBuffer.createDepthResources(vulkanDevice, vulkanSwapChain.getSwapChainExtent());

	textureBuffer.createTextureImage(vulkanDevice, commandBuffer);

	vertexBuffer.createVertexBuffer(vulkanDevice, commandBuffer.getCommandPool());

	vertexBuffer.createIndexBuffer(vulkanDevice, commandBuffer.getCommandPool());

	uniformBuffer.createUniformBuffers(vulkanDevice, commandBuffer.getCommandPool());

	graphicsPipeline.createDescriptorPool(vulkanDevice.getLogicalDevice());

	graphicsPipeline.createDescriptorSets(vulkanDevice.getLogicalDevice(), uniformBuffer.getBuffers(), textureBuffer);

	commandBuffer.createCommandBuffers(vulkanDevice.getLogicalDevice());

	renderController.createSyncObjects(vulkanDevice.getLogicalDevice(), vulkanSwapChain.getSwapChainImages().size());
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
	renderController.drawFrame(vulkanDevice, vulkanSwapChain,
		commandBuffer, graphicsPipeline,
		glfwWindow.framebufferResized, vulkanSurface.getSurface(),
		glfwWindow.getGLFWwindow(), vertexBuffer, uniformBuffer, depthBuffer);
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