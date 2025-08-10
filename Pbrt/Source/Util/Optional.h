#pragma once
#include <cpu_gpu.h>
#include <iostream>
#include <stdio.h>


namespace Render {
    template <typename T>
    class Optional {
    public:
        using value_type = T;

        Optional() = default;

        CPU_GPU
            Optional(const T& v) : set(true) { new (ptr()) T(v); }
        CPU_GPU
            Optional(T&& v) : set(true) { new (ptr()) T(std::move(v)); }
        CPU_GPU
            Optional(const Optional& v) : set(v.has_value()) {
            if (v.has_value())
                new (ptr()) T(v.value());
        }
        CPU_GPU
            Optional(Optional&& v) : set(v.has_value()) {
            if (v.has_value()) {
                new (ptr()) T(std::move(v.value()));
                v.reset();
            }
        }

        CPU_GPU
            Optional& operator=(const T& v) {  
            reset();
            new (ptr()) T(v);
            set = true;
            return *this;
        }
        
        CPU_GPU
            Optional& operator=(T&& v) {
            reset();
            new (ptr()) T(std::move(v));
            set = true;
            return *this;
        }
        
        CPU_GPU
            Optional& operator=(const Optional& v) {
            reset();
            if (v.has_value()) {
                new (ptr()) T(v.value());
                set = true;
            }
            return *this;
        }
        
        CPU_GPU
            Optional& operator=(Optional&& v) {
            reset();
            if (v.has_value()) {
                new (ptr()) T(std::move(v.value()));
                set = true;
                v.reset();
            }
            return *this;
        }

        CPU_GPU
            ~Optional() { reset(); }

        CPU_GPU
            explicit operator bool() const { return set; }

        CPU_GPU
            T value_or(const T& alt) const { return set ? value() : alt; }

        CPU_GPU
            T* operator->() { return &value(); }

        CPU_GPU
            const T* operator->() const { return &value(); }

        CPU_GPU
            T& operator*() { return value(); }

        CPU_GPU
            const T& operator*() const { return value(); }

        CPU_GPU
            T& value() {
            return *ptr();
        }

        CPU_GPU 
            const T& value() const {
            return *ptr();
        }

        CPU_GPU
            void reset() {
            if (set) {
                value().~T();
                set = false;
            }
        }

        CPU_GPU
            bool has_value() const { return set; }

        CPU_GPU
            T* ptr() { return std::launder(reinterpret_cast<T*>(&optionalValue)); }

        CPU_GPU
            const T* ptr() const {
            return std::launder(reinterpret_cast<const T*>(&optionalValue));
        }
    private:
        std::aligned_storage_t<sizeof(T), alignof(T)> optionalValue;
        bool set = false;
    };
}