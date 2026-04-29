#pragma once
#include <VulkanRHI1/VulkanObjects/VulkanDevice.h>

class VulkanTextureView
{
public:
    VulkanTextureView(VulkanDevice* InDevice) : Device(InDevice), Handle(VK_NULL_HANDLE), Image(VK_NULL_HANDLE), ViewId(0)
    {
    }
    ~VulkanTextureView();
    VkImageView GetHandle() const;

    void create(VulkanDevice& Device, VkImage image, VkImageViewType ResourceType, std::uint8_t InFormat, VkImageAspectFlags aspectFlags, uint32_t mipLevels, uint32_t arraySize);
    void create(VulkanDevice& Device, VkImage InImage, VkImageViewType ViewType, VkImageAspectFlags AspectFlags, VkFormat Format,
        uint32_t FirstMip, uint32_t NumMips, uint32_t ArraySliceIndex, uint32_t NumArraySlices);
private:
    VulkanDevice* Device;
    VkImageView Handle;   
    VkImage Image;
    uint32_t ViewId;
private:
    static VkImageView staticCreate(VulkanDevice& Device, VkImage InImage, VkImageViewType ViewType, VkImageAspectFlags AspectFlags,
        VkFormat Format, uint32_t FirstMip, uint32_t NumMips, uint32_t ArraySliceIndex, uint32_t NumArraySlices,
        bool bUseIdentitySwizzle);

    //static VkImageView StaticCreate(VulkanDevice& Device, VkImage InImage, VkImageViewType ViewType, VkImageAspectFlags AspectFlags, 
    //    EPixelFormat UEFormat, VkFormat Format, uint32 FirstMip, uint32 NumMips, uint32 ArraySliceIndex, uint32 NumArraySlices, bool bUseIdentitySwizzle, const FSamplerYcbcrConversionInitializer* ConversionInitializer);
};
