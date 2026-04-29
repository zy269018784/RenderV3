#pragma once
#include <cstdint>

/** specifies an update region for a texture */
struct RHIUpdateTextureRegion1D
{
	/** offset in texture */
	std::uint32_t DestX; 

	/** offset in source image data */
	std::int32_t SrcX; 

	/** size of region to copy */
	std::uint32_t Width;  

	RHIUpdateTextureRegion1D() {}

	RHIUpdateTextureRegion1D(std::uint32_t InDestX, std::int32_t InSrcX, std::uint32_t InWidth)
		: DestX(InDestX) 
		, SrcX(InSrcX) 
		, Width(InWidth) 
	{}
};

/** specifies an update region for a texture */
struct RHIUpdateTextureRegion2D
{
	/** offset in texture */
	std::uint32_t DestX;
	std::uint32_t DestY;

	/** offset in source image data */
	std::int32_t SrcX;
	std::int32_t SrcY;

	/** size of region to copy */
	std::uint32_t Width;
	std::uint32_t Height;

	RHIUpdateTextureRegion2D()
	{}

	RHIUpdateTextureRegion2D(std::uint32_t InDestX, std::uint32_t InDestY, std::int32_t InSrcX, std::int32_t InSrcY, std::uint32_t InWidth, std::uint32_t InHeight)
		: DestX(InDestX)
		, DestY(InDestY)
		, SrcX(InSrcX)
		, SrcY(InSrcY)
		, Width(InWidth)
		, Height(InHeight)
	{}
};
