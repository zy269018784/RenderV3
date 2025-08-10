#if 0
#pragma once
#include <cpu_gpu.h>
#include <vector>
#include <type_traits>
namespace Render {
    namespace Span_internal {

        // Wrappers for access to container data pointers.
        template <typename C>
       CPU_GPU inline constexpr auto GetDataImpl(C& c, char) noexcept
            -> decltype(c.data()) {
            return c.data();
        }

        template <typename C>
        CPU_GPU inline constexpr auto GetData(C& c) noexcept -> decltype(GetDataImpl(c, 0)) {
            return GetDataImpl(c, 0);
        }

        // Detection idioms for size() and data().
        template <typename C>
        using HasSize =
            std::is_integral<typename std::decay_t<decltype(std::declval<C&>().size())>>;

        // We want to enable conversion from vector<T*> to span<const T* const> but
        // disable conversion from vector<Derived> to span<Base>. Here we use
        // the fact that U** is convertible to Q* const* if and only if Q is the same
        // type or a more cv-qualified version of U.  We also decay the result type of
        // data() to avoid problems with classes which have a member function data()
        // which returns a reference.
        template <typename T, typename C>
        using HasData =
            std::is_convertible<typename std::decay_t<decltype(GetData(std::declval<C&>()))>*,
            T* const*>;

    }  // namespace span_internal

    inline constexpr std::size_t dynamic_extent = -1;

    // Span implementation partially based on absl::Span from Google's Abseil library.
    template <typename T>
    class Span {
    public:
        // Used to determine whether a Span can be constructed from a container of
        // type C.
        template <typename C>
        using EnableIfConvertibleFrom =
            typename std::enable_if_t<Span_internal::HasData<T, C>::value&&
            Span_internal::HasSize<C>::value>;

        // Used to SFINAE-enable a function when the slice elements are const.
        template <typename U>
        using EnableIfConstView = typename std::enable_if_t<std::is_const_v<T>, U>;

        // Used to SFINAE-enable a function when the slice elements are mutable.
        template <typename U>
        using EnableIfMutableView = typename std::enable_if_t<!std::is_const_v<T>, U>;

        using value_type = typename std::remove_cv_t<T>;
        using iterator = T*;
        using const_iterator = const T*;

        CPU_GPU
            Span() : ptr(nullptr), n(0) {}
        CPU_GPU
            Span(T* ptr, size_t n) : ptr(ptr), n(n) {}
        template <size_t N>
        CPU_GPU Span(T(&a)[N]) : Span(a, N) {}
        CPU_GPU
            Span(std::initializer_list<value_type> v) : Span(v.begin(), v.size()) {}

        // Explicit reference constructor for a mutable `Span<T>` type. Can be
        // replaced with MakeSpan() to infer the type parameter.
        template <typename V, typename X = EnableIfConvertibleFrom<V>,
            typename Y = EnableIfMutableView<V>>
            CPU_GPU explicit Span(V& v) noexcept : Span(v.data(), v.size()) {}

        // Hack: explicit constructors for std::vector to work around warnings
        // about calling a host function (e.g. vector::size()) form a
        // host+device function (the regular Span constructor.)
        template <typename V>
        Span(std::vector<V>& v) noexcept : Span(v.data(), v.size()) {}
        template <typename V>
        Span(const std::vector<V>& v) noexcept : Span(v.data(), v.size()) {}

        // Implicit reference constructor for a read-only `Span<const T>` type
        template <typename V, typename X = EnableIfConvertibleFrom<V>,
            typename Y = EnableIfConstView<V>>
            CPU_GPU constexpr Span(const V& v) noexcept : Span(v.data(), v.size()) {}

        CPU_GPU
            iterator begin() { return ptr; }
        CPU_GPU
            iterator end() { return ptr + n; }
        CPU_GPU
            const_iterator begin() const { return ptr; }
        CPU_GPU
            const_iterator end() const { return ptr + n; }

        CPU_GPU
            T& operator[](size_t i) {
            return ptr[i];
        }
        CPU_GPU
            const T& operator[](size_t i) const {
            return ptr[i];
        }

        CPU_GPU
            size_t size() const { return n; };
        CPU_GPU
            bool empty() const { return size() == 0; }
        CPU_GPU
            T* data() { return ptr; }
        CPU_GPU
            const T* data() const { return ptr; }

        CPU_GPU
            T front() const { return ptr[0]; }
        CPU_GPU
            T back() const { return ptr[n - 1]; }

        CPU_GPU
            void remove_prefix(size_t count) {
            // assert(size() >= count);
            ptr += count;
            n -= count;
        }
        CPU_GPU
            void remove_suffix(size_t count) {
            // assert(size() > = count);
            n -= count;
        }

        CPU_GPU
            Span subSpan(size_t pos, size_t count = Render::dynamic_extent) {
            size_t np = count < (size() - pos) ? count : (size() - pos);
            return Span(ptr + pos, np);
        }

    private:
        T* ptr;
        size_t n;
    };

    template <int &...ExplicitArgumentBarrier, typename T>
    CPU_GPU inline constexpr Span<T> MakeSpan(T* ptr, size_t size) noexcept {
        return Span<T>(ptr, size);
    }


}

#endif