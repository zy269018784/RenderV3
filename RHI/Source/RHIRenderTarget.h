#pragma once
#include <RHIFrameBuffer.h>
#include <RHIRenderPass.h>
#include <RHITexture.h> 
#include <vector>
//class RHIRenderBuffer : public RHIResource
//{
//public:
//	enum Type {
//		DepthStencil,
//		Color
//	};
//
//	enum Flag {
//		UsedWithSwapChainOnly = 1 << 0
//	};
//	Q_DECLARE_FLAGS(Flags, Flag)
//
//	struct NativeRenderBuffer {
//		uint64_t object;
//	};
//	 
//
//	Type type() const { return m_type; }
//	void setType(Type t) { m_type = t; }
//
//	QSize pixelSize() const { return m_pixelSize; }
//	void setPixelSize(const QSize& sz) { m_pixelSize = sz; }
//
//	int sampleCount() const { return m_sampleCount; }
//	void setSampleCount(int s) { m_sampleCount = s; }
//
//	Flags flags() const { return m_flags; }
//	void setFlags(Flags f) { m_flags = f; }
//
//	virtual bool create() = 0;
//	virtual bool createFrom(NativeRenderBuffer src);
//
//	virtual RHITexture::Format backingFormat() const = 0;
//
//protected:
//	RHIRenderBuffer(QRhiImplementation* rhi, Type type_, const QSize& pixelSize_,
//		int sampleCount_, Flags flags_, RHITexture::Format backingFormatHint_);
//	Type m_type;
//	QSize m_pixelSize;
//	int m_sampleCount;
//	Flags m_flags;
//	QRhiTexture::Format m_backingFormatHint;
//};

class RHIColorAttachment
{
public:
	RHIColorAttachment() = default;
	RHIColorAttachment(RHITexture* texture) {
		m_texture = texture;
	}
	//RHIColorAttachment(RHIRenderBuffer* renderBuffer);

	RHITexture* texture() const { return m_texture; }
	void setTexture(RHITexture* tex) { m_texture = tex; }

	//RHIRenderBuffer* renderBuffer() const { return m_renderBuffer; }
	//void setRenderBuffer(RHIRenderBuffer* rb) { m_renderBuffer = rb; }

	int layer() const { return m_layer; }
	void setLayer(int layer) { m_layer = layer; }

	int level() const { return m_level; }
	void setLevel(int level) { m_level = level; }

	RHITexture* resolveTexture() const { return m_resolveTexture; }
	void setResolveTexture(RHITexture* tex) { m_resolveTexture = tex; }

	int resolveLayer() const { return m_resolveLayer; }
	void setResolveLayer(int layer) { m_resolveLayer = layer; }

	int resolveLevel() const { return m_resolveLevel; }
	void setResolveLevel(int level) { m_resolveLevel = level; }

	int multiViewCount() const { return m_multiViewCount; }
	void setMultiViewCount(int count) { m_multiViewCount = count; }

private:
	RHITexture* m_texture = nullptr;
	//RHIRenderBuffer* m_renderBuffer = nullptr;
	int m_layer = 0;
	int m_level = 0;

	RHITexture* m_resolveTexture = nullptr;
	int m_resolveLayer = 0;
	int m_resolveLevel = 0;
	int m_multiViewCount = 0;
};

class RHIRenderTargetDesc
{
public:
	RHIRenderTargetDesc() {}
	RHIRenderTargetDesc(const RHIColorAttachment& colorAttachment)
	{

	}

	//RHIRenderTargetDesc(const RHIColorAttachment& colorAttachment, QRhiRenderBuffer* depthStencilBuffer);
	RHIRenderTargetDesc(RHIColorAttachment* colorAttachment, RHITexture* pDepthTexture) {
		m_colorAttachments.push_back(colorAttachment);
		m_depthTexture = pDepthTexture;
	}

	void AddColorAttachment(RHIColorAttachment* colorAttachment) {
		m_colorAttachments.push_back(colorAttachment);
	}
	size_t colorAttachmentCount() const {
		return m_colorAttachments.size();
	}

	//RHURenderBuffer* depthStencilBuffer() const { return m_depthStencilBuffer; }
	//void setDepthStencilBuffer(RHURenderBuffer* renderBuffer) { m_depthStencilBuffer = renderBuffer; }

	RHITexture* depthTexture() const { return m_depthTexture; }
	void setDepthTexture(RHITexture* texture) { m_depthTexture = texture; }

	RHITexture* depthResolveTexture() const { return m_depthResolveTexture; }
	void setDepthResolveTexture(RHITexture* tex) { m_depthResolveTexture = tex; }

public:
	std::vector<RHIColorAttachment*> m_colorAttachments;

	//RHIRenderBuffer* m_depthStencilBuffer = nullptr;
	RHITexture* m_depthTexture = nullptr;
	RHITexture* m_depthResolveTexture = nullptr;
};


class RHIRenderTarget
{
public:     
	virtual void SetColorDepth(RHITexture* rtColor, RHITexture* rtDepth) = 0; 
	virtual bool create() = 0;
	//virtual RHIRenderPass* createOffscreenRenderPass(VulkanRenderPassDesc* rpDesc, std::vector<RHIColorAttachment*>& m_colorAttachments,
	//	bool preserveColor,
	//	bool preserveDs,
	//	bool storeDs,
	//	//QRhiRenderBuffer* depthStencilBuffer,
	//	RHITexture* depthTexture,
	//	RHITexture* depthResolveTexture) = 0;

	RHIRenderPassDesc* renderPassDescriptor() const { return m_rpDesc; }
	void setRenderPassDescriptor(RHIRenderPassDesc* desc) { m_rpDesc = desc; }


protected:
	RHIRenderPassDesc* m_rpDesc = nullptr;
};
 

class RHITextureRenderTarget : public RHIRenderTarget
{
public:   
	//virtual RHIRenderPassDesc* newRenderPassDesc() = 0; 
	//virtual bool create() = 0;

	//virtual RHIRenderPass* createOffscreenRenderPass(VulkanRenderPassDesc* rpDesc, std::vector<RHIColorAttachment*>& m_colorAttachments,
	//	bool preserveColor,
	//	bool preserveDs,
	//	bool storeDs,
	//	//QRhiRenderBuffer* depthStencilBuffer,
	//	RHITexture* depthTexture,
	//	RHITexture* depthResolveTexture) = 0;

	virtual void SetColorDepth(RHITexture* rtColor, RHITexture* rtDepth) = 0;
	virtual void SetColorDepth(std::vector<RHITexture*> rtColors, RHITexture* rtDepth) = 0;

	virtual RHIRenderPass* GetRenderPass() = 0;
	virtual RHIFrameBuffer* GetFrameBuffer() = 0;

//	RHIRenderTargetDesc getDesc() const  { 
//		return m_Desc;
//	}
//	void setDesc(const RHIRenderTargetDesc& desc) {
//		m_Desc = desc; 
//	}
protected:
	RHITextureRenderTarget(const RHIRenderTargetDesc& Desc) : m_Desc(Desc) 
	{ 
	} 
	RHIRenderTargetDesc m_Desc;
};