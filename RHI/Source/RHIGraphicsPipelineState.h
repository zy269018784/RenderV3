#pragma once
#include <RHIResource.h>
#include <RHIDefinitions.h>
#include <RHIVertexInput.h>
#include <vector>

struct RHIShaderStageInfo {

};

struct RHIVertexInputInfo {
	std::vector<RHIVertexInputBinding> bindings;
	std::vector<RHIVertexInputAttribute> attributes;
};

struct RHIInputAssemblyInfo {

};

struct RHITessellationInfo {

};

struct RHIViewportInfo {

};

enum ECullMode {
	CULL_NONE = 0,
	CULL_FRONT = 0x00000001,
	CULL_BACK = 0x00000002,
	CULL_FRONT_AND_BACK = 0x00000003,
	CULL_MODE_MAX_ENUM = 0x7FFFFFFF
};

enum EFrontFace {
	CCW = 0,
	CW = 1,
	FRONT_FACE_MAX_ENUM = 0x7FFFFFFF
};

struct RHIRasterizationInfo{
	ECullMode cullMode;
	EFrontFace frontFace; 
};

struct RHIMultisampleInfo {

};

struct RHIDepthStencilInfo {
	bool depthTestEnable;
	bool depthWriteEnable;
	ECompareFunction eDepthCmpFunc = ECompareFunction::CF_LessEqual;
};
 
struct RHIColorBlendInfo {
	int colorAttachmentCount;
};

struct RHIDynamicInfo {

}; 
 