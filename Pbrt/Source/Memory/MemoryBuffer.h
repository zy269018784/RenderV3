#pragma once
#include <cpu_gpu.h>
#include <Memory/PolymorphicAllocator.h>
#include <stdio.h>
namespace Render {
    template <typename FORMAT>
    class MemoryBuffer
    {
    public:
        CPU_GPU MemoryBuffer(size_t count, Allocator* alloc = nullptr);
        CPU_GPU ~MemoryBuffer();
        CPU_GPU void Resize(size_t count);
        CPU_GPU size_t Size();
        CPU_GPU size_t Count();
        CPU_GPU FORMAT* getPointer();

    private:
        Allocator* alloc = nullptr;
        FORMAT* m_ptr = nullptr;
        size_t m_size = 0;
        size_t m_count = 0;
    };

    template <typename FORMAT>
    MemoryBuffer<FORMAT>::MemoryBuffer(size_t count, Allocator* allocator)
    {
        if (allocator) {
            alloc = allocator;
            m_count = count;
            m_size = count * sizeof(FORMAT);
            Resize(count);
        }
    }

    template <typename FORMAT>
    MemoryBuffer<FORMAT>::~MemoryBuffer()
    {
        alloc->deallocate_object<FORMAT>(m_ptr, m_count);
        //alloc->deallocate_bytes(m_ptr, m_size);
    }

    template <typename FORMAT>
    size_t MemoryBuffer<FORMAT>::Size()
    {
        return m_size;
    }

    template <typename FORMAT>
    size_t MemoryBuffer<FORMAT>::Count()
    {
        return m_count;
    }


    template <typename FORMAT>
    void MemoryBuffer<FORMAT>::Resize(size_t count)
    {   
        if (count > 0) {
            if (m_ptr != nullptr)
                alloc->deallocate_object<FORMAT>(m_ptr, m_count);
            m_count = count;
            m_size = count * sizeof(FORMAT);
            m_ptr = alloc->allocate_object<FORMAT>(m_count);
        }
    }

    template <typename FORMAT>
    FORMAT* MemoryBuffer<FORMAT>::getPointer()
    {
        return m_ptr;
    }
}