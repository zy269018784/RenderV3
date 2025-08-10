#pragma once
#include <Memory/MemoryResource.h>
#include <cstddef>
#include <type_traits>
#include <utility>
namespace Render {
    MemoryResource* new_delete_resource() noexcept;

    class PolymorphicAllocator {
    public:
        using value_type = std::byte;

        // 默认CPU内存分配器
        PolymorphicAllocator() noexcept {
            memoryResource = new_delete_resource();
        }

        PolymorphicAllocator(MemoryResource* r) : memoryResource(r) {
        }

        // 拷贝构造函数
        PolymorphicAllocator(const PolymorphicAllocator& other) noexcept
            : memoryResource(other.resource()) { }

        // 删除赋值构造函数
        PolymorphicAllocator& operator=(const PolymorphicAllocator& rhs) = delete;

        // 已分配的字节数
        long long count() const {
            return resource()->count();
        }

        // 分配nbytes个字节.
        [[nodiscard]] value_type* allocate(size_t n) {
            return static_cast<value_type*>(resource()->allocate(n * sizeof(value_type), alignof(value_type)));
        }

        // 释放nbytes个字节.
        void deallocate(value_type* p, size_t n) {
            resource()->deallocate(p, n);
        }

        // 分配nbytes个字节, 按alignment对齐.
        void* allocate_bytes(size_t nbytes, size_t alignment = alignof(max_align_t)) {
            return resource()->allocate(nbytes, alignment);
        }

        // 释放nbytes个字节, 按alignment对齐.
        void deallocate_bytes(void* p, size_t nbytes,
            size_t alignment = alignof(std::max_align_t)) {
            return resource()->deallocate(p, nbytes, alignment);
        }

        // 分配n个类型T的对象.
        template <class T>
        T* allocate_object(size_t n = 1) {
            return static_cast<T*>(allocate_bytes(n * sizeof(T), alignof(T)));
        }

        // 释放n个类型T的对象.
        template <class T>
        void deallocate_object(T* p, size_t n = 1) {
            deallocate_bytes(p, n * sizeof(T), alignof(T));
        }

        // new一个类型T的对象.
        template <class T, class... Args>
        T* new_object(Args &&...args) {
            // NOTE: this doesn't handle constructors that throw exceptions...
            T* p = allocate_object<T>();
            construct(p, std::forward<Args>(args)...);
            return p;
        }

        // delete一个类型T的对象.
        template <class T>
        void delete_object(T* p) {
            destroy(p);
            deallocate_object(p);
        }

        // 调用类型T的构造函数.
        template <class T, class... Args>
        void construct(T* p, Args &&...args) {
            ::new ((void*)p) T(std::forward<Args>(args)...);
        }

        // 调用类型T的析构函数.
        template <class T>
        void destroy(T* p) {
            p->~T();
        }

        // 初始化内存
        template <class T>
        void memset(T* p, int value, size_t count) {
            resource()->memset(p, value, count);
        }

        // 内存分配器
        MemoryResource* resource() const { return memoryResource; }

    private:
        MemoryResource* memoryResource;
    };

    inline bool operator==(const PolymorphicAllocator& a,
        const PolymorphicAllocator& b) noexcept {
        return a.resource() == b.resource();
    }

    inline bool operator!=(const PolymorphicAllocator& a,
        const PolymorphicAllocator& b) noexcept {
        return !(a == b);
    }

    using Allocator = PolymorphicAllocator;
}
