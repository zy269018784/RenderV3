#include <VulkanRHI/VulkanObjects/VulkanShaderBindings.h>
#include <VulkanRHI/VulkanObjects/VulkanBuffer.h>
#include <VulkanRHI/VulkanObjects/Texture/VulkanTexture.h>
#include <VulkanRHI/VulkanObjects/VulkanRaytracing.h>
#include <iostream>
VulkanShaderBindings::VulkanShaderBindings(VulkanDevice* InDevice)
	: Device(InDevice)
{

}

VulkanShaderBindings::~VulkanShaderBindings()
{
	Device->DestroyDescriptorPool(DescriptorPool, nullptr);
	Device->DestroyDescriptorSetLayout(DescSetLayout, nullptr);
}

VkDescriptorSet VulkanShaderBindings::GetDescriptorSet()
{
	return DescriptorSet;
}

VkDescriptorSetLayout VulkanShaderBindings::GetDescriptorSetLayout()
{
	return DescSetLayout;
}

void VulkanShaderBindings::AddBinding(RHIShaderBinding& InBinding)
{
	Bindings.push_back(InBinding);
}

void VulkanShaderBindings::UpdateBinding(uint32_t InBinding, RHIShaderBinding& InshaderBinding)
{
	for (int i = 0; i < Bindings.size(); i++)
	{
		if (Bindings[i].binding == InBinding)
		{
			Bindings[i] = InshaderBinding;
		}
	}
}

void VulkanShaderBindings::SetBindings(std::vector<RHIShaderBinding> InBindings) 
{
	Bindings = InBindings;
}

void VulkanShaderBindings::Create()
{
	/*
		创建描述符池
	*/
	CreateDescriptorPool();
	/*
		创建描述符集合布局
	*/
	CreateDescriptorSetLayout();
	/*
		分配描述符集
	*/
	AllocateDescriptorSets();
	/*
		更新描述符集
	*/
	UpdateDescriptorSet();
}

void VulkanShaderBindings::Update()
{
	/*
		释放旧的描述符池
	*/
	Device->DestroyDescriptorPool(DescriptorPool, nullptr);
	/*
		创建新的描述符池
	*/
	CreateDescriptorPool();
	/*
		释放旧的描述符集合布局
	*/
	Device->DestroyDescriptorSetLayout(DescSetLayout, nullptr);
	/*
		创建新的描述符集合布局
	*/
	CreateDescriptorSetLayout();
	/*
		分配描述符集
	*/
	AllocateDescriptorSets();
	/*
		更新描述符集
	*/
	UpdateDescriptorSet();
}

void VulkanShaderBindings::CreateDescriptorPool()
{	
	uint32_t bindingSize = Bindings.size();  
	std::vector<VkDescriptorPoolSize> poolSizes; 
	uint32_t UniformBufferCount = 0;
	uint32_t TextureCount = 0;
	uint32_t StorageBufferCount = 0;
	uint32_t StorageImageCount = 0;
	uint32_t AccStructCount = 0;
	for (const auto &binding : Bindings) { 
		switch (binding.type) {
		case RHIShaderBinding::EType::UNIFORM_BUFFER:
			UniformBufferCount++;
			break;
		case RHIShaderBinding::EType::COMBINED_IMAGE_SAMPLER:
			TextureCount += binding.count;
			break;
		case RHIShaderBinding::EType::STORAGE_BUFFER:
			StorageBufferCount++;
			break;
		case RHIShaderBinding::EType::STORAGE_IMAGE:
			StorageImageCount++;
			break;
		case RHIShaderBinding::EType::ACCELERATION_STRUCTURE:
			AccStructCount++;
			break;
		}//switch
	}
 
	if (UniformBufferCount > 0) {
		VkDescriptorPoolSize poolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ,UniformBufferCount };
		poolSizes.push_back(poolSize);
	}

	if (TextureCount > 0) {
		VkDescriptorPoolSize poolSize{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ,TextureCount };
		poolSizes.push_back(poolSize);
	}

	if (StorageBufferCount > 0) {
		VkDescriptorPoolSize poolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ,StorageBufferCount };
		poolSizes.push_back(poolSize);
	}

	if (StorageImageCount > 0) {
		VkDescriptorPoolSize poolSize{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ,StorageImageCount };
		poolSizes.push_back(poolSize);
	}

	if (AccStructCount > 0) {
		VkDescriptorPoolSize poolSize{ VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR ,AccStructCount };
		poolSizes.push_back(poolSize);
	}

	VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	descriptorPoolInfo.pPoolSizes = poolSizes.data();
	descriptorPoolInfo.maxSets = bindingSize; 
	Device->CreateDescriptorPool( &descriptorPoolInfo, nullptr, &DescriptorPool);
}

void VulkanShaderBindings::CreateDescriptorSetLayout()
{ 	
	//std::vector<VkDescriptorBindingFlags> BindingFlags;	
	//BindingFlags.reserve(Bindings.size());
	std::vector<VkDescriptorSetLayoutBinding> DescriptorSetLayoutBindings;
	DescriptorSetLayoutBindings.reserve(Bindings.size());

	for (const auto& b : Bindings) 
	{ 
		VkDescriptorSetLayoutBinding DescriptorSetLayoutBinding = {};
		DescriptorSetLayoutBinding.binding = b.binding;
		DescriptorSetLayoutBinding.descriptorType = toVkDescriptorType(b.type);
		DescriptorSetLayoutBinding.descriptorCount = b.count;
		DescriptorSetLayoutBinding.stageFlags = b.stageFlags_;
		DescriptorSetLayoutBindings.emplace_back(DescriptorSetLayoutBinding);
	}

	VkDescriptorSetLayoutCreateInfo descSetLayoutInfo{};
	descSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descSetLayoutInfo.bindingCount = static_cast<uint32_t>(DescriptorSetLayoutBindings.size());
	descSetLayoutInfo.pBindings = DescriptorSetLayoutBindings.data();

	if (Device->CreateDescriptorSetLayout(&descSetLayoutInfo, nullptr, &DescSetLayout) != VK_SUCCESS) 
	{
		printf("create DescriptorSetLayout failed\n");
	}
}

void VulkanShaderBindings::AllocateDescriptorSets()
{
	VkDescriptorSetAllocateInfo AllocateInfo = {};
	AllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	AllocateInfo.descriptorPool = DescriptorPool;
	AllocateInfo.pSetLayouts = &DescSetLayout;
	AllocateInfo.descriptorSetCount = 1;
	Device->AllocateDescriptorSets(&AllocateInfo, &DescriptorSet);
}

void VulkanShaderBindings::UpdateDescriptorSet() 
{   
	std::vector<VkWriteDescriptorSet> WriteDescriptorSets;
	using DescImageInfos = std::vector<VkDescriptorImageInfo>;// imageInfos;

	std::vector<DescImageInfos> vDescImageInfos;

	std::vector<VkWriteDescriptorSetAccelerationStructureKHR> asInfos; 
	  
	vDescImageInfos.resize(Bindings.size());
	asInfos.reserve(Bindings.size());
	int i = 0;
	int j = 0;
	for (const auto &shaderbinding : Bindings)
	{
		VkWriteDescriptorSet WriteDescriptorSet{};
		WriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		WriteDescriptorSet.dstSet = DescriptorSet;
		WriteDescriptorSet.descriptorType = toVkDescriptorType(shaderbinding.type);
		WriteDescriptorSet.dstBinding = shaderbinding.binding;
		WriteDescriptorSet.descriptorCount = shaderbinding.count;
		//printf("descriptorCount %d %d\n", shaderbinding.count, shaderbinding.binding);
			
		vDescImageInfos[i].resize(shaderbinding.count);
		vDescImageInfos[i] = {}; 
		asInfos[j] = {};
		if (shaderbinding.type == RHIShaderBinding::EType::UNIFORM_BUFFER) 
		{
			VulkanBuffer* pVkBuffer = dynamic_cast<VulkanBuffer*>(shaderbinding.buffer);
			WriteDescriptorSet.pBufferInfo = &pVkBuffer->Descriptor;
		}
		else if (shaderbinding.type == RHIShaderBinding::EType::COMBINED_IMAGE_SAMPLER) 
		{ 

			if (shaderbinding.textures.size() > 0) 
			{ 
				for (int d = 0; d < shaderbinding.textures.size(); d++) 
				{ 
					VulkanSampler* pSampler = dynamic_cast<VulkanSampler*>(shaderbinding.sampler);
					vDescImageInfos[i][d].imageView = GetImageViewFromTexture(shaderbinding.textures[d]);
					vDescImageInfos[i][d].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					vDescImageInfos[i][d].sampler = pSampler->Handle;
				}
				WriteDescriptorSet.pImageInfo = vDescImageInfos[i].data();

			}
			else 
			{
				VulkanSampler* pSampler = dynamic_cast<VulkanSampler*>(shaderbinding.sampler);
				vDescImageInfos[i][0].imageView = GetImageViewFromTexture(shaderbinding.textures[0]);
				vDescImageInfos[i][0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				vDescImageInfos[i][0].sampler = pSampler->Handle;
				WriteDescriptorSet.pImageInfo = vDescImageInfos[i].data(); //&imageInfos[i];
			}

		}
		else if (shaderbinding.type == RHIShaderBinding::EType::STORAGE_BUFFER) 
		{
			VulkanBuffer* pVkBuffer = dynamic_cast<VulkanBuffer*>(shaderbinding.buffer);
			WriteDescriptorSet.pBufferInfo = &pVkBuffer->Descriptor;
		} 
		else if (shaderbinding.type == RHIShaderBinding::EType::STORAGE_IMAGE) 
		{ 
			VulkanSampler* pSampler = dynamic_cast<VulkanSampler*>(shaderbinding.sampler);
			vDescImageInfos[i][0].imageView = GetImageViewFromTexture(shaderbinding.textures[0]);
			vDescImageInfos[i][0].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			vDescImageInfos[i][0].sampler = pSampler->Handle;
			WriteDescriptorSet.pImageInfo = vDescImageInfos[i].data();
		}
		else if (shaderbinding.type == RHIShaderBinding::EType::ACCELERATION_STRUCTURE)
		{ 
			VulkanRaytracingTLAS* TLAS = dynamic_cast<VulkanRaytracingTLAS*>(shaderbinding.AS); 
			asInfos[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
			asInfos[j].accelerationStructureCount = 1;
			asInfos[j].pAccelerationStructures = &TLAS->Handle;
			WriteDescriptorSet.pNext = &asInfos[j]; // The specialized acceleration structure descriptor has to be chained
		}
		else {
			printf("todo : not support shaderbinding type\n");
		}
		WriteDescriptorSets.push_back(WriteDescriptorSet);
		i++;
		j++;
	}  
	Device->UpdateDescriptorSets(static_cast<uint32_t>(WriteDescriptorSets.size()), WriteDescriptorSets.data(), 0, VK_NULL_HANDLE);
}


VkImageView VulkanShaderBindings::GetImageViewFromTexture(RHITexture* Texure)
{
	VkImageView ImageView = {};
	RHIResourceType Type = Texure->GetResourceType();
	if (Type == RRT_Texture2D) 
	{
		VulkanTexture2D* Tex2d = dynamic_cast<VulkanTexture2D*>(Texure);
		ImageView = Tex2d->GetImageView();
	}
	else if (Type == RRT_Texture3D) 
	{
		VulkanTexture3D* Tex3d = dynamic_cast<VulkanTexture3D*>(Texure);
		ImageView = Tex3d->GetImageView();
	}
	else if (Type == RRT_Texture1D) 
	{
		VulkanTexture1D* Tex1d = dynamic_cast<VulkanTexture1D*>(Texure);
		ImageView = Tex1d->GetImageView();
	}
	else if (Type == RRT_Texture2DArray) 
	{
		VulkanTexture2DArray* Tex2darray = dynamic_cast<VulkanTexture2DArray*>(Texure);
		ImageView = Tex2darray->GetImageView();
	}
	else if (Type == RRT_TextureCube)
	{
		VulkanTextureCube* TexCube = dynamic_cast<VulkanTextureCube*>(Texure);
		ImageView = TexCube->GetImageView();
	}
	if (ImageView == VK_NULL_HANDLE)
	{
		printf("imageView == VK_NULL_HANDLE \n");
	}
	return ImageView;
}

VkDescriptorType VulkanShaderBindings::toVkDescriptorType(RHIShaderBinding::EType Type)
{ 
	switch (Type)
	{
	case RHIShaderBinding::UNIFORM_BUFFER:
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	case RHIShaderBinding::COMBINED_IMAGE_SAMPLER:
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	case RHIShaderBinding::SAMPLED_IMAGE:
		return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	case RHIShaderBinding::STORAGE_IMAGE:			//storage image
		return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	case RHIShaderBinding::STORAGE_BUFFER:			//storage buffer
		return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	case RHIShaderBinding::ACCELERATION_STRUCTURE:
		return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	default:
		printf("unknown ShaderBinding type %d\n", Type);
		return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}
}

