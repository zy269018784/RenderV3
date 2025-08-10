#pragma once
#include <cpu_gpu.h>
#include <Container/Span.h>
#include <initializer_list>
#include <Type/Types.h>
#include <Type/TypeDeclaration.h>
#include <Memory/PolymorphicAllocator.h>
#include <Math/Util.h>
/*
    Documentation:
        

*/
namespace Render {

    // Array2D Definition
    template <typename T>
    class Array2D {
    public:
        // Array2D Type Definitions
        using value_type = T;
        using iterator = value_type*;
        using const_iterator = const value_type*;
        using allocator_type = PolymorphicAllocator;

        // Array2D Public Methods
        Array2D(allocator_type allocator = {}) : Array2D({ {0, 0}, {0, 0} }, allocator) {}

        Array2D(Bounds2i extent, Allocator allocator = {})
            : extent(extent), allocator(allocator) {
            int n = extent.Area();
            values = allocator.allocate_object<T>(n);
            for (int i = 0; i < n; ++i)
                allocator.construct(values + i);
        }

        Array2D(Bounds2i extent, T def, allocator_type allocator = {})
            : Array2D(extent, allocator) {
            std::fill(begin(), end(), def);
        }
        template <typename InputIt,
            typename = typename std::enable_if_t<
            !std::is_integral_v<InputIt>&&
            std::is_base_of<
            std::input_iterator_tag,
            typename std::iterator_traits<InputIt>::iterator_category>::value>>
            Array2D(InputIt first, InputIt last, int nx, int ny, allocator_type allocator = {})
            : Array2D({ {0, 0}, {nx, ny} }, allocator) {
            std::copy(first, last, begin());
        }
        Array2D(int nx, int ny, allocator_type allocator = {})
            : Array2D({ {0, 0}, {nx, ny} }, allocator) {}
        Array2D(int nx, int ny, T def, allocator_type allocator = {})
            : Array2D({ {0, 0}, {nx, ny} }, def, allocator) {}
        Array2D(const Array2D& a, allocator_type allocator = {})
            : Array2D(a.begin(), a.end(), a.XSize(), a.YSize(), allocator) {}

        ~Array2D() {
            int n = extent.Area();
            for (int i = 0; i < n; ++i)
                allocator.destroy(values + i);
            allocator.deallocate_object(values, n);
        }

        Array2D(Array2D&& a, allocator_type allocator = {})
            : extent(a.extent), allocator(allocator) {
            if (allocator == a.allocator) {
                values = a.values;
                a.extent = Bounds2i({ 0, 0 }, { 0, 0 });
                a.values = nullptr;
            }
            else {
                values = allocator.allocate_object<T>(extent.Area());
                std::copy(a.begin(), a.end(), begin());
            }
        }
        Array2D& operator=(const Array2D& a) = delete;

        Array2D& operator=(Array2D&& other) {
            if (allocator == other.allocator) {
                Swap(extent, other.extent);
                Swap(values, other.values);
            }
            else if (extent == other.extent) {
                int n = extent.Area();
                for (int i = 0; i < n; ++i) {
                    allocator.destroy(values + i);
                    allocator.construct(values + i, other.values[i]);
                }
                extent = other.extent;
            }
            else {
                int n = extent.Area();
                for (int i = 0; i < n; ++i)
                    allocator.destroy(values + i);
                allocator.deallocate_object(values, n);

                int no = other.extent.Area();
                values = allocator.allocate_object<T>(no);
                for (int i = 0; i < no; ++i)
                    allocator.construct(values + i, other.values[i]);
            }
            return *this;
        }

        CPU_GPU
            T& operator[](Point2i p) {
            //DCHECK(InsideExclusive(p, extent));
            p.x -= extent.pMin.x;
            p.y -= extent.pMin.y;
            return values[p.x + (extent.pMax.x - extent.pMin.x) * p.y];
        }
        CPU_GPU T& operator()(int x, int y) { return (*this)[{x, y}]; }

        CPU_GPU
            const T& operator()(int x, int y) const { return (*this)[{x, y}]; }
        CPU_GPU
            const T& operator[](Point2i p) const {
            //DCHECK(InsideExclusive(p, extent));
            p.x -= extent.pMin.x;
            p.y -= extent.pMin.y;
            return values[p.x + (extent.pMax.x - extent.pMin.x) * p.y];
        }

        CPU_GPU
            int size() const { return extent.Area(); }
        CPU_GPU
            int XSize() const { return extent.pMax.x - extent.pMin.x; }
        CPU_GPU
            int YSize() const { return extent.pMax.y - extent.pMin.y; }

        CPU_GPU
            iterator begin() { return values; }
        CPU_GPU
            iterator end() { return begin() + size(); }

        CPU_GPU
            const_iterator begin() const { return values; }
        CPU_GPU
            const_iterator end() const { return begin() + size(); }

        CPU_GPU
            operator Span<T>() { return Span<T>(values, size()); }
        CPU_GPU
            operator Span<const T>() const { return Span<const T>(values, size()); }

        //std::string ToString() const {
        //    std::string s = StringPrintf("[ Array2D extent: %s values: [", extent);
        //    for (int y = extent.pMin.y; y < extent.pMax.y; ++y) {
        //        s += " [ ";
        //        for (int x = extent.pMin.x; x < extent.pMax.x; ++x) {
        //            T value = (*this)(x, y);
        //            s += StringPrintf("%s, ", value);
        //        }
        //        s += "], ";
        //    }
        //    s += " ] ]";
        //    return s;
        //}

    private:
        // Array2D Private Members
        Bounds2i extent;
        Allocator allocator;
        T* values;
    };

}