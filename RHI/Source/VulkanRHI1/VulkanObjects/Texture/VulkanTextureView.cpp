#include "VulkanTextureView.h"

VulkanTextureView::~VulkanTextureView()
{
	Device->DestroyImageView(Handle, nullptr);
}

VkImageView VulkanTextureView::GetHandle() const
{
    return Handle;
}

//void VulkanTextureView::Destroy(VulkanDevice* Device)
//{
//    if (m_ImageView != VK_NULL_HANDLE)
//        Device->DestroyImageView(m_ImageView, nullptr);
//}

void VulkanTextureView::create(VulkanDevice& Device, VkImage image, VkImageViewType ResourceType, std::uint8_t InFormat, VkImageAspectFlags aspectFlags, uint32_t mipLevels, uint32_t arraySize)
{
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = ResourceType;
    viewInfo.format = (VkFormat)GPixelFormats[InFormat].PlatformFormat;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = arraySize;// (ResourceType == VK_IMAGE_VIEW_TYPE_CUBE) ? VK_REMAINING_ARRAY_LAYERS : 1;
    //std::cout << "VulkanTextureView::create " << "InFormat " << (int)InFormat << "  viewInfo.format " << viewInfo.format << std::endl;

    //if (RHIPixelFormat::PF_R8G8B8A8_UBYTE == InFormat)
    //{
    //    //std::cout << "create storage image " << std::endl;
    //    viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    //    viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    //    viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    //    viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    //}
    //else if (RHIPixelFormat::PF_B8G8R8A8_UBYTE == InFormat)
    //{
    //    viewInfo.components.r = VK_COMPONENT_SWIZZLE_B;
    //    viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    //    viewInfo.components.b = VK_COMPONENT_SWIZZLE_R;
    //    viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    //}
    if (viewInfo.format == VK_FORMAT_R8G8_UNORM)
    {
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_R;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_R;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_G;
    }
    if (Device.CreateImageView(&viewInfo, nullptr, &Handle) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create texture image view!");
    }
}

void VulkanTextureView::create(VulkanDevice& Device, VkImage InImage, VkImageViewType ViewType, VkImageAspectFlags AspectFlags, VkFormat Format,
    uint32_t FirstMip, uint32_t NumMips, uint32_t ArraySliceIndex, uint32_t NumArraySlices)
{
    Handle = staticCreate(Device, InImage, ViewType, AspectFlags, Format, FirstMip, NumMips, ArraySliceIndex, NumArraySlices, false);
    Image = InImage;
#if 0
    if (UseVulkanDescriptorCache()) {
        ViewId = ++GVulkanImageViewHandleIdCounter;
    }
#endif
}
