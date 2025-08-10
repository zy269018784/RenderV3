#pragma once
#include <cpu_gpu.h>
#include <initializer_list>
namespace Render {
    template <typename T, int N>
    class Array {
    public:
        using value_type = T;
        using iterator = value_type*;
        using const_iterator = const value_type*;
        using size_t = std::size_t;

        Array() = default;
        CPU_GPU
            Array(std::initializer_list<T> v) {
            size_t i = 0;
            for (const T& val : v)
                values[i++] = val;
        }

        CPU_GPU
            void fill(const T& v) {
            for (int i = 0; i < N; ++i)
                values[i] = v;
        }

        CPU_GPU
            bool operator==(const Array<T, N>& a) const {
            for (int i = 0; i < N; ++i)
                if (values[i] != a.values[i])
                    return false;
            return true;
        }
        CPU_GPU
            bool operator!=(const Array<T, N>& a) const { return !(*this == a); }

        CPU_GPU
            iterator begin() { return values; }
        CPU_GPU
            iterator end() { return values + N; }
        CPU_GPU
            const_iterator begin() const { return values; }
        CPU_GPU
            const_iterator end() const { return values + N; }

        CPU_GPU
            size_t size() const { return N; }

        CPU_GPU
            T& operator[](size_t i) { return values[i]; }
        CPU_GPU
            const T& operator[](size_t i) const { return values[i]; }

        CPU_GPU
            T* data() { return values; }
        CPU_GPU
            const T* data() const { return values; }

    private:
        T values[N] = {};
    };
}