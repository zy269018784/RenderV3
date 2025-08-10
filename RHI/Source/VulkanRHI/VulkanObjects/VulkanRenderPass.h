#pragma once
#include <VulkanRHI/Vulkan.h>
#include <VulkanRHI/VulkanObjects/VulkanDevice.h>
#include <VulkanRHI/VulkanObjects/VulkanRenderTarget.h>
#include <RHIRenderPass.h>
#include <array>
#include <cstdint>

enum class ESubpassHint : uint8_t
{ 
	None,					// Regular rendering 
	DepthReadSubpass,		// Render pass has depth reading subpass 
	DeferredShadingSubpass,	// Mobile defferred shading subpass 
	CustomResolveSubpass,	// Mobile MSAA custom resolve subpass. Includes DepthReadSubpass.
};

//const int MaxSimultaneousRenderTargets = 8;
//class VulkanRenderTargetLayout
//{
//public:
//	FVulkanRenderTargVulkanRenderTargetLayoutetLayout(const FGraphicsPipelineStateInitializer& Initializer);
//	VulkanRenderTargetLayout(VulkanDevice& InDevice, const FRHISetRenderTargetsInfo& RTInfo);
//	VulkanRenderTargetLayout(VulkanDevice& InDevice, const FRHIRenderPassInfo& RPInfo, VkImageLayout CurrentDepthLayout, VkImageLayout CurrentStencilLayout);
//
//	inline uint32 GetRenderPassCompatibleHash() const {
//		check(bCalculatedHash);
//		return RenderPassCompatibleHash;
//	}
//
//	inline uint32 GetRenderPassFullHash() const {
//		check(bCalculatedHash);
//		return RenderPassFullHash;
//	}
//
//	inline const VkOffset2D& GetOffset2D() const { return Offset.Offset2D; }
//	inline const VkOffset3D& GetOffset3D() const { return Offset.Offset3D; }
//	inline const VkExtent2D& GetExtent2D() const { return Extent.Extent2D; }
//	inline const VkExtent3D& GetExtent3D() const { return Extent.Extent3D; }
//
//	inline const VkAttachmentDescription* GetAttachmentDescriptions() const { return Desc; }
//	inline uint32_t GetNumColorAttachments() const { return NumColorAttachments; }
//	inline bool GetHasDepthStencil() const { return bHasDepthStencil != 0; }
//	inline bool GetHasResolveAttachments() const { return bHasResolveAttachments != 0; }
//	inline bool GetHasFragmentDensityAttachment() const { return bHasFragmentDensityAttachment != 0; }
//	inline uint32_t GetNumAttachmentDescriptions() const { return NumAttachmentDescriptions; }
//	inline uint32_t GetNumSamples() const { return NumSamples; }
//	inline uint32_t GetNumUsedClearValues() const { return NumUsedClearValues; }
//	inline bool GetIsMultiView() const { return MultiViewCount != 0; }
//	inline uint32_t GetMultiViewCount() const { return MultiViewCount; }
//
//
//	inline const VkAttachmentReference* GetColorAttachmentReferences() const { return NumColorAttachments > 0 ? ColorReferences : nullptr; }
//	inline const VkAttachmentReference* GetResolveAttachmentReferences() const { return bHasResolveAttachments ? ResolveReferences : nullptr; }
//	inline const VkAttachmentReference* GetDepthAttachmentReference() const { return bHasDepthStencil ? &DepthReference : nullptr; }
//	inline const VkAttachmentReferenceStencilLayout* GetStencilAttachmentReference() const { return bHasDepthStencil ? &StencilReference : nullptr; }
//	inline const VkAttachmentReference* GetFragmentDensityAttachmentReference() const { return bHasFragmentDensityAttachment ? &FragmentDensityReference : nullptr; }
//
//	inline const VkAttachmentDescriptionStencilLayout* GetStencilDesc() const { return bHasDepthStencil ? &StencilDesc : nullptr; }
//
//	inline const ESubpassHint GetSubpassHint() const { return SubpassHint; }
//	inline const VkSurfaceTransformFlagBitsKHR GetQCOMRenderPassTransform() const { return QCOMRenderPassTransform; }
//
//protected:
//	VkImageLayout GetVRSImageLayout() const;
//
//protected:
//	VkSurfaceTransformFlagBitsKHR QCOMRenderPassTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
//	VkAttachmentReference ColorReferences[MaxSimultaneousRenderTargets];
//	VkAttachmentReference DepthReference;
//
//	//VkAttachmentReferenceStencilLayout StencilReference;
//	//VkAttachmentReference FragmentDensityReference;
//	//VkAttachmentReference ResolveReferences[MaxSimultaneousRenderTargets];
//	
//	VkAttachmentDescription Desc[MaxSimultaneousRenderTargets * 2 + 2];		// Depth goes in the "+1" slot and the Shading Rate texture goes in the "+2" slot.
//	VkAttachmentDescriptionStencilLayout StencilDesc;
//
//	uint8_t NumAttachmentDescriptions;
//	uint8_t NumColorAttachments;
//	uint8_t NumInputAttachments = 0;
//	uint8_t bHasDepthStencil;
//	uint8_t bHasResolveAttachments;
//	uint8_t bHasFragmentDensityAttachment;
//	uint8_t NumSamples;
//	uint8_t NumUsedClearValues;
//
//	ESubpassHint SubpassHint = ESubpassHint::None;
//	uint8_t MultiViewCount;
//
//	// Hash for a compatible RenderPass
//	uint32_t RenderPassCompatibleHash = 0;
//	// Hash for the render pass including the load/store operations
//	uint32_t RenderPassFullHash = 0;
//
//	union
//	{
//		VkOffset3D Offset3D;
//		VkOffset2D Offset2D;
//	} Offset;
//
//	union
//	{
//		VkExtent3D	Extent3D;
//		VkExtent2D	Extent2D;
//	} Extent;
//
//	inline void ResetAttachments()
//	{
//		FMemory::Memzero(ColorReferences);
//		FMemory::Memzero(DepthReference);
//		FMemory::Memzero(FragmentDensityReference);
//		FMemory::Memzero(ResolveReferences);
//		FMemory::Memzero(Desc);
//		FMemory::Memzero(Offset);
//		FMemory::Memzero(Extent);
//
//		ZeroVulkanStruct(StencilReference, VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_STENCIL_LAYOUT);
//		ZeroVulkanStruct(StencilDesc, VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_STENCIL_LAYOUT);
//	}
//
//	FVulkanRenderTargetLayout()
//	{
//		NumAttachmentDescriptions = 0;
//		NumColorAttachments = 0;
//		bHasDepthStencil = 0;
//		bHasResolveAttachments = 0;
//		bHasFragmentDensityAttachment = 0;
//		NumSamples = 0;
//		NumUsedClearValues = 0;
//		MultiViewCount = 0;
//
//		ResetAttachments();
//	}
//
//	bool bCalculatedHash = false;
//	void CalculateRenderPassHashes(const FRHISetRenderTargetsInfo& RTInfo);
//
//	friend class FVulkanPipelineStateCacheManager;
//	friend struct FGfxPipelineDesc;
//};

class VulkanRenderPassDesc// : public RHIRenderPassDesc
{
public:
	VulkanRenderPassDesc() {}
	~VulkanRenderPassDesc() {}

	uint32_t multiViewCount = 0;
	bool hasDepthStencil = false;
	bool hasDepthStencilResolve = false;	 
	std::vector<VkAttachmentDescription> attDescs;//8

	std::vector<VkAttachmentReference> colorRefs; //8
	VkAttachmentReference dsRef = {};
	std::vector<VkAttachmentReference> resolveRefs; //8
	VkAttachmentReference dsResolveRef = {};

	std::vector<VkSubpassDependency> subpassDeps;  //2 
	 
	VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
	int samples = 1; 
};
 


class VulkanRenderPass : public RHIRenderPass
{
public:
	VulkanRenderPass();
	VulkanRenderPass(VulkanDevice *InDevice, VkFormat Format);
	VulkanRenderPass(VulkanDevice* InDevice, VulkanRenderPassDesc* InRenderPassDesc);
	~VulkanRenderPass();

	virtual void create(); 

public:
	//inline const FVulkanRenderTargetLayout& GetLayout() const {
	//	return Layout;
	//}

	inline VkRenderPass GetHandle() const
	{
		return Handle;
	}

	inline uint32_t GetNumUsedClearValues() const
	{
		return ClearValueCount;
	}
protected:	
	void SetSubpassDependency(); 
	void SetRenderPassCreateInfo(VkRenderPassCreateInfo* rpInfoOut, VkSubpassDescription* subpassDesc);


public: 
	VulkanRenderPassDesc* pRenderPassDesc = nullptr;


	//VkRect2D		RenderArea;

	uint32_t		ClearValueCount = 0;
	VkClearValue*	pClearValues = nullptr; 

	VkRenderPass	Handle = VK_NULL_HANDLE;
	VulkanDevice*	Device = nullptr; 
	
};