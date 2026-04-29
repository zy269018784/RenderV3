#pragma once
#include <RHITexture.h>
#include <VulkanRHI1/Vulkan.h>
#include <VulkanRHI1/VulkanObjects/VulkanDevice.h> 
#include <VulkanRHI1/VulkanObjects/Texture/VulkanImage.h>
#include <VulkanRHI1/VulkanObjects/Texture/VulkanTextureView.h>
#include <RHIPixelFormat.h>
#include <unordered_map> 
#include <exception>

#define VULKAN_CPU_ALLOCATOR nullptr

//texture base
class VulkanTexture
{
public:
    VulkanTexture(VulkanDevice* InDevice, VkImageViewType ResourceType, std::uint8_t InFormat, uint32_t SizeX, uint32_t SizeY, uint32_t SizeZ,
        uint32_t ArraySize, uint32_t InNumMips, TextureCreateFlags InFlags);
    ~VulkanTexture();
    VkImageView GetImageView() const;
    VkImage GetImage() const;
    VkFormat GetImageFormat() const;
    void UpdateImage(uint32_t baseArrayLayer, const void* InData);
protected:  
    VulkanDevice* Device = nullptr;
    VulkanImage *Image = nullptr;
    VulkanTextureView *TextureView = nullptr;
};
class VulkanTexture1D : public RHITexture1D, public VulkanTexture
{
public:
    VulkanTexture1D(VulkanDevice* pVulkanDevice, std::uint32_t InSizeX, std::uint32_t InNumMips, uint8_t InFormat, TextureCreateFlags InFlags); 
    ~VulkanTexture1D() {
        //printf("~VulkanTexture1D\n"); 
    }

    VkFormat getVkFormat() const {
        return Image->GetFormat();
    }

    VkSampleCountFlagBits getSamples() const {
        return VK_SAMPLE_COUNT_1_BIT; //todo...
    }

    uint8_t* getData() {
        return Image->GetData();
    }
};

class VulkanTexture2D : public RHITexture2D, public VulkanTexture
{
public:
    VulkanTexture2D( VulkanDevice* pVulkanDevice, std::uint32_t InSizeX, std::uint32_t InSizeY, std::uint32_t InNumMips,
        std::uint8_t InFormat, TextureCreateFlags InFlags);

    ~VulkanTexture2D();

    VkFormat getVkFormat() const {
        return Image->GetFormat();
    }

    VkSampleCountFlagBits getSamples() const {
        return VK_SAMPLE_COUNT_1_BIT; //todo...
    }

    uint8_t* getData() {
        return Image->GetData();
    }
};

class VulkanTexture2DArray : public RHITexture2DArray, public VulkanTexture
{
public:
    VulkanTexture2DArray(VulkanDevice* pVulkanDevice, 
        uint32_t InSizeX, uint32_t InSizeY, uint32_t ArraySize, 
        uint32_t InNumMips,
        uint8_t InFormat, TextureCreateFlags InFlags);

    ~VulkanTexture2DArray() {

    }
};

class VulkanTexture3D : public VulkanTexture, public RHITexture3D
{
public:
    VulkanTexture3D(VulkanDevice* pVulkanDevice,
        uint32_t InSizeX, uint32_t InSizeY, uint32_t InSizeZ,
        uint32_t InNumMips,
        uint8_t InFormat, TextureCreateFlags InFlags);

    ~VulkanTexture3D() {

    }
};


class VulkanTextureCube : public RHITextureCube, public VulkanTexture 
{
public: 
    VulkanTextureCube(VulkanDevice* pVulkanDevice,
        uint32_t InSizeX, uint32_t InSizeY, uint32_t ArraySize,
        uint32_t InNumMips,
        uint8_t InFormat, TextureCreateFlags InFlags);

    virtual ~VulkanTextureCube()
    {

    }

 
};

class VulkanTextureCubeArray : public RHITextureCubeArray, public VulkanTexture
{
public:
    VulkanTextureCubeArray(VulkanDevice* pVulkanDevice,
        uint32_t InSizeX, uint32_t InSizeY, uint32_t ArraySize,
        uint32_t InNumMips,
        uint8_t InFormat, TextureCreateFlags InFlags);

    virtual ~VulkanTextureCubeArray()
    {

    }
};
 