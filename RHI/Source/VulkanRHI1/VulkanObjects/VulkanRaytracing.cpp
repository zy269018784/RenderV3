#include <VulkanRHI1/VulkanObjects/VulkanRaytracing.h>
#include <VulkanRHI1/VulkanObjects/VulkanDevice.h>
//#include <RHIRaytracing.h>
#include <VulkanRHI1/VulkanLoader.h>
#include <VulkanRHI1/VulkanObjects/VulkanBuffer.h>
#include <VulkanRHI1/VulkanObjects/VulkanCommandBuffer.h>
#include <iostream>

const bool GVulkanRayTracingTLASPreferFastTrace = true;

static VkBuildAccelerationStructureFlagsKHR toVkASFlags(ERayTracingASFlags BuildFlags)
{
	VkBuildAccelerationStructureFlagsKHR flags = (EnumHasAnyFlags(BuildFlags, ERayTracingASFlags::FastBuild)) ? VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR
		: VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;

	if (EnumHasAnyFlags(BuildFlags, ERayTracingASFlags::AllowUpdate)) 
		flags |= VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR; 

	if (EnumHasAnyFlags(BuildFlags, ERayTracingASFlags::AllowCompaction)) 
		flags |= VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR; 

	return flags;
}

static VkTransformMatrixKHR toVkTransform(glm::mat4 matrix) {
	VkTransformMatrixKHR transformMatrix;
	transformMatrix.matrix[0][0] = matrix[0][0];
	transformMatrix.matrix[0][1] = matrix[0][1];
	transformMatrix.matrix[0][2] = matrix[0][2];
	transformMatrix.matrix[0][3] = matrix[0][3];

	transformMatrix.matrix[1][0] = matrix[1][0];
	transformMatrix.matrix[1][1] = matrix[1][1];
	transformMatrix.matrix[1][2] = matrix[1][2];
	transformMatrix.matrix[1][3] = matrix[1][3];

	transformMatrix.matrix[2][0] = matrix[2][0];
	transformMatrix.matrix[2][1] = matrix[2][1];
	transformMatrix.matrix[2][2] = matrix[2][2];
	transformMatrix.matrix[2][3] = matrix[2][3];
	return transformMatrix;
}

///////////////////////////////////////VulkanRaytracingBLAS
VulkanRaytracingBLAS::VulkanRaytracingBLAS(VulkanDevice* InDevice, RHIRaytracingGeometryData* geoData)
{
	Device = InDevice;
	asBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	asBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

	asBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	if (geoData) {
		GetBuildData(geoData);
	}
	else {
		printf("have no geodata to build blas\n");
		exit(-1);
	} 
}

static uint64_t RoundUp(uint64_t numToRound, uint64_t multiple)
{
	return ((numToRound + multiple - 1) / multiple) * multiple;
}

VulkanRaytracingBLAS::VulkanRaytracingBLAS(VulkanDevice* InDevice, RHIRaytracingGeometryDataSimple* geoData)
{
	Device = InDevice; 
 
	//for()
	{
		RHIRaytracingASGeometry* pGeometry = geoData->pASGeometry;

		VkAccelerationStructureGeometryKHR asGeometry{};
		asGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		asGeometry.flags = VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR;//VK_GEOMETRY_OPAQUE_BIT_KHR;
		asGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
		asGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR; 
		asGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
		asGeometry.geometry.triangles.vertexData.deviceAddress = pGeometry->vertexAddress;
		asGeometry.geometry.triangles.maxVertex = pGeometry->MaxVertices;
		asGeometry.geometry.triangles.vertexStride = pGeometry->VertexBufferStride;
		asGeometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
		asGeometry.geometry.triangles.indexData.deviceAddress = pGeometry->indexAddress;
		asGeometry.geometry.triangles.transformData.deviceAddress = pGeometry->transformAddress;
	 
		VkAccelerationStructureBuildRangeInfoKHR asBuildRangeInfo{};
		asBuildRangeInfo.primitiveCount = pGeometry->indexCount / 3;
		asBuildRangeInfo.primitiveOffset = pGeometry->IndexBufferOffset;
		//printf("IndexBufferOffset %u, VertexBufferOffset %u\n", pGeometry->IndexBufferOffset, pGeometry->VertexBufferOffset);
		asBuildRangeInfo.firstVertex = pGeometry->VertexBufferOffset / pGeometry->VertexBufferStride;
		asBuildRangeInfo.transformOffset = 0;

		asGeometries.push_back(asGeometry);
		asBuildRangeInfos.push_back(asBuildRangeInfo);
	}
	 
	// 获取建立加速结构所需的内存大小 
	asBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	asBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	asBuildGeometryInfo.flags =  VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;//VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR;
	asBuildGeometryInfo.geometryCount = asGeometries.size();
	asBuildGeometryInfo.pGeometries = asGeometries.data();

	std::vector<uint32_t> MaxPrimitiveCounts;
	MaxPrimitiveCounts.reserve(asBuildRangeInfos.size());
	for (int i = 0; i < asBuildRangeInfos.size(); i++) {
		MaxPrimitiveCounts[i] = asBuildRangeInfos[i].primitiveCount;
	} 


	asBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	Device->GetAccelerationStructureBuildSizes(&asBuildGeometryInfo, MaxPrimitiveCounts.data(), &asBuildSizesInfo);

	SizeInfo.ResultSize = RoundUp(asBuildSizesInfo.accelerationStructureSize,256); //需要按256的倍数对齐 
	//printf("asBuildSizesInfo.accelerationStructureSize %u\n", asBuildSizesInfo.accelerationStructureSize);
	SizeInfo.BuildScratchSize = RoundUp(asBuildSizesInfo.buildScratchSize,Device->PhysicalDevice->GetAccelerationStructureProperties().minAccelerationStructureScratchOffsetAlignment);
	SizeInfo.UpdateScratchSize = asBuildSizesInfo.updateScratchSize;
 	//printf("%d", int(Device->PhysicalDevice->accelerationStructureProperties.minAccelerationStructureScratchOffsetAlignment)); //128
}

VulkanRaytracingBLAS::~VulkanRaytracingBLAS()
{ 
	if (asBuffer) { 
		delete asBuffer;
	} 

	if (Device)
		Device->DestroyAccelerationStructureKHR(Handle, nullptr);
}

void VulkanRaytracingBLAS::create()
{  
	CreateASBuffer(); 
	BuildAS(); 
}

void VulkanRaytracingBLAS::build()
{
	//创建加速结构
	asBuffer = new VulkanBuffer(Device, 0, asBuildSizesInfo.accelerationStructureSize, BUF_AccelerationStructure, nullptr);
	VkAccelerationStructureCreateInfoKHR asInfo{};
	asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	asInfo.buffer = asBuffer->Handle;
	asInfo.size = asBuildSizesInfo.accelerationStructureSize;
	asInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	Device->CreateAccelerationStructure(asInfo, &Handle);

	//创建临时缓冲区，建立加速结构
	VulkanBuffer* rtScratchBuffer = new VulkanBuffer(Device, 0, asBuildSizesInfo.buildScratchSize, BUF_RayTracingScratch, nullptr);
	asBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	asBuildGeometryInfo.dstAccelerationStructure = Handle;
	asBuildGeometryInfo.scratchData.deviceAddress = rtScratchBuffer->DeviceAddress;
	const VkAccelerationStructureBuildRangeInfoKHR* buildOffsetInfo = asBuildRangeInfos.data();
	VkCommandBuffer cmdBuffer = BeginCommand();
	VKRT::vkCmdBuildAccelerationStructuresKHR(cmdBuffer, 1, &asBuildGeometryInfo, &buildOffsetInfo);
	EndCommand(cmdBuffer);

	//获取加速结构地址
	VkAccelerationStructureDeviceAddressInfoKHR asDeviceAddressInfo{};
	asDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	asDeviceAddressInfo.accelerationStructure = Handle;
	DeviceAddress = Device->GetAccelerationStructureDeviceAddress(&asDeviceAddressInfo);
	delete rtScratchBuffer; 
}

void VulkanRaytracingBLAS::build(RHIBuffer* asBuffer, uint64_t offset, RHIBuffer* scratchBuffer, uint64_t scratchOffset)
{ 
	VulkanBuffer* vkAsBuffer = dynamic_cast<VulkanBuffer*>(asBuffer);
	VulkanBuffer* vkScratchBuffer = dynamic_cast<VulkanBuffer*>(scratchBuffer);

	//创建加速结构
	VkAccelerationStructureCreateInfoKHR asInfo = {};
	asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	asInfo.pNext = nullptr;
	asInfo.type = asBuildGeometryInfo.type;
	asInfo.size = asBuildSizesInfo.accelerationStructureSize;
	asInfo.buffer = vkAsBuffer->Handle;
	asInfo.offset = offset;
	//std::cout << "asInfo.offset " << offset << "asBuildSizesInfo.accelerationStructureSize " << asBuildSizesInfo.accelerationStructureSize << std::endl;
 
	if (VK_SUCCESS != Device->CreateAccelerationStructure(asInfo, &Handle)) {
		printf("CreateAccelerationStructure failed\n");
		exit(-1);
	}
	//printf("CreateAccelerationStructure ok\n");
	//创建临时缓冲区，建立加速结构
	asBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	asBuildGeometryInfo.dstAccelerationStructure = Handle;
	asBuildGeometryInfo.scratchData.deviceAddress = vkScratchBuffer->DeviceAddress + scratchOffset;
	const VkAccelerationStructureBuildRangeInfoKHR* buildOffsetInfo = asBuildRangeInfos.data();
	VkCommandBuffer cmdBuffer = BeginCommand();
	VKRT::vkCmdBuildAccelerationStructuresKHR(cmdBuffer, 1, &asBuildGeometryInfo, &buildOffsetInfo);
	EndCommand(cmdBuffer);
	 
	//获取加速结构地址
	VkAccelerationStructureDeviceAddressInfoKHR asDeviceAddressInfo{};
	asDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	asDeviceAddressInfo.accelerationStructure = Handle;
	DeviceAddress = Device->GetAccelerationStructureDeviceAddress(&asDeviceAddressInfo); 
}

void VulkanRaytracingBLAS::createAS(RHIBuffer* asBuffer, uint64_t offset)
{
	//printf("CreateAccelerationStructure \n");
	VulkanBuffer* vkAsBuffer = dynamic_cast<VulkanBuffer*>(asBuffer);

	//创建加速结构
	VkAccelerationStructureCreateInfoKHR asInfo = {};
	asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	asInfo.pNext = nullptr;
	asInfo.type = asBuildGeometryInfo.type;
	asInfo.size = asBuildSizesInfo.accelerationStructureSize;
	asInfo.buffer = vkAsBuffer->Handle;
	asInfo.offset = offset;
	//std::cout << "asInfo.offset " << offset << "asBuildSizesInfo.accelerationStructureSize " << asBuildSizesInfo.accelerationStructureSize << std::endl; 
	if (VK_SUCCESS != Device->CreateAccelerationStructure(asInfo, &Handle)) {
		printf("CreateAccelerationStructure failed\n");
		exit(-1);
	}
	//printf("CreateAccelerationStructure ok 1\n");
}

void VulkanRaytracingBLAS::buildAS(RHICommandBuffer* cmdBuffer, RHIBuffer* scratchBuffer, uint64_t scratchOffset)
{
	if (!cmdBuffer) {
		printf("cmdBuffer none\n");
	}
	if (!scratchBuffer) {
		printf("scratchBuffer none\n");
	}
	printf("scratchOffset %d\n", scratchOffset);
	VulkanBuffer* vkScratchBuffer = dynamic_cast<VulkanBuffer*>(scratchBuffer);
	VulkanCommandBuffer* vkCmdBuffer = dynamic_cast<VulkanCommandBuffer*>(cmdBuffer);
 
	asBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	asBuildGeometryInfo.dstAccelerationStructure = Handle;
	asBuildGeometryInfo.scratchData.deviceAddress = vkScratchBuffer->DeviceAddress + scratchOffset;
	const VkAccelerationStructureBuildRangeInfoKHR* buildOffsetInfo = asBuildRangeInfos.data();
	if(vkCmdBuffer && vkCmdBuffer->GetHandle())
		VKRT::vkCmdBuildAccelerationStructuresKHR(vkCmdBuffer->GetHandle(), 1, &asBuildGeometryInfo, &buildOffsetInfo);

}

void VulkanRaytracingBLAS::build2(RHICommandBuffer* cmdBuffer, RHIBuffer* asBuffer, uint64_t offset, RHIBuffer* scratchBuffer, uint64_t scratchOffset)
{
	//printf("CreateAccelerationStructure ok -1\n");
	VulkanBuffer* vkAsBuffer = dynamic_cast<VulkanBuffer*>(asBuffer);
	VulkanBuffer* vkScratchBuffer = dynamic_cast<VulkanBuffer*>(scratchBuffer);
	VulkanCommandBuffer* vkCmdBuffer= dynamic_cast<VulkanCommandBuffer*>(cmdBuffer);
	//创建加速结构
	VkAccelerationStructureCreateInfoKHR asInfo = {};
	asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	asInfo.pNext = nullptr;
	asInfo.type = asBuildGeometryInfo.type;
	asInfo.size = asBuildSizesInfo.accelerationStructureSize;
	asInfo.buffer = vkAsBuffer->Handle;
	asInfo.offset = offset;
	//std::cout << "asInfo.offset " << offset << "asBuildSizesInfo.accelerationStructureSize " << asBuildSizesInfo.accelerationStructureSize << std::endl;
	if (VK_SUCCESS != Device->CreateAccelerationStructure(asInfo, &Handle)) {
		printf("CreateAccelerationStructure failed\n");
		exit(-1);
	}
	//printf("CreateAccelerationStructure ok 1\n");
	//创建临时缓冲区，建立加速结构
	asBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	asBuildGeometryInfo.dstAccelerationStructure = Handle;
	asBuildGeometryInfo.scratchData.deviceAddress = vkScratchBuffer->DeviceAddress + scratchOffset;
	const VkAccelerationStructureBuildRangeInfoKHR* buildOffsetInfo = asBuildRangeInfos.data(); 
	VKRT::vkCmdBuildAccelerationStructuresKHR(vkCmdBuffer->GetHandle(), 1, &asBuildGeometryInfo, &buildOffsetInfo);
	//printf("CreateAccelerationStructure ok 2\n");
}

void VulkanRaytracingBLAS::build3(VkCommandBuffer cmdBuffer, RHIBuffer* asBuffer, uint64_t offset, RHIBuffer* scratchBuffer, uint64_t scratchOffset)
{ 	
	VulkanBuffer* vkAsBuffer = dynamic_cast<VulkanBuffer*>(asBuffer); 
	VulkanBuffer* vkScratchBuffer = dynamic_cast<VulkanBuffer*>(scratchBuffer);  

	//创建加速结构
	VkAccelerationStructureCreateInfoKHR asInfo = {};
	asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	asInfo.pNext = nullptr;
	asInfo.type = asBuildGeometryInfo.type;
	asInfo.size = asBuildSizesInfo.accelerationStructureSize;
	asInfo.buffer = vkAsBuffer->Handle;
	asInfo.offset = offset; 
	if (VK_SUCCESS != Device->CreateAccelerationStructure(asInfo, &Handle)) {
		printf("CreateAccelerationStructure failed\n");
		exit(-1);
	}

	asBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	asBuildGeometryInfo.dstAccelerationStructure = Handle;
	asBuildGeometryInfo.scratchData.deviceAddress = vkScratchBuffer->DeviceAddress + scratchOffset;
	const VkAccelerationStructureBuildRangeInfoKHR* buildOffsetInfo = asBuildRangeInfos.data();
	VKRT::vkCmdBuildAccelerationStructuresKHR(cmdBuffer, 1, &asBuildGeometryInfo, &buildOffsetInfo);
}

void VulkanRaytracingBLAS::GetASDeviceAddress() //获取加速结构地址
{ 
	VkAccelerationStructureDeviceAddressInfoKHR asDeviceAddressInfo{};
	asDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	asDeviceAddressInfo.accelerationStructure = Handle;
	DeviceAddress = Device->GetAccelerationStructureDeviceAddress(&asDeviceAddressInfo);
}

void VulkanRaytracingBLAS::GetBuildData(RHIRaytracingGeometryData* geoData)
{ 
	uint32_t instanceCount = static_cast<uint32_t>(geoData->ASGeometries.size());
	std::vector<uint32_t> maxPrimitiveCounts(instanceCount, 0); 
	for (uint32_t i = 0; i < instanceCount; ++i) {
		RHIRaytracingASGeometry* geo = geoData->ASGeometries[i];
		  
		VkAccelerationStructureGeometryKHR asGeometry{};
		asGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		asGeometry.flags = VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR;//VK_GEOMETRY_OPAQUE_BIT_KHR;//   anyhit透明起作用让三角形只命中一次
		asGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
		asGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
		asGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
		asGeometry.geometry.triangles.vertexData.deviceAddress = geo->vertexAddress;
		asGeometry.geometry.triangles.maxVertex = geo->MaxVertices;// 1;
		asGeometry.geometry.triangles.vertexStride = geo->VertexBufferStride;
		asGeometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
		asGeometry.geometry.triangles.indexData.deviceAddress = geo->indexAddress;
		asGeometry.geometry.triangles.transformData.deviceAddress = 0;
		asGeometry.geometry.triangles.transformData.hostAddress = nullptr;
		asGeometry.geometry.triangles.transformData.deviceAddress = geo->transformAddress;

		maxPrimitiveCounts[i] = geo->indexCount / 3;

		VkAccelerationStructureBuildRangeInfoKHR asBuildRangeInfo = {};
		asBuildRangeInfo.primitiveCount = maxPrimitiveCounts[i];
		asBuildRangeInfo.primitiveOffset = 0;
		asBuildRangeInfo.firstVertex = 0;
		asBuildRangeInfo.transformOffset = 0;
		asGeometries.push_back(asGeometry);
		asBuildRangeInfos.push_back(asBuildRangeInfo);
	}

	//VkAccelerationStructureBuildGeometryInfoKHR asBuildGeometryInfo{};
	asBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	asBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	asBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	asBuildGeometryInfo.geometryCount = asGeometries.size();
	asBuildGeometryInfo.pGeometries = asGeometries.data(); 

	//VkAccelerationStructureBuildSizesInfoKHR asBuildSizesInfo{};
	asBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	//获取内存大小
	Device->GetAccelerationStructureBuildSizes(&asBuildGeometryInfo, maxPrimitiveCounts.data(), &asBuildSizesInfo);
 
	//创建加速结构
	asBuffer = new VulkanBuffer(Device, 0, asBuildSizesInfo.accelerationStructureSize, BUF_AccelerationStructure, nullptr);
	VkAccelerationStructureCreateInfoKHR asInfo{};
	asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	asInfo.buffer = asBuffer->Handle;
	asInfo.size = asBuildSizesInfo.accelerationStructureSize;
	asInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	Device->CreateAccelerationStructure(asInfo, &Handle);
 
	//创建临时缓冲区，建立加速结构
	VulkanBuffer* rtScratchBuffer = new VulkanBuffer(Device, 0, asBuildSizesInfo.buildScratchSize, BUF_RayTracingScratch, nullptr);  
	asBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	asBuildGeometryInfo.dstAccelerationStructure = Handle;
	asBuildGeometryInfo.scratchData.deviceAddress = rtScratchBuffer->DeviceAddress;
	const VkAccelerationStructureBuildRangeInfoKHR* buildOffsetInfo = asBuildRangeInfos.data();
	VkCommandBuffer cmdBuffer = BeginCommand();
	VKRT::vkCmdBuildAccelerationStructuresKHR(cmdBuffer, 1, &asBuildGeometryInfo, &buildOffsetInfo);
	EndCommand(cmdBuffer); 

	//获取加速结构地址
	VkAccelerationStructureDeviceAddressInfoKHR asDeviceAddressInfo{};
	asDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	asDeviceAddressInfo.accelerationStructure = Handle;
	DeviceAddress = Device->GetAccelerationStructureDeviceAddress(&asDeviceAddressInfo);
	delete rtScratchBuffer; 
}

void VulkanRaytracingBLAS::CreateASBuffer()
{
	asBuffer = new VulkanBuffer(Device, 0, asBuildSizesInfo.accelerationStructureSize, BUF_AccelerationStructure, nullptr);
	 
	VkAccelerationStructureCreateInfoKHR asInfo{};
	asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	asInfo.buffer = asBuffer->Handle;
	asInfo.size = asBuildSizesInfo.accelerationStructureSize;
	asInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;  
	Device->CreateAccelerationStructure(asInfo, &Handle);
}

VkCommandBuffer VulkanRaytracingBLAS::BeginCommand()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = Device->m_CommandPool; 
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer cmdBuffer = VK_NULL_HANDLE;
	Device->AllocateCommandBuffers(&allocInfo, &cmdBuffer);
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(cmdBuffer, &beginInfo);
	return cmdBuffer;
}

void VulkanRaytracingBLAS::EndCommand(VkCommandBuffer cmdBuffer)
{
	vkEndCommandBuffer(cmdBuffer);
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer;
	vkQueueSubmit(Device->m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(Device->m_graphicsQueue);
	Device->FreeCommandBuffers(Device->m_CommandPool, 1, &cmdBuffer);
}

void VulkanRaytracingBLAS::BuildAS()
{
	VkCommandBuffer cmdBuffer = BeginCommand();
	//VKRT::vkCmdBuildAccelerationStructuresKHR(cmdBuffer, 1, &GeometryInfo, Ranges.data());
	EndCommand(cmdBuffer);

	VkAccelerationStructureDeviceAddressInfoKHR asDeviceAddressInfo{};
	asDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	asDeviceAddressInfo.accelerationStructure = Handle; 
	DeviceAddress = Device->GetAccelerationStructureDeviceAddress(&asDeviceAddressInfo);
}

static void GetTLASBuildData( const VkDevice Device, const uint32_t NumInstances, const VkDeviceAddress InstanceBufferAddress, TLASBuildData& buildData)
{
	VkDeviceOrHostAddressConstKHR InstanceBufferDeviceAddress = {};
	InstanceBufferDeviceAddress.deviceAddress = InstanceBufferAddress;

	buildData.Geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
	buildData.Geometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
	buildData.Geometry.geometry.instances.arrayOfPointers = VK_FALSE;
	buildData.Geometry.geometry.instances.data = InstanceBufferDeviceAddress;

	buildData.GeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	buildData.GeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	buildData.GeometryInfo.flags = GVulkanRayTracingTLASPreferFastTrace ? VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR : VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR;
	buildData.GeometryInfo.geometryCount = 1;
	buildData.GeometryInfo.pGeometries = &buildData.Geometry;

	VKRT::vkGetAccelerationStructureBuildSizesKHR( Device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &buildData.GeometryInfo, &NumInstances, &buildData.SizesInfo);
}

///////////////////////////////////////VulkanRayTracingTLAS
VulkanRaytracingTLAS::VulkanRaytracingTLAS(VulkanDevice* InDevice, uint64_t deviceAddress)
	: Device(InDevice)
{ 
	std::vector<VkAccelerationStructureInstanceKHR> instanceData; //多个实例

	uint32_t instanceCount = 1; 	
	uint32_t primitiveCount = instanceCount;
#if 1
	VkTransformMatrixKHR transformMatrix = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f 
	}; 
#else
	VkTransformMatrixKHR transformMatrix = {
		1.0f, 0.0f, 0.0f, -2.906799281277017E-7,
		0.0f, 1.0f, 0.0f, -1.010655638086928E1,
		0.0f, 0.0f, 1.0f, -2.1322210941103084E1
	};
#endif
	VkAccelerationStructureInstanceKHR instance{};
	instance.transform = transformMatrix;
	instance.instanceCustomIndex = 0;
	instance.mask = 0xFF;
	instance.instanceShaderBindingTableRecordOffset = 0;
	instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
	instance.accelerationStructureReference = deviceAddress;// bottomLevelAS.deviceAddress;
	 
	InstancesBuffer = new VulkanBuffer(Device, 0, sizeof(VkAccelerationStructureInstanceKHR), BUF_AccelerationStructure, &instance);// Buffer for instance data
	  
	VkDeviceOrHostAddressConstKHR instanceDataDeviceAddress{};
	instanceDataDeviceAddress.deviceAddress = InstancesBuffer->DeviceAddress;

	VkAccelerationStructureGeometryKHR asGeometry{};
	asGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	asGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
	asGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;//VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR;// 
	asGeometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
	asGeometry.geometry.instances.arrayOfPointers = VK_FALSE;
	asGeometry.geometry.instances.data = instanceDataDeviceAddress;
	 
	VkAccelerationStructureBuildGeometryInfoKHR asBuildGeometryInfo{};
	asBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	asBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	asBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	asBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	asBuildGeometryInfo.geometryCount = 1;
	asBuildGeometryInfo.pGeometries = &asGeometry;
	 
	VkAccelerationStructureBuildSizesInfoKHR asBuildSizesInfo{};
	asBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	Device->GetAccelerationStructureBuildSizes(&asBuildGeometryInfo, &primitiveCount, &asBuildSizesInfo);
	 
	asBuffer = new VulkanBuffer(Device, 0, asBuildSizesInfo.accelerationStructureSize, BUF_AccelerationStructure, nullptr);

	VkAccelerationStructureCreateInfoKHR asInfo{};
	asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	asInfo.buffer = asBuffer->Handle;
	asInfo.size = asBuildSizesInfo.accelerationStructureSize;
	asInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	Device->CreateAccelerationStructure(asInfo, &Handle);
	  
	VulkanBuffer* ScratchBuffer = new VulkanBuffer(Device, 0, asBuildSizesInfo.buildScratchSize, BUF_RayTracingScratch, nullptr); 
  
	asBuildGeometryInfo.dstAccelerationStructure = Handle; 
	asBuildGeometryInfo.scratchData.deviceAddress = ScratchBuffer->DeviceAddress;

	VkAccelerationStructureBuildRangeInfoKHR asBuildRangeInfo = { primitiveCount, 0, 0, 0 }; 

	std::vector<VkAccelerationStructureBuildRangeInfoKHR*> accelerationBuildStructureRangeInfos = { &asBuildRangeInfo };
 
	VkCommandBuffer cmdBuffer = BeginCommand();
	VKRT::vkCmdBuildAccelerationStructuresKHR( cmdBuffer, 1, &asBuildGeometryInfo, accelerationBuildStructureRangeInfos.data());
	EndCommand(cmdBuffer);

	VkAccelerationStructureDeviceAddressInfoKHR asDeviceAddressInfo{};
	asDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	asDeviceAddressInfo.accelerationStructure = Handle;
	DeviceAddress = Device->GetAccelerationStructureDeviceAddress(&asDeviceAddressInfo);

	delete ScratchBuffer;
}

VulkanRaytracingTLAS::VulkanRaytracingTLAS(VulkanDevice* InDevice, std::vector< RHIRaytracingBLAS* > BLASs)
{
	uint32_t instanceCount = BLASs.size();
	std::vector<VkAccelerationStructureInstanceKHR> geometryInstances; 
	geometryInstances.reserve(instanceCount);
	for (auto instanceId = 0; instanceId < instanceCount; ++instanceId) {
		geometryInstances.push_back( CreateInstance(*dynamic_cast<VulkanRaytracingBLAS*>(BLASs[instanceId]), glm::mat4(1), instanceId));
	}

	//创建所有实例缓冲区
	const auto size = sizeof(geometryInstances[0]) * geometryInstances.size();
	InstancesBuffer = new VulkanBuffer(Device, 0, size, BUF_AccelerationStructure, geometryInstances.data());// Buffer for instance data
 
	VkDeviceOrHostAddressConstKHR instanceDataDeviceAddress{};
	instanceDataDeviceAddress.deviceAddress = InstancesBuffer->DeviceAddress; 
	 
	VkAccelerationStructureGeometryKHR asGeometry{};
	asGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	asGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
	asGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;//VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR;// 
	asGeometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
	asGeometry.geometry.instances.arrayOfPointers = VK_FALSE;
	asGeometry.geometry.instances.data = instanceDataDeviceAddress;

	VkAccelerationStructureBuildGeometryInfoKHR asBuildGeometryInfo{};
	asBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	asBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	asBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	asBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	asBuildGeometryInfo.geometryCount = 1;
	asBuildGeometryInfo.pGeometries = &asGeometry; 
	asBuildGeometryInfo.srcAccelerationStructure = nullptr;
	 
	VkAccelerationStructureBuildSizesInfoKHR asBuildSizesInfo{};
	asBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	//获取构建加速结构需要的内存大小
	Device->GetAccelerationStructureBuildSizes(&asBuildGeometryInfo, &instanceCount, &asBuildSizesInfo);
  
	//创建加速结构
	asBuffer = new VulkanBuffer(Device, 0, asBuildSizesInfo.accelerationStructureSize, BUF_AccelerationStructure, nullptr);
	VkAccelerationStructureCreateInfoKHR asInfo{};
	asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	asInfo.buffer = asBuffer->Handle;
	asInfo.size = asBuildSizesInfo.accelerationStructureSize;
	asInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	Device->CreateAccelerationStructure(asInfo, &Handle);

	//创建临时缓冲区，建立加速结构
	VulkanBuffer* ScratchBuffer = new VulkanBuffer(Device, 0, asBuildSizesInfo.buildScratchSize, BUF_RayTracingScratch, nullptr);  
	asBuildGeometryInfo.dstAccelerationStructure = Handle;
	asBuildGeometryInfo.scratchData.deviceAddress = ScratchBuffer->DeviceAddress;
	VkAccelerationStructureBuildRangeInfoKHR asBuildRangeInfo = { instanceCount, 0, 0, 0 };
	std::vector<VkAccelerationStructureBuildRangeInfoKHR*> accelerationBuildStructureRangeInfos = { &asBuildRangeInfo };
	VkCommandBuffer cmdBuffer = BeginCommand();
	VKRT::vkCmdBuildAccelerationStructuresKHR(cmdBuffer, 1, &asBuildGeometryInfo, accelerationBuildStructureRangeInfos.data());
	EndCommand(cmdBuffer);

	//获取加速结构地址
	VkAccelerationStructureDeviceAddressInfoKHR asDeviceAddressInfo{};
	asDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	asDeviceAddressInfo.accelerationStructure = Handle;
	DeviceAddress = Device->GetAccelerationStructureDeviceAddress(&asDeviceAddressInfo);

	delete ScratchBuffer; 
}

VulkanRaytracingTLAS::VulkanRaytracingTLAS(VulkanDevice* InDevice, std::vector< RHIRaytracingBLAS* > BLASs, std::vector< glm::mat4 > transforms)
	: Device(InDevice)
{ 
	uint32_t instanceCount = BLASs.size(); 
	//创建所有实例缓冲区
	std::vector<VkAccelerationStructureInstanceKHR> geometryInstances; 
	for (auto instanceId = 0; instanceId < instanceCount; ++instanceId) {	
		VkAccelerationStructureInstanceKHR asInstance = {}; 
		asInstance.transform = toVkTransform(transforms[instanceId]);
		asInstance.instanceCustomIndex = instanceId;
		asInstance.mask = 0xFF;
		asInstance.instanceShaderBindingTableRecordOffset = 0;
		asInstance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
		asInstance.accelerationStructureReference = BLASs[instanceId]->GetDeviceAddress(); 
		geometryInstances.push_back(asInstance);
	} 
	
	uint32_t size = sizeof(VkAccelerationStructureInstanceKHR) * geometryInstances.size();
	InstancesBuffer = new VulkanBuffer(Device, 0, size, BUF_AccelerationStructure, geometryInstances.data());// Buffer for instance data

	//获取构建加速结构需要的内存大小
	VkAccelerationStructureGeometryKHR asGeometry{};
	asGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	asGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
	asGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;//VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR;// 
	asGeometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
	asGeometry.geometry.instances.arrayOfPointers = VK_FALSE;
	asGeometry.geometry.instances.data.deviceAddress = InstancesBuffer->DeviceAddress;

	VkAccelerationStructureBuildGeometryInfoKHR asBuildGeometryInfo{};
	asBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	asBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	asBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	asBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	asBuildGeometryInfo.geometryCount = 1;
	asBuildGeometryInfo.pGeometries = &asGeometry;
	asBuildGeometryInfo.srcAccelerationStructure = nullptr;

	VkAccelerationStructureBuildSizesInfoKHR asBuildSizesInfo{};
	asBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR; 
	Device->GetAccelerationStructureBuildSizes(&asBuildGeometryInfo, &instanceCount, &asBuildSizesInfo);

	//创建加速结构
	asBuffer = new VulkanBuffer(Device, 0, asBuildSizesInfo.accelerationStructureSize, BUF_AccelerationStructure, nullptr);
	VkAccelerationStructureCreateInfoKHR asInfo{};
	asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	asInfo.buffer = asBuffer->Handle;
	asInfo.size = asBuildSizesInfo.accelerationStructureSize;
	asInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	Device->CreateAccelerationStructure(asInfo, &Handle);

	//创建临时缓冲区，建立加速结构
	VulkanBuffer* ScratchBuffer = new VulkanBuffer(Device, 0, asBuildSizesInfo.buildScratchSize, BUF_RayTracingScratch, nullptr);
	asBuildGeometryInfo.dstAccelerationStructure = Handle;
	asBuildGeometryInfo.scratchData.deviceAddress = ScratchBuffer->DeviceAddress;
	VkAccelerationStructureBuildRangeInfoKHR asBuildRangeInfo = { instanceCount, 0, 0, 0 };
	std::vector<VkAccelerationStructureBuildRangeInfoKHR*> accelerationBuildStructureRangeInfos = { &asBuildRangeInfo };
	VkCommandBuffer cmdBuffer = BeginCommand();
	VKRT::vkCmdBuildAccelerationStructuresKHR(cmdBuffer, 1, &asBuildGeometryInfo, accelerationBuildStructureRangeInfos.data());
	EndCommand(cmdBuffer);

	//获取加速结构地址
	VkAccelerationStructureDeviceAddressInfoKHR asDeviceAddressInfo{};
	asDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	asDeviceAddressInfo.accelerationStructure = Handle;
	DeviceAddress = Device->GetAccelerationStructureDeviceAddress(&asDeviceAddressInfo);
	delete ScratchBuffer; 
}

VulkanRaytracingTLAS::~VulkanRaytracingTLAS()
{
	delete asBuffer;
	delete InstancesBuffer;
	delete InstanceBuffer;
	if (Device != nullptr)
		Device->DestroyAccelerationStructureKHR(Handle, nullptr);
}

VkAccelerationStructureInstanceKHR VulkanRaytracingTLAS::CreateInstance(const VulkanRaytracingBLAS& blas, const glm::mat4& transform, uint32_t instanceId)
{ 
	VkAccelerationStructureInstanceKHR geometryInstance = {};
	geometryInstance.transform = toVkTransform(transform);
	geometryInstance.instanceCustomIndex = instanceId;
	geometryInstance.mask = 0xFF;
	geometryInstance.instanceShaderBindingTableRecordOffset = 0;
	geometryInstance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
	geometryInstance.accelerationStructureReference = blas.DeviceAddress; 
	return geometryInstance;
}

VkCommandBuffer VulkanRaytracingTLAS::BeginCommand()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = Device->m_CommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer cmdBuffer = VK_NULL_HANDLE;
	Device->AllocateCommandBuffers(&allocInfo, &cmdBuffer);
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(cmdBuffer, &beginInfo);
	return cmdBuffer;
}

void VulkanRaytracingTLAS::EndCommand(VkCommandBuffer cmdBuffer)
{
	vkEndCommandBuffer(cmdBuffer);
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer;
	vkQueueSubmit(Device->m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(Device->m_graphicsQueue);
	Device->FreeCommandBuffers(Device->m_CommandPool, 1, &cmdBuffer);
}

void VulkanRaytracingTLAS::createInstanceBuffer(uint64_t blasDeviceAddress)
{
	VkTransformMatrixKHR transformMatrix = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f }; 
	VkAccelerationStructureInstanceKHR instance{};
	instance.transform = transformMatrix;
	instance.instanceCustomIndex = 0;
	instance.mask = 0xFF;
	instance.instanceShaderBindingTableRecordOffset = 0;
	instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
	instance.accelerationStructureReference = blasDeviceAddress; 
	InstanceBuffer = new VulkanBuffer(Device, 0, sizeof(VkAccelerationStructureInstanceKHR), BUF_AccelerationStructure, &instance);

	// Buffer for instance data
	//vks::Buffer instancesBuffer;
	//vulkanDevice->createBuffer(
	//	VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
	//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	//	&instancesBuffer,
	//	sizeof(VkAccelerationStructureInstanceKHR),
	//	&instance));

	//VkDeviceOrHostAddressConstKHR instanceDataDeviceAddress{};
	//instanceDataDeviceAddress.deviceAddress = getBufferDeviceAddress(instancesBuffer.buffer);
}

void VulkanRaytracingTLAS::createScratchBuffer()
{

}

#if 0
void VulkanRaytracingTLAS::createASBuffer(AccelerationStructure& accelerationStructure, VkAccelerationStructureTypeKHR type, VkAccelerationStructureBuildSizesInfoKHR buildSizeInfo)
{
	// Buffer and memory
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = buildSizeInfo.accelerationStructureSize;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	vkCreateBuffer(Device->Handle, &bufferCreateInfo, nullptr, &accelerationStructure.buffer);

	VkMemoryRequirements memoryRequirements{};
	vkGetBufferMemoryRequirements(Device->Handle, accelerationStructure.buffer, &memoryRequirements);
	VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo{};
	memoryAllocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
	memoryAllocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;

	VkMemoryAllocateInfo memoryAllocateInfo{};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = &memoryAllocateFlagsInfo;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = Device->getMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vkAllocateMemory(Device->Handle, &memoryAllocateInfo, nullptr, &accelerationStructure.memory);
	vkBindBufferMemory(Device->Handle, accelerationStructure.buffer, accelerationStructure.memory, 0);

	// Acceleration structure
	VkAccelerationStructureCreateInfoKHR accelerationStructureCreate_info{};
	accelerationStructureCreate_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	accelerationStructureCreate_info.buffer = accelerationStructure.buffer;
	accelerationStructureCreate_info.size = buildSizeInfo.accelerationStructureSize;
	accelerationStructureCreate_info.type = type;
	vkCreateAccelerationStructureKHR(Device->Handle, &accelerationStructureCreate_info, nullptr, &accelerationStructure.handle);

	// AS device address
	VkAccelerationStructureDeviceAddressInfoKHR accelerationDeviceAddressInfo{};
	accelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	accelerationDeviceAddressInfo.accelerationStructure = accelerationStructure.handle;
	accelerationStructure.deviceAddress = vkGetAccelerationStructureDeviceAddressKHR(Device->Handle, &accelerationDeviceAddressInfo);
}
#endif

void VulkanRaytracingTLAS::create()
{
#if 0
	//1.create Instance Buffer
	uint64_t blasDeviceAddress = 0;//todo...
	createInstanceBuffer(blasDeviceAddress); 
	 
	//GetSize
	GetTLASBuildData(Device->Handle, 1, InstanceBuffer->DeviceAddress, BuildData); 

	//2.create AccelerationStructure Buffer
	createASBuffer();

	//3.create Scratch Buffer
	createScratchBuffer();

	BuildData.GeometryInfo.dstAccelerationStructure = topLevelAS.handle;
	BuildData.GeometryInfo.scratchData.deviceAddress = ScratchBuffer->DeviceAddress;

	//VkDeviceOrHostAddressConstKHR instanceDataDeviceAddress{};
	//instanceDataDeviceAddress.deviceAddress = InstanceBuffer->DeviceAddress;

	//ASGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR; 
	//ASGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
	//ASGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
	//ASGeometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
	//ASGeometry.geometry.instances.arrayOfPointers = VK_FALSE;
	//ASGeometry.geometry.instances.data = instanceDataDeviceAddress;
	// 
	//// Get size info
	//VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo = {};
	//accelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	//accelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	//accelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	//accelerationStructureBuildGeometryInfo.geometryCount = 1;
	//accelerationStructureBuildGeometryInfo.pGeometries = &ASGeometry;

	//uint32_t primitive_count = 1;  
	//ASBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	//vkGetAccelerationStructureBuildSizesKHR(Device->Handle, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &accelerationStructureBuildGeometryInfo, &primitive_count, &ASBuildSizesInfo);

	//2.createASBuffer
	createAS();

	//3.createScratchBuffer

	ASBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	ASBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	ASBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	ASBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR; 
	ASBuildGeometryInfo.geometryCount = 1;
	ASBuildGeometryInfo.pGeometries = &ASGeometry;	
	ASBuildGeometryInfo.dstAccelerationStructure = topLevelAS.handle;
	ASBuildGeometryInfo.scratchData.deviceAddress = scratchBuffer.deviceAddress;

	VkAccelerationStructureBuildRangeInfoKHR accelerationStructureBuildRangeInfo{};
	accelerationStructureBuildRangeInfo.primitiveCount = 1;  //可以多层，但基本固定1层
	accelerationStructureBuildRangeInfo.primitiveOffset = 0;
	accelerationStructureBuildRangeInfo.firstVertex = 0;
	accelerationStructureBuildRangeInfo.transformOffset = 0;
	std::vector<VkAccelerationStructureBuildRangeInfoKHR*> accelerationBuildStructureRangeInfos = { &accelerationStructureBuildRangeInfo };

	// Build the acceleration structure on the device via a one-time command buffer submission
	// Some implementations may support acceleration structure building on the host (VkPhysicalDeviceAccelerationStructureFeaturesKHR->accelerationStructureHostCommands), but we prefer device builds
	// 在GPU构造加速结构
	VkCommandBuffer commandBuffer = vulkanDevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
	VKRT::vkCmdBuildAccelerationStructuresKHR( commandBuffer, 1, &ASBuildGeometryInfo, accelerationBuildStructureRangeInfos.data());
	vulkanDevice->flushCommandBuffer(commandBuffer, queue);
#endif
}

/////////////////////////////////////////////////////////VulkanShaderBindingTables

VulkanShaderBindingTables::VulkanShaderBindingTables(VulkanDevice* InDevice, VulkanRaytracingPipeline* pipeline)
	: Device(InDevice)
{
	printf("VulkanShaderBindingTables\n");
	rayTracingPipelineProperties = Device->PhysicalDevice->GetRayTracingPipelineProperties();
	const uint32_t handleSize = rayTracingPipelineProperties.shaderGroupHandleSize;
	const uint32_t handleSizeAligned = alignedSize(rayTracingPipelineProperties.shaderGroupHandleSize, rayTracingPipelineProperties.shaderGroupHandleAlignment);
	const uint32_t groupCount = static_cast<uint32_t>(pipeline->shaderGroups.size());
	printf("groupCount %d\n", groupCount);
	const uint32_t sbtSize = groupCount * handleSizeAligned;
	std::vector<uint8_t> shaderHandleStorage(sbtSize);

	Device->GetRayTracingShaderGroupHandles(pipeline->Handle, groupCount, sbtSize, shaderHandleStorage.data());

	raygenSBT = new VulkanBuffer(Device, 0, handleSize, BUF_ShaderBindingTable, shaderHandleStorage.data());
	missSBT = new VulkanBuffer(Device, 0, handleSize * pipeline->missTableCount, BUF_ShaderBindingTable, shaderHandleStorage.data() + handleSizeAligned);
	hitSBT = new VulkanBuffer(Device, 0, handleSize * pipeline->hitgroupTableCount, BUF_ShaderBindingTable, shaderHandleStorage.data() + handleSizeAligned * (pipeline->missTableCount+1));

	raygenSbtEntry_.deviceAddress = raygenSBT->DeviceAddress;
	raygenSbtEntry_.stride = handleSizeAligned;
	raygenSbtEntry_.size = handleSizeAligned;

	raymissSbtEntry_.deviceAddress = missSBT->DeviceAddress;
	raymissSbtEntry_.stride = handleSizeAligned;
	raymissSbtEntry_.size = handleSizeAligned * pipeline->missTableCount;

	rayhitSbtEntry_.deviceAddress = hitSBT->DeviceAddress;
	rayhitSbtEntry_.stride = handleSizeAligned;
	rayhitSbtEntry_.size = handleSizeAligned * pipeline->hitgroupTableCount;

	callableSbtEntry_ = {};
}

VulkanShaderBindingTables::~VulkanShaderBindingTables()
{
	if (raygenSBT) {
		delete raygenSBT;
	}
	if (missSBT) {
		delete missSBT;
	}
	if (hitSBT) {
		delete hitSBT;
	} 
}