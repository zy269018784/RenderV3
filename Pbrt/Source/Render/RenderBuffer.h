#pragma once
#include <Init.h>
#include <Math/Math.h>
#include <Memory/MemoryBuffer.h>

namespace Render {
    class RenderBuffer
    {
    public:
        CPU_GPU RenderBuffer(size_t width, size_t height, Allocator* alloc = nullptr);
        CPU_GPU ~RenderBuffer();
        CPU_GPU void Resize(size_t width, size_t height);
        CPU_GPU void* getColorBufferPointer();
        CPU_GPU void* getAccBufferPointer();
        CPU_GPU size_t getColorBufferSize();
        CPU_GPU size_t Width() { return m_width; };
        CPU_GPU size_t Height() { return m_height; };
    private:
#ifdef OPENGL_RENDER
        MemoryBuffer<unsigned char>* colorBuffer = nullptr;
#else
        MemoryBuffer<Float>* colorBuffer = nullptr;
#endif
        MemoryBuffer<Float>* accBuffer = nullptr;
        Allocator* alloc = nullptr;
        size_t m_width;
        size_t m_height;
    };

}