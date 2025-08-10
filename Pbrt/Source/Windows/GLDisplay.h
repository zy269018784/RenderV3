#pragma once
#include <vector>
// #include <cuda_runtime.h>
#include <stdio.h>
#include <Init.h>

namespace Render {
	template <typename PIXEL_FORMAT>
	class GLDisplay
	{
	public:
		CPU_GPU GLDisplay(int32_t width, int32_t height, Allocator* alloc = nullptr);
		CPU_GPU ~GLDisplay();
		CPU_GPU void resize(int32_t width, int32_t height);
		size_t size();
		CPU_GPU int32_t        width() const { return m_width; };
		CPU_GPU int32_t        height() const { return m_height; };

		CPU_GPU PIXEL_FORMAT* getPointer();

	private:
		PIXEL_FORMAT* m_device_pixels = nullptr;
		PIXEL_FORMAT* m_host_zcopy_pixels = nullptr;
		//std::vector<PIXEL_FORMAT>  m_host_pixels;

		int32_t                    m_width = 0u;
		int32_t                    m_height = 0u;
		int32_t                    m_size;
		Allocator* alloc = nullptr;
	};

    template <typename PIXEL_FORMAT>
    CPU_GPU GLDisplay<PIXEL_FORMAT>::GLDisplay(int32_t width, int32_t height, Allocator* allocator)
    {
        if (allocator) {
            this->alloc = allocator;
            resize(width, height);
        }
    }

    template <typename PIXEL_FORMAT>
    CPU_GPU GLDisplay<PIXEL_FORMAT>::~GLDisplay()
    {
        alloc->deallocate_bytes(m_device_pixels, m_size);
    }

    template <typename PIXEL_FORMAT>
    CPU_GPU size_t GLDisplay<PIXEL_FORMAT>::size()
    {
        return m_size;
    }

    template <typename PIXEL_FORMAT>
    CPU_GPU void GLDisplay<PIXEL_FORMAT>::resize(int32_t width, int32_t height)
    {
        m_size = 3 * width * height;
        m_width = width;
        m_height = height;
        if (m_device_pixels != nullptr)
            alloc->deallocate_bytes(m_device_pixels, m_size);
        m_device_pixels = (PIXEL_FORMAT *)alloc->allocate_bytes(m_size);
    }

	template <typename PIXEL_FORMAT>
	CPU_GPU PIXEL_FORMAT* GLDisplay<PIXEL_FORMAT>::getPointer()
	{
		return m_device_pixels;
	}
}