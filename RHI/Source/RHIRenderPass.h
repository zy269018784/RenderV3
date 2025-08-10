#pragma once
#include <RHIPixelFormat.h>
#include <RHIResource.h>

class RHIRenderPassDesc : public RHIResource
{
public: 
    virtual bool isCompatible(const RHIRenderPassDesc* other) const = 0;
    virtual RHIRenderPassDesc* newRenderPassDescriptor() const = 0;
};


class RHIRenderPass 
{
public:  
	virtual void create() = 0; 
    virtual void setColorFormat(RHIPixelFormat pixelFormat) {
        ColorFormat_ = pixelFormat;
    }

    virtual void setSampleCount(int sampleCount) {
        SampleCount_ = sampleCount;
    }

protected:
    RHIPixelFormat ColorFormat_ = RHIPixelFormat::PF_R8G8B8A8_UBYTE;
    int SampleCount_ = 1; //
};