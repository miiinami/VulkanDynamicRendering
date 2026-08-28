#include"VulkanSwapChain.hpp"

VulkanSwapChain::VulkanSwapChain():m_swapchain(nullptr)
{

}

void VulkanSwapChain::createSwapChain(const vk::raii::PhysicalDevice& physicaldevice, const vk::raii::Device& device, const vk::raii::SurfaceKHR& surface, GLFWwindow* window)
{
    vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicaldevice.getSurfaceCapabilitiesKHR(*surface);
    m_swapChainExtent = chooseSwapExtent(surfaceCapabilities, window);
    uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities);

    std::vector<vk::SurfaceFormatKHR> availableFormats = physicaldevice.getSurfaceFormatsKHR(*surface);
    m_swapChainSurfaceFormat = chooseSwapSurfaceFormat(availableFormats);

    std::vector<vk::PresentModeKHR> availablePresentModes = physicaldevice.getSurfacePresentModesKHR(*surface);
    vk::PresentModeKHR presentMode = chooseSwapPresentMode(availablePresentModes);

    vk::SwapchainCreateInfoKHR swapChainCreateInfo{
        .surface = *surface,
        .minImageCount = minImageCount,
        .imageFormat = m_swapChainSurfaceFormat.format,
        .imageColorSpace = m_swapChainSurfaceFormat.colorSpace,
        .imageExtent = m_swapChainExtent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = surfaceCapabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = chooseSwapPresentMode(availablePresentModes),
        .clipped = true
    };

    m_swapchain = vk::raii::SwapchainKHR(device, swapChainCreateInfo);
    m_swapChainImages = m_swapchain.getImages();
}

vk::SurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
    //assert(!availableFormats.empty());
    const auto formatIt = std::ranges::find_if(
        availableFormats,
        [](const auto& format) { return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear; });
    return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
}

vk::PresentModeKHR VulkanSwapChain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
{
    //assert(std::ranges::any_of(availablePresentModes, [](auto presentMode) { return presentMode == vk::PresentModeKHR::eFifo; }));
    return std::ranges::any_of(availablePresentModes,
        [](const vk::PresentModeKHR value) { return vk::PresentModeKHR::eMailbox == value; }) ? 
        vk::PresentModeKHR::eMailbox : vk::PresentModeKHR::eFifo;
}

vk::Extent2D VulkanSwapChain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    return {
        std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    };
}

uint32_t VulkanSwapChain::chooseSwapMinImageCount(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities)
{
    auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
    if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount))
    {
        minImageCount = surfaceCapabilities.maxImageCount;
    }
    return minImageCount;
}

void VulkanSwapChain::createImageView(const vk::raii::Device& device)
{
    vk::ImageViewCreateInfo imageViewCreateInfo{
        .viewType = vk::ImageViewType::e2D,
        .format = m_swapChainSurfaceFormat.format,
        .subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
    };

    for (auto& image : m_swapChainImages)
    {
        imageViewCreateInfo.image = image;
        m_swapChainImageViews.emplace_back(device, imageViewCreateInfo);
    }
}

const vk::raii::SwapchainKHR& VulkanSwapChain::getSwapChain() const
{
    return m_swapchain;
}

const std::vector<vk::Image>& VulkanSwapChain::getSwapChainImages() const
{
    return m_swapChainImages;
}

const std::vector<vk::raii::ImageView>& VulkanSwapChain::getSwapChainImageViews() const
{
    return m_swapChainImageViews;
}

const vk::SurfaceFormatKHR& VulkanSwapChain::getSwapChainSurfaceFormat() const
{
    return m_swapChainSurfaceFormat;
}

const vk::Extent2D& VulkanSwapChain::getSwapChainExtent() const
{
    return m_swapChainExtent;
}