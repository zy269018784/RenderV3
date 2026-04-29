#pragma once
#include <RHIResource.h>
#include <RHIDefinitions.h>


class RHIShader : public RHIResource
{
public:
	RHIShader() = delete;
	RHIShader(RHIResourceType InResourceType, RHIShaderStage InShaderType)
		: RHIResource(InResourceType), ShaderType(InShaderType)
	{
	}
	inline RHIShaderStage GetShaderType() const
	{
		return ShaderType;
	}
private:
	RHIShaderStage ShaderType;
};

class RHIGraphicsShader : public RHIShader
{
public:
	explicit RHIGraphicsShader(RHIResourceType InResourceType, RHIShaderStage InShaderType)
		: RHIShader(InResourceType, InShaderType) {}
}; 

class RHIVertexShader : public RHIGraphicsShader
{
public:
	RHIVertexShader() : RHIGraphicsShader(RRT_VertexShader, SS_Vertex) {}
};

class RHIPixelShader : public RHIGraphicsShader
{
public:
	RHIPixelShader() : RHIGraphicsShader(RRT_PixelShader, SS_Pixel) {}
};

class RHIGeometryShader : public RHIGraphicsShader
{
public:
	RHIGeometryShader() : RHIGraphicsShader(RRT_GeometryShader, SS_Geometry) {}
};

class RHIComputeShader : public RHIGraphicsShader
{
public:
	RHIComputeShader() : RHIGraphicsShader(RRT_GeometryShader, SS_Compute) {}
};


class RHIRayTracingShader : public RHIShader
{
public:
	explicit RHIRayTracingShader(RHIShaderStage InShaderType) : RHIShader(RRT_RayTracingShader, InShaderType) {}

	uint32_t RayTracingPayloadType = 0; // This corresponds to the ERayTracingPayloadType enum associated with the shader
	uint32_t RayTracingPayloadSize = 0; // The (maximum) size of the payload associated with this shader
};

class RHIRayGenShader : public RHIRayTracingShader
{
public:
	RHIRayGenShader() : RHIRayTracingShader(SS_RayGen) {}
};

class RHIRayMissShader : public RHIRayTracingShader
{
public:
	RHIRayMissShader() : RHIRayTracingShader(SS_RayMiss) {}
};

class RHIRayCallableShader : public RHIRayTracingShader
{
public:
	RHIRayCallableShader() : RHIRayTracingShader(SS_RayCallable) {}
};

class RHIRayHitGroupShader : public RHIRayTracingShader
{
public:
	RHIRayHitGroupShader() : RHIRayTracingShader(SS_RayHitGroup) {}
};