#include"VulkanSwapChain.hpp"
#include"Helper/ImageHelper/ImageHelper.hpp"

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

void VulkanSwapChain::createImageViews(const vk::raii::Device& device)
{
    m_swapChainImageViews.reserve(m_swapChainImages.size());
    for (auto& image : m_swapChainImages)
    {
        m_swapChainImageViews.emplace_back(ImageHelper::createImageView(device, image, m_swapChainSurfaceFormat.format, vk::ImageAspectFlagBits::eColor));
    }
}

void VulkanSwapChain::recreateSwapChain(const vk::raii::PhysicalDevice& physicaldevice, const vk::raii::Device& device, const vk::raii::SurfaceKHR& surface, GLFWwindow* window)
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }


    device.waitIdle();

    cleanupSwapChain();
    createSwapChain(physicaldevice, device, surface, window);
    createImageViews(device);
}

void VulkanSwapChain::cleanupSwapChain()
{
    m_swapChainImageViews.clear();
    m_swapchain = nullptr;
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