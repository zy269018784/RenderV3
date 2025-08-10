#pragma once
#include <RHITexture.h>
#include <VulkanRHI/VulkanObjects/VulkanDevice.h>

class VulkanImage {
public:
    VulkanImage(VulkanDevice* InDevice, VkImageViewType ResourceType, uint8_t InFormat,
        uint32_t SizeX, uint32_t SizeY, uint32_t SizeZ, uint32_t ArraySize, uint32_t NumMips, TextureCreateFlags InFlags = TextureCreateFlags::None);
    VulkanImage(VulkanDevice* InDevice, VkImageViewType ResourceType, VkImageUsageFlags usage, VkFormat InFormat,
        uint32_t SizeX, uint32_t SizeY, uint32_t numSamples);

    virtual ~VulkanImage();

    static VkImageCreateInfo GetDefaultImageCreateInfo(void);

    VkImage GetHandle() const;
    uint32_t GetMemorySize() const;
    VkImageViewType GetViewType() const;
    VkImageTiling GetTiling() const;
    uint32_t GetNumMips() const;
    uint32_t GetNumSamples() const;
    uint32_t GetNumberOfArrayLevels() const;
    VkFormat GetFormat() const;
    VkDeviceMemory GetMemory() const;
   
    void UpdateImage(uint32_t baseArrayLayer, const void* InData);

    uint8_t* GetData();
    void Destroy();
    void transitionImageLayout(VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
    VkImageType getImageTypeFromViewType(VkImageViewType ResourceType);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void transitionImageLayout(VkCommandBuffer cmdBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageSubresourceRange subresourceRange);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    void copyBufferToImage(VkCommandBuffer cmdBuffer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t depth = 1, uint32_t baseArrayLayer = 0);
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

    void createImage(VkImageViewType ResourceType, uint8_t InFormat,
        uint32_t SizeX, uint32_t SizeY, uint32_t SizeZ, uint32_t ArraySize, uint32_t NumMips);

    void createAttachmentImage(VkImageViewType ResourceType, VkImageUsageFlags usage, VkFormat InFormat,
        uint32_t SizeX, uint32_t SizeY, uint32_t numSamples = 1);

private:
    VulkanDevice* Device = nullptr; 
    VkImage Handle = VK_NULL_HANDLE;
    VkDeviceMemory Memory;

    uint8_t* pixels = nullptr;
    VkFormat StorageFormat;// Removes SRGB if requested, used to upload data

    VkFormat m_format;// Format for SRVs, render targets

    uint32_t m_width, m_height, m_depth, m_arraysize;// Width, Height, Depth, ArraySize;

    uint32_t m_NumMips = 1;
    uint32_t m_NumSamples;
    uint8_t PixelFormat;
    TextureCreateFlags flags = TextureCreateFlags::None;

    VkMemoryPropertyFlags MemProps;
    VkMemoryRequirements MemoryRequirements;
    VkImageTiling Tiling;
    VkImageViewType	ViewType;
    uint32_t NumMips;
    uint32_t NumSamples;

    VkImageAspectFlags FullAspectMask;
    VkImageAspectFlags PartialAspectMask;

    friend struct VulkanTexture;
};
