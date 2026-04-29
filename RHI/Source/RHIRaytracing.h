#pragma once
#include <RHIResource.h>
#include <stdint.h>
#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class RHICommandBuffer;

enum class ERayTracingInstanceFlags : uint8_t
{
	None = 0,
	TriangleCullDisable = 1 << 1, // No back face culling. Triangle is visible from both sides.
	TriangleCullReverse = 1 << 2, // Makes triangle front-facing if its vertices are counterclockwise from ray origin.
	ForceOpaque = 1 << 3, // Disable any-hit shader invocation for this instance.
	ForceNonOpaque = 1 << 4, // Force any-hit shader invocation even if geometries inside the instance were marked opaque.
};
ENUM_CLASS_FLAGS(ERayTracingInstanceFlags);

struct RayTracingAccelerationStructureSize
{
	uint64_t ResultSize = 0;
	uint64_t BuildScratchSize = 0;
	uint64_t UpdateScratchSize = 0;
};

enum class ERayTracingASFlags
{
	None = 0,
	AllowUpdate = 1 << 0,
	AllowCompaction = 1 << 1,
	FastTrace = 1 << 2,
	FastBuild = 1 << 3,
	MinimizeMemory = 1 << 4,
};
ENUM_CLASS_FLAGS(ERayTracingASFlags);


//AccelerationStructure
class RHIRayTracingAccelerationStructure : public RHIResource
#if ENABLE_RHI_VALIDATION
	, public RHIValidation::FAccelerationStructureResource
#endif
{
public:
	RHIRayTracingAccelerationStructure() : RHIResource(RRT_RayTracingAS) {}

	RayTracingAccelerationStructureSize GetSizeInfo() const {
		return SizeInfo;
	};
protected:
	RayTracingAccelerationStructureSize SizeInfo = {};
};


using FRayTracingAccelerationStructureAddress = uint64_t;

class RHIRayTracingGeometry;
  
enum ERayTracingGeometryType {
	// Indexed or non-indexed triangle list with fixed function ray intersection.
	// Vertex buffer must contain vertex positions as VET_Float3.
	// Vertex stride must be at least 12 bytes, but may be larger to support custom per-vertex data.
	// Index buffer may be provided for indexed triangle lists. Implicit triangle list is assumed otherwise.
	RTGT_Triangles, 
	// Custom primitive type that requires an intersection shader.
	// Vertex buffer for procedural geometry must contain one AABB per primitive as {float3 MinXYZ, float3 MaxXYZ}.
	// Vertex stride must be at least 24 bytes, but may be larger to support custom per-primitive data.
	// Index buffers can't be used with procedural geometry.
	RTGT_Procedural,
};

enum class ERayTracingGeometryInitializerType
{ 
	Rendering,// Fully initializes the RayTracingGeometry object: creates underlying buffer and initializes shader parameters. 
	StreamingDestination,// Does not create underlying buffer or shader parameters. Used by the streaming system as an object that is streamed into.  
	StreamingSource,// Creates buffers but does not create shader parameters. Used for intermediate objects in the streaming system.
};

struct RayTracingGeometrySegment
{
public:
	RHIBuffer* VertexBuffer = nullptr;
	//EVertexElementType VertexBufferElementType = VET_Float3; 
	uint32_t VertexBufferOffset = 0;// Offset in bytes from the base address of the vertex buffer.

	// Number of bytes between elements of the vertex buffer (sizeof VET_Float3 by default). Must be equal or greater than the size of the position vector.
	uint32_t VertexBufferStride = 12;

	// Number of vertices (positions) in VertexBuffer. If an index buffer is present, this must be at least the maximum index value in the index buffer + 1.
	uint32_t MaxVertices = 0;

	// Primitive range for this segment.
	uint32_t FirstPrimitive = 0;
	uint32_t NumPrimitives = 0;

	// Indicates whether any-hit shader could be invoked when hitting this geometry segment. Setting this to `false` turns off any-hit shaders, making the section "opaque" and improving ray tracing performance.
	bool bForceOpaque = false;

	// Any-hit shader may be invoked multiple times for the same primitive during ray traversal.
	// Setting this to `false` guarantees that only a single instance of any-hit shader will run per primitive, at some performance cost.
	bool bAllowDuplicateAnyHitShaderInvocation = true; 
	bool bEnabled = true;// Indicates whether this section is enabled and should be taken into account during acceleration structure creation
};

struct BLASDesc
{
public:
	RHIBuffer* IndexBuffer = nullptr; 
	uint32_t IndexBufferOffset = 0;		// Offset in bytes from the base address of the index buffer. 
	ERayTracingGeometryType GeometryType = RTGT_Triangles; 
	uint32_t TotalPrimitiveCount = 0;	// Total number of primitives in all segments of the geometry. Only used for validation. 
	std::vector<RayTracingGeometrySegment> Segments;	// Partitions of geometry to allow different shader and resource bindings. All ray tracing geometries must have at least one segment. 
	//FResourceArrayInterface* OfflineData = nullptr;	// Offline built geometry data. If null, the geometry will be built by the RHI at runtime. 
	RHIRayTracingGeometry* SourceGeometry = nullptr;	// Pointer to an existing ray tracing geometry which the new geometry is built from.

	bool bFastBuild = false;
	bool bAllowUpdate = false;
	bool bAllowCompaction = true;
	ERayTracingGeometryInitializerType Type = ERayTracingGeometryInitializerType::Rendering; 
	//FDebugName DebugName;	// Use FDebugName for auto-generated debug names with numbered suffixes, it is a variation of FMemoryImageName with optional number postfix. 
	//FName OwnerName;		// Store the path name of the owner object for resource tracking. FMemoryImageName allows a conversion to/from FName.
};

struct RHIRaytracingASGeometry
{
public:
	RHIBuffer* VertexBuffer = nullptr;
	EVertexElementType VertexBufferElementType = VET_Float3;

	// Offset in bytes from the base address of the vertex buffer.
	uint32_t VertexBufferOffset = 0;

	uint32_t IndexBufferOffset = 0;

	// Number of bytes between elements of the vertex buffer (sizeof VET_Float3 by default).
	// Must be equal or greater than the size of the position vector.
	uint32_t VertexBufferStride = 12;

	// Number of vertices (positions) in VertexBuffer.
	// If an index buffer is present, this must be at least the maximum index value in the index buffer + 1.
	uint32_t MaxVertices = 0;

	// Primitive range for this segment.
	uint32_t FirstPrimitive = 0;
	uint32_t NumPrimitives = 0;

	// Indicates whether any-hit shader could be invoked when hitting this geometry segment.
	// Setting this to `false` turns off any-hit shaders, making the section "opaque" and improving ray tracing performance.
	bool bForceOpaque = false;

	// Any-hit shader may be invoked multiple times for the same primitive during ray traversal.
	// Setting this to `false` guarantees that only a single instance of any-hit shader will run per primitive, at some performance cost.
	bool bAllowDuplicateAnyHitShaderInvocation = true;

	// Indicates whether this section is enabled and should be taken into account during acceleration structure creation
	bool bEnabled = true;

	uint64_t vertexAddress = 0;
	uint64_t indexAddress = 0;
	uint64_t transformAddress = 0;
	 
	uint32_t indexCount = 0;

};


struct RHIRaytracingGeometryData {
	ERayTracingASFlags BuildFlags;
	ERayTracingGeometryType GeoType;

	glm::mat4 matrix;
	RHIBuffer* IndexBuffer = nullptr;
	RHIBuffer* VertexBuffer = nullptr;

	std::vector<RHIRaytracingASGeometry*> ASGeometries;
};

struct RHIRaytracingGeometryDataSimple {
	ERayTracingASFlags BuildFlags;
	ERayTracingGeometryType GeoType; 
	RHIRaytracingASGeometry* pASGeometry;
};

/** Bottom level ray tracing acceleration structure (contains triangles). Geometry*/ 
//BLAS
class RHIRaytracingBLAS : public RHIRayTracingAccelerationStructure
{
public:
	RHIRaytracingBLAS() = default;
	RHIRaytracingBLAS(const BLASDesc& desc) : desc_(desc) , InitializedType(desc_.Type) {}
	virtual ~RHIRaytracingBLAS() {}

	virtual uint64_t GetDeviceAddress() = 0;
	//virtual FRayTracingAccelerationStructureAddress GetAccelerationStructureAddress(uint64_t GPUIndex) const = 0;
	//virtual void SetInitializer(const FRayTracingGeometryInitializer& Initializer) = 0;
	//virtual bool IsCompressed() const { return false; }

	//const FRayTracingGeometryInitializer& GetInitializer() const
	//{
	//	return Initializer;
	//}

	//uint32_t GetNumSegments() const
	//{
	//	return Initializer.Segments.Num();
	//}
	virtual void create() = 0;
	virtual void build() = 0;
	virtual void build(RHIBuffer* asBuffer, uint64_t offset, RHIBuffer* scratchBuffer, uint64_t scratchOffset) = 0;
	virtual void createAS(RHIBuffer* asBuffer, uint64_t offset) = 0;
	virtual void buildAS(RHICommandBuffer* cmdBuffer, RHIBuffer* scratchBuffer, uint64_t scratchOffset) = 0;
	virtual void build2(RHICommandBuffer* cmdBuffer, RHIBuffer* asBuffer, uint64_t offset, RHIBuffer* scratchBuffer, uint64_t scratchOffset) = 0;
	virtual void GetASDeviceAddress() = 0;
protected:
	BLASDesc desc_ = {}; 
	ERayTracingGeometryInitializerType InitializedType = ERayTracingGeometryInitializerType::Rendering;
};


/** Top level ray tracing acceleration structure (contains instances of meshes). Scene*/ 
class RHIRaytracingTLAS : public RHIRayTracingAccelerationStructure
{
public:
	virtual ~RHIRaytracingTLAS() {};
	//virtual const FRayTracingSceneInitializer2& GetInitializer() const = 0;

	//// Returns a buffer view for RHI-specific system parameters associated with this scene.
	//// This may be needed to access ray tracing geometry data in shaders that use ray queries.
	//// Returns NULL if current RHI does not require this buffer.
	//virtual FRHIShaderResourceView* GetOrCreateMetadataBufferSRV(FRHICommandListImmediate& RHICmdList)
	//{
	//	return nullptr;
	//}

	virtual uint32_t GetLayerBufferOffset(uint32_t LayerIndex) const = 0;

	virtual void create() = 0;
};


class RHIShaderBindingTable
{
public:

};

class RHIShaderBindingTables
{
public:	
	RHIShaderBindingTables() = default;
	virtual ~RHIShaderBindingTables() {}
	virtual void create() = 0;

	//virtual void destroy() = 0;
};

#if 0//todo...
class RHIRaytracer
{
public:
	RHIRaytracer(GlfwRender* scene);
};

#endif