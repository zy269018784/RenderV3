 
#include "VulkanRHI1/Vulkan.h"
#include "VulkanRHI1/VulkanRHI.h"
#include "VulkanRHI1/VulkanObjects/VulkanDevice.h"
#include <VulkanRHI1/VulkanObjects/VulkanCommon.h>

#include <cstring>
#include <iostream>
#include <string>
#include <set>

void VulkanRHI1::GetInstanceLayersAndExtensions()
{
	const char* LayerName = nullptr;
	std::uint32_t PropertyCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &PropertyCount, nullptr);

	if (PropertyCount > 0)
	{
		VkExtensionProperties* pProperties = new VkExtensionProperties[PropertyCount];
		vkEnumerateInstanceExtensionProperties(nullptr, &PropertyCount, pProperties);

		for (int i = 0; i < PropertyCount; i++)
		{
			//qDebug()
			//	<< pProperties[i].extensionName
			//	<< pProperties[i].specVersion;
		}
	}

	std::uint32_t InstanceLayerCount = 0;
	vkEnumerateInstanceLayerProperties(&InstanceLayerCount, nullptr);
	VkLayerProperties* pLayerProperties;
	if (InstanceLayerCount > 0)
	{
		pLayerProperties = new VkLayerProperties[InstanceLayerCount];
		vkEnumerateInstanceLayerProperties(&InstanceLayerCount, pLayerProperties);

		for (int i = 0; i < InstanceLayerCount; i++)
		{
			//qDebug()
			//	<< pLayerProperties[i].layerName
			//	<< pLayerProperties[i].specVersion
			//	<< pLayerProperties[i].implementationVersion
			//	<< pLayerProperties[i].description;


			vkEnumerateInstanceExtensionProperties(pLayerProperties[i].layerName, &PropertyCount, nullptr);

			VkExtensionProperties* pProperties = new VkExtensionProperties[PropertyCount];
			vkEnumerateInstanceExtensionProperties(pLayerProperties[i].layerName, &PropertyCount, pProperties);
			for (int i = 0; i < PropertyCount; i++)
			{
				//qDebug()
				//	<< "\t"
				//	<< pProperties[i].extensionName
				//	<< pProperties[i].specVersion;
			}
		}
	}
}

bool VulkanRHI1::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr); 
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data()); 

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end()); 
	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

//VulkanRHI1::QueueFamilyIndices VulkanRHI1::findQueueFamilies(VkPhysicalDevice device)
//{
//	QueueFamilyIndices indices; 
//	uint32_t queueFamilyCount = 0;
//	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr); 
//	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
//	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
//
//	int i = 0;
//	for (const auto& queueFamily : queueFamilies) {
//		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
//			indices.graphicsFamily = i;
//		} 
//		if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
//			indices.computeFamily = i;
//		} 
//		VkBool32 presentSupport = false;
//		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface->Handle, &presentSupport); 
//		if (presentSupport) {
//			indices.presentFamily = i;
//		} 
//		if (indices.isComplete()) {
//			break;
//		} 
//		i++;
//	} 
//	return indices;
//}

bool VulkanRHI1::isDeviceSuitable(VkPhysicalDevice physicalDevice)
{
	//有窗口
	if (Surface) {
		vk::QueueFamilyIndices indices = vk::findQueueFamilies(physicalDevice, Surface->Handle);
		bool extensionsSupported = checkDeviceExtensionSupport(physicalDevice);
		bool swapChainAdequate = false;
		if (extensionsSupported) {
			vk::SwapChainSupportDetails swapChainSupport = vk::querySwapChainSupport(physicalDevice, Surface->Handle);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		} 
		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);
		return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}

	//无窗口
	vk::QueueFamilyIndices indices = vk::findQueueFamilies(physicalDevice, VK_NULL_HANDLE);
	return indices.isComplete(false); 
}

void VulkanRHI1::CreatePhysicalDevice(int Index)
{
	/*
		枚举物理设备
	*/
	uint32_t PhysicalDeviceCount = 0;
	Instance->EnumeratePhysicalDevices( &PhysicalDeviceCount, nullptr);
	std::cout << "PhysicalDeviceCount " << PhysicalDeviceCount << std::endl;

	std::vector<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
	Instance->EnumeratePhysicalDevices(&PhysicalDeviceCount, PhysicalDevices.data());

	for (int i = 0; i < PhysicalDeviceCount; i++)
	{
		VkPhysicalDeviceProperties Properties;
		vkGetPhysicalDeviceProperties(PhysicalDevices[i], &Properties);
		std::cout << "deviceName " << Properties.deviceName << " "
		          << "deviceType " << Properties.deviceType << std::endl;
	}

	VkPhysicalDevice PhysicalDeviceHandle = PhysicalDevices[Index];
	if (isDeviceSuitable(PhysicalDeviceHandle))
	{
		PhysicalDevice = new VulkanPhysicalDevice(PhysicalDeviceHandle);
		auto QueueFamilyIndices = vk::findQueueFamilies(PhysicalDeviceHandle, (Surface) ? Surface->Handle : VK_NULL_HANDLE);
		PhysicalDevice->SetQueueFamilyIndices(QueueFamilyIndices);
		PhysicalDevice->GetPhysicalDeviceProperties();
		printf("GPU: %s\n", PhysicalDevice->GetProperties().deviceName);
		printf("maxMemoryAllocationCount: %u\n", PhysicalDevice->GetProperties().limits.maxMemoryAllocationCount);
	}

	if (PhysicalDevice->GetHandle() == VK_NULL_HANDLE) 
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

void VulkanRHI1::CreatePhysicalDevice()
{
	uint32_t PhysicalDeviceCount = 0;  
	Instance->EnumeratePhysicalDevices(&PhysicalDeviceCount, nullptr);
	std::cout << "PhysicalDeviceCount " << PhysicalDeviceCount << std::endl;
	std::vector<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
	Instance->EnumeratePhysicalDevices(&PhysicalDeviceCount, PhysicalDevices.data());;

	for (const auto& PhysicalDeviceHandle : PhysicalDevices) 
	{
		if (isDeviceSuitable(PhysicalDeviceHandle)) 
		{
			PhysicalDevice = new VulkanPhysicalDevice(PhysicalDeviceHandle);
			auto QueueFamilyIndices = vk::findQueueFamilies(PhysicalDeviceHandle, (Surface) ? Surface->Handle : VK_NULL_HANDLE);
			PhysicalDevice->SetQueueFamilyIndices(QueueFamilyIndices);
			PhysicalDevice->GetPhysicalDeviceProperties();  
			printf("GPU: %s\n", PhysicalDevice->GetProperties().deviceName);
			printf("maxMemoryAllocationCount: %u\n", PhysicalDevice->GetProperties().limits.maxMemoryAllocationCount);
			break;
		}
	}

	if (PhysicalDevice->GetHandle() == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}



	//VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties = nullptr;
	//std::uint32_t PhysicalDeviceGroupCount = 0;
	//vkEnumeratePhysicalDeviceGroups(Instance->Handle, &PhysicalDeviceGroupCount, nullptr);
	//pPhysicalDeviceGroupProperties = new VkPhysicalDeviceGroupProperties[PhysicalDeviceGroupCount];
	//vkEnumeratePhysicalDeviceGroups(Instance->Handle, &PhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);

	//std::cout << "PhysicalDeviceGroupCount " << PhysicalDeviceGroupCount << std::endl;
	//for (int i = 0; i < PhysicalDeviceGroupCount; i++)
	//{
	//	std::cout << "VkPhysicalDeviceGroupProperties physicalDeviceCount "
	//		
	//		<< pPhysicalDeviceGroupProperties[i].physicalDeviceCount << " "
	//		<< "subsetAllocation" << " "
	//		<< pPhysicalDeviceGroupProperties[i].subsetAllocation
	//		
	//		<< std::endl;
	//}

	//VkPhysicalDevice* pPhysicalDevices = new VkPhysicalDevice[PhysicalDeviceCount];
	//vkEnumeratePhysicalDevices(Instance->Handle, &PhysicalDeviceCount, pPhysicalDevices);
	//for (int i = 0; i < PhysicalDeviceCount; i++)
	//{
	//	VkPhysicalDeviceProperties DeviceProperties{};
	//	vkGetPhysicalDeviceProperties(pPhysicalDevices[i], &DeviceProperties);
	//
	//	VkPhysicalDeviceFeatures deviceFeatures;
	//	vkGetPhysicalDeviceFeatures(pPhysicalDevices[i], &deviceFeatures);
	//
	//}
}

void VulkanRHI1::QueryAdvanceFeathers()
{

	PhysicalDevice->GetPhysicalDeviceFeatures2();
}

void VulkanRHI1::QueryAdvanceProperties()
{
	// Get ray tracing pipeline properties, which will be used later on in the sample 
	PhysicalDevice->GetPhysicalDeviceProperties2(); 
}

void VulkanRHI1::AddRaytracingExtensions()
{
	//光追 Ray tracing related extensions required by this sample
	deviceExtensions.push_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
	deviceExtensions.push_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);

	//加速结构 Required by VK_KHR_acceleration_structure
	deviceExtensions.push_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
	deviceExtensions.push_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
	deviceExtensions.push_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);

	deviceExtensions.push_back(VK_KHR_SPIRV_1_4_EXTENSION_NAME);// Required for VK_KHR_ray_tracing_pipeline 
	deviceExtensions.push_back(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);// Required by VK_KHR_spirv_1_4	

	deviceExtensions.push_back(VK_KHR_MAINTENANCE3_EXTENSION_NAME);
}

void VulkanRHI1::CreateDevice()
{
	//uint32_t proCount = 0;
	//std::vector<VkExtensionProperties> pros;
	//PhysicalDevice->EnumerateDeviceExtensionProperties(nullptr, &proCount, nullptr);
	//pros.resize(proCount);
	//PhysicalDevice->EnumerateDeviceExtensionProperties(nullptr, &proCount, pros.data());
	//std::cout << "vkEnumerateDeviceExtensionProperties" << std::endl;
	//for (int i = 0; i < pros.size(); i++)
	//{
	//	std::cout << pros[i].extensionName << std::endl;
	//}

	vk::QueueFamilyIndices indices = vk::findQueueFamilies(PhysicalDevice->GetHandle(), Surface->Handle);
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

#if 1 //Raytracing
	// Ray tracing related extensions required by this sample
	deviceExtensions.push_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
	deviceExtensions.push_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);

	// Required by VK_KHR_acceleration_structure
	deviceExtensions.push_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME); 
	deviceExtensions.push_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
	deviceExtensions.push_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);

	deviceExtensions.push_back(VK_KHR_SPIRV_1_4_EXTENSION_NAME);// Required for VK_KHR_ray_tracing_pipeline 
	deviceExtensions.push_back(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);// Required by VK_KHR_spirv_1_4	

	deviceExtensions.push_back(VK_KHR_MAINTENANCE3_EXTENSION_NAME); 



	QueryAdvanceProperties();
	QueryAdvanceFeathers();
#endif 

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkPhysicalDeviceBufferDeviceAddressFeatures enabledBufferDeviceAddresFeatures{};
	// Enable features required for ray tracing using feature chaining via pNext		
	enabledBufferDeviceAddresFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	enabledBufferDeviceAddresFeatures.bufferDeviceAddress = VK_TRUE;

	VkPhysicalDeviceDescriptorIndexingFeatures enabledDescriptorIndexingFeatures = {};
	enabledDescriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
	enabledDescriptorIndexingFeatures.pNext = &enabledBufferDeviceAddresFeatures;
	enabledDescriptorIndexingFeatures.runtimeDescriptorArray = true;
	enabledDescriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = true;

	VkPhysicalDeviceRayTracingPipelineFeaturesKHR enabledRayTracingPipelineFeatures{};
	enabledRayTracingPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
	enabledRayTracingPipelineFeatures.rayTracingPipeline = VK_TRUE;
	enabledRayTracingPipelineFeatures.pNext = &enabledDescriptorIndexingFeatures;

	VkPhysicalDeviceAccelerationStructureFeaturesKHR enabledAccelerationStructureFeatures{}; 
	enabledAccelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	enabledAccelerationStructureFeatures.accelerationStructure = VK_TRUE;
	enabledAccelerationStructureFeatures.pNext = &enabledRayTracingPipelineFeatures;

	//deviceCreatepNextChain = &enabledAccelerationStructureFeatures;

	deviceFeatures.shaderInt64 = true;  //shader 支持int64类型
	deviceFeatures.samplerAnisotropy = true; //开启采样器支持各向异性



	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data(); 
	createInfo.pEnabledFeatures = &deviceFeatures;
	 
	void* pNextChain = &enabledAccelerationStructureFeatures;// If a pNext(Chain) has been passed, we need to add it to the device creation info 

	VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{};
	if (pNextChain) {
		physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		physicalDeviceFeatures2.features = deviceFeatures;
		physicalDeviceFeatures2.pNext = pNextChain;
		createInfo.pEnabledFeatures = nullptr;
		createInfo.pNext = &physicalDeviceFeatures2; 
	}

	
	//for (const char* enabledExtension : deviceExtensions) { //检验是否支持
	//	if (!extensionSupported(enabledExtension)) {
	//		std::cerr << "Enabled device extension \"" << enabledExtension << "\" is not present at device level\n";
	//	}
	//}	
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	


#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}
	 
	VkDevice DeviceHandle = VK_NULL_HANDLE;
	if (PhysicalDevice->CreateDevice(&createInfo, nullptr, &DeviceHandle) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	Device = new VulkanDevice(DeviceHandle);
	if (!Device)
	{
		std::cout << "VulkanDevice failed" << std::endl;
	}


	GraphicsQueue.Init(Device, indices.graphicsFamily.value());
	PresentQueue.Init(Device, indices.presentFamily.value());
	ComputeQueue.Init(Device, indices.computeFamily.value());


	Device->PhysicalDevice = PhysicalDevice;
	Device->m_graphicsQueue = GraphicsQueue.Handle; 
	//VkPhysicalDeviceMemoryProperties memProperties;
	//vkGetPhysicalDeviceMemoryProperties(PhysicalDevice->Handle, &memProperties);
	//for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
	//	if (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
	//		Device.HostVisibleMemoryIndex = i;
	//	}
	//}

	VkPhysicalDeviceMemoryProperties memProperties;
	PhysicalDevice->GetPhysicalDeviceMemoryProperties(&memProperties);
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
		if (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
			if (1 == memProperties.memoryTypes[i].heapIndex)
			{
				Device->HostVisibleMemoryTypeIndexWithMaxHeapSize = i;
				break;
			}
			std::cout << "heapIndex " << memProperties.memoryTypes[i].heapIndex << std::endl;
			//break;
		} 
	}

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
		if (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
			Device->DeviceLocalMemoryTypeIndexWithMaxHeapSize = i;
			std::cout << "heapIndex " << memProperties.memoryTypes[i].heapIndex << std::endl;
			break;
		}
	}

	//std::cout << "Device.HostVisibleMemoryIndex " << Device.HostVisibleMemoryIndex << " heapIndex " << memProperties.memoryTypes[Device.HostVisibleMemoryIndex].heapIndex << std::endl;
}

void VulkanRHI1::CreateDeviceWithoutUI()
{
	deviceExtensions.clear();

	uint32_t proCount = 0;
	std::vector<VkExtensionProperties> pros;
	PhysicalDevice->EnumerateDeviceExtensionProperties(nullptr, &proCount, nullptr);
	pros.resize(proCount);
	PhysicalDevice->EnumerateDeviceExtensionProperties(nullptr, &proCount, pros.data());
	std::cout << "vkEnumerateDeviceExtensionProperties" << std::endl;
	for (int i = 0; i < pros.size(); i++)
	{
		std::cout << pros[i].extensionName << std::endl;
	}

	AddRaytracingExtensions(); //添加硬件光追扩展

	QueryAdvanceProperties();
	QueryAdvanceFeathers();
	 
	VkPhysicalDeviceBufferDeviceAddressFeatures enabledBufferDeviceAddresFeatures{};
	// Enable features required for ray tracing using feature chaining via pNext		
	enabledBufferDeviceAddresFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	enabledBufferDeviceAddresFeatures.bufferDeviceAddress = VK_TRUE;

	VkPhysicalDeviceDescriptorIndexingFeatures enabledDescriptorIndexingFeatures = {};
	enabledDescriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
	enabledDescriptorIndexingFeatures.pNext = &enabledBufferDeviceAddresFeatures;
	enabledDescriptorIndexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
	enabledDescriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;
	enabledDescriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
	 
	VkPhysicalDeviceRayTracingPipelineFeaturesKHR enabledRayTracingPipelineFeatures{};
	enabledRayTracingPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
	enabledRayTracingPipelineFeatures.rayTracingPipeline = VK_TRUE;
	enabledRayTracingPipelineFeatures.pNext = &enabledDescriptorIndexingFeatures;

	VkPhysicalDeviceAccelerationStructureFeaturesKHR enabledAccelerationStructureFeatures{};
	enabledAccelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	enabledAccelerationStructureFeatures.accelerationStructure = VK_TRUE;
	enabledAccelerationStructureFeatures.pNext = &enabledRayTracingPipelineFeatures;
	 
	void* pNextChain = &enabledAccelerationStructureFeatures;// If a pNext(Chain) has been passed, we need to add it to the device creation info 
	 
	vk::QueueFamilyIndices indices = PhysicalDevice->GetQueueFamilyIndices();

	//queueCreateInfo

	const float defaultQueuePriority(0.0f);
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos; 

	uint32_t queueFamilyCount;
	PhysicalDevice->GetPhysicalDeviceQueueFamilyProperties(&queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	PhysicalDevice->GetPhysicalDeviceQueueFamilyProperties(&queueFamilyCount, queueFamilyProperties.data());

	for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++) {
		if (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueFamilyIndex = i;
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = i;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &defaultQueuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
			break;
		}
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.shaderInt64 = true;  //shader 支持int64类型
	deviceFeatures.samplerAnisotropy = true; //开启采样器支持各向异性

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{};
	if (pNextChain) {
		physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		physicalDeviceFeatures2.features = deviceFeatures;
		physicalDeviceFeatures2.pNext = pNextChain;
		createInfo.pEnabledFeatures = nullptr;
		createInfo.pNext = &physicalDeviceFeatures2;
	}
	 
#ifdef NDEBUG
	createInfo.enabledLayerCount = 0; 
#else	
	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	createInfo.ppEnabledLayerNames = validationLayers.data(); 
#endif 

	VkDevice DeviceHandle = VK_NULL_HANDLE;
	if (PhysicalDevice->CreateDevice(&createInfo, nullptr, &DeviceHandle) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	Device = new VulkanDevice(DeviceHandle);
	if (!Device)
	{
		std::cout << "VulkanDevice failed" << std::endl;
	}

	GraphicsQueue.Init(Device, indices.graphicsFamily.value());
	//PresentQueue.Init(&Device, indices.presentFamily.value());
	ComputeQueue.Init(Device, indices.computeFamily.value());

	Device->PhysicalDevice = PhysicalDevice;
	Device->m_graphicsQueue = GraphicsQueue.Handle;

	VkPhysicalDeviceMemoryProperties memProperties;
	PhysicalDevice->GetPhysicalDeviceMemoryProperties(&memProperties);

	/*============================================================================================================*/
	for (uint32_t i = 0; i < memProperties.memoryHeapCount; ++i)
	{
		std::cout << "heap " << i << " size " << memProperties.memoryHeaps[i].size << std::endl;
	}

	/*
		支持Host Visible的Memory Type中, heap size最大的Memory Type.
	*/
	int HostVisibleMemoryTypeIndexWithMaxHeapSize = -1;
	/*
		支持Device Local的Memory Type中, heap size最大的Memory Type.
	*/
	int DeviceLocalMemoryTypeIndexWithMaxHeapSize = -1;
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
	{
		std::cout << "memory type " << i << " heap index " << memProperties.memoryTypes[i].heapIndex << " support ";
		if (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		{
			if (-1 == HostVisibleMemoryTypeIndexWithMaxHeapSize)
			{
				HostVisibleMemoryTypeIndexWithMaxHeapSize = i;
			}
			else 
			{
				VkDeviceSize CurSize = memProperties.memoryHeaps[memProperties.memoryTypes[i].heapIndex].size;
				VkDeviceSize MaxSize = memProperties.memoryHeaps[memProperties.memoryTypes[HostVisibleMemoryTypeIndexWithMaxHeapSize].heapIndex].size;
				if (CurSize > MaxSize)
					HostVisibleMemoryTypeIndexWithMaxHeapSize = i;
			}
			std::cout << " VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ";
		}
		if (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
		{
			std::cout << " VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ";

			if (-1 == DeviceLocalMemoryTypeIndexWithMaxHeapSize)
			{
				DeviceLocalMemoryTypeIndexWithMaxHeapSize = i;
			}
			else
			{
				VkDeviceSize CurSize = memProperties.memoryHeaps[memProperties.memoryTypes[i].heapIndex].size;
				VkDeviceSize MaxSize = memProperties.memoryHeaps[memProperties.memoryTypes[DeviceLocalMemoryTypeIndexWithMaxHeapSize].heapIndex].size;
				if (CurSize > MaxSize)
					DeviceLocalMemoryTypeIndexWithMaxHeapSize = i;
			}
		}
		std::cout << std::endl;
	}		
	std::cout 
			<< "  MaxSize "
			<< HostVisibleMemoryTypeIndexWithMaxHeapSize << " "
			<< memProperties.memoryHeaps[memProperties.memoryTypes[HostVisibleMemoryTypeIndexWithMaxHeapSize].heapIndex].size << " " << std::endl;
	/*============================================================================================================*/
	Device->HostVisibleMemoryTypeIndexWithMaxHeapSize = HostVisibleMemoryTypeIndexWithMaxHeapSize;
	Device->DeviceLocalMemoryTypeIndexWithMaxHeapSize = DeviceLocalMemoryTypeIndexWithMaxHeapSize;
	//for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
	//	if (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
	//		if (1 == memProperties.memoryTypes[i].heapIndex)
	//		{
	//			Device.HostVisibleMemoryTypeIndexWithMaxHeapSize = i;
	//			std::cout << "heapIndex " << memProperties.memoryTypes[i].heapIndex << std::endl;
	//			break;
	//		}
	//		
	//	}
	//}
	//for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
	//	if (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
	//		Device.DeviceLocalMemoryIndex = i;
	//		std::cout << "VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT heap Index " << memProperties.memoryTypes[i].heapIndex
	//			<< " memory type index " << i 
	//			<< std::endl;
	//		break;
	//	}
	//}
	//Device.DeviceLocalMemoryIndex = Device.DeviceLocalMemoryTypeIndexWithMaxHeapSize;
	std::cout << "Device.DeviceLocalMemoryTypeIndexWithMaxHeapSize " << Device->DeviceLocalMemoryTypeIndexWithMaxHeapSize 
		<< " heapIndex " << memProperties.memoryTypes[Device->DeviceLocalMemoryTypeIndexWithMaxHeapSize].heapIndex << std::endl;


}

void VulkanRHI1::CreateSyncObjects()
{
	for (size_t i = 0; i < ConcurrentFrameCount; i++) {
		VulkanSemaphore* pImageAvailableSemaphore = new VulkanSemaphore(Device);
		ImageAvailableSemaphores.push_back(pImageAvailableSemaphore);

		VulkanSemaphore* pRenderFinishedSemaphore = new VulkanSemaphore(Device);
		RenderFinishedSemaphores.push_back(pRenderFinishedSemaphore);

		std::cout << "pImageAvailableSemaphore " << pImageAvailableSemaphore->Handle << " pRenderFinishedSemaphore " << pRenderFinishedSemaphore->Handle << std::endl;

		VulkanFence* pInFlightFence = new VulkanFence(Device);
		InFlightFences.push_back(pInFlightFence);
	}

	Fence = new VulkanFence(Device);
}

void VulkanRHI1::ComputeShaderResource()
{
	ComputeCommandPool = new VulkanCommandPool(Device, PhysicalDevice->GetQueueFamilyIndices().computeFamily.value());
	ComputeCommandBuffer = new VulkanCommandBuffer(Device, ComputeCommandPool);
	ComputeFence = new VulkanFence(Device);
	//std::cout << "new ComputeCommandBuffer " << ComputeCommandBuffer->GetHandle() << std::endl;
}

void VulkanRHI1::CreateCommandBuffer()
{
	CommandPool = new VulkanCommandPool(Device, queueFamilyIndex);
	Device->m_CommandPool = CommandPool->GetHandle();
	CommandBuffer = new VulkanCommandBuffer(Device, CommandPool);
	//std::cout << "new CommandBuffer " << CommandBuffer->GetHandle() << std::endl;
}