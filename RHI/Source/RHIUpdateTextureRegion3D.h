#pragma once
#include <cstdint>
struct RHIUpdateTextureRegion3D
{
	/** offset in texture */
	std::uint32_t DestX;
	std::uint32_t DestY;
	std::uint32_t DestZ;

	/** offset in source image data */
	std::int32_t SrcX;
	std::int32_t SrcY;
	std::int32_t SrcZ;

	/** size of region to copy */
	std::uint32_t Width;
	std::uint32_t Height;
	std::uint32_t Depth;

	RHIUpdateTextureRegion3D()
	{}

	RHIUpdateTextureRegion3D(std::uint32_t InDestX, std::uint32_t InDestY,	std::uint32_t InDestZ, 
							 std::int32_t   InSrcX,	std::int32_t   InSrcY,	std::int32_t   InSrcZ, 
							 std::uint32_t InWidth, std::uint32_t InHeight, std::uint32_t InDepth)
		: DestX(InDestX)
		, DestY(InDestY)
		, DestZ(InDestZ)
		, SrcX(InSrcX)
		, SrcY(InSrcY)
		, SrcZ(InSrcZ)
		, Width(InWidth)
		, Height(InHeight)
		, Depth(InDepth)
	{}

	//FUpdateTextureRegion3D(FIntVector InDest, FIntVector InSource, FIntVector InSourceSize)
	//	: DestX(InDest.X)
	//	, DestY(InDest.Y)
	//	, DestZ(InDest.Z)
	//	, SrcX(InSource.X)
	//	, SrcY(InSource.Y)
	//	, SrcZ(InSource.Z)
	//	, Width(InSourceSize.X)
	//	, Height(InSourceSize.Y)
	//	, Depth(InSourceSize.Z)
	//{}
};