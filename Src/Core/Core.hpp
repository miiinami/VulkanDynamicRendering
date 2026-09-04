#pragma once
#include"GLFWwidnow/GLFWwindowClass.hpp"
#include"VulkanInstance/VulkanInstance.hpp"
#include"DebugMessenger/DebugMessenger.hpp"
#include"VulkanSurface/VulkanSurface.hpp"
#include"VulkanDevice/VulkanDevice.hpp"
#include"SwapChain/VulkanSwapChain.hpp"
#include"GraphicsPipeline/GraphicsPipeline.hpp"
#include"Buffer/VertexBuffer/VertexBuffer.hpp"
#include"Buffer/CommandBuffer/CommandBuffer.hpp"
#include"RenderController/RenderController.hpp"
#include"Buffer/UniformBuffer/UniformBuffer.hpp"
#include"Buffer/TextureBuffer/TextureBuffer.hpp"
#include"Buffer/DepthBuffer/DepthBuffer.hpp"

class Core
{
public:
	void Init();
	void CleanUp();
	bool WindowShouldClose();
	void drawFrame();
	void deviceWaitIdle();

private:
	void InitVulkan();
	//void MainLoop();

private:
	GLFWWindowClass glfwWindow;
	VulkanInstance vulkanInstance;
	DebugMessenger debugMessenger;
	VulkanSurface vulkanSurface;
	VulkanDevice vulkanDevice;
	VulkanSwapChain vulkanSwapChain;
	GraphicsPipeline graphicsPipeline;
	VertexBuffer vertexBuffer;
	CommandBuffer commandBuffer;
	RenderController renderController;
	UniformBuffer uniformBuffer;
	TextureBuffer textureBuffer;
	DepthBuffer depthBuffer;
};