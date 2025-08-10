#include "VulkanImage.h"
#include "VulkanRHI/VulkanObjects/VulkanBuffer.h"
#include "VulkanRHI/VulkanCommon.h"
#include <iostream>

static uint32_t alignedSize(uint32_t value, uint32_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}



#if VK_HEADER_VERSION >= 141
//workaround for removed defines in sdk 141
#define VK_DESCRIPTOR_TYPE_BEGIN_RANGE (VK_DESCRIPTOR_TYPE_SAMPLER)
#define VK_DESCRIPTOR_TYPE_END_RANGE (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
#define VK_DESCRIPTOR_TYPE_RANGE_SIZE (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT - VK_DESCRIPTOR_TYPE_SAMPLER + 1)
#define VK_IMAGE_VIEW_TYPE_RANGE_SIZE (VK_IMAGE_VIEW_TYPE_CUBE_ARRAY - VK_IMAGE_VIEW_TYPE_1D + 1)
#define VK_DYNAMIC_STATE_BEGIN_RANGE (VK_DYNAMIC_STATE_VIEWPORT)
#define VK_DYNAMIC_STATE_END_RANGE (VK_DYNAMIC_STATE_STENCIL_REFERENCE)
#define VK_DYNAMIC_STATE_RANGE_SIZE (VK_DYNAMIC_STATE_STENCIL_REFERENCE - VK_DYNAMIC_STATE_VIEWPORT + 1)
#define VK_FORMAT_RANGE_SIZE (VK_FORMAT_ASTC_12x12_SRGB_BLOCK - VK_FORMAT_UNDEFINED + 1)
#endif

static const VkImageTiling GVulkanViewTypeTilingMode[VK_IMAGE_VIEW_TYPE_RANGE_SIZE] =
{
    VK_IMAGE_TILING_OPTIMAL,		// VK_IMAGE_VIEW_TYPE_1D
    VK_IMAGE_TILING_OPTIMAL,	// VK_IMAGE_VIEW_TYPE_2D
    VK_IMAGE_TILING_OPTIMAL,	// VK_IMAGE_VIEW_TYPE_3D
    VK_IMAGE_TILING_OPTIMAL,	// VK_IMAGE_VIEW_TYPE_CUBE
    VK_IMAGE_TILING_LINEAR,		// VK_IMAGE_VIEW_TYPE_1D_ARRAY
    VK_IMAGE_TILING_OPTIMAL,	// VK_IMAGE_VIEW_TYPE_2D_ARRAY
    VK_IMAGE_TILING_OPTIMAL,	// VK_IMAGE_VIEW_TYPE_CUBE_ARRAY
};



//static constexpr inline bool isDepthTextureFormat(RHIPixelFormat format)
bool isDepthTextureFormat(RHIPixelFormat format)
{
    switch (format) {
    case RHIPixelFormat::PF_DepthStencil:
        return true;
    default:
        return false;
    }
}

//static constexpr inline VkImageAspectFlags aspectMaskForTextureFormat(RHIPixelFormat format)
VkImageAspectFlags aspectMaskForTextureFormat(RHIPixelFormat format)
{
    return isDepthTextureFormat(format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
}


VkImageType VulkanImage::getImageTypeFromViewType(VkImageViewType ResourceType)
{
    VkImageType imageType;
    switch (ResourceType)
    {
    case VK_IMAGE_VIEW_TYPE_1D:
        imageType = VK_IMAGE_TYPE_1D;
        //check(SizeX <= DeviceProperties.limits.maxImageDimension1D);
        break;
    case VK_IMAGE_VIEW_TYPE_CUBE:
    case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:
        //check(SizeX == SizeY);
        //check(SizeX <= DeviceProperties.limits.maxImageDimensionCube);
        //check(SizeY <= DeviceProperties.limits.maxImageDimensionCube);
        imageType = VK_IMAGE_TYPE_2D;
        break;
    case VK_IMAGE_VIEW_TYPE_2D:
    case VK_IMAGE_VIEW_TYPE_2D_ARRAY:
        //check(SizeX <= DeviceProperties.limits.maxImageDimension2D);
        //check(SizeY <= DeviceProperties.limits.maxImageDimension2D);
        imageType = VK_IMAGE_TYPE_2D;
        break;
    case VK_IMAGE_VIEW_TYPE_3D:
        //check(SizeY <= DeviceProperties.limits.maxImageDimension3D);
        imageType = VK_IMAGE_TYPE_3D;
        break;
    default:
        //checkf(false, TEXT("Unhandled image type %d"), (int32)ResourceType);
        break;
    }
    return imageType;
}

uint32_t VulkanImage::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    Device->PhysicalDevice->GetPhysicalDeviceMemoryProperties(&memProperties);
    //std::cout << "typeFilter " << typeFilter << " properties " << properties << std::endl;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
    {
        //std::cout << i << " (typeFilter & (1 << i)) " << (typeFilter & (1 << i)) << std::endl;
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            //std::cout << "findMemoryType " << i << std::endl;
            return i;
        }
    }
    throw std::runtime_error("Failed to find suitable memory type!");
}

VulkanImage::VulkanImage(VulkanDevice* InDevice, VkImageViewType ResourceType, uint8_t InFormat,
    uint32_t SizeX, uint32_t SizeY, uint32_t SizeZ, uint32_t ArraySize, uint32_t NumMips, TextureCreateFlags InFlags)
    : Device(InDevice)
{
    flags = InFlags;
    createImage(ResourceType, InFormat, SizeX, SizeY, SizeZ, ArraySize, NumMips);

}

VulkanImage::VulkanImage(VulkanDevice* InDevice, VkImageViewType ResourceType, VkImageUsageFlags usage, VkFormat InFormat,
    uint32_t SizeX, uint32_t SizeY, uint32_t numSamples)
    : Device(InDevice)
{
    createAttachmentImage(ResourceType, usage, InFormat, SizeX, SizeY, numSamples);
}

VulkanImage::~VulkanImage()
{
    Destroy();
}

VkImageCreateInfo VulkanImage::GetDefaultImageCreateInfo(void)
{
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.pNext = nullptr;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    return imageInfo;
}

VkImage VulkanImage::GetHandle() const
{
    return Handle;
}

uint32_t VulkanImage::GetMemorySize() const
{
    return MemoryRequirements.size;
}

VkImageViewType VulkanImage::GetViewType() const 
{ 
    return ViewType;
}

VkImageTiling VulkanImage::GetTiling() const 
{ 
    return Tiling;
}

uint32_t VulkanImage::GetNumMips() const
{ 
    return NumMips;
}

uint32_t VulkanImage::GetNumSamples() const 
{ 
    return NumSamples;
}

uint32_t VulkanImage::GetNumberOfArrayLevels() const
{
    switch (ViewType)
    {
    case VK_IMAGE_VIEW_TYPE_1D:
    case VK_IMAGE_VIEW_TYPE_2D:
    case VK_IMAGE_VIEW_TYPE_3D:
        return 1;
    case VK_IMAGE_VIEW_TYPE_2D_ARRAY:
        return m_arraysize;
    case VK_IMAGE_VIEW_TYPE_CUBE:
        return 6;
    case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:
        return 6 * m_arraysize;
    default:
        //ErrorInvalidViewType();
        return 1;
    }
}

VkFormat VulkanImage::GetFormat() const
{
    return m_format;
}

VkDeviceMemory VulkanImage::GetMemory() const
{
    return Memory;
}

void VulkanImage::UpdateImage(uint32_t baseArrayLayer, const void* InData)
{
    const std::uint32_t FormatBPP = GPixelFormats[PixelFormat].BlockBytes; //todo...
    VkDeviceSize imageSize = m_width * m_height * m_depth * FormatBPP;
    //printf("\nupdateImage imageSize: [%d,%d,%d,%d] %d,%d %d\n",m_width,m_height,m_depth,FormatBPP, m_NumMips, m_arraysize, baseArrayLayer);
    //VkDeviceSize totalSize = imageSize * m_arraysize; 

    VkCommandBuffer cmdBuffer = beginSingleTimeCommands();

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = aspectMaskForTextureFormat((RHIPixelFormat)PixelFormat);// VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = m_NumMips;
    subresourceRange.layerCount = m_arraysize;

    VkImageLayout oldImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageLayout newImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    //if ((uint64_t)flags & (uint64_t)TextureCreateFlags::Presentable) {
    //    newImageLayout = VK_IMAGE_LAYOUT_GENERAL;
    //    printf("TextureCreateFlags::Presentable\n");
    //}

    transitionImageLayout(cmdBuffer, Handle, m_format, oldImageLayout, newImageLayout, subresourceRange);

    VulkanBuffer* stagingBuffer = nullptr;
    // if (this->flags != TextureCreateFlags::Presentable) 
    {
        stagingBuffer = new VulkanBuffer(Device, 0, imageSize, BufferUsageFlags::TransferSrcBuffer, InData);
        copyBufferToImage(cmdBuffer, stagingBuffer->Handle, Handle, m_width, m_height, m_depth, baseArrayLayer);
        //generateMipmaps(m_Image, m_format, m_width, m_height, m_NumMips);
        oldImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        newImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        if (EnumHasAnyFlags(flags, TextureCreateFlags::Presentable | TextureCreateFlags::UAV)) {
            oldImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            newImageLayout = VK_IMAGE_LAYOUT_GENERAL;
        }
        transitionImageLayout(cmdBuffer, Handle, m_format, oldImageLayout, newImageLayout, subresourceRange);
    }
    endSingleTimeCommands(cmdBuffer);

    if (stagingBuffer)
        delete stagingBuffer;

    //printf("updateImage end\n");
}


void VulkanImage::Destroy()
{
    if (pixels) {
        delete[] pixels;
        pixels = nullptr;
    }

    //std::cout << "m_Image " << Handle << std::endl;

    if (Handle != VK_NULL_HANDLE)
        Device->DestroyImage(Handle, nullptr);

    if (Memory != VK_NULL_HANDLE)
        Device->FreeMemory(Memory, nullptr);
}

void VulkanImage::transitionImageLayout(VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags srcStage, dstStage;
    if (oldLayout == VK_IMAGE_LAYOUT_GENERAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT; // 假设之前是计算着色器写入
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        srcStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }

    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}


uint8_t* VulkanImage::GetData()
{
#if 0
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(Device->PhysicalDevice->Handle, &props);
    size_t alignment = props.limits.minMemoryMapAlignment;
    printf("alignment %d\n", alignment);
#else
    size_t alignment = 4; //按4字节对齐，其他会出错
#endif 
    size_t alignedRowSize = alignedSize(m_width * 4, alignment);
    size_t bufferSize = alignedRowSize * m_height;

    VkBuffer stagingBuffer;// 1. 创建对齐的暂存缓冲区 
    VkDeviceMemory stagingBufferMemory;
    VkBufferCreateInfo CreateInfo;
    std::memset(&CreateInfo, 0, sizeof(CreateInfo));
    CreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    CreateInfo.size = bufferSize;
    CreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    Device->CreateBuffer(&CreateInfo, nullptr, &stagingBuffer);
    Device->GetBufferMemoryRequirements(stagingBuffer, &MemoryRequirements);
    VkMemoryAllocateInfo memAllocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, nullptr, MemoryRequirements.size, Device->HostVisibleMemoryTypeIndexWithMaxHeapSize };
    VkResult res = Device->AllocateMemory(&memAllocInfo, nullptr, &stagingBufferMemory);
    if (res != VK_SUCCESS) {
        std::cout << "Failed to allocate memory! error code = " << res << std::endl;
        exit(-1);
    }

    Device->BindBufferMemory(stagingBuffer, stagingBufferMemory, 0);

    VkCommandBuffer cmd = beginSingleTimeCommands(); // 2. 复制图像到缓冲区（包含对齐配置）  
    transitionImageLayout(cmd, Handle, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    // 配置复制区域
    VkBufferImageCopy region = {};
    region.bufferRowLength = m_width;    // 关键：指定原始宽度
    region.bufferImageHeight = m_height;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.layerCount = 1;
    region.imageExtent = { m_width, m_height, 1 };
    vkCmdCopyImageToBuffer(cmd, Handle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, stagingBuffer, 1, &region);
    transitionImageLayout(cmd, Handle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
    endSingleTimeCommands(cmd);

    void* mappedData; // 3. 映射并处理数据
    Device->MapMemory(stagingBufferMemory, 0, VK_WHOLE_SIZE, 0, &mappedData);
    if (pixels) {
        delete[] pixels;
        pixels = nullptr;
    }
    pixels = new uint8_t[m_width * m_height * 4];
    for (uint32_t y = 0; y < m_height; y++) {
        uint8_t* src = (uint8_t*)mappedData + y * alignedRowSize;
        uint8_t* dst = pixels + y * m_width * 4;
        memcpy(dst, src, m_width * 4);
    }
    Device->UnmapMemory(stagingBufferMemory);
    //stbi_write_png(filename, m_width, m_height, 4, pixels.data(), m_width * 4); // 4. 保存为PNG 
    // 清理暂存资源
    Device->DestroyBuffer(stagingBuffer, nullptr);
    Device->FreeMemory(stagingBufferMemory, nullptr);
    return pixels;
}

void VulkanImage::createImage(VkImageViewType ResourceType, uint8_t InFormat,
    uint32_t SizeX, uint32_t SizeY, uint32_t SizeZ, uint32_t ArraySize, uint32_t NumMips)
{
    // printf("createImage begin\n");
    m_width = SizeX;
    m_height = SizeY;
    m_depth = SizeZ;
    m_arraysize = ArraySize;
    m_NumMips = NumMips;
    PixelFormat = InFormat;
    m_format = (VkFormat)GPixelFormats[PixelFormat].PlatformFormat;//VK_FORMAT_R8G8B8A8_UNORM; //todo...
    //std::cout << "createImage " << (int)PixelFormat << "    PlatformFormat  " << m_format << std::endl;

    const std::uint32_t FormatBPP = GPixelFormats[PixelFormat].BlockBytes;
    VkDeviceSize imageSize = SizeX * SizeY * SizeZ * FormatBPP;

    bool bForceLinearTexture = false;
    if (EnumHasAnyFlags(flags, TextureCreateFlags::CPUReadback)) {
        bForceLinearTexture = true;
    }
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = getImageTypeFromViewType(ResourceType);
    imageInfo.extent.width = SizeX;
    imageInfo.extent.height = SizeY;
    imageInfo.extent.depth = SizeZ;
    imageInfo.mipLevels = NumMips;
    imageInfo.arrayLayers = ArraySize;
    imageInfo.format = m_format;
    imageInfo.tiling = bForceLinearTexture ? VK_IMAGE_TILING_LINEAR : GVulkanViewTypeTilingMode[ResourceType];
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;// usage; todo... 

    if (EnumHasAnyFlags(flags, TextureCreateFlags::Presentable)) {
        imageInfo.usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    }
    else if (EnumHasAnyFlags(flags, TextureCreateFlags::RenderTargetable | TextureCreateFlags::DepthStencilTargetable)) {
        if (EnumHasAllFlags(flags, TextureCreateFlags::InputAttachmentRead)) {
            imageInfo.usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        }
        imageInfo.usage |= (EnumHasAnyFlags(flags, TextureCreateFlags::RenderTargetable) ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        //if (EnumHasAllFlags(UEFlags, TexCreate_Memoryless) && InDevice.GetDeviceMemoryManager().SupportsMemoryless()) {
        //    imageInfo.usage |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
        //    // Remove the transfer and sampled bits, as they are incompatible with the transient bit.
        //    imageInfo.usage &= ~(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
        //}
    }
    else if (EnumHasAnyFlags(flags, TextureCreateFlags::DepthStencilResolveTarget)) {
        imageInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    }
    else if (EnumHasAnyFlags(flags, TextureCreateFlags::ResolveTargetable)) {
        imageInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    }

    if (EnumHasAnyFlags(flags, TextureCreateFlags::UAV)) {
        //cannot have the storage bit on a memoryless texture
        //ensure(!EnumHasAnyFlags(UEFlags, TexCreate_Memoryless));
        imageInfo.usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    }

    imageInfo.flags = (ResourceType == VK_IMAGE_VIEW_TYPE_CUBE || ResourceType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY) ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;//toVkMultisampleCount numSamples; 
    Device->CreateImage(&imageInfo, nullptr, &Handle);
    // printf("CreateImage\n");

     //VkMemoryPropertyFlags memProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
     //if (imageInfo.tiling == VK_IMAGE_TILING_LINEAR) {
     //    memProps |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
     //}

     //VkMemoryRequirements memRequirementsImage;
    Device->GetImageMemoryRequirements(Handle, &MemoryRequirements);// Fetch image size 
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = MemoryRequirements.size;
    //allocInfo.memoryTypeIndex = findMemoryType(MemoryRequirements.memoryTypeBits, memProps);
    /*
        纹理用Device Local内存
    */
    allocInfo.memoryTypeIndex = Device->DeviceLocalMemoryTypeIndexWithMaxHeapSize;

    //std::cout << "###allocInfo.memoryTypeIndex  " << allocInfo.memoryTypeIndex << std::endl;
    VkResult res = Device->AllocateMemory(&allocInfo, nullptr, &Memory);
    if (res != VK_SUCCESS) {
        std::cout << "Failed to allocate memory! error code = " << res << std::endl;
        exit(-1);
    }
    Device->BindImageMemory(Handle, Memory, 0);
}

void VulkanImage::createAttachmentImage(VkImageViewType ResourceType, VkImageUsageFlags usage, VkFormat InFormat,
    uint32_t SizeX, uint32_t SizeY, uint32_t numSamples)
{
    printf("createAttachmentImage\n");
    m_width = SizeX;
    m_height = SizeY;
    m_depth = 1;
    m_arraysize = 1;
    m_NumMips = 1;

    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = getImageTypeFromViewType(ResourceType);
    imageInfo.extent.width = SizeX;
    imageInfo.extent.height = SizeY;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = InFormat;
    imageInfo.tiling = GVulkanViewTypeTilingMode[ResourceType]; //bForceLinearTexture ? VK_IMAGE_TILING_LINEAR : GVulkanViewTypeTilingMode[ResourceType];
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.flags = 0;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = toVkMultisampleCount(numSamples);
    VkResult ret;
    ret = Device->CreateImage(&imageInfo, nullptr, &Handle);
    if (VK_SUCCESS != ret)
    {
        std::cout << "vkCreateImage failed : ret " << ret << std::endl;
    }


    VkMemoryRequirements memRequirementsImage;
    Device->GetImageMemoryRequirements(Handle, &memRequirementsImage);
    VkMemoryAllocateInfo AllocateInfo = {};
    AllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    AllocateInfo.allocationSize = memRequirementsImage.size;
    AllocateInfo.memoryTypeIndex = findMemoryType(memRequirementsImage.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    ret = Device->AllocateMemory(&AllocateInfo, nullptr, &Memory);
    if (VK_SUCCESS != ret)
    {
        std::cout << "Failed to allocate memory! error code = " << ret << std::endl;
        exit(-1);
    }
    Device->BindImageMemory(Handle, Memory, 0);
}


VkCommandBuffer VulkanImage::beginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = Device->m_CommandPool;// commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    Device->AllocateCommandBuffers(&allocInfo, &commandBuffer);
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

void VulkanImage::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.signalSemaphoreCount = 0;
    vkQueueSubmit(Device->m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(Device->m_graphicsQueue);
    Device->FreeCommandBuffers(Device->m_CommandPool, 1, &commandBuffer);
}

void VulkanImage::transitionImageLayout(VkCommandBuffer cmdBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageSubresourceRange subresourceRange)
{
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange = subresourceRange;

    VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

    switch (oldLayout)
    {
    case VK_IMAGE_LAYOUT_UNDEFINED:
        barrier.srcAccessMask = 0;
        break;
    case VK_IMAGE_LAYOUT_PREINITIALIZED:
        barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        break;

    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;
    default:
        break;
    }

    switch (newLayout)
    {
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        break;

    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        barrier.dstAccessMask = barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        if (barrier.srcAccessMask == 0) {
            barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        }
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;
    default:
        break;
    }
    vkCmdPipelineBarrier(cmdBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void VulkanImage::copyBufferToImage(VkCommandBuffer cmdBuffer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t depth/* = 1*/, uint32_t baseArrayLayer/* = 0*/)
{
    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = baseArrayLayer;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, depth };
    vkCmdCopyBufferToImage(cmdBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void VulkanImage::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
    VkFormatProperties formatProperties;
    Device->PhysicalDevice->GetPhysicalDeviceFormatProperties(imageFormat, &formatProperties);
    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {// Check if image format supports linear blitting.
        throw std::runtime_error("Texture image format does not support linear blitting!");
    }

    VkCommandBuffer commandBuffer = beginSingleTimeCommands();
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;
    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;
    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        VkImageBlit blit = {};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1,  mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        if (mipWidth > 1) {
            mipWidth /= 2;
        }

        if (mipHeight > 1) {
            mipHeight /= 2;
        }
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    endSingleTimeCommands(commandBuffer);
}
