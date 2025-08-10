#include <VulkanRHI/VulkanObjects/VulkanSwapChain.h>
#include <VulkanRHI/VulkanRHI.h>
#include <VulkanRHI/VulkanObjects/VulkanInstance.h>
#include <stdexcept>
#include <iostream>
 
VulkanSwapChain::VulkanSwapChain(VulkanDevice* InDevice, VulkanPhysicalDevice& physicalDevice,VulkanSurface& surface)
    : Device(InDevice)
{ 
    vk::SwapChainSupportDetails swapChainSupport = vk::querySwapChainSupport(physicalDevice.GetHandle(), surface.Handle);
    for (int i = 0; i < swapChainSupport.formats.size(); i++)
    {
        std::cout  
            << "format     " << swapChainSupport.formats[i].format       << " "
            << "colorSpace " << swapChainSupport.formats[i].colorSpace   << " "
            << std::endl;
    }

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    std::cout << "surfaceFormat " << surfaceFormat.format << std::endl;
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.surface = surface.Handle; 
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    //todo...
    // Enable transfer source on swap chain images if supported
    //if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
    {
        createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }

    // Enable transfer destination on swap chain images if supported
    //if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
    {
        createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    vk::QueueFamilyIndices indices = vk::findQueueFamilies(physicalDevice.GetHandle(), surface.Handle);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE; 
    createInfo.oldSwapchain = VK_NULL_HANDLE;

  
    if (Device->CreateSwapchainKHR(&createInfo, nullptr, &Handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    Device->GetSwapchainImagesKHR(Handle, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    Device->GetSwapchainImagesKHR(Handle, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;

    CreateImageViews();
}

void VulkanSwapChain::CreateImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (Device->CreateImageView(&createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
{
    for (const auto& availableFormat : availableFormats) {
       //if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
       //    return availableFormat;
       //}

        if (availableFormat.format == VK_FORMAT_R8G8B8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

static uint32_t clamp(uint32_t v, uint32_t minValue, uint32_t maxValue) {
    return std::max(std::min(v, maxValue), minValue);
}

VkExtent2D VulkanSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } 
    else {
        int width{ 0 }, height{ 0 };
        //glfwGetFramebufferSize(window, &width, &height); 
        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        }; 
        actualExtent.width = clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height); 
        return actualExtent;
    }
}

void VulkanSwapChain::CopyImage(RHICommandBuffer* cmdBuffer, RHITexture* tex,uint32_t imageIndex)
{
    VulkanCommandBuffer* pCmdBuffer = dynamic_cast<VulkanCommandBuffer*>(cmdBuffer);
    VulkanTexture2D* pTex = dynamic_cast<VulkanTexture2D*>(tex);

    pCmdBuffer->SetImageBarrier(swapChainImages[imageIndex], EImageLayout::Undefined, EImageLayout::TransferDstOptimal); 
    // Prepare ray tracing output image as transfer source 
    pCmdBuffer->SetImageBarrier(tex, RHIPipelineStage::ALL_COMMANDS_BIT, RHIPipelineStage::ALL_COMMANDS_BIT, EImageLayout::General, EImageLayout::TransferSrcOptimal);

    VkImageCopy copyRegion{};
    copyRegion.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
    copyRegion.srcOffset = { 0, 0, 0 };
    copyRegion.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
    copyRegion.dstOffset = { 0, 0, 0 };
    copyRegion.extent = { swapChainExtent.width, swapChainExtent.height, 1 };
    pCmdBuffer->CmdCopyImage(pTex->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, swapChainImages[imageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

    // Transition swap chain image back for presentation
    pCmdBuffer->SetImageBarrier(swapChainImages[imageIndex], EImageLayout::TransferDstOptimal, EImageLayout::PresentSrc);
    // Transition ray tracing output image back to general layout
    pCmdBuffer->SetImageBarrier(tex, RHIPipelineStage::ALL_COMMANDS_BIT, RHIPipelineStage::ALL_COMMANDS_BIT, EImageLayout::TransferSrcOptimal, EImageLayout::General);
}

void SetImageBarrier(VkCommandBuffer cmdbuffer, VkImage image, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask,
    VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
    VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
    VkImageSubresourceRange subresourceRange)
{
    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.srcAccessMask = srcAccessMask;
    imageMemoryBarrier.dstAccessMask = dstAccessMask;
    imageMemoryBarrier.oldLayout = oldImageLayout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange = subresourceRange;

    vkCmdPipelineBarrier(cmdbuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

uint8_t* VulkanSwapChain::BlitImage(uint32_t imageIndex)
{
    printf("02\n");
    //VK_FORMAT_R8G8B8A8_UNORM
    VulkanImage image(Device, VK_IMAGE_VIEW_TYPE_2D, PF_R8G8B8A8_SRGB, swapChainExtent.width, swapChainExtent.height, 1, 1, 1, TextureCreateFlags::CPUReadback);
    VkCommandBuffer cmdBuffer = image.beginSingleTimeCommands();// Device->CreateCommandBufferOneTime();
    printf("0\n");
    SetImageBarrier(cmdBuffer, image.GetHandle(), 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
    SetImageBarrier(cmdBuffer, swapChainImages[imageIndex], VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

    //pCmdBuffer->SetImageBarrier(image.Handle, EImageLayout::Undefined, EImageLayout::TransferDstOptimal); // Transition destination image to transfer destination layout
    //pCmdBuffer->SetImageBarrier(swapChainImages[imageIndex], EImageLayout::PresentSrc, EImageLayout::TransferSrcOptimal);// Transition swapchain image from present to transfer source layout

    if (1) {
        VkOffset3D blitSize = { swapChainExtent.width, swapChainExtent.height, 1 };    // Define the region to blit (we will blit the whole swapchain image) 
        VkImageBlit blitRegion{};
        blitRegion.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
        blitRegion.srcOffsets[1] = blitSize;
        blitRegion.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
        blitRegion.dstOffsets[1] = blitSize;
        // Issue the blit command
        vkCmdBlitImage(cmdBuffer, swapChainImages[imageIndex], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image.GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitRegion, VK_FILTER_NEAREST);

        //pCmdBuffer->CmdBlitImage(swapChainImages[imageIndex], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image.Handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitRegion, VK_FILTER_NEAREST);
    }

    printf("1\n");
    SetImageBarrier(cmdBuffer, image.GetHandle(), VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
    SetImageBarrier(cmdBuffer, swapChainImages[imageIndex], VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

    // pCmdBuffer->SetImageBarrier(image.Handle, EImageLayout::TransferDstOptimal, EImageLayout::General); // Transition destination image to general layout, which is the required layout for mapping the image memory later on
    // pCmdBuffer->SetImageBarrier(swapChainImages[imageIndex], EImageLayout::TransferSrcOptimal, EImageLayout::PresentSrc);// Transition back the swap chain image after the blit is done

    image.endSingleTimeCommands(cmdBuffer);
    //Device->FlushCommandBufferOneTime(cmdBuffer);


    // Get layout of the image (including row pitch)
    VkImageSubresource subResource{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 0 };
    VkSubresourceLayout subResourceLayout;
    Device->GetImageSubresourceLayout(image.GetHandle(), &subResource, &subResourceLayout);

    uint8_t* data;
    Device->MapMemory(image.GetMemory(), 0, image.GetMemorySize(), 0, (void**)&data);
    uint8_t* pixels = new uint8_t[image.GetMemorySize()];
    uint8_t* row = (uint8_t*)data + subResourceLayout.offset;
    for (uint32_t y = 0; y < swapChainExtent.height; y++) {
        memcpy(pixels + y * ((swapChainExtent.width * 4 + 3) & ~3), row, swapChainExtent.width * 4);
        row += subResourceLayout.rowPitch;
    }
    return pixels;
}

void VulkanSwapChain::ReleaseBlitImageData(uint8_t* data)
{
    if (data) {
        delete[] data;
        data = nullptr;
    }
}