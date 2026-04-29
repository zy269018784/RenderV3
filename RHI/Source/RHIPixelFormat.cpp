#include <RHIPixelFormat.h>

RHIPixelFormatInfo::RHIPixelFormatInfo(
	RHIPixelFormat InUnrealFormat,
	const char* InName,
	std::int32_t InBlockSizeX,
	std::int32_t InBlockSizeY,
	std::int32_t InBlockSizeZ,
	std::int32_t InBlockBytes,
	std::int32_t InNumComponents)
	: Name(InName)
	, UnrealFormat(InUnrealFormat)
	, BlockSizeX(InBlockSizeX)
	, BlockSizeY(InBlockSizeY)
	, BlockSizeZ(InBlockSizeZ)
	, BlockBytes(InBlockBytes)
	, NumComponents(InNumComponents)
{

}


RHIPixelFormatInfo    GPixelFormats[PF_MAX] =
{
	//						UnrealFormat			Name					BlockSizeX		BlockSizeY		BlockSizeZ		BlockBytes		  NumComponents 
	RHIPixelFormatInfo(		PF_Unknown,             "unknown",              0,				0,				0,				0,				  0					),
	RHIPixelFormatInfo(		PF_R8_SBYTE,			"R8_SBYTE",				0,				0,				0,				1,				  1					),
	RHIPixelFormatInfo(		PF_R8_UBYTE,			"R8_UBYTE",				0,				0,				0,				1,				  1					),
	RHIPixelFormatInfo(		PF_R16_SINT,			"R16_SINT",				0,				0,				0,				2,				  1					),
	RHIPixelFormatInfo(		PF_R16_UINT,			"R16_UINT",				0,				0,				0,				2,				  1					),
	RHIPixelFormatInfo(		PF_R32_SINT,			"R32_SINT",				0,				0,				0,				4,				  1					),
	RHIPixelFormatInfo(		PF_R32_UINT,			"R32_UINT",				0,				0,				0,				4,				  1					),
	RHIPixelFormatInfo(		PF_R16_FLOAT,			"R16_FLOAT",			0,				0,				0,				2,				  1					),
	RHIPixelFormatInfo(		PF_R32_FLOAT,			"R32_FLOAT",			0,				0,				0,				4,				  1					),
	RHIPixelFormatInfo(		PF_DepthStencil,		"DepthStencil",			1,				1,				1,				4,				  1					), 

	RHIPixelFormatInfo(		PF_R8G8_SBYTE,			"R8G8_SBYTE",			0,				0,				0,				2,				  2					),
	RHIPixelFormatInfo(		PF_R8G8_UBYTE,			"R8G8_UBYTE",			0,				0,				0,				2,				  2					),
	RHIPixelFormatInfo(		PF_R16G16_SINT,			"R16G16_SINT",			0,				0,				0,				4,				  2					),
	RHIPixelFormatInfo(		PF_R16G16_UINT,			"R16G16_UINT",			0,				0,				0,				4,				  2					),
	RHIPixelFormatInfo(		PF_R32G32_SINT,			"R32G32_SINT",			0,				0,				0,				8,				  2					),
	RHIPixelFormatInfo(		PF_R32G32_UINT,			"R32G32_UINT",			0,				0,				0,				8,				  2					),
	RHIPixelFormatInfo(		PF_R16G16_FLOAT,		"R16G16_FLOAT",			0,				0,				0,				4,				  2					),
	RHIPixelFormatInfo(		PF_R32G32_FLOAT,		"R32G32_FLOAT",			0,				0,				0,				8,				  2					),
	
	RHIPixelFormatInfo(		PF_R8G8B8_SBYTE,		"R8G8B8_SBYTE",			0,				0,				0,				3,				  3					),
	RHIPixelFormatInfo(		PF_R8G8B8_UBYTE,		"R8G8B8_UBYTE",			0,				0,				0,				3,				  3					),
	RHIPixelFormatInfo(		PF_R16G16B16_SINT,		"R16G16B16_SINT",		0,				0,				0,				6,				  3					),
	RHIPixelFormatInfo(		PF_R16G16B16_UINT,		"R16G16B16_UINT",		0,				0,				0,				6,				  3					),
	RHIPixelFormatInfo(		PF_R32G32B32_SINT,		"R32G32B32_SINT",		0,				0,				0,				12,				  3					),
	RHIPixelFormatInfo(		PF_R32G32B32_UINT,		"R32G32B32_UINT",		0,				0,				0,				12,				  3					),
	RHIPixelFormatInfo(		PF_R16G16B16_FLOAT,		"R16G16B16_FLOAT",		0,				0,				0,				6,				  3					),
	RHIPixelFormatInfo(		PF_R32G32B32_FLOAT,		"R32G32B32_FLOAT",		0,				0,				0,				12,				  3					),

	RHIPixelFormatInfo(		PF_B8G8R8A8_SRGB,		"B8G8R8A8_SRGB",		0,				0,				0,				4,				  4					),
	RHIPixelFormatInfo(		PF_R8G8B8A8_SRGB,		"R8G8B8A8_SRGB",		0,				0,				0,				4,				  4					),

	RHIPixelFormatInfo(		PF_R8G8B8A8_SBYTE,		"R8G8B8A8_SBYTE",		0,				0,				0,				4,				  4					),
	RHIPixelFormatInfo(		PF_R8G8B8A8_UBYTE,		"R8G8B8A8_UBYTE",		0,				0,				0,				4,				  4					),
	RHIPixelFormatInfo(		PF_R16G16B16A16_SINT,	"R16G16B16A16_SINT",	0,				0,				0,				8,				  4					),
	RHIPixelFormatInfo(		PF_R16G16B16A16_UINT,	"R16G16B16A16_UINT",	0,				0,				0,				8,				  4					),
	RHIPixelFormatInfo(		PF_R32G32B32A32_SINT,	"R32G32B32A32_SINT",	0,				0,				0,				16,				  4					),
	RHIPixelFormatInfo(		PF_R32G32B32A32_UINT,	"R32G32B32A32_UINT",	0,				0,				0,				16,				  4					),
	RHIPixelFormatInfo(		PF_R16G16B16A16_FLOAT,	"R16G16B16A16_FLOAT",	0,				0,				0,				8,				  4					),
	RHIPixelFormatInfo(		PF_R32G32B32A32_FLOAT,	"R32G32B32A32_FLOAT",	0,				0,				0,				16,				  4					),

	RHIPixelFormatInfo(		PF_B8G8R8A8_SBYTE,		"B8G8R8A8_SBYTE",		0,				0,				0,				4,				  4					),
	RHIPixelFormatInfo(		PF_B8G8R8A8_UBYTE,		"B8G8R8A8_UBYTE",		0,				0,				0,				4,				  4					),
};