#pragma once
#include <VulkanRHI/Vulkan.h>
#include <VulkanRHI/VulkanObjects/VulkanCommon.h>
#include <VulkanRHI/VulkanObjects/VulkanDevice.h>
#include <VulkanRHI/VulkanObjects/VulkanPhysicalDevice.h>
#include <VulkanRHI/VulkanObjects/VulkanSurface.h>
#include <RHICommandBuffer.h>
#include <vector>
 


class VulkanSwapChain {
public:
	VulkanSwapChain(VulkanDevice *InDevice, VulkanPhysicalDevice& physicalDevice, VulkanSurface& surface);

    void CreateImageViews();
      
    void CopyImage(RHICommandBuffer* cmdBuffer, RHITexture* tex, uint32_t imageIndex);

    uint8_t* BlitImage(uint32_t imageIndex);

    void ReleaseBlitImageData(uint8_t* data);

protected:
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    
public:
    VulkanDevice *Device;
    VkSwapchainKHR Handle; 
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
};
