#pragma once
#include <VulkanRHI1/Vulkan.h>
#include <VulkanRHI1/VulkanObjects/VulkanRenderPass.h>
#include <VulkanRHI1/VulkanObjects/VulkanDevice.h>
#include <VulkanRHI1/VulkanObjects/VulkanSwapChain.h>
#include <VulkanRHI1/VulkanObjects/Texture/VulkanTexture.h>

#include <RHIFrameBuffer.h>



class VulkanFrameBuffer : public RHIFrameBuffer
{
public:
	struct Attachment {
		VkImageView view;
		VkFormat format;
	};

	VulkanFrameBuffer(VulkanDevice* InDevice); 
	VulkanFrameBuffer(VulkanDevice& Device, VulkanRenderPass& RenderPass, VulkanSwapChain* SwapChain,int32_t nImageViewIndex);

	VulkanFrameBuffer(VulkanDevice* Device, VulkanRenderPass* RenderPass, VulkanRenderTarget* rt);

public:
	VkExtent2D getExtent() const {
		return Extent_;
	}

	void attachColorTarget(const VulkanTexture2D* pColorTexture, uint32_t rtIndex, uint32_t mipLevel = 0); 
	void attachDepthStencilTarget(const VulkanTexture2D* pDepthStencil, uint32_t mipLevel = 0);

	virtual void create();

public:
	VkFramebuffer Handle;
	 
private:
	//VulkanImage *pColorImage = nullptr;
	//VulkanTextureView colorImageView;

	//VulkanImage *pDepthImage = nullptr;
	//VulkanTextureView depthImageView; 
	 
	std::vector<VkAttachmentDescription> attachmentDesc;
	std::vector<VkAttachmentReference>	 colorAttachmentRefs;
	 
	 

	std::vector<VkImageView> attachments;

	VkExtent2D Extent_;

	VulkanRenderPass* RenderPass = nullptr;
	VulkanDevice* Device = nullptr;
};