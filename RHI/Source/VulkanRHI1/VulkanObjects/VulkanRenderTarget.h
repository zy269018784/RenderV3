#pragma once
#include <vulkan/vulkan.h>   
#include <RHIRenderTarget.h>
#include <RHIRenderPass.h> 

class VulkanDevice;
class VulkanRenderPassDesc;
class VulkanRenderPass;
class VulkanFrameBuffer; 

#define MAX_COLOR_ATTACHMENTS 8

struct VulkanRenderTargetData
{
	VkFramebuffer fb = VK_NULL_HANDLE;
	VulkanRenderPassDesc* rpDesc = nullptr;
	float dpr = 1;
	int sampleCount = 1;
	int colorAttCount = 0;
	int dsAttCount = 0;
	int resolveAttCount = 0;
	int dsResolveAttCount = 0;
	int multiViewCount = 0;  
	int width;
	int height;
};

class VulkanRenderTarget : public RHITextureRenderTarget 
{
public:  
	VulkanRenderTarget(VulkanDevice* InDevice, const RHIRenderTargetDesc& Desc);
	~VulkanRenderTarget() {} 

	virtual bool create(); 
 
	virtual void SetColorDepth(RHITexture* rtColor, RHITexture* rtDepth); 
	virtual void SetColorDepth(std::vector<RHITexture*> rtColors, RHITexture* rtDepth);


	virtual RHIRenderPass* GetRenderPass() {
		return RenderPass_;
	}

	virtual RHIFrameBuffer* GetFrameBuffer() {
		return FrameBuffer_;
	}


	void createRenderPass();

	VkExtent2D getExtent() const {
		return Extent_;
	}

	VkOffset2D getOffset() const {
		return Offset_;
	}
	std::vector< VkAttachmentDescription > colorAttachments;
	std::vector< VkAttachmentReference > colorAttachmentRefs;

	std::vector< VkImageView > attachments;

	VkOffset2D Offset_ = { 0, 0 };
	VkExtent2D Extent_ = { 512,512 };

	VulkanRenderTargetData rtData;
	VkImageView rtv[MAX_COLOR_ATTACHMENTS];
	VkImageView dsv = VK_NULL_HANDLE;
	VkImageView resrtv[MAX_COLOR_ATTACHMENTS];
	VkImageView resdsv = VK_NULL_HANDLE;
  
	RHIRenderPass* RenderPass_ = nullptr;
	RHIFrameBuffer* FrameBuffer_ = nullptr;
	VulkanDevice* Device = nullptr;
};