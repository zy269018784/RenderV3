#pragma once

enum RHIResourceType
{
	RRT_None,
	RRT_VertexAttributeState,
	RRT_RasterizerState,
	RRT_BoundShaderState,
	RRT_ViewportState,
	RRT_DepthStencilState,
	RRT_DepthRangeState,
	RRT_BlendState,
	RRT_ScissorState,
	RRT_SamplerState,	
	RRT_GraphicsPipelineState,
	RRT_ComputePipelineState,
	RRT_RayTracingPipelineState,

	RRT_VertexDeclaration,

	RRT_VertexShader,
	RRT_MeshShader,
	RRT_AmplificationShader,
	RRT_PixelShader,
	RRT_GeometryShader,
	RRT_RayTracingShader,
	RRT_ComputeShader,

	RRT_Texture,
	RRT_Texture1D,
	RRT_Texture2D,
	RRT_Texture2DArray,
	RRT_Texture3D,
	RRT_TextureCube,
	RRT_TextureCubeArray,
	RRT_TextureReference,

	RRT_Buffer,
	RRT_UniformBufferLayout,
	RRT_UniformBuffer,
	RRT_RayTracingAS,

	RRT_Num
};


class RHIResource 
{
public:
	RHIResource()
		: ResourceType(RRT_None)
	{
	}

	RHIResource(RHIResourceType InResourceType)
		: ResourceType(InResourceType)
	{
	}

	virtual ~RHIResource()
	{
	}

	RHIResourceType GetResourceType() const {
		return ResourceType;
	}
private:
	const RHIResourceType ResourceType;
};

enum EVertexElementType {
	VET_None,
	VET_Float1,
	VET_Float2,
	VET_Float3,
	VET_Float4,
	VET_PackedNormal,	// FPackedNormal
	VET_UByte4,
	VET_UByte4N,
	VET_Color,
	VET_Short2,
	VET_Short4,
	VET_Short2N,		// 16 bit word normalized to (value/32767.0,value/32767.0,0,0,1)
	VET_Half2,			// 16 bit float using 1 bit sign, 5 bit exponent, 10 bit mantissa 
	VET_Half4,
	VET_Short4N,		// 4 X 16 bit word, normalized 
	VET_UShort2,
	VET_UShort4,
	VET_UShort2N,		// 16 bit word normalized to (value/65535.0,value/65535.0,0,0,1)
	VET_UShort4N,		// 4 X 16 bit word unsigned, normalized 
	VET_URGB10A2N,		// 10 bit r, g, b and 2 bit a normalized to (value/1023.0f, value/1023.0f, value/1023.0f, value/3.0f)
	VET_UInt,
	VET_MAX,

	VET_NumBits = 5,
}; 