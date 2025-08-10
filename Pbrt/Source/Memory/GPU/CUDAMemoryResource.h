#pragma once

#include <Memory/MemoryResource.h>

namespace Render {
    class CUDAMemoryResource : public MemoryResource {
    private:
        void* do_allocate(size_t size, size_t alignment);
        void do_deallocate(void* p, size_t bytes, size_t alignment);
        void do_memset(void* devPtr, int value, size_t count);
        bool do_is_equal(const MemoryResource& other) const noexcept;
        long long do_count() const;
    private:
        long long count;
    public:
        static CUDAMemoryResource singleton;
    };
}

