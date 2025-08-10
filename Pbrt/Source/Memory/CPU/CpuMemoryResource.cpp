#include <Memory/CPU/CpuMemoryResource.h>
namespace Render {
    CpuMemoryResource CpuMemoryResource::singleton;

    void* CpuMemoryResource::do_allocate(size_t bytes, size_t alignment) {
        count += bytes;
#if defined(RENDER_HAVE__ALIGNED_MALLOC)
        return _aligned_malloc(bytes, alignment);
#elif defined(RENDER_HAVE_POSIX_MEMALIGN)
        void* ptr;
        if (alignment < sizeof(void*))
            return malloc(bytes);
        if (posix_memalign(&ptr, alignment, bytes) != 0)
            ptr = nullptr;
        return ptr;
#else
        return memalign(alignment, bytes);
#endif
    }

    void CpuMemoryResource::do_deallocate(void* ptr, size_t bytes, size_t alignment) {
        if (!ptr)
            return;
#if defined(RENDER_HAVE__ALIGNED_MALLOC)
        _aligned_free(ptr);
#else
        free(ptr);
#endif
        count -= bytes;
    }

    void CpuMemoryResource::do_memset(void* devPtr, int value, size_t bytes) {
        ::memset(devPtr, value, bytes);
    }

    bool CpuMemoryResource::do_is_equal(const MemoryResource& other) const noexcept {
        return this == &other;
    }

    long long CpuMemoryResource::do_count() const {
        return count;
    }
}