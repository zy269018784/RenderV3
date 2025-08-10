#pragma once
#include <cpu_gpu.h>
#include <Memory/PolymorphicAllocator.h>
#include <iterator>
#include <Math/Util.h>
namespace Render {
    template <typename T, int N, class Allocator = PolymorphicAllocator>
    class InlinedVector {
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
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        InlinedVector(const Allocator& alloc = {}) : alloc(alloc) {}
        InlinedVector(size_t count, const T& value, const Allocator& alloc = {})
            : alloc(alloc) {
            reserve(count);
            for (size_t i = 0; i < count; ++i)
                this->alloc.template construct<T>(begin() + i, value);
            nStored = count;
        }
        InlinedVector(size_t count, const Allocator& alloc = {})
            : InlinedVector(count, T{}, alloc) {}
        InlinedVector(const InlinedVector& other, const Allocator& alloc = {})
            : alloc(alloc) {
            reserve(other.size());
            for (size_t i = 0; i < other.size(); ++i)
                this->alloc.template construct<T>(begin() + i, other[i]);
            nStored = other.size();
        }
        template <class InputIt>
        InlinedVector(InputIt first, InputIt last, const Allocator& alloc = {})
            : alloc(alloc) {
            reserve(last - first);
            for (InputIt iter = first; iter != last; ++iter, ++nStored)
                this->alloc.template construct<T>(begin() + nStored, *iter);
        }
        InlinedVector(InlinedVector&& other) : alloc(other.alloc) {
            nStored = other.nStored;
            nAlloc = other.nAlloc;
            ptr = other.ptr;
            if (other.nStored <= N)
                for (int i = 0; i < other.nStored; ++i)
                    alloc.template construct<T>(fixed + i, std::move(other.fixed[i]));
            // Leave other.nStored as is, so that the detrius left after we
            // moved out of fixed has its destructors run...
            else
                other.nStored = 0;

            other.nAlloc = 0;
            other.ptr = nullptr;
        }
        InlinedVector(InlinedVector&& other, const Allocator& alloc) {

            if (alloc == other.alloc) {
                ptr = other.ptr;
                nAlloc = other.nAlloc;
                nStored = other.nStored;
                if (other.nStored <= N)
                    for (int i = 0; i < other.nStored; ++i)
                        fixed[i] = std::move(other.fixed[i]);

                other.ptr = nullptr;
                other.nAlloc = other.nStored = 0;
            }
            else {
                reserve(other.size());
                for (size_t i = 0; i < other.size(); ++i)
                    alloc.template construct<T>(begin() + i, std::move(other[i]));
                nStored = other.size();
            }
        }
        InlinedVector(std::initializer_list<T> init, const Allocator& alloc = {})
            : InlinedVector(init.begin(), init.end(), alloc) {}

        InlinedVector& operator=(const InlinedVector& other) {
            if (this == &other)
                return *this;

            clear();
            reserve(other.size());
            for (size_t i = 0; i < other.size(); ++i)
                alloc.template construct<T>(begin() + i, other[i]);
            nStored = other.size();

            return *this;
        }
        InlinedVector& operator=(InlinedVector&& other) {
            if (this == &other)
                return *this;

            clear();
            if (alloc == other.alloc) {
                Swap(ptr, other.ptr);
                Swap(nAlloc, other.nAlloc);
                Swap(nStored, other.nStored);
                if (nStored > 0 && !ptr) {
                    for (int i = 0; i < nStored; ++i)
                        alloc.template construct<T>(fixed + i, std::move(other.fixed[i]));
                    other.nStored = nStored;  // so that dtors run...
                }
            }
            else {
                reserve(other.size());
                for (size_t i = 0; i < other.size(); ++i)
                    alloc.template construct<T>(begin() + i, std::move(other[i]));
                nStored = other.size();
            }

            return *this;
        }
        InlinedVector& operator=(std::initializer_list<T>& init) {
            clear();
            reserve(init.size());
            for (const auto& value : init) {
                alloc.template construct<T>(begin() + nStored, value);
                ++nStored;
            }
            return *this;
        }

        void assign(size_type count, const T& value) {
            clear();
            reserve(count);
            for (size_t i = 0; i < count; ++i)
                alloc.template construct<T>(begin() + i, value);
            nStored = count;
        }
        template <class InputIt>
        void assign(InputIt first, InputIt last) {
            // TODO
            //LOG_FATAL("TODO");
        }
        void assign(std::initializer_list<T>& init) { assign(init.begin(), init.end()); }

        ~InlinedVector() {
            clear();
            alloc.deallocate_object(ptr, nAlloc);
        }

        CPU_GPU
            iterator begin() { return ptr ? ptr : fixed; }
        CPU_GPU
            iterator end() { return begin() + nStored; }
        CPU_GPU
            const_iterator begin() const { return ptr ? ptr : fixed; }
        CPU_GPU
            const_iterator end() const { return begin() + nStored; }
        CPU_GPU
            const_iterator cbegin() const { return ptr ? ptr : fixed; }
        CPU_GPU
            const_iterator cend() const { return begin() + nStored; }

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
            size_t capacity() const { return ptr ? nAlloc : N; }

        void reserve(size_t n) {
            if (capacity() >= n)
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
            return begin()[index];
        }
        CPU_GPU
            const_reference operator[](size_type index) const {
            return begin()[index];
        }
        CPU_GPU
            reference front() { return *begin(); }
        CPU_GPU
            const_reference front() const { return *begin(); }
        CPU_GPU
            reference back() { return *(begin() + nStored - 1); }
        CPU_GPU
            const_reference back() const { return *(begin() + nStored - 1); }
        CPU_GPU
            pointer data() { return ptr ? ptr : fixed; }
        CPU_GPU
            const_pointer data() const { return ptr ? ptr : fixed; }

        void clear() {
            for (int i = 0; i < nStored; ++i)
                alloc.destroy(begin() + i);
            nStored = 0;
        }

        iterator insert(const_iterator, const T& value) {
            // TODO
            //LOG_FATAL("TODO");
        }
        iterator insert(const_iterator, T&& value) {
            // TODO
           //LOG_FATAL("TODO");
        }
        iterator insert(const_iterator pos, size_type count, const T& value) {
            // TODO
            //LOG_FATAL("TODO");
        }
        template <class InputIt>
        iterator insert(const_iterator pos, InputIt first, InputIt last) {
            if (pos == end()) {
                reserve(size() + (last - first));
                iterator pos = end();
                for (auto iter = first; iter != last; ++iter, ++pos)
                    alloc.template construct<T>(pos, *iter);
                nStored += last - first;
                return pos;
            }
            else {
                // TODO
                //LOG_FATAL("TODO");
            }
        }
        iterator insert(const_iterator pos, std::initializer_list<T> init) {
            // TODO
            //LOG_FATAL("TODO");
        }

        template <class... Args>
        iterator emplace(const_iterator pos, Args &&...args) {
            // TODO
            //LOG_FATAL("TODO");
        }
        template <class... Args>
        void emplace_back(Args &&...args) {
            // TODO
            //LOG_FATAL("TODO");
        }

        iterator erase(const_iterator cpos) {
            iterator pos =
                begin() + (cpos - begin());  // non-const iterator, thank you very much
            while (pos != end() - 1) {
                *pos = std::move(*(pos + 1));
                ++pos;
            }
            alloc.destroy(pos);
            --nStored;
            return begin() + (cpos - begin());
        }
        iterator erase(const_iterator first, const_iterator last) {
            // TODO
            //LOG_FATAL("TODO");
        }

        void push_back(const T& value) {
            if (size() == capacity())
                reserve(2 * capacity());

            alloc.construct(begin() + nStored, value);
            ++nStored;
        }
        void push_back(T&& value) {
            if (size() == capacity())
                reserve(2 * capacity());

            alloc.construct(begin() + nStored, std::move(value));
            ++nStored;
        }
        void pop_back() {
            DCHECK(!empty());
            alloc.destroy(begin() + nStored - 1);
            --nStored;
        }

        void resize(size_type n) {
            if (n < size()) {
                for (size_t i = n; n < size(); ++i)
                    alloc.destroy(begin() + i);
            }
            else if (n > size()) {
                reserve(n);
                for (size_t i = nStored; i < n; ++i)
                    alloc.construct(begin() + i);
            }
            nStored = n;
        }
        void resize(size_type count, const value_type& value) {
            // TODO
            //LOG_FATAL("TODO");
        }

        void swap(InlinedVector& other) {
            // TODO
            //LOG_FATAL("TODO");
        }

    private:
        Allocator alloc;
        // ptr non-null is discriminator for whether fixed[] is valid...
        T* ptr = nullptr;
        union {
            T fixed[N];
        };
        size_t nAlloc = 0, nStored = 0;
    };

}