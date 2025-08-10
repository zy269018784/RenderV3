#include <VulkanRHI/VulkanObjects/VulkanFrameBuffer.h>
#include <stdexcept>

VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* InDevice)
{
    Device = InDevice;
}


VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice& Device, VulkanRenderPass& RenderPass, VulkanSwapChain* SwapChain, int32_t nImageViewIndex)
{
    Extent_ = SwapChain->swapChainExtent;
 

    //pColorImage = new VulkanImage(Device, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
    //    SwapChain->swapChainImageFormat, Extent_.width, Extent_.height, 1);
    //colorImageView.create(Device, pColorImage->m_Image, VK_IMAGE_VIEW_TYPE_2D, SwapChain->swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1);
 
 

    //pDepthImage = new VulkanImage(Device, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    //    VK_FORMAT_D32_SFLOAT, Extent_.width, Extent_.height, 1);
    //depthImageView.create(Device, pColorImage->m_Image, VK_IMAGE_VIEW_TYPE_2D, SwapChain->swapChainImageFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, 1);


    std::vector<VkImageView> attachments = { 
        //colorImageView.m_ImageView,
        //depthImageView.m_ImageView,
        SwapChain->swapChainImageViews[nImageViewIndex]
    };


    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = RenderPass.Handle;
    framebufferInfo.attachmentCount = attachments.size();
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = Extent_.width;
    framebufferInfo.height = Extent_.height;
    framebufferInfo.layers = 1; 
    if (Device.CreateFramebuffer(&framebufferInfo, nullptr, &Handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to create framebuffer!");
    } 
}

VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* InDevice, VulkanRenderPass* InRenderPass, VulkanRenderTarget* rt)
{
    Device = InDevice;
    RenderPass = InRenderPass;
    Extent_ = rt->getExtent();
    printf("VulkanFrameBuffer %d\n", Extent_.width);
    attachments = rt->attachments; 
}

void VulkanFrameBuffer::attachColorTarget(const VulkanTexture2D* pColorTexture, uint32_t rtIndex, uint32_t mipLevel/* = 0*/)
{
    //if (rtIndex >= mColorAttachments.size())
    //{
    //    logError("Error when attaching texture to FBO. Requested color index " + std::to_string(rtIndex) + ", but context only supports " + std::to_string(mColorAttachments.size()) + " targets");
    //    return;
    //}

    //attachments.push_back( { 
    //        pColorTexture->getImageView(),
    //        pColorTexture->getVkFormat()
    //    }
    //);


    //VkFramebufferCreateInfo framebufferInfo{};
    //framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    //framebufferInfo.renderPass = RenderPass.Handle;
    //framebufferInfo.attachmentCount = attachments.size();
    //framebufferInfo.pAttachments = attachments.data();
    //framebufferInfo.width = Extent_.width;
    //framebufferInfo.height = Extent_.height;
    //framebufferInfo.layers = 1;
    //if (vkCreateFramebuffer(Device.Handle, &framebufferInfo, nullptr, &Handle) != VK_SUCCESS) {
    //    throw std::runtime_error("failed to create framebuffer!");
    //}

    //if (checkAttachmentParams(pTexture.get(), mipLevel, firstArraySlice, arraySize, false))
    //{
    //    mpDesc = nullptr;
    //    mColorAttachments[rtIndex].pTexture = pTexture;
    //    mColorAttachments[rtIndex].mipLevel = mipLevel;
    //    mColorAttachments[rtIndex].firstArraySlice = firstArraySlice;
    //    mColorAttachments[rtIndex].arraySize = arraySize;
    //    bool allowUav = false;
    //    if (pTexture)
    //    {
    //        allowUav = ((pTexture->getBindFlags() & Texture::BindFlags::UnorderedAccess) != Texture::BindFlags::None);
    //    }

    //    mTempDesc.setColorTarget(rtIndex, pTexture ? pTexture->getFormat() : ResourceFormat::Unknown, allowUav);
    //    applyColorAttachment(rtIndex);
    //}
}

void VulkanFrameBuffer::attachDepthStencilTarget(const VulkanTexture2D* pDepthStencil, uint32_t mipLevel/* = 0*/)
{
    //attachments.push_back({
    //    pDepthStencil->getImageView(),
    //    pDepthStencil->getVkFormat()
    //    }
    //);
}

void VulkanFrameBuffer::create()
{   
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = RenderPass->Handle;
    framebufferInfo.attachmentCount = attachments.size();
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = Extent_.width;
    framebufferInfo.height = Extent_.height;
    framebufferInfo.layers = 1;
    if (Device->CreateFramebuffer(&framebufferInfo, nullptr, &Handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to create framebuffer!");
    } 
}