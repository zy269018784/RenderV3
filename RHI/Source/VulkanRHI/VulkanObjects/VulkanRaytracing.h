#pragma once
#include <RHIPipeline.h>
#include <RHIRaytracing.h>

#include <VulkanRHI/Vulkan.h>
#include <VulkanRHI/VulkanObjects/VulkanPipeline.h> 

class VulkanBuffer;
class VulkanDevice;
class VulkanCommandBuffer;
class RHICommandBuffer;
//struct RHIRaytracingGeometryData;
//class RHIRaytracingBLAS;
//class RHIRaytracingTLAS;
//class RHIShaderBindingTable;

class VulkanRaytracingBLAS : public RHIRaytracingBLAS
{
public:
	VulkanRaytracingBLAS(VulkanDevice* InDevice, RHIRaytracingGeometryData* geoData);
	VulkanRaytracingBLAS(VulkanDevice* InDevice, RHIRaytracingGeometryDataSimple* geoData);
	~VulkanRaytracingBLAS();

	virtual uint64_t GetDeviceAddress() {
		return DeviceAddress;
	}
	virtual void create(); 
	virtual void build();
	virtual void build(RHIBuffer* asBuffer,uint64_t offset, RHIBuffer* scratchBuffer,uint64_t scratchOffset);
	virtual void createAS(RHIBuffer* asBuffer, uint64_t offset);
	virtual void buildAS(RHICommandBuffer* cmdBuffer, RHIBuffer* scratchBuffer, uint64_t scratchOffset);
	virtual void build2(RHICommandBuffer* cmdBuffer, RHIBuffer* asBuffer, uint64_t offset, RHIBuffer* scratchBuffer, uint64_t scratchOffset);
	void build3(VkCommandBuffer cmdBuffer, RHIBuffer* asBuffer, uint64_t offset, RHIBuffer* scratchBuffer, uint64_t scratchOffset);
	virtual void GetASDeviceAddress();

	void GetBuildData(RHIRaytracingGeometryData* geoData);
	void CreateASBuffer();
	void BuildAS();

	VkCommandBuffer BeginCommand();
	void EndCommand(VkCommandBuffer commandBuffer);

public:
	VulkanBuffer* asBuffer = nullptr;
	VkAccelerationStructureKHR Handle = VK_NULL_HANDLE;
	VkDeviceAddress DeviceAddress = 0;

	//BuildData
	std::vector< VkAccelerationStructureGeometryKHR > asGeometries; 
	std::vector<VkAccelerationStructureBuildRangeInfoKHR> asBuildRangeInfos; 
	VkAccelerationStructureBuildGeometryInfoKHR asBuildGeometryInfo{};
	VkAccelerationStructureBuildSizesInfoKHR asBuildSizesInfo{};

	VulkanDevice* Device = nullptr;
};


struct TLASBuildData
{
	TLASBuildData() {
		Geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		GeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		SizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR; 
	}

	VkAccelerationStructureGeometryKHR Geometry = {};
	VkAccelerationStructureBuildGeometryInfoKHR GeometryInfo = {};
	VkAccelerationStructureBuildSizesInfoKHR SizesInfo = {};
};

struct AccelerationStructure {
	VkAccelerationStructureKHR handle;
	uint64_t deviceAddress = 0;
	VkDeviceMemory memory;
	VkBuffer buffer;
};

//TLAS
class VulkanRaytracingTLAS : public RHIRaytracingTLAS
{
public:
	VulkanRaytracingTLAS(VulkanDevice* InDevice, uint64_t DeviceAddress);
	VulkanRaytracingTLAS(VulkanDevice* InDevice, std::vector< RHIRaytracingBLAS* > BLASs);
	VulkanRaytracingTLAS(VulkanDevice* InDevice, std::vector< RHIRaytracingBLAS* > BLASs, std::vector< glm::mat4 > transforms);
	~VulkanRaytracingTLAS();
	virtual uint32_t GetLayerBufferOffset(uint32_t LayerIndex) const {
		return 0;
	}

	virtual void create();
	// Ray tracing shader bindings can be processed in parallel.
	// Each concurrent worker gets its own dedicated descriptor cache instance to avoid contention or locking.
	// Scaling beyond 5 total threads does not yield any speedup in practice (RHI thread + 4 parallel workers).
	static constexpr uint32_t MaxBindingWorkers = 1; // :todo-jn:

	/*static */VkAccelerationStructureInstanceKHR CreateInstance( const VulkanRaytracingBLAS& blas, const glm::mat4& transform, uint32_t instanceId);

	VkCommandBuffer BeginCommand();
	void EndCommand(VkCommandBuffer cmdBuffer);

public:
	VulkanBuffer* asBuffer = nullptr;
	VkAccelerationStructureKHR Handle;
	VulkanDevice* Device = nullptr;
	VkDeviceAddress DeviceAddress = 0;
#if 0
	const FRayTracingSceneInitializer2& GetInitializer() const override final { return Initializer; }
	uint32_t GetLayerBufferOffset(uint32_t LayerIndex) const override final { return Layers[LayerIndex].BufferOffset; }

	void BindBuffer(FRHIBuffer* InBuffer, uint32_t InBufferOffset);
	void BuildAccelerationStructure(
		FVulkanCommandListContext& CommandContext,
		FVulkanResourceMultiBuffer* ScratchBuffer, uint32_t ScratchOffset,
		FVulkanResourceMultiBuffer* InstanceBuffer, uint32_t InstanceOffset);

	virtual FRHIShaderResourceView* GetOrCreateMetadataBufferSRV(FRHICommandListImmediate& RHICmdList) override final
	{
		if (!PerInstanceGeometryParameterSRV.IsValid())
		{
			PerInstanceGeometryParameterSRV = RHICmdList.CreateShaderResourceView(PerInstanceGeometryParameterBuffer, FRHIViewDesc::CreateBufferSRV().SetType(FRHIViewDesc::EBufferType::Structured));
		}

		return PerInstanceGeometryParameterSRV.GetReference();
	}

	VulkanRayTracingShaderTable* FindOrCreateShaderTable(const VulkanRayTracingPipeline* Pipeline);

	inline uint32_t GetHitRecordBaseIndex(uint32_t InstanceIndex, uint32_t SegmentIndex) const
	{
		return (Initializer.SegmentPrefixSum[InstanceIndex] + SegmentIndex) * Initializer.ShaderSlotsPerGeometrySegment;
	}
#endif

	inline bool IsBuilt() const
	{
		return bBuilt;
	}

public: 
	VulkanBuffer* ScratchBuffer = nullptr;
	VulkanBuffer* InstanceBuffer = nullptr;
	VulkanBuffer* InstancesBuffer = nullptr;
	TLASBuildData BuildData;

	VkAccelerationStructureGeometryKHR ASGeometry{};
	VkAccelerationStructureBuildGeometryInfoKHR ASBuildGeometryInfo{};
	VkAccelerationStructureBuildSizesInfoKHR ASBuildSizesInfo{};

private:
	void createInstanceBuffer(uint64_t blasDeviceAddress);
	//void createASBuffer();
	void createScratchBuffer();
	 
	//const FRayTracingSceneInitializer2 Initializer;

	//// Native TLAS handles are owned by SRV objects in Vulkan RHI.
	//// D3D12 and other RHIs allow creating TLAS SRVs from any GPU address at any point
	//// and do not require them for operations such as build or update.
	//// FVulkanRayTracingScene can't own the VkAccelerationStructureKHR directly because
	//// we allow TLAS memory to be allocated using transient resource allocator and 
	//// the lifetime of the scene object may be different from the lifetime of the buffer.
	//// Many VkAccelerationStructureKHR-s may be created, pointing at the same buffer.

	//struct FLayerData
	//{
	//	TUniquePtr<FVulkanView> View;
	//	uint32 BufferOffset;
	//	uint32 ScratchBufferOffset;
	//};

	//TArray<FLayerData> Layers;

	//TRefCountPtr<FVulkanResourceMultiBuffer> AccelerationStructureBuffer;

	//// Buffer that contains per-instance index and vertex buffer binding data
	//TRefCountPtr<FVulkanResourceMultiBuffer> PerInstanceGeometryParameterBuffer;
	//FShaderResourceViewRHIRef PerInstanceGeometryParameterSRV;

	//TMap<const FVulkanRayTracingPipelineState*, FVulkanRayTracingShaderTable*> ShaderTables;

	//void BuildPerInstanceGeometryParameterBuffer(FVulkanCommandListContext& CommandContext);

	bool bBuilt = false;
};


class VulkanRayTracingShaderTable : public RHIShaderBindingTable
{
public:
	VulkanRayTracingShaderTable(VulkanDevice* InDevice)
		: Device(InDevice)
	{

	}
	~VulkanRayTracingShaderTable()
	{

	}

	//void Init(const VulkanRayTracingScene* Scene, const VulkanRayTracingPipeline* Pipeline)
	//{

	//}

public:
	VulkanDevice* Device = nullptr;
};

class VulkanShaderBindingTables : public RHIShaderBindingTables
{
public:
	VulkanShaderBindingTables(VulkanDevice* InDevice, VulkanRaytracingPipeline* pipeline);
	~VulkanShaderBindingTables();
	uint32_t alignedSize(uint32_t value, uint32_t alignment) {
		return (value + alignment - 1) & ~(alignment - 1);
	}

	virtual void create()
	{  
	}

public:
	VulkanBuffer* raygenSBT = nullptr;
	VulkanBuffer* missSBT = nullptr;
	VulkanBuffer* hitSBT = nullptr;

	VkStridedDeviceAddressRegionKHR raygenSbtEntry_;
	VkStridedDeviceAddressRegionKHR raymissSbtEntry_;
	VkStridedDeviceAddressRegionKHR rayhitSbtEntry_;
	VkStridedDeviceAddressRegionKHR callableSbtEntry_;

	VkPhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingPipelineProperties = {};
	VulkanDevice* Device = nullptr;
};