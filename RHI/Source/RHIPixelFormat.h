#pragma once
#include <cstdint>

#if defined(PF_MAX)
#undef PF_MAX
#endif

enum RHIPixelFormat
{
	PF_Unknown = 0,
	PF_R8_SBYTE,			//  8 bit signed byte			R
	PF_R8_UBYTE,			//  8 bit unsigned byte			R
	PF_R16_SINT,			// 16 bit signed short			R
	PF_R16_UINT,			// 16 bit unsigned short		R
	PF_R32_SINT,			// 32 bit signed int			R
	PF_R32_UINT,			// 32 bit unsigned int			R
	PF_R16_FLOAT,			// 16 bit half float			R
	PF_R32_FLOAT,			// 32 bit float					R
	PF_DepthStencil,		// DepthStencil
	PF_R8G8_SBYTE,			//  8 bit signed byte			R	G
	PF_R8G8_UBYTE,			//  8 bit unsigned byte			R	G
	PF_R16G16_SINT,			// 16 bit signed short			R	G
	PF_R16G16_UINT,			// 16 bit unsigned short		R	G
	PF_R32G32_SINT,			// 32 bit signed int			R	G
	PF_R32G32_UINT,			// 32 bit unsigned int			R	G
	PF_R16G16_FLOAT,		// 16 bit half float			R	G
	PF_R32G32_FLOAT,		// 32 bit float					R	G
	PF_R8G8B8_SBYTE,		//  8 bit signed byte			R	G	B	
	PF_R8G8B8_UBYTE,		//  8 bit unsigned byte			R	G	B	
	PF_R16G16B16_SINT,		// 16 bit signed short			R	G	B	
	PF_R16G16B16_UINT,		// 16 bit unsigned short		R	G	B	
	PF_R32G32B32_SINT,		// 32 bit signed int			R	G	B	
	PF_R32G32B32_UINT,		// 32 bit unsigned int			R	G	B	
	PF_R16G16B16_FLOAT,		// 16 bit half float			R	G	B	
	PF_R32G32B32_FLOAT,		// 32 bit float					R	G	B	

	PF_B8G8R8A8_SRGB,		// BGRA	for swapchain			B	G	R	A 
	PF_R8G8B8A8_SRGB,		// RGBA	for swapchain			R	G	B	A 

	PF_R8G8B8A8_SBYTE,		//  8 bit signed byte			R	G	B	A	
	PF_R8G8B8A8_UBYTE,		//  8 bit unsigned byte			R	G	B	A	
	PF_R16G16B16A16_SINT,	// 16 bit signed short			R	G	B	A	
	PF_R16G16B16A16_UINT,	// 16 bit unsigned short		R	G	B	A	
	PF_R32G32B32A32_SINT,	// 32 bit signed int			R	G	B	A	
	PF_R32G32B32A32_UINT,	// 32 bit unsigned int			R	G	B	A	
	PF_R16G16B16A16_FLOAT,	// 16 bit half float			R	G	B	A	
	PF_R32G32B32A32_FLOAT,	// 32 bit float					R	G	B	A	

	PF_B8G8R8A8_SBYTE,		//  8 bit signed byte			B	G	R	A	
	PF_B8G8R8A8_UBYTE,		//  8 bit unsigned byte			B	G	R	A	
	PF_MAX
};

struct RHIPixelFormatInfo
{
	RHIPixelFormatInfo() = delete;
	RHIPixelFormatInfo(
		RHIPixelFormat InUnrealFormat,
		const char* InName,
		std::int32_t InBlockSizeX,
		std::int32_t InBlockSizeY,
		std::int32_t InBlockSizeZ,
		std::int32_t InBlockBytes,
		std::int32_t InNumComponents);

	const char*		Name;
	RHIPixelFormat		UnrealFormat;
	std::int32_t	BlockSizeX;
	std::int32_t	BlockSizeY;
	std::int32_t	BlockSizeZ;
	std::int32_t	BlockBytes;
	std::int32_t	NumComponents;

	uint32_t		PlatformFormat{ 0 }; //VkFormat
};

extern RHIPixelFormatInfo GPixelFormats[PF_MAX];