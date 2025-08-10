#pragma once
#include <cstddef>
namespace Render {
    class MemoryResource {
        static constexpr size_t max_align = alignof(std::max_align_t);
    public:
        virtual ~MemoryResource();

        // 分配nbytes个字节, 按alignment对齐.
        void* allocate(size_t bytes, size_t alignment = max_align) {
            if (bytes == 0)
                return nullptr;
            return do_allocate(bytes, alignment);
        }

        // 释放nbytes个字节, 按alignment对齐.
        void deallocate(void* p, size_t bytes, size_t alignment = max_align) {
            if (!p)
                return;
            return do_deallocate(p, bytes, alignment);
        }

        // 内存分配器释放相等
        bool is_equal(const MemoryResource& other) const noexcept {
            return do_is_equal(other);
        }

        // 初始化内存
        void memset(void* devPtr, int value, size_t count) {
            do_memset(devPtr, value, count);
        }

        // 已分配的字节数
        long long count() const {
            return do_count();
        }

    private:
        virtual void* do_allocate(size_t bytes, size_t alignment) = 0;
        virtual void do_deallocate(void* p, size_t bytes, size_t alignment) = 0;
        virtual bool do_is_equal(const MemoryResource& other) const noexcept = 0;
        virtual void do_memset(void* devPtr, int value, size_t count) = 0;
        virtual long long do_count() const = 0;
    };
}