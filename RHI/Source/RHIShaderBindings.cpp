#include <RHIBuffer.h>
#include <RHISampler.h>
#include <RHITexture.h>

#include <vector>

////Type{ UniformBuffer, SampledTexture, Texture, Sampler, ImageLoad, …, BufferLoadStore }
//
//class RHIShaderBinding {
//public:
//	enum EStageFlag
//	{
//		Vertex = 1 << 0,
//		TessellationControl = 1 << 1,
//		TessellationEvaluation = 1 << 2,
//		Fragment = 1 << 3,
//		Compute = 1 << 4,
//		Geometry = 1 << 5
//	};
//	//typedef enum VkDescriptorType {
//	//	VK_DESCRIPTOR_TYPE_SAMPLER = 0,
//	//	VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
//	//	VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
//	//	VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
//	//	VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
//	//	VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
//	//	VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
//	//	VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
//	//	VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
//	//	VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
//	//	VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,
//	//	VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK = 1000138000,
//	//	VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR = 1000150000,
//	//	VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV = 1000165000,
//	//	VK_DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM = 1000440000,
//	//	VK_DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM = 1000440001,
//	//	VK_DESCRIPTOR_TYPE_MUTABLE_EXT = 1000351000,
//	//	VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK,
//	//	VK_DESCRIPTOR_TYPE_MUTABLE_VALVE = VK_DESCRIPTOR_TYPE_MUTABLE_EXT,
//	//	VK_DESCRIPTOR_TYPE_MAX_ENUM = 0x7FFFFFFF
//	//} VkDescriptorType;
//	enum EType {
//		SAMPLER = 0,
//		COMBINED_IMAGE_SAMPLER = 1,
//		SAMPLED_IMAGE = 2,
//		STORAGE_IMAGE = 3,
//		UNIFORM_TEXEL_BUFFER = 4,
//		STORAGE_TEXEL_BUFFER = 5,
//		UNIFORM_BUFFER = 6,
//		STORAGE_BUFFER = 7,
//
//	};
//public:
//	//virtual void setUniformBuffer(uint32_t binding, EStageFlag flags, RHIBuffer* buf) = 0;
//	//virtual void setSampler(uint32_t binding, EStageFlag flags, RHISampler* sampler) = 0;
//	//virtual void setTexture(uint32_t binding, EStageFlag flags, RHITexture* tex) = 0; 
//	//virtual void setStorageBuffer(uint32_t binding, EStageFlag flags, RHIBuffer* buf) = 0;
//
//	static RHIShaderBinding UniformBuffer(uint32_t binding, EStageFlag flags, RHIBuffer* buf) {
//		static RHIShaderBinding sb;
//		sb.binding = binding;
//		sb.flags = flags;
//		sb.type = EType::UNIFORM_BUFFER;
//		sb.data = buf;
//		return sb;
//	} 
//
//	static RHIShaderBinding Texture(uint32_t binding, EStageFlag flags, RHITexture* tex) {
//		static RHIShaderBinding sb;
//		sb.binding = binding;
//		sb.flags = flags;
//		sb.type = EType::COMBINED_IMAGE_SAMPLER;
//		sb.data = tex;
//		return sb;
//	}
//	 
//	uint32_t binding;
//	EStageFlag flags;
//	EType type;
//	void* data = nullptr;
//};
//
//class RHIShaderBindings {
//public:
//
//	virtual void addBinding(RHIShaderBinding& binding) = 0;
//	virtual void setBindings(std::vector<RHIShaderBinding> bindings) = 0;
//
//	virtual void create() = 0;
//};