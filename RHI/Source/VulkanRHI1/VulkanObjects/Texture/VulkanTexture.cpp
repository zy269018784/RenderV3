#include <VulkanRHI1/VulkanObjects/Texture/VulkanTexture.h>
#include <VulkanRHI1/VulkanObjects/Texture/VulkanImage.h>
#include <VulkanRHI1/VulkanObjects/VulkanBuffer.h>
#include <RHITexture.h>
#include <RHIPixelFormat.h>
#include <stdexcept>
#include <iostream>
#include <cstring>

VkImageView VulkanTextureView::staticCreate(VulkanDevice& Device, VkImage InImage, VkImageViewType ViewType, VkImageAspectFlags AspectFlags,
    VkFormat Format, uint32_t FirstMip, uint32_t NumMips, uint32_t ArraySliceIndex, uint32_t NumArraySlices,
    bool bUseIdentitySwizzle)
{
    VkImageView OutView = VK_NULL_HANDLE;

    VkImageViewCreateInfo ViewInfo;
#if 0
    ZeroVulkanStruct(ViewInfo, VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO);
#else
    ViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    memset(((uint8_t*)&ViewInfo) + sizeof(VkStructureType), 0, sizeof(VkImageViewCreateInfo) - sizeof(VkStructureType));
#endif
    ViewInfo.image = InImage;
    ViewInfo.viewType = ViewType;
    ViewInfo.format = Format;

    //ViewInfo.components = Device.GetFormatComponentMapping(UEFormat);

    ViewInfo.subresourceRange.aspectMask = AspectFlags;
    ViewInfo.subresourceRange.baseMipLevel = FirstMip;
    //ensure(NumMips != 0xFFFFFFFF);
    ViewInfo.subresourceRange.levelCount = NumMips;

#if 0 //Device还没接口
    auto CheckUseNvidiaWorkaround = [&Device]() -> bool
        {
            if (Device.GetVendorId() == EGpuVendorId::Nvidia)
            {
                // Workaround for 20xx family not copying last mips correctly, so instead the view is created without the last 1x1 and 2x2 mips
                if (GRHIAdapterName.Contains(TEXT("RTX 20")))
                {
                    UNvidiaDriverVersion NvidiaVersion;
                    const VkPhysicalDeviceProperties& Props = Device.GetDeviceProperties();
                    static_assert(sizeof(NvidiaVersion) == sizeof(Props.driverVersion), "Mismatched Nvidia pack driver version!");
                    NvidiaVersion.Packed = Props.driverVersion;
                    if (NvidiaVersion.Major < 430)
                    {
                        return true;
                    }
                }
            }
            return false;
        };
#endif
    static bool bNvidiaWorkaround = false;//CheckUseNvidiaWorkaround();
    if (bNvidiaWorkaround && Format >= VK_FORMAT_BC1_RGB_UNORM_BLOCK && Format <= VK_FORMAT_BC7_SRGB_BLOCK && NumMips > 1) {
        ViewInfo.subresourceRange.levelCount = std::max(1, int32_t(NumMips) - 2);
    }

    //ensure(ArraySliceIndex != 0xFFFFFFFF);
    ViewInfo.subresourceRange.baseArrayLayer = ArraySliceIndex;
    //ensure(NumArraySlices != 0xFFFFFFFF);

    switch (ViewType)
    {
    case VK_IMAGE_VIEW_TYPE_3D:
        ViewInfo.subresourceRange.layerCount = 1;
        break;
    case VK_IMAGE_VIEW_TYPE_CUBE:
        //ensure(NumArraySlices == 1);
        ViewInfo.subresourceRange.layerCount = 6;
        break;
    case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:
        ViewInfo.subresourceRange.layerCount = 6 * NumArraySlices;
        break;
    case VK_IMAGE_VIEW_TYPE_1D_ARRAY:
    case VK_IMAGE_VIEW_TYPE_2D_ARRAY:
        ViewInfo.subresourceRange.layerCount = NumArraySlices;
        break;
    default:
        ViewInfo.subresourceRange.layerCount = 1;
        break;
    }

#if 0
    //HACK.  DX11 on PC currently uses a D24S8 depthbuffer and so needs an X24_G8 SRV to visualize stencil.
    //So take that as our cue to visualize stencil.  In the future, the platform independent code will have a real format
    //instead of PF_DepthStencil, so the cross-platform code could figure out the proper format to pass in for this.
    if (UEFormat == PF_X24_G8)
    {
        ensure((ViewInfo.format == (VkFormat)GPixelFormats[PF_DepthStencil].PlatformFormat) && (ViewInfo.format != VK_FORMAT_UNDEFINED));
        ViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
    } 
    INC_DWORD_STAT(STAT_VulkanNumImageViews);
#endif
    VkResult err = Device.CreateImageView(&ViewInfo, VULKAN_CPU_ALLOCATOR, &OutView);
    if (err != VK_SUCCESS) {
        printf("Failed to create image view for texture: %d", err);
    } 
    return OutView;
}


VulkanTexture::VulkanTexture(VulkanDevice* InDevice, VkImageViewType ResourceType, std::uint8_t InFormat,
    uint32_t SizeX, uint32_t SizeY, uint32_t SizeZ,
    uint32_t ArraySize, uint32_t InNumMips, TextureCreateFlags InFlags)
    : Device(InDevice)
{ 
    Image = new VulkanImage(Device, ResourceType, InFormat, SizeX, SizeY, SizeZ, ArraySize, InNumMips, InFlags);
    TextureView = new VulkanTextureView(Device);

    VkImageAspectFlagBits aspect = VK_IMAGE_ASPECT_COLOR_BIT;
    if (InFormat == RHIPixelFormat::PF_DepthStencil) {
        aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    TextureView->create(*Device, Image->Handle, ResourceType, InFormat, aspect, InNumMips, ArraySize);
}

VulkanTexture::~VulkanTexture() 
{
    delete Image;
    delete TextureView;
}

VkImageView VulkanTexture::GetImageView() const 
{
    return TextureView->GetHandle();
}

VkImage VulkanTexture::GetImage() const
{
    return Image->GetHandle();
}

VkFormat VulkanTexture::GetImageFormat() const
{
    return Image->m_format;
}

void VulkanTexture::UpdateImage(uint32_t baseArrayLayer, const void* InData)
{
    Image->UpdateImage(baseArrayLayer, InData);
}

VulkanTexture1D::VulkanTexture1D(VulkanDevice* pVulkanDevice, std::uint32_t InSizeX, std::uint32_t InNumMips, uint8_t InFormat, TextureCreateFlags InFlags)
    : RHITexture1D(InSizeX, InNumMips, InFormat, InFlags)
    , VulkanTexture(pVulkanDevice, VK_IMAGE_VIEW_TYPE_1D, InFormat, InSizeX, 1, 1, 1, InNumMips, InFlags)
{

}


VulkanTexture2D::VulkanTexture2D(VulkanDevice* pVulkanDevice, std::uint32_t InSizeX, std::uint32_t InSizeY, std::uint32_t InNumMips,
    std::uint8_t InFormat, TextureCreateFlags InFlags)
    : RHITexture2D(InSizeX, InSizeY, InNumMips, InFormat, InFlags)
    , VulkanTexture(pVulkanDevice,VK_IMAGE_VIEW_TYPE_2D, InFormat, InSizeX, InSizeY, 1, 1, InNumMips, InFlags)
{ 

}

VulkanTexture2D::~VulkanTexture2D()
{
  
}


VulkanTexture2DArray::VulkanTexture2DArray(VulkanDevice* pVulkanDevice,
    uint32_t InSizeX, uint32_t InSizeY, uint32_t ArraySize, uint32_t InNumMips,
    uint8_t InFormat, TextureCreateFlags InFlags)
    : RHITexture2DArray(InSizeX, InSizeY, ArraySize, InNumMips, InFormat, InFlags)
    , VulkanTexture(pVulkanDevice, VK_IMAGE_VIEW_TYPE_2D_ARRAY, InFormat, InSizeX, InSizeY, 1, ArraySize, InNumMips, InFlags)
{

}

VulkanTexture3D::VulkanTexture3D(VulkanDevice* pVulkanDevice,
    uint32_t InSizeX, uint32_t InSizeY, uint32_t InSizeZ, uint32_t InNumMips,
    uint8_t InFormat, TextureCreateFlags InFlags)
    : RHITexture3D(InSizeX, InSizeY, InSizeZ, InNumMips, InFormat, InFlags)
    , VulkanTexture(pVulkanDevice, VK_IMAGE_VIEW_TYPE_3D, InFormat, InSizeX, InSizeY, InSizeZ, 1, InNumMips, InFlags)
{

}


VulkanTextureCube::VulkanTextureCube(VulkanDevice* pVulkanDevice,
    uint32_t InSizeX, uint32_t InSizeY, uint32_t ArraySize,
    uint32_t InNumMips,
    uint8_t InFormat, TextureCreateFlags InFlags)
    : RHITextureCube(InSizeX, InNumMips, InFormat, InFlags)
    , VulkanTexture(pVulkanDevice, VK_IMAGE_VIEW_TYPE_CUBE, InFormat, InSizeX, InSizeX, 1, ArraySize, InNumMips, InFlags)
{

}

VulkanTextureCubeArray::VulkanTextureCubeArray(VulkanDevice* pVulkanDevice,
    uint32_t InSizeX, uint32_t InSizeY, uint32_t ArraySize,
    uint32_t InNumMips,
    uint8_t InFormat, TextureCreateFlags InFlags)
    : RHITextureCubeArray(InSizeX, ArraySize, InNumMips, InFormat, InFlags)
    , VulkanTexture(pVulkanDevice, VK_IMAGE_VIEW_TYPE_CUBE_ARRAY, InFormat, InSizeX, InSizeX, InSizeX, ArraySize, InNumMips, InFlags)
{

}