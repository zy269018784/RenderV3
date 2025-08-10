#include <Render/RenderBuffer.h>
namespace Render {


    RenderBuffer::RenderBuffer(size_t width, size_t height, Allocator* allocator)
    {
        if (allocator) {
            alloc = allocator;
            m_width = width;
            m_height = height;
            colorBuffer = nullptr;
            accBuffer = nullptr;
            Resize(width, height);
        }
    }


    RenderBuffer::~RenderBuffer()
    {
        if (colorBuffer != nullptr) {
#ifdef OPENGL_RENDER
            alloc->deallocate_object<MemoryBuffer<uint8_t>>(colorBuffer, 1);
#else
            alloc->deallocate_object<MemoryBuffer<Float>>(colorBuffer, 1);
#endif
        }
        if (accBuffer != nullptr) {
            alloc->deallocate_object<MemoryBuffer<Float>>(accBuffer, 1);
        }
    }

    void RenderBuffer::Resize(size_t width, size_t height)
    {
        m_width = width;
        m_height = height;
        if (colorBuffer != nullptr) {
#ifdef OPENGL_RENDER
            alloc->deallocate_object<MemoryBuffer<uint8_t>>(colorBuffer, 1);
#else
            alloc->deallocate_object<MemoryBuffer<Float>>(colorBuffer, 1);
#endif
        }
        if (accBuffer != nullptr) {
            alloc->deallocate_object<MemoryBuffer<Float>>(accBuffer, 1);
        }
#ifdef OPENGL_RENDER
        colorBuffer = alloc->new_object<MemoryBuffer<uint8_t>>(3 * width * height, alloc);
#else
        colorBuffer = alloc->new_object<MemoryBuffer<Float>>(3 * width * height, alloc);
#endif
        accBuffer = alloc->new_object<MemoryBuffer<Float>>(3 * width * height, alloc);
    }

    void* RenderBuffer::getColorBufferPointer()
    {
        return colorBuffer->getPointer();
    }

    size_t RenderBuffer::getColorBufferSize() {
        return colorBuffer->Size();
    }

    CPU_GPU void* RenderBuffer::getAccBufferPointer() {
        return accBuffer->getPointer();
    }

}