#include <VulkanRHI1/VulkanObjects/VulkanRenderTarget.h>

#include <RHITexture.h>
#include <RHIRenderPass.h>
#include <VulkanRHI1/VulkanObjects/VulkanDevice.h>
#include <VulkanRHI1/VulkanObjects/Texture/VulkanTexture.h>   
#include <VulkanRHI1/VulkanObjects/VulkanFrameBuffer.h>
#include <VulkanRHI1/VulkanObjects/VulkanRenderPass.h>

VulkanRenderTarget::VulkanRenderTarget(VulkanDevice* InDevice, const RHIRenderTargetDesc& Desc)
    : RHITextureRenderTarget(Desc)
    , Device(InDevice)
{ 

    //for (int att = 0; att < MAX_COLOR_ATTACHMENTS; ++att) {
    //    rtv[att] = VK_NULL_HANDLE;
    //    resrtv[att] = VK_NULL_HANDLE;
    //}
}

void VulkanRenderTarget::SetColorDepth(RHITexture* rtColor, RHITexture* rtDepth)
{
	Extent_.width = rtColor->GetSizeX();
	Extent_.height = rtColor->GetSizeY();
	 
	std::vector< VkAttachmentReference > colorAttachmentRefs;

	VkFormat format;
	if (rtColor->GetResourceType() == RRT_Texture2D) {
		VulkanTexture2D* pTex = dynamic_cast<VulkanTexture2D*>(rtColor); 
		attachments.push_back(pTex->GetImageView());
		format = pTex->GetImageFormat();
	} 
	else {
		//not support
	}

	VkAttachmentDescription colorAttachment = {}; 
	colorAttachment.format = format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; 
	colorAttachments.push_back(colorAttachment);

	VkAttachmentReference colorReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	colorAttachmentRefs.push_back(colorReference);


	if (rtDepth) { 
		VkFormat format;
		if (rtDepth->GetResourceType() == RRT_Texture2D) {
			VulkanTexture2D* pTex = dynamic_cast<VulkanTexture2D*>(rtDepth);
			attachments.push_back(pTex->GetImageView());
			format = pTex->GetImageFormat();
		} else {
			//not support
		}

		// Depth attachment
		VkAttachmentDescription depthAttachment = {}; 
		depthAttachment.format = format;
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		colorAttachments.push_back(depthAttachment);

		VkAttachmentReference depthReference = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
		colorAttachmentRefs.push_back(depthReference);
	}


}


void VulkanRenderTarget::SetColorDepth(std::vector<RHITexture*> rtColors, RHITexture* rtDepth)
{
	//Extent_.width = rtColor->GetSizeX();
	//Extent_.height = rtColor->GetSizeY();

	//std::vector< VkAttachmentReference > colorAttachmentRefs;

	//VkFormat format;
	//if (rtColor->GetResourceType() == RRT_Texture2D) {
	//	VulkanTexture2D* pTex = dynamic_cast<VulkanTexture2D*>(rtColor);
	//	attachments.push_back(pTex->getImageView());
	//	format = pTex->m_VulkanImage.m_format;
	//}
	//else {
	//	//not support
	//}

	//VkAttachmentDescription colorAttachment = {};
	//colorAttachment.format = format;
	//colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	//colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	//colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	//colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	//colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	//colorAttachments.push_back(colorAttachment);

	//VkAttachmentReference colorReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	//colorAttachmentRefs.push_back(colorReference);


	//if (rtDepth) {
	//	VkFormat format;
	//	if (rtDepth->GetResourceType() == RRT_Texture2D) {
	//		VulkanTexture2D* pTex = dynamic_cast<VulkanTexture2D*>(rtDepth);
	//		attachments.push_back(pTex->getImageView());
	//		format = pTex->m_VulkanImage.m_format;
	//	}
	//	else {
	//		//not support
	//	}

	//	// Depth attachment
	//	VkAttachmentDescription depthAttachment = {};
	//	depthAttachment.format = format;
	//	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	//	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	//	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	//	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//	colorAttachments.push_back(depthAttachment);

	//	VkAttachmentReference depthReference = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
	//	colorAttachmentRefs.push_back(depthReference);
	//}
}

void VulkanRenderTarget::createRenderPass()
{
    printf("createRenderPass\n");
    RHIRenderTargetDesc& d = this->m_Desc;
    VulkanRenderPassDesc* rpDesc = rtData.rpDesc = new VulkanRenderPassDesc();

    bool preserveColor = true;
    bool preserveDs = true;
    bool storeDs = true;
     
    for (int i = 0; i < d.m_colorAttachments.size(); i++) {
        RHIColorAttachment* colorAtt = d.m_colorAttachments[i]; 
        if (colorAtt) { 
            RHITexture* pTex = colorAtt->texture();
            VulkanTexture2D* pVkTex = (pTex) ? dynamic_cast<VulkanTexture2D*>(pTex) : nullptr;
            if (pVkTex) {
                const VkFormat vkformat = pVkTex->getVkFormat();// pTex ? pTex->getFormat() : rbD->vkformat;
                const VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;// pTex->samples; // pTex ? pTex->samples : rbD->samples;  
                if (i == 0) {
                    rpDesc->samples = samples;
                    rpDesc->format = vkformat;
                    Extent_.width = pVkTex->GetSizeX();
                    Extent_.height = pVkTex->GetSizeY(); 
                }

                VkAttachmentDescription attDesc = {};
                attDesc.format = vkformat;
                attDesc.samples = samples;
                attDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;// preserveColor ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR;
                attDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // (colorAtt->resolveTexture() && !preserveColor) ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE;
                attDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//// preserveColor ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;
                attDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;// VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                rpDesc->attDescs.push_back(attDesc);
                const VkAttachmentReference ref = { uint32_t(rpDesc->attDescs.size() - 1), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
                rpDesc->colorRefs.push_back(ref);
                attachments.push_back(pVkTex->GetImageView());
            }
        }
    } 


    RHITexture* pDepthTexture = m_Desc.depthTexture();
    rpDesc->hasDepthStencil = pDepthTexture ? true : false; //depthStencilBuffer || depthTexture
    if (rpDesc->hasDepthStencil) {
        VulkanTexture2D* pTexDepth = dynamic_cast<VulkanTexture2D*>(pDepthTexture);
        const VkFormat dsFormat = pTexDepth->getVkFormat(); //todo...
        const VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;////depthTexture ? QRHI_RES(QVkTexture, depthTexture)->samples : QRHI_RES(QVkRenderBuffer, depthStencilBuffer)->samples; 
        const VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //preserveDs ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR;
        const VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; //storeDs ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
        VkAttachmentDescription attDesc = {};
        attDesc.format = dsFormat;
        attDesc.samples = samples;
        attDesc.loadOp = loadOp;
        attDesc.storeOp = storeOp;
        attDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;// loadOp;
        attDesc.stencilStoreOp = storeOp;
        attDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;// preserveDs ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;
        attDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        rpDesc->attDescs.push_back(attDesc); 
        attachments.push_back(pTexDepth->GetImageView());
    }
    rpDesc->dsRef = { uint32_t(rpDesc->attDescs.size() - 1), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

    //resolve
    for (int i = 0; i < d.m_colorAttachments.size(); i++) {
        RHIColorAttachment* colorAtt = d.m_colorAttachments[i];
        if (colorAtt->resolveTexture()) {
            VulkanTexture2D* pTexResolve = dynamic_cast<VulkanTexture2D*>(colorAtt->resolveTexture());
            const VkFormat dstFormat = pTexResolve->getVkFormat();
            if (pTexResolve->getSamples() > VK_SAMPLE_COUNT_1_BIT)
                printf("Resolving into a multisample texture is not supported");

            VulkanTexture2D* texD = dynamic_cast<VulkanTexture2D*>(colorAtt->texture());
            const VkFormat srcFormat = texD->getVkFormat();// texD ? texD->vkformat : rbD->vkformat;
            if (srcFormat != dstFormat) { // This is a validation error. But some implementations survive, actually. Warn about it however, because it's an error with some other backends (like D3D) as well.
                printf("Multisample resolve between different formats (%d and %d) is not supported.", int(srcFormat), int(dstFormat));
            }
            VkAttachmentDescription attDesc = {};
            attDesc.format = texD->getVkFormat();// texD->viewFormat;
            attDesc.samples = VK_SAMPLE_COUNT_1_BIT;
            attDesc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // ignored
            attDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            rpDesc->attDescs.push_back(attDesc);
            const VkAttachmentReference ref = { uint32_t(rpDesc->attDescs.size() - 1), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
            rpDesc->resolveRefs.push_back(ref);
        }
        else {
            const VkAttachmentReference ref = { VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
            rpDesc->resolveRefs.push_back(ref);
        }
    } 

    RHITexture* pDepthResolveTexture = m_Desc.depthResolveTexture();
    rpDesc->hasDepthStencilResolve = rpDesc->hasDepthStencil && pDepthResolveTexture;
    if (rpDesc->hasDepthStencilResolve) { 
        VulkanTexture2D* dTexDepthResolve = dynamic_cast<VulkanTexture2D*>(pDepthResolveTexture); 
        VkAttachmentDescription attDesc = {};
        attDesc.format = dTexDepthResolve->getVkFormat(); 
        attDesc.samples = VK_SAMPLE_COUNT_1_BIT;
        attDesc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; 
        attDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attDesc.stencilLoadOp = attDesc.loadOp;
        attDesc.stencilStoreOp = attDesc.storeOp;
        attDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        rpDesc->attDescs.push_back(attDesc);  
        rpDesc->dsResolveRef = { uint32_t(rpDesc->attDescs.size() - 1), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
    }
     
    printf("createRenderPass attachments %d\n", attachments.size());
    RenderPass_ = new VulkanRenderPass(Device, rpDesc);
    RenderPass_->create();
}


bool VulkanRenderTarget::create()
{  
    createRenderPass();
    FrameBuffer_ = new VulkanFrameBuffer(Device, dynamic_cast<VulkanRenderPass*>(RenderPass_), this);
    FrameBuffer_->create();
#if 0
	std::vector<VkImageView> views;
	for (int i = 0; i < d.colorAttCount; i++) {
		views.push_back(rtv[i]);
	}

    d.multiViewCount = 0;

    d.colorAttCount = 0;
    int attIndex = 0;
    for (auto it = m_desc.cbeginColorAttachments(), itEnd = m_desc.cendColorAttachments(); it != itEnd; ++it, ++attIndex) {
        d.colorAttCount += 1;
        QVkTexture* texD = QRHI_RES(QVkTexture, it->texture());
        QVkRenderBuffer* rbD = QRHI_RES(QVkRenderBuffer, it->renderBuffer());
        Q_ASSERT(texD || rbD);
        if (texD) {
            Q_ASSERT(texD->flags().testFlag(QRhiTexture::RenderTarget));
            const bool is1D = texD->flags().testFlag(QRhiTexture::OneDimensional);
            const bool isMultiView = it->multiViewCount() >= 2;
            if (isMultiView && d.multiViewCount == 0)
                d.multiViewCount = it->multiViewCount();
            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = texD->image;
            viewInfo.viewType = is1D ? VK_IMAGE_VIEW_TYPE_1D
                : (isMultiView ? VK_IMAGE_VIEW_TYPE_2D_ARRAY
                    : VK_IMAGE_VIEW_TYPE_2D);
            viewInfo.format = texD->viewFormat;
            viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
            viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
            viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
            viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = uint32_t(it->level());
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = uint32_t(it->layer());
            viewInfo.subresourceRange.layerCount = uint32_t(isMultiView ? it->multiViewCount() : 1);
            VkResult err = rhiD->df->vkCreateImageView(rhiD->dev, &viewInfo, nullptr, &rtv[attIndex]);
            if (err != VK_SUCCESS) {
                qWarning("Failed to create render target image view: %d", err);
                return false;
            }
            views.append(rtv[attIndex]);
            if (attIndex == 0) {
                d.pixelSize = rhiD->q->sizeForMipLevel(it->level(), texD->pixelSize());
                d.sampleCount = texD->samples;
            }
        }
        else if (rbD) {
            Q_ASSERT(rbD->backingTexture);
            views.append(rbD->backingTexture->imageView);
            if (attIndex == 0) {
                d.pixelSize = rbD->pixelSize();
                d.sampleCount = rbD->samples;
            }
        }
    }
    d.dpr = 1;

    if (hasDepthStencil) {
        if (m_desc.depthTexture()) {
            QVkTexture* depthTexD = QRHI_RES(QVkTexture, m_desc.depthTexture());
            // need a dedicated view just because viewFormat may differ from vkformat
            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = depthTexD->image;
            viewInfo.viewType = d.multiViewCount > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = depthTexD->viewFormat;
            viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
            viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
            viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
            viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.layerCount = qMax<uint32_t>(1, d.multiViewCount);
            VkResult err = rhiD->df->vkCreateImageView(rhiD->dev, &viewInfo, nullptr, &dsv);
            if (err != VK_SUCCESS) {
                qWarning("Failed to create depth-stencil image view for rt: %d", err);
                return false;
            }
            views.append(dsv);
            if (d.colorAttCount == 0) {
                d.pixelSize = depthTexD->pixelSize();
                d.sampleCount = depthTexD->samples;
            }
        }
        else {
            QVkRenderBuffer* depthRbD = QRHI_RES(QVkRenderBuffer, m_desc.depthStencilBuffer());
            views.append(depthRbD->imageView);
            if (d.colorAttCount == 0) {
                d.pixelSize = depthRbD->pixelSize();
                d.sampleCount = depthRbD->samples;
            }
        }
        d.dsAttCount = 1;
    }
    else {
        d.dsAttCount = 0;
    }

    d.resolveAttCount = 0;
    attIndex = 0;
    Q_ASSERT(d.multiViewCount == 0 || d.multiViewCount >= 2);
    for (auto it = m_desc.cbeginColorAttachments(), itEnd = m_desc.cendColorAttachments(); it != itEnd; ++it, ++attIndex) {
        if (it->resolveTexture()) {
            QVkTexture* resTexD = QRHI_RES(QVkTexture, it->resolveTexture());
            Q_ASSERT(resTexD->flags().testFlag(QRhiTexture::RenderTarget));
            d.resolveAttCount += 1;

            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = resTexD->image;
            viewInfo.viewType = d.multiViewCount ? VK_IMAGE_VIEW_TYPE_2D_ARRAY
                : VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = resTexD->viewFormat;
            viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
            viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
            viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
            viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = uint32_t(it->resolveLevel());
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = uint32_t(it->resolveLayer());
            viewInfo.subresourceRange.layerCount = qMax<uint32_t>(1, d.multiViewCount);
            VkResult err = rhiD->df->vkCreateImageView(rhiD->dev, &viewInfo, nullptr, &resrtv[attIndex]);
            if (err != VK_SUCCESS) {
                qWarning("Failed to create render target resolve image view: %d", err);
                return false;
            }
            views.append(resrtv[attIndex]);
        }
    }

    if (m_desc.depthResolveTexture()) {
        QVkTexture* resTexD = QRHI_RES(QVkTexture, m_desc.depthResolveTexture());
        Q_ASSERT(resTexD->flags().testFlag(QRhiTexture::RenderTarget));

        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = resTexD->image;
        viewInfo.viewType = d.multiViewCount ? VK_IMAGE_VIEW_TYPE_2D_ARRAY
            : VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = resTexD->viewFormat;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = qMax<uint32_t>(1, d.multiViewCount);
        VkResult err = rhiD->df->vkCreateImageView(rhiD->dev, &viewInfo, nullptr, &resdsv);
        if (err != VK_SUCCESS) {
            qWarning("Failed to create render target depth resolve image view: %d", err);
            return false;
        }
        views.append(resdsv);
        d.dsResolveAttCount = 1;
    }
    else {
        d.dsResolveAttCount = 0;
    }

    if (!m_renderPassDesc)
        qWarning("QVkTextureRenderTarget: No renderpass descriptor set. See newCompatibleRenderPassDescriptor() and setRenderPassDescriptor().");

    d.rp = QRHI_RES(QVkRenderPassDescriptor, m_renderPassDesc); 


	VkFramebufferCreateInfo fbInfo = {};
	fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fbInfo.renderPass = RenderPass->Handle;
	fbInfo.attachmentCount = views.size();		// uint32_t(d.colorAttCount + d.dsAttCount + d.resolveAttCount + d.dsResolveAttCount);
	fbInfo.pAttachments = views.data();
	fbInfo.width = d.width;
	fbInfo.height = d.height;
	fbInfo.layers = 1;

	VkResult err = vkCreateFramebuffer(rhiD->dev, &fbInfo, nullptr, &d.fb);
	if (err != VK_SUCCESS) {
		printf("Failed to create framebuffer: %d", err);
		return false;
	}
#endif
	return true;
}