#pragma once
#include <Memory/MemoryResource.h>
#include <stdlib.h>
#include <cstring>
namespace Render {
    class CpuMemoryResource : public MemoryResource {
    private:
        void* do_allocate(size_t size, size_t alignment);
        void do_deallocate(void* ptr, size_t bytes, size_t alignment);
        void do_memset(void* devPtr, int value, size_t count);
        bool do_is_equal(const MemoryResource& other) const noexcept;
        long long do_count() const;
    private:
        long long count;
    public:
        static CpuMemoryResource singleton;
    };


}