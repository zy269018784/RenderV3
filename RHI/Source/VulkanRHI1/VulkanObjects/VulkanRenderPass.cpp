#include <VulkanRHI1/VulkanObjects/VulkanRenderPass.h>
#include <VulkanRHI1/VulkanObjects/VulkanRenderTarget.h>
#include <VulkanRHI1/VulkanCommon.h>

#include <string>
#include <stdexcept>

VulkanRenderPass::VulkanRenderPass()
{ 
    ClearValueCount = 1;
    pClearValues = new VkClearValue[ClearValueCount];
    pClearValues[0].color.float32[0] = 0.0f;
    pClearValues[0].color.float32[1] = 0.0f;
    pClearValues[0].color.float32[2] = 0.0f;
    pClearValues[0].color.float32[3] = 0.0f;  
}

VulkanRenderPass::VulkanRenderPass(VulkanDevice* Device, VkFormat Format)
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = Format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (Device->CreateRenderPass(&renderPassInfo, nullptr, &Handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

VulkanRenderPass::VulkanRenderPass(VulkanDevice* InDevice, VulkanRenderPassDesc* InRenderPassDesc)
{
    Device = InDevice; 
    pRenderPassDesc = InRenderPassDesc;

    //VkSampleCountFlagBits samples = toVkMultisampleCount(InRenderPassDesc->samples);
    //VkFormat format = InRenderPassDesc->format; 
    //{//color AttachmentDescription
    //    VkAttachmentDescription attDesc = {};
    //    attDesc.format = format;
    //    attDesc.samples = samples;
    //    attDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    //    attDesc.storeOp = samples > VK_SAMPLE_COUNT_1_BIT ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE;
    //    attDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    //    attDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    //    attDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //    attDesc.finalLayout = samples > VK_SAMPLE_COUNT_1_BIT ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    //    pRenderPassDesc->attDescs.push_back(attDesc);
    //    pRenderPassDesc->colorRefs.push_back({ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
    //}
    // 
    ////depth stencil
    //if (pRenderPassDesc->hasDepthStencil) { // clear on load + no store + lazy alloc + transient image should play // nicely with tiled GPUs (no physical backing necessary for ds buffer)
    //    VkAttachmentDescription attDesc = {};
    //    Device->GetSupportedDepthFormat(&attDesc.format);
    //    attDesc.samples = samples;
    //    attDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    //    attDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    //    attDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    //    attDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    //    attDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //    attDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    //    pRenderPassDesc->attDescs.push_back(attDesc);
    //    pRenderPassDesc->dsRef = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
    //}

    ////resolve
    //if (samples > VK_SAMPLE_COUNT_1_BIT) { //MSAA会用到
    //    VkAttachmentDescription attDesc = {};
    //    attDesc.format = format;
    //    attDesc.samples = samples;
    //    attDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    //    attDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    //    attDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    //    attDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    //    attDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //    attDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    //    pRenderPassDesc->attDescs.push_back(attDesc);
    //    pRenderPassDesc->resolveRefs.push_back({ 2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
    //}

}

VulkanRenderPass::~VulkanRenderPass()
{
	delete[] pClearValues;
}

void VulkanRenderPass::SetSubpassDependency()
{
    // Replace the first implicit dep (TOP_OF_PIPE / ALL_COMMANDS) with our own.
    VkSubpassDependency subpassDep = {};
    subpassDep.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDep.dstSubpass = 0;
    subpassDep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDep.srcAccessMask = 0;
    subpassDep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    pRenderPassDesc->subpassDeps.push_back(subpassDep); 
    if (pRenderPassDesc->hasDepthStencil) {
        memset(&subpassDep, 0, sizeof(subpassDep));
        subpassDep.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpassDep.dstSubpass = 0;
        subpassDep.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        subpassDep.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        subpassDep.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        subpassDep.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        pRenderPassDesc->subpassDeps.push_back(subpassDep);
    }
}

void VulkanRenderPass::SetRenderPassCreateInfo(VkRenderPassCreateInfo* rpInfoOut, VkSubpassDescription* subpassDesc) 
{
    memset(subpassDesc, 0, sizeof(VkSubpassDescription));
    subpassDesc->pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDesc->colorAttachmentCount = uint32_t(pRenderPassDesc->colorRefs.size());
    subpassDesc->pColorAttachments = !pRenderPassDesc->colorRefs.empty() ? pRenderPassDesc->colorRefs.data() : nullptr;
    subpassDesc->pDepthStencilAttachment = pRenderPassDesc->hasDepthStencil ? &pRenderPassDesc->dsRef : nullptr;
    subpassDesc->pResolveAttachments = !pRenderPassDesc->resolveRefs.empty() ? pRenderPassDesc->resolveRefs.data() : nullptr;

    memset(rpInfoOut, 0, sizeof(VkRenderPassCreateInfo));
    rpInfoOut->sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpInfoOut->attachmentCount = uint32_t(pRenderPassDesc->attDescs.size());
    rpInfoOut->pAttachments = pRenderPassDesc->attDescs.data();
    rpInfoOut->subpassCount = 1;
    rpInfoOut->pSubpasses = subpassDesc;
    rpInfoOut->dependencyCount = uint32_t(pRenderPassDesc->subpassDeps.size());
    rpInfoOut->pDependencies = !pRenderPassDesc->subpassDeps.empty() ? pRenderPassDesc->subpassDeps.data() : nullptr;
}

void VulkanRenderPass::create()
{    
    //SubpassDependency
    VkSubpassDependency subpassDependency = {};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    pRenderPassDesc->subpassDeps.push_back(subpassDependency);

    if (pRenderPassDesc->hasDepthStencil) {
        VkSubpassDependency subpassDependency = {}; 
        subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpassDependency.dstSubpass = 0;
        subpassDependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        subpassDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        subpassDependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        subpassDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        pRenderPassDesc->subpassDeps.push_back(subpassDependency);
    }
    //SetSubpassDependency(); 
    


    //SubpassDescription
    VkSubpassDescription subpassDesc = {};  
    subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDesc.colorAttachmentCount = pRenderPassDesc->colorRefs.size();
    subpassDesc.pColorAttachments = !pRenderPassDesc->colorRefs.empty() ? pRenderPassDesc->colorRefs.data() : nullptr;
    subpassDesc.pDepthStencilAttachment = pRenderPassDesc->hasDepthStencil ? &pRenderPassDesc->dsRef : nullptr;
    subpassDesc.pResolveAttachments = !pRenderPassDesc->resolveRefs.empty() ? pRenderPassDesc->resolveRefs.data() : nullptr;

    VkRenderPassCreateInfo renderPassInfo{};  
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = pRenderPassDesc->attDescs.size();
    renderPassInfo.pAttachments = pRenderPassDesc->attDescs.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDesc;
    renderPassInfo.dependencyCount = uint32_t(pRenderPassDesc->subpassDeps.size());
    renderPassInfo.pDependencies = !pRenderPassDesc->subpassDeps.empty() ? pRenderPassDesc->subpassDeps.data() : nullptr;
    //SetRenderPassCreateInfo(&renderPassInfo, &subpass); 
    if (Device->CreateRenderPass(&renderPassInfo, nullptr, &Handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}