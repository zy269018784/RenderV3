#pragma once
#include <RHIResource.h>
#include <EnumClassFlags.h>
#include <cstdint>
enum class BufferUsageFlags : std::uint32_t
{
	None = 0,

	/** The buffer will be written to once. */
	Static = 1 << 0,

	/** The buffer will be written to occasionally, GPU read only, CPU write only.  The data lifetime is until the next update, or the buffer is destroyed. */
	Dynamic = 1 << 1,

	/** The buffer's data will have a lifetime of one frame.  It MUST be written to each frame, or a new one created each frame. */
	Volatile = 1 << 2,

	/** Allows an unordered access view to be created for the buffer. */
	UnorderedAccess = 1 << 3,

	/** Create a byte address buffer, which is basically a structured buffer with a std::uint32_t type. */
	ByteAddressBuffer = 1 << 4,

	/** Buffer that the GPU will use as a source for a copy. */
	SourceCopy = 1 << 5,

	/** Create a buffer that can be bound as a stream output target. */
	StreamOutput = 1 << 6,

	/** Create a buffer which contains the arguments used by DispatchIndirect or DrawIndirect. */
	DrawIndirect = 1 << 7,

	/**
	 * Create a buffer that can be bound as a shader resource.
	 * This is only needed for buffer types which wouldn't ordinarily be used as a shader resource, like a vertex buffer.
	 */
	ShaderResource = 1 << 8,

	/** Request that this buffer is directly CPU accessible. */
	KeepCPUAccessible = 1 << 9,

	/** Buffer should go in fast vram (hint only). Requires BUF_Transient */
	FastVRAM = 1 << 10,

	/** Buffer should be allocated from transient memory. */
	Transient = None,

	/** Create a buffer that can be shared with an external RHI or process. */
	Shared = 1 << 12,

	/**
	 * Buffer contains opaque ray tracing acceleration structure data.
	 * Resources with this flag can't be bound directly to any shader stage and only can be used with ray tracing APIs.
	 * This flag is mutually exclusive with all other buffer flags except BUF_Static.
	*/
	AccelerationStructure = 1 << 13,

	/*
		VBO
	*/
	VertexBuffer = 1 << 14,
	/*
		EBO
	*/
	IndexBuffer = 1 << 15,
	/*
		SSBO
	*/
	StoregeBuffer = 1 << 16,
	/*
		UBO
	*/
	UniformBuffer = 1 << 17,
	/*
		Indrect Buffer
	*/
	IndrectBuffer = 1 << 18,

	TransferSrcBuffer = 1 << 19,

	TransferDstBuffer = 1 << 20,

	RayTracingScratch = 1 << 21,

	ShaderBindingTable = 1 << 22,
	// Helper bit-masks
	AnyDynamic = (Dynamic | Volatile),
};
ENUM_CLASS_FLAGS(BufferUsageFlags);

#define BUF_None                   BufferUsageFlags::None
#define BUF_Static                 BufferUsageFlags::Static
#define BUF_Dynamic                BufferUsageFlags::Dynamic
#define BUF_Volatile               BufferUsageFlags::Volatile
#define BUF_UnorderedAccess        BufferUsageFlags::UnorderedAccess
#define BUF_ByteAddressBuffer      BufferUsageFlags::ByteAddressBuffer
#define BUF_SourceCopy             BufferUsageFlags::SourceCopy
#define BUF_StreamOutput           BufferUsageFlags::StreamOutput
#define BUF_DrawIndirect           BufferUsageFlags::DrawIndirect
#define BUF_ShaderResource         BufferUsageFlags::ShaderResource
#define BUF_KeepCPUAccessible      BufferUsageFlags::KeepCPUAccessible
#define BUF_FastVRAM               BufferUsageFlags::FastVRAM
#define BUF_Transient              BufferUsageFlags::Transient
#define BUF_Shared                 BufferUsageFlags::Shared
#define BUF_AccelerationStructure  BufferUsageFlags::AccelerationStructure
#define BUF_RayTracingScratch	   BufferUsageFlags::RayTracingScratch
#define BUF_VertexBuffer           BufferUsageFlags::VertexBuffer
#define BUF_IndexBuffer            BufferUsageFlags::IndexBuffer
#define BUF_StorageBuffer		   BufferUsageFlags::StoregeBuffer
#define BUF_UniformBuffer		   BufferUsageFlags::UniformBuffer
#define BUF_IndrectBuffer		   BufferUsageFlags::IndrectBuffer
#define BUF_TransferSrcBuffer	   BufferUsageFlags::TransferSrcBuffer
#define BUF_TransferDstBuffer	   BufferUsageFlags::TransferDstBuffer
#define BUF_ShaderBindingTable	   BufferUsageFlags::ShaderBindingTable

class RHIBuffer : public RHIResource
{
public:
	RHIBuffer() : RHIResource(RRT_Buffer) {}
	RHIBuffer(std::uint32_t InSize, BufferUsageFlags InUsage, std::uint32_t InStride)
		: RHIResource(RRT_Buffer)
		, Size(InSize)
		, Stride(InStride)
		, Usage(InUsage)
	{
	}

	virtual ~RHIBuffer() {};

	/** @return The number of bytes in the buffer. */
	std::uint64_t GetSize() const { return Size; }

	/** @return The stride in bytes of the buffer. */
	std::uint32_t GetStride() const { return Stride; }

	/** @return The usage flags used to create the buffer. */
	BufferUsageFlags GetUsage() const { return Usage; }

	virtual void UpdateData(std::uint32_t InSize, const void* InData) = 0;
	virtual uint64_t GetDeviceAddress() = 0;
private:
	std::uint64_t Size{};
	std::uint32_t Stride{};
	BufferUsageFlags Usage{};
	//char* BufferName;
};