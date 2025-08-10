#pragma once
#include <cpu_gpu.h>
#include <Math/Math.h>
#include <Type/Types.h>
#include <HDRI/Rgbe.h>
#include <Container/Span.h>
#include <Memory/MemoryBuffer.h>
#include <Container/Array2D.h>
#include <Container/InlinedVector.h>
#include <vector>
#include <string>
namespace Render
{
    struct ImageAndMetadata;
    // PixelFormat Definition
    enum class PixelFormat { U256, Half, Float };

    // PixelFormat Inline Functions
    CPU_GPU inline bool Is8Bit(PixelFormat format) {
        return format == PixelFormat::U256;
    }
    CPU_GPU inline bool Is16Bit(PixelFormat format) {
        return format == PixelFormat::Half;
    }
    CPU_GPU inline bool Is32Bit(PixelFormat format) {
        return format == PixelFormat::Float;
    }

    enum class WrapMode { Black, Clamp, Repeat, OctahedralSphere };
    struct WrapMode2D {
        CPU_GPU
            WrapMode2D(WrapMode w) : wrap{ w, w } {}
        CPU_GPU
            WrapMode2D(WrapMode x, WrapMode y) : wrap{ x, y } {}

        WrapMode wrap[2];
    };

    // ResampleWeight Definition
    struct ResampleWeight {
        int firstPixel;
        Float weight[4];
    };

    CPU_GPU inline bool RemapPixelCoords(Point2i* pp, Point2i resolution,
        WrapMode2D wrapMode) {
        Point2i& p = *pp;

        if (wrapMode.wrap[0] == WrapMode::OctahedralSphere) {
            if (p[0] < 0) {
                p[0] = -p[0];                     // mirror across u = 0
                p[1] = resolution[1] - 1 - p[1];  // mirror across v = 0.5
            }
            else if (p[0] >= resolution[0]) {
                p[0] = 2 * resolution[0] - 1 - p[0];  // mirror across u = 1
                p[1] = resolution[1] - 1 - p[1];      // mirror across v = 0.5
            }

            if (p[1] < 0) {
                p[0] = resolution[0] - 1 - p[0];  // mirror across u = 0.5
                p[1] = -p[1];                     // mirror across v = 0;
            }
            else if (p[1] >= resolution[1]) {
                p[0] = resolution[0] - 1 - p[0];      // mirror across u = 0.5
                p[1] = 2 * resolution[1] - 1 - p[1];  // mirror across v = 1
            }

            // Bleh: things don't go as expected for 1x1 images.
            if (resolution[0] == 1)
                p[0] = 0;
            if (resolution[1] == 1)
                p[1] = 0;

            return true;
        }

        for (int c = 0; c < 2; ++c) {
            if (p[c] >= 0 && p[c] < resolution[c])
                // in bounds
                continue;
            // printf("1 p[c] %d, %d\n", p[c], resolution[c]);
             //p[c] = fabsf(p[c]);
         //    printf("2 p[c] %d, %d\n", p[c], resolution[c]);
            switch (wrapMode.wrap[c]) {
            case WrapMode::Repeat:
                p[c] = Mod(p[c], resolution[c]);
                //  printf("3 p[c] %d, %d\n", p[c], resolution[c]);
                break;
            case WrapMode::Clamp:
                p[c] = Clamp(p[c], 0, resolution[c] - 1);
                break;
            case WrapMode::Black:
                return false;
            default:
                printf("Unhandled WrapMode mode");
            }
        }
        return true;
    }

    struct ImageChannelDesc {
        operator bool() const { return size() > 0; }

        size_t size() const { return offset.size(); }
        bool IsIdentity() const {
            for (size_t i = 0; i < offset.size(); ++i)
                if (offset[i] != i)
                    return false;
            return true;
        }
        std::string ToString() const;

        InlinedVector<int, 4> offset;
    };

    class Image
    {
    public:
        Image();
        Image(uint8_t* p8, Point2i resolution,
            char** channelNames, int n);
        Image(Float* p32, Point2i resolution,
            char** channelNames, int n);
        Image(PixelFormat format, Point2i resolution,
            char** channelNames, int n, Allocator* alloc);

        void ToneMapping();
        // 像素格式
        CPU_GPU
            PixelFormat Format() const { return format; }

        // 图像分辨率
        CPU_GPU
            Point2i Resolution() const { return resolution; }

        // 通道数量
        CPU_GPU
            int NChannels() const { return channelNameSize; }

        // 像素p的偏移
        CPU_GPU
            size_t PixelOffset(Point2i p) const {
            return NChannels() * (p.y * resolution.x + p.x);
        }

        CPU_GPU
            void ScalePixel(Float s) {
            scalePixel = s;
        }

        CPU_GPU
            void ClampPixel(Float s) {
            clampPixel = s;
        }

        // PixelFormat Inline Functions
        CPU_GPU  bool Is8Bit() const {
            return format == PixelFormat::U256;
        }
        CPU_GPU bool Is16Bit() const {
            return format == PixelFormat::Half;
        }
        CPU_GPU bool Is32Bit() const {
            return format == PixelFormat::Float;
        }

        // 获取像素p的第c个通道
        CPU_GPU
            Float GetChannel(Point2i p, int c, WrapMode2D wrapMode = WrapMode::Clamp) const {

            if (!RemapPixelCoords(&p, resolution, wrapMode))
                return 0;

            //p.x = Clamp(p.x, 0, resolution.x);
            //p.y = Clamp(p.y, 0, resolution.y);
            //p.x = 1;
            //p.y = 1;
            //return 0;

            Float v = 0.f;
            switch (format) {
            case PixelFormat::U256: {
                v = p8[PixelOffset(p) + c];
                v = Clamp(v * scalePixel, 0, clampPixel);
                break;
            }
            case PixelFormat::Half: {
                break;
            }
            case PixelFormat::Float: {
                v = p32[PixelOffset(p) + c];
                //v = Clamp(v * scalePixel, 0, clampPixel);
                break;
            }
            default:
                break;
            }
            return v;
        }

        CPU_GPU 
            void SetChannel(Point2i p, int c, Float v) {
            switch (format) {
            case PixelFormat::U256:
                p8[PixelOffset(p) + c] = v;
                break;
            case PixelFormat::Half: break;
            case PixelFormat::Float:
                p32[PixelOffset(p) + c] = v;
                break;
            default: break;
            }
        }

        // 获取像素p指针
        CPU_GPU
            const void* RawPointer(Point2i p) const {
            if (Is8Bit())
                return p8 + PixelOffset(p);
            //if (Is16Bit(format))
            //    return p16 + PixelOffset(p);
            else {
                //CHECK(Is32Bit(format));
                return p32 + PixelOffset(p);
            }
        }

        // 获取像素p指针
        CPU_GPU
            void* RawPointer(Point2i p) {
            if (Is8Bit()) {
                //    printf("is 8 bit %p\n", p8);
                return p8 + PixelOffset(p);
            }
            else {
                return p32 + PixelOffset(p);
            }
        }

        CPU_GPU size_t Size() {
            if (Is8Bit()) {
                if (p8_buffer)
                    return p8_buffer->Size();
                return resolution.x * resolution.y * NChannels() * sizeof(uint8_t);
            }
            else {
                if (p32_buffer)
                    return p32_buffer->Size();
                return resolution.x * resolution.y * NChannels() * sizeof(Float);
            }
        }

        CPU_GPU
            Float BilerpChannel(Point2f p, int c, WrapMode2D wrapMode) const {
            // Compute discrete pixel coordinates and offsets for _p_
            Float x = p[0] * resolution.x - 0.5f, y = p[1] * resolution.y - 0.5f;
            int xi = std::floor(x), yi = std::floor(y);
            Float dx = x - xi, dy = y - yi;

            // Load pixel channel values and return bilinearly interpolated value
            Array<Float, 4> v = { GetChannel({xi, yi}, c, wrapMode),
                                  GetChannel({xi + 1, yi}, c, wrapMode),
                                  GetChannel({xi, yi + 1}, c, wrapMode),
                                  GetChannel({xi + 1, yi + 1}, c, wrapMode) };
            return ((1 - dx) * (1 - dy) * v[0] + dx * (1 - dy) * v[1] + (1 - dx) * dy * v[2] +
                dx * dy * v[3]);

        }

        Image FloatResizeUp(Point2i newResolution, WrapMode2D wrap) const;

        void CopyRectOut(const Bounds2i& extent, Span<float> buf,
            WrapMode2D wrapMode = WrapMode::Clamp) const;

        void CopyRectIn(const Bounds2i& extent, Span<const float> buf);

        ImageChannelDesc GetChannelDesc(Span<const std::string> channels) const;

        template <typename F>
        Array2D<Float> GetSamplingDistribution(
            F dxdA, const Bounds2f& domain = Bounds2f(Point2f(0, 0), Point2f(1, 1)),
            Allocator alloc = {});
        Array2D<Float> GetSamplingDistribution() {
            return GetSamplingDistribution([](Point2f) { return Float(1); });
        }

        // 写文件
        bool Write(std::string name) const;
    private:
        bool WritePNG_lodepng(std::string name) const;
        bool WritePNG(std::string name) const;
        bool WriteJPG(std::string name) const;
        bool WriteTGA(std::string name) const;
        bool WriteBMP(std::string name) const;
        bool WriteHDR(std::string name) const;
        bool WriteEXR(std::string name) const;
    public:
        static Image* Read(std::string filename, Allocator* alloc = nullptr);
        static Image* ReadPNG_lodepng(const std::string& name, Allocator* alloc);
        static Image* ReadPNG(const std::string& name, Allocator* alloc);
        static Image* ReadJPG(const std::string& name, Allocator* alloc);
        static Image* ReadTGA(const std::string& name, Allocator* alloc);
        static Image* ReadBMP(const std::string& name, Allocator* alloc);
        static Image* ReadHDR(const std::string& name, Allocator* alloc);
        static Image* ReadEXR(const std::string& name, Allocator* alloc);
    private:
        static std::vector<ResampleWeight> ResampleWeights(int oldRes, int newRes);
    public:
        Allocator* alloc;
        PixelFormat format;
        Point2i resolution;
        char** channelNames;
        int channelNameSize;
        Float scalePixel = 1.f;
        Float clampPixel = 255.f;
        Float* p32;
        uint8_t* p8;
        MemoryBuffer<uint8_t>* p8_buffer;
        MemoryBuffer<Float>* p32_buffer;
    };

    struct ImageAndMetadata {
        Image image;
        //ImageMetadata metadata;
    };
}

template <typename F>
inline Array2D<Float> Image::GetSamplingDistribution(F dxdA, const Bounds2f& domain,
    Allocator alloc) {
    Array2D<Float> dist(resolution[0], resolution[1], alloc);

    for (int y = 0; y < resolution[1]; ++y) {
        for (int x = 0; x < resolution[0]; ++x) {
            // This is noticeably better than MaxValue: discuss / show
            // example..
            //Float value = GetChannel({ x, y }).Average();
            Point2i uv = { x, y };
            Float r = GetChannel(uv, 0);
            Float g = GetChannel(uv, 1);
            Float b = GetChannel(uv, 2);

            Float value = (r + g + b) / 3.f;

            // Assume Jacobian term is basically constant over the
            // region.
            Point2f p = domain.Lerp2(Point2f((x + .5f) / resolution[0], (y + .5f) / resolution[1]));
            dist(x, y) = value * dxdA(p);
            //if (value > 10)printf("dxda %f value %f\n", dxdA(p), value);
        }
    }
    return dist;
}