#pragma once
#include <cpu_gpu.h>
#include <Memory/PolymorphicAllocator.h>
#include <Math/Util.h>
namespace Render {
    template <typename T, class Allocator = PolymorphicAllocator>
    class Vector {
    public:
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = T*;
        using const_pointer = const T*;
        using iterator = T*;
        using const_iterator = const T*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const iterator>;

        Vector(const Allocator& alloc = {}) : alloc(alloc) {}
        Vector(size_t count, const T& value, const Allocator& alloc = {}) : alloc(alloc) {
            reserve(count);
            for (size_t i = 0; i < count; ++i)
                this->alloc.template construct<T>(ptr + i, value);
            nStored = count;
        }
        Vector(size_t count, const Allocator& alloc = {}) : Vector(count, T{}, alloc) {}
        Vector(const Vector& other, const Allocator& alloc = {}) : alloc(alloc) {
            reserve(other.size());
            for (size_t i = 0; i < other.size(); ++i)
                this->alloc.template construct<T>(ptr + i, other[i]);
            nStored = other.size();
        }
        template <class InputIt>
        Vector(InputIt first, InputIt last, const Allocator& alloc = {}) : alloc(alloc) {
            reserve(last - first);
            size_t i = 0;
            for (InputIt iter = first; iter != last; ++iter, ++i)
                this->alloc.template construct<T>(ptr + i, *iter);
            nStored = nAlloc;
        }
        Vector(Vector&& other) : alloc(other.alloc) {
            nStored = other.nStored;
            nAlloc = other.nAlloc;
            ptr = other.ptr;

            other.nStored = other.nAlloc = 0;
            other.ptr = nullptr;
        }
        Vector(Vector&& other, const Allocator& alloc) {
            if (alloc == other.alloc) {
                ptr = other.ptr;
                nAlloc = other.nAlloc;
                nStored = other.nStored;

                other.ptr = nullptr;
                other.nAlloc = other.nStored = 0;
            }
            else {
                reserve(other.size());
                for (size_t i = 0; i < other.size(); ++i)
                    alloc.template construct<T>(ptr + i, std::move(other[i]));
                nStored = other.size();
            }
        }
        Vector(std::initializer_list<T> init, const Allocator& alloc = {})
            : Vector(init.begin(), init.end(), alloc) {}

        Vector& operator=(const Vector& other) {
            if (this == &other)
                return *this;

            clear();
            reserve(other.size());
            for (size_t i = 0; i < other.size(); ++i)
                alloc.template construct<T>(ptr + i, other[i]);
            nStored = other.size();

            return *this;
        }
        Vector& operator=(Vector&& other) {
            if (this == &other)
                return *this;

            if (alloc == other.alloc) {
                Swap(ptr, other.ptr);
                Swap(nAlloc, other.nAlloc);
                Swap(nStored, other.nStored);
            }
            else {
                clear();
                reserve(other.size());
                for (size_t i = 0; i < other.size(); ++i)
                    alloc.template construct<T>(ptr + i, std::move(other[i]));
                nStored = other.size();
            }

            return *this;
        }
        Vector& operator=(std::initializer_list<T>& init) {
            reserve(init.size());
            clear();
            iterator iter = begin();
            for (const auto& value : init) {
                *iter = value;
                ++iter;
            }
            return *this;
        }

        void assign(size_type count, const T& value) {
            clear();
            reserve(count);
            for (size_t i = 0; i < count; ++i)
                push_back(value);
        }
        template <class InputIt>
        void assign(InputIt first, InputIt last) {
            // LOG_FATAL("TODO");
            // TODO
        }
        void assign(std::initializer_list<T>& init) { assign(init.begin(), init.end()); }

        ~Vector() {
            clear();
            alloc.deallocate_object(ptr, nAlloc);
        }

        CPU_GPU
            iterator begin() { return ptr; }
        CPU_GPU
            iterator end() { return ptr + nStored; }
        CPU_GPU
            const_iterator begin() const { return ptr; }
        CPU_GPU
            const_iterator end() const { return ptr + nStored; }
        CPU_GPU
            const_iterator cbegin() const { return ptr; }
        CPU_GPU
            const_iterator cend() const { return ptr + nStored; }

        CPU_GPU
            reverse_iterator rbegin() { return reverse_iterator(end()); }
        CPU_GPU
            reverse_iterator rend() { return reverse_iterator(begin()); }
        CPU_GPU
            const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        CPU_GPU
            const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        allocator_type get_allocator() const { return alloc; }
        CPU_GPU
            size_t size() const { return nStored; }
        CPU_GPU
            bool empty() const { return size() == 0; }
        CPU_GPU
            size_t max_size() const { return (size_t)-1; }
        CPU_GPU
            size_t capacity() const { return nAlloc; }
        void reserve(size_t n) {
            if (nAlloc >= n)
                return;

            T* ra = alloc.template allocate_object<T>(n);
            for (int i = 0; i < nStored; ++i) {
                alloc.template construct<T>(ra + i, std::move(begin()[i]));
                alloc.destroy(begin() + i);
            }

            alloc.deallocate_object(ptr, nAlloc);
            nAlloc = n;
            ptr = ra;
        }
        // TODO: shrink_to_fit

        CPU_GPU
            reference operator[](size_type index) {
            //DCHECK_LT(index, size());
            return ptr[index];
        }
        CPU_GPU
            const_reference operator[](size_type index) const {
            //DCHECK_LT(index, size());
            return ptr[index];
        }
        CPU_GPU
            reference front() { return ptr[0]; }
        CPU_GPU
            const_reference front() const { return ptr[0]; }
        CPU_GPU
            reference back() { return ptr[nStored - 1]; }
        CPU_GPU
            const_reference back() const { return ptr[nStored - 1]; }
        CPU_GPU
            pointer data() { return ptr; }
        CPU_GPU
            const_pointer data() const { return ptr; }

        void clear() {
            for (int i = 0; i < nStored; ++i)
                alloc.destroy(&ptr[i]);
            nStored = 0;
        }

        iterator insert(const_iterator, const T& value) {
            // TODO
            // LOG_FATAL("TODO");
        }
        iterator insert(const_iterator, T&& value) {
            // TODO
            // LOG_FATAL("TODO");
        }
        iterator insert(const_iterator pos, size_type count, const T& value) {
            // TODO
            // LOG_FATAL("TODO");
        }
        template <class InputIt>
        iterator insert(const_iterator pos, InputIt first, InputIt last) {
            if (pos == end()) {
                size_t firstOffset = size();
                for (auto iter = first; iter != last; ++iter)
                    push_back(*iter);
                return begin() + firstOffset;
            }
            //else
            //    LOG_FATAL("TODO");
        }
        iterator insert(const_iterator pos, std::initializer_list<T> init) {
            // TODO
            // LOG_FATAL("TODO");
        }

        template <class... Args>
        iterator emplace(const_iterator pos, Args &&...args) {
            // TODO
            // LOG_FATAL("TODO");
        }
        template <class... Args>
        void emplace_back(Args &&...args) {
            if (nAlloc == nStored)
                reserve(nAlloc == 0 ? 4 : 2 * nAlloc);

            alloc.construct(ptr + nStored, std::forward<Args>(args)...);
            ++nStored;
        }

        iterator erase(const_iterator pos) {
            // TODO
            // LOG_FATAL("TODO");
        }
        iterator erase(const_iterator first, const_iterator last) {
            // TODO
            // LOG_FATAL("TODO");
        }

        void push_back(const T& value) {
            if (nAlloc == nStored)
                reserve(nAlloc == 0 ? 4 : 2 * nAlloc);

            alloc.construct(ptr + nStored, value);
            ++nStored;
        }
        void push_back(T&& value) {
            if (nAlloc == nStored)
                reserve(nAlloc == 0 ? 4 : 2 * nAlloc);

            alloc.construct(ptr + nStored, std::move(value));
            ++nStored;
        }
        void pop_back() {
           // DCHECK(!empty());
            alloc.destroy(ptr + nStored - 1);
            --nStored;
        }

        void resize(size_type n) {
            if (n < size()) {
                for (size_t i = n; i < size(); ++i)
                    alloc.destroy(ptr + i);
                if (n == 0) {
                    alloc.deallocate_object(ptr, nAlloc);
                    ptr = nullptr;
                    nAlloc = 0;
                }
            }
            else if (n > size()) {
                reserve(n);
                for (size_t i = nStored; i < n; ++i)
                    alloc.construct(ptr + i);
            }
            nStored = n;
        }
        void resize(size_type count, const value_type& value) {
            // TODO
           // LOG_FATAL("TODO");
        }

        void swap(Vector& other) {
            //CHECK(alloc == other.alloc);  // TODO: handle this
            std::swap(ptr, other.ptr);
            std::swap(nAlloc, other.nAlloc);
            std::swap(nStored, other.nStored);
        }

    private:
        Allocator alloc;
        T* ptr = nullptr;
        size_t nAlloc = 0, nStored = 0;
    };

}