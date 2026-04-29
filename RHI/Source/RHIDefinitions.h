#pragma once
#include <cstdint>
enum RHIPrimitiveType
{	
	// Topology that defines a point N with a single vertex N.
	PT_PointList = 0,

	// Topology that defines a line with 2 vertex extremities: 2*N+0, 2*N+1.
	PT_LineList = 1,

	// Topology that defines a triangle N with 3 vertex extremities: 3*N+0, 3*N+1, 3*N+2.
	PT_TriangleList = 2,

	// Topology that defines a triangle N with 3 vertex extremities: N+0, N+1, N+2.
	PT_TriangleStrip = 3,

	PT_Num,
};


enum  class GpuVendorId : std::uint32_t
{
	Unknown = 0,
	NotQueried = 0xFFFFFFFF,

	Amd = 0x1002,
	ImgTec = 0x1010,
	Nvidia = 0x10DE,
	Arm = 0x13B5,
	Broadcom = 0x14E4,
	Qualcomm = 0x5143,
	Intel = 0x8086,
	Apple = 0x106B,
	Vivante = 0x7a05,
	VeriSilicon = 0x1EB1,

	Kazan = 0x10003,	// VkVendorId
	Codeplay = 0x10004,	// VkVendorId
	Mesa = 0x10005,		// VkVendorId
};

inline GpuVendorId RHIConvertToGpuVendorId(std::uint32_t VendorId)
{
	switch ((GpuVendorId)VendorId)
	{
	case GpuVendorId::NotQueried:
		return GpuVendorId::NotQueried;

	case GpuVendorId::Amd:
	case GpuVendorId::Mesa:
	case GpuVendorId::ImgTec:
	case GpuVendorId::Nvidia:
	case GpuVendorId::Arm:
	case GpuVendorId::Broadcom:
	case GpuVendorId::Qualcomm:
	case GpuVendorId::Intel:
		return (GpuVendorId)VendorId;

	default:
		break;
	}

	return GpuVendorId::Unknown;
}


enum EAddressMode {
	REPEAT = 0,
	MIRRORED_REPEAT = 1,
	CLAMP_TO_EDGE = 2,
	CLAMP_TO_BORDER = 3,
	MIRRORED_CLAMP_TO_EDGE = 4
};

enum EFilter {
	NEAREST = 0,
	LINEAR = 1,
	//CUBIC_EXT = 1000015000,
	//CUBIC_IMG = VK_FILTER_CUBIC_EXT,
	//MAX_ENUM = 0x7FFFFFFF
};

//enum ECompareOp {
//	NEVER = 0,
//	LESS,
//	EQUAL,
//	LESS_OR_EQUAL,
//	GREATER,
//	NEQUAL,
//	GEQUAL,
//	ALWAYS
//};

enum EStencilMask
{
	SM_Default,
	SM_255,
	SM_1,
	SM_2,
	SM_4,
	SM_8,
	SM_16,
	SM_32,
	SM_64,
	SM_128,
	SM_Count
};

enum EStencilOp
{
	SO_Keep,
	SO_Zero,
	SO_Replace,
	SO_SaturatedIncrement,
	SO_SaturatedDecrement,
	SO_Invert,
	SO_Increment,
	SO_Decrement,

	EStencilOp_Num,
	EStencilOp_NumBits = 3,
};

enum ECompareFunction
{
	CF_Less,
	CF_LessEqual,
	CF_Greater,
	CF_GreaterEqual,
	CF_Equal,
	CF_NotEqual,
	CF_Never,
	CF_Always,

	ECompareFunction_Num,
	ECompareFunction_NumBits = 3,

	// Utility enumerations
	//CF_DepthNearOrEqual = (((std::int32_t)ERHIZBuffer::IsInverted != 0) ? CF_GreaterEqual : CF_LessEqual),
	//CF_DepthNear = (((int32)ERHIZBuffer::IsInverted != 0) ? CF_Greater : CF_Less),
	//CF_DepthFartherOrEqual = (((int32)ERHIZBuffer::IsInverted != 0) ? CF_LessEqual : CF_GreaterEqual),
	//CF_DepthFarther = (((int32)ERHIZBuffer::IsInverted != 0) ? CF_Less : CF_Greater),
};

//enum EShaderStage
//{
//	SHADER_STAGE_VERTEX = 0,
//	SHADER_STAGE_PIXEL,
//	SHADER_STAGE_GEOMETRY,
//	SHADER_STAGE_COMPUTE,
//	NUM_SHADER_STAGES,
//};

//enum ERasterizerFillMode
//{
//	FM_Point,									// Point
//	FM_Wireframe,								// Line
//	FM_Solid,									// Fill
//};

enum ERasterizerCullMode
{
	CM_None,
	CM_Front,
	CM_Back,
	CM_Front_And_Back
};

enum ERasterizerFrontFace
{
	FF_None,
	FF_CW,
	FF_CCW,
};

enum EBlendOperation
{
	BO_Add,
	BO_Subtract,
	BO_Min,
	BO_Max,
	BO_ReverseSubtract,

	EBlendOperation_Num,
	EBlendOperation_NumBits = 3,
};
#if 1
enum EBlendFactor
{
	BF_Zero,
	BF_One,
	BF_SourceColor,
	BF_InverseSourceColor,
	BF_SourceAlpha,
	BF_InverseSourceAlpha,
	BF_DestAlpha,
	BF_InverseDestAlpha,
	BF_DestColor,
	BF_InverseDestColor,
	BF_ConstantBlendFactor,
	BF_InverseConstantBlendFactor,
	BF_Source1Color,
	BF_InverseSource1Color,
	BF_Source1Alpha,
	BF_InverseSource1Alpha,

	EBlendFactor_Num,
	EBlendFactor_NumBits = 4,
};

enum EColorWriteMask
{
	CW_RED = 0x01,
	CW_GREEN = 0x02,
	CW_BLUE = 0x04,
	CW_ALPHA = 0x08,

	CW_NONE = 0,
	CW_RGB = CW_RED | CW_GREEN | CW_BLUE,
	CW_RGBA = CW_RED | CW_GREEN | CW_BLUE | CW_ALPHA,
	CW_RG = CW_RED | CW_GREEN,
	CW_BA = CW_BLUE | CW_ALPHA,

	EColorWriteMask_NumBits = 4,
};

enum EShaderFrequency : std::uint8_t
{
	SF_Vertex = 0,
	SF_Mesh = 1,
	SF_Amplification = 2,
	SF_Pixel = 3,
	SF_Geometry = 4,
	SF_Compute = 5,
	SF_RayGen = 6,
	SF_RayMiss = 7,
	SF_RayHitGroup = 8,
	SF_RayCallable = 9,

	SF_NumFrequencies = 10,

	// Number of standard shader frequencies for graphics pipeline (excluding compute)
	SF_NumGraphicsFrequencies = 5,

	// Number of standard shader frequencies (including compute)
	SF_NumStandardFrequencies = 6,

	SF_NumBits = 4,
};

enum RHIShaderStage : std::uint8_t
{
	SS_Vertex = 0,
	SS_Pixel = 1,
	SS_Geometry = 2,
	SS_Compute = 3,

//#if RHI_RAYTRACING
	SS_RayGen = 4,
	SS_RayMiss = 5,
	SS_RayHitGroup = 6,
	SS_RayCallable = 7,
//#endif

	SS_NumStages
}; 

enum EIndexType : std::uint8_t {
	Int16 = 0,
	Int32 = 1,
	None = 2,	//VK_INDEX_TYPE_NONE_KHR | VK_INDEX_TYPE_NONE_N V= 1000165000 
	UInt8 = 3	//VK_INDEX_TYPE_UINT8_KHR | VK_INDEX_TYPE_UINT8_EXT = 1000265000, 
};


typedef union RHIClearColor {
	float       float32[4];
	int32_t     int32[4];
	uint32_t    uint32[4];
} RHIClearColor;

typedef struct RHIClearDepthStencil {
	float       depth;
	uint32_t    stencil;
} RHIClearDepthStencil;

typedef union RHIClearValue {
	RHIClearColor           color;
	RHIClearDepthStencil    depthStencil;
} RHIClearValue;

enum class EImageLayout : uint32_t
{
	Undefined = 0,
	General = 1,
	ColorAttachmentOptimal = 2,
	DepthStencilAttachmentOptimal = 3,
	DepthStencilReadOnlyOptimal = 4,
	ShaderReadOnlyOptimal = 5,
	TransferSrcOptimal = 6,
	TransferDstOptimal = 7,
	PreInitialized = 8,
	DepthReadOnlyStencilAttachmentOptimal = 1000117000,
	DepthAttachmentStencilReadOnlyOptimal = 1000117001,
	DepthAttachmentOptimal = 1000241000,
	DepthReadOnlyOptimal = 1000241001,
	StencilAttachmentOptimal = 1000241002,
	StencilReadOnlyOptimal = 1000241003,
	ReadOnlyOptimal = 1000314000,
	AttachmentOptimal = 1000314001,
	PresentSrc = 1000001002,
	SharedPresent = 1000111000,
	FragmentDensityMapOptimalEXT = 1000218000,
	FragmentShadingRateAttachmentOptimalKHR = 1000164003,

	// Added to handle compute pipeline
	ComputeRead = 2022,
	ComputeWrite = 2023,

	MaxEnum = 0x7FFFFFFF
};
#endif

enum class RHIPipelineStage {
	None = 0,
	Top = 0x00000001,
	DrawIndirect = 0x00000002,
	VertexInput = 0x00000004,
	VertexShader = 0x00000008,
	TCS = 0x00000010,
	TES = 0x00000020,
	GeometryShader = 0x00000040,
	FragmentShader = 0x00000080,
	EarlyFragment = 0x00000100,
	LateFragment = 0x00000200,
	ColorAttachmentOutput = 0x00000400,
	ComputeShader = 0x00000800,
	Transfer = 0x00001000,
	Bottom = 0x00002000,
	//VK_PIPELINE_STAGE_HOST_BIT = 0x00004000,
	//ALL_GRAPHICS_BIT = 0x00008000,
	ALL_COMMANDS_BIT = 0x00010000, 
};