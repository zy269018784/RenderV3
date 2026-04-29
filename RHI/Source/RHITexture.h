#pragma once
#include <RHIResource.h>
#include <EnumClassFlags.h>
#include <cstdint>

enum CubeMapFace : uint8_t
{
    RIGHT_FACE = 0,
    LEFT_FACE = 1, 
    TOP_FACE = 2,
    BOTTOM_FACE = 3,
    FRONT_FACE = 4,
    BACK_FACE = 5,
};

/** Flags used for texture creation */
enum class TextureCreateFlags : uint64_t
{
    None = 0,

    // Texture can be used as a render target
    RenderTargetable = 1ull << 0,
    // Texture can be used as a resolve target
    ResolveTargetable = 1ull << 1,
    // Texture can be used as a depth-stencil target.
    DepthStencilTargetable = 1ull << 2,
    // Texture can be used as a shader resource.
    ShaderResource = 1ull << 3,
    // Texture is encoded in sRGB gamma space
    SRGB = 1ull << 4,
    // Texture data is writable by the CPU
    CPUWritable = 1ull << 5,
    // Texture will be created with an un-tiled format
    NoTiling = 1ull << 6,
    // Texture will be used for video decode
    VideoDecode = 1ull << 7,
    // Texture that may be updated every frame
    Dynamic = 1ull << 8,
    // Texture will be used as a render pass attachment that will be read from
    InputAttachmentRead = 1ull << 9,
    /** Texture represents a foveation attachment */
    Foveation = 1ull << 10,
    // Prefer 3D internal surface tiling mode for volume textures when possible
    Tiling3D = 1ull << 11,
    // This texture has no GPU or CPU backing. It only exists in tile memory on TBDR GPUs (i.e., mobile).
    Memoryless = 1ull << 12,
    // Create the texture with the flag that allows mip generation later, only applicable to D3D11
    GenerateMipCapable = 1ull << 13,
    // The texture can be partially allocated in fastvram
    FastVRAMPartialAlloc = 1ull << 14,
    // Do not create associated shader resource view, only applicable to D3D11 and D3D12
    DisableSRVCreation = 1ull << 15,
    // Do not allow Delta Color Compression (DCC) to be used with this texture
    DisableDCC = 1ull << 16,
    // UnorderedAccessView (DX11 only)
    // Warning: Causes additional synchronization between draw calls when using a render target allocated with this flag, use sparingly
    // See: GCNPerformanceTweets.pdf Tip 37
    UAV = 1ull << 17,
    // Render target texture that will be displayed on screen (back buffer)
    Presentable = 1ull << 18,
    // Texture data is accessible by the CPU
    CPUReadback = 1ull << 19,
    // Texture was processed offline (via a texture conversion process for the current platform)
    OfflineProcessed = 1ull << 20,
    // Texture needs to go in fast VRAM if available (HINT only)
    FastVRAM = 1ull << 21,
    // by default the texture is not showing up in the list - this is to reduce clutter, using the FULL option this can be ignored
    HideInVisualizeTexture = 1ull << 22,
    // Texture should be created in virtual memory, with no physical memory allocation made
    // You must make further calls to RHIVirtualTextureSetFirstMipInMemory to allocate physical memory
    // and RHIVirtualTextureSetFirstMipVisible to map the first mip visible to the GPU
    Virtual = 1ull << 23,
    // Creates a RenderTargetView for each array slice of the texture
    // Warning: if this was specified when the resource was created, you can't use SV_RenderTargetArrayIndex to route to other slices!
    TargetArraySlicesIndependently = 1ull << 24,
    // Texture that may be shared with DX9 or other devices
    Shared = 1ull << 25,
    // RenderTarget will not use full-texture fast clear functionality.
    NoFastClear = 1ull << 26,
    // Texture is a depth stencil resolve target
    DepthStencilResolveTarget = 1ull << 27,
    // Flag used to indicted this texture is a streamable 2D texture, and should be counted towards the texture streaming pool budget.
    Streamable = 1ull << 28,
    // Render target will not FinalizeFastClear; Caches and meta data will be flushed, but clearing will be skipped (avoids potentially trashing metadata)
    NoFastClearFinalize = 1ull << 29,
    // Hint to the driver that this resource is managed properly by the engine for Alternate-Frame-Rendering in mGPU usage.
    AFRManual = 1ull << 30,
    // Workaround for 128^3 volume textures getting bloated 4x due to tiling mode on some platforms.
    ReduceMemoryWithTilingMode = 1ull << 31,
    /** Texture should be allocated from transient memory. */
    Transient = None,
    /** Texture needs to support atomic operations */
    AtomicCompatible = 1ull << 33,
    /** Texture should be allocated for external access. Vulkan only */
    External = 1ull << 34,
    /** Don't automatically transfer across GPUs in multi-GPU scenarios.  For example, if you are transferring it yourself manually. */
    MultiGPUGraphIgnore = 1ull << 35,
    /** Texture needs to support atomic operations */
    Atomic64Compatible = 1ull << 36,
};
ENUM_CLASS_FLAGS(TextureCreateFlags);


class RHITexture : public RHIResource
{
public:
    /** Initialization constructor. */
    RHITexture(RHIResourceType InResourceType, std::uint32_t InSizeX, std::uint32_t InSizeY, std::uint32_t InSizeZ,  std::uint32_t InNumMips, std::uint32_t InNumSamples, std::uint8_t InFormat, TextureCreateFlags InFlags)
        : RHIResource(InResourceType),
          SizeX(InSizeX),
          SizeY(InSizeY),
          SizeZ(InSizeZ),
          NumMips(InNumMips),
          NumSamples(InNumSamples),
          Format(InFormat),
          Flags(InFlags)
    {}

    virtual ~RHITexture();

    /** @return The width of the texture. */
    virtual std::uint32_t GetSizeX() const { return SizeX; }
    
     /** @return The height of the texture. */
    virtual std::uint32_t GetSizeY() const { return SizeY; }
    
     /** @return The depth of the texture. */
    virtual std::uint32_t GetSizeZ() const { return SizeZ; }
    
    virtual std::uint32_t GetNumMips() const { return  NumMips; }
    
    virtual std::uint8_t GetFormat() const { return Format; }
private:
    std::uint32_t NumMips;
    std::uint32_t NumSamples;
    std::uint8_t  Format;
    TextureCreateFlags Flags;
    std::uint32_t SizeX;
    std::uint32_t SizeY;
    std::uint32_t SizeZ;
};

class RHITexture1D : public RHITexture
{
public:
    /** Initialization constructor. */
    RHITexture1D(std::uint32_t InSizeX, std::uint32_t InNumMips, std::uint8_t InFormat, TextureCreateFlags InFlags)
        : RHITexture(RRT_Texture1D, InSizeX, 1, 1, InNumMips, 1, InFormat, InFlags) 
    {}
    // Dynamic cast methods.
    virtual RHITexture1D* GetTexture1D() { return this; }
};

class RHITexture2D : public RHITexture
{
public:
    /** Initialization constructor. */
    RHITexture2D(std::uint32_t InSizeX, std::uint32_t InSizeY, std::uint32_t InNumMips, std::uint8_t InFormat, TextureCreateFlags InFlags)
        : RHITexture(RRT_Texture2D, InSizeX, InSizeY, 0, InNumMips, 1, InFormat, InFlags)
      
    {}
    virtual ~RHITexture2D();
    // Dynamic cast methods.
    virtual RHITexture2D* GetTexture2D() { return this; }
};

class RHITexture2DArray : public RHITexture
{
public:
    /** Initialization constructor. */
    RHITexture2DArray(std::uint32_t InSizeX, std::uint32_t InSizeY, std::uint32_t InArraySize, std::uint32_t InNumMips, std::uint8_t InFormat, TextureCreateFlags InFlags)
        : RHITexture(RRT_Texture2DArray, InSizeX, InSizeY, 0, InNumMips, 1, InFormat, InFlags)

    {}
    // Dynamic cast methods.
    virtual RHITexture2DArray* GetTexture2DArray() { return this; }

};

class RHITexture3D : public RHITexture
{
public:
    /** Initialization constructor. */
    RHITexture3D(std::uint32_t InSizeX, std::uint32_t InSizeY, std::uint32_t InSizeZ, std::uint32_t InNumMips, std::uint8_t InFormat, TextureCreateFlags InFlags)
        : RHITexture(RRT_Texture3D, InSizeX, InSizeY, InSizeZ, InNumMips, 1, InFormat, InFlags)

    {}
    // Dynamic cast methods.
    virtual RHITexture3D* GetTexture3D() { return this; }
};

class RHITextureCube : public RHITexture
{
public:
    /** Initialization constructor. */
    RHITextureCube(std::uint32_t InSize, std::uint32_t InNumMips, std::uint8_t InFormat, TextureCreateFlags InFlags)
        : RHITexture(RRT_TextureCube, InSize, InSize, InSize, InNumMips, 1, InFormat, InFlags)

    {}
    // Dynamic cast methods.
    virtual RHITextureCube* GetTextureCube() { return this; }
};

class RHITextureCubeArray : public RHITexture
{
public:
    /** Initialization constructor. */
    RHITextureCubeArray(std::uint32_t InSize, std::uint32_t InArraySize, std::uint32_t InNumMips, std::uint8_t InFormat, TextureCreateFlags InFlags)
        : RHITexture(RRT_TextureCube, InSize, InSize, InSize, InNumMips, 1, InFormat, InFlags)

    {}
    // Dynamic cast methods.
    virtual RHITextureCubeArray* GetTextureCube() { return this; }
};

