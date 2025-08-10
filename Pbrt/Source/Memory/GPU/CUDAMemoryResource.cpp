#include <Memory/GPU/CUDAMemoryResource.h>
#if RENDER_IS_GPU_CODE
    #include <cuda.h>
    #include <cuda_runtime_api.h>
#endif
namespace Render {
    CUDAMemoryResource CUDAMemoryResource::singleton;

    void* CUDAMemoryResource::do_allocate(size_t bytes, size_t alignment) {
        void* ptr;
#if RENDER_IS_GPU_CODE
        cudaMallocManaged(&ptr, bytes);
#endif
        count += bytes;
        return ptr;
    }

    void CUDAMemoryResource::do_deallocate(void* p, size_t bytes, size_t alignment) {
#if RENDER_IS_GPU_CODE
        cudaFree(p);
#endif
        count -= bytes;
    }

    void CUDAMemoryResource::do_memset(void* devPtr, int value, size_t count) {
#if RENDER_IS_GPU_CODE
        cudaMemset(devPtr, value, count);
#endif
    }

    bool CUDAMemoryResource::do_is_equal(const MemoryResource& other) const noexcept {
        return this == &other;
    }

    long long CUDAMemoryResource::do_count() const {
        return count;
    }
}