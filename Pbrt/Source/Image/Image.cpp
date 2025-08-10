#include <Image/Image.h>
#include <Util/File.h>
#include <Container/Span.h>
#include <Util/Parallel.h>
#include <Texture/RGB.h>
#include <Util/File.h>

#define LODEPNG_NO_COMPILE_DISK
#include <lodepng/lodepng.h>

// use lodepng and get 16-bit.
//#define STBI_NO_PNG
// too old school
//#define STBI_NO_PIC
//#define STBI_ASSERT CHECK
#define STBI_WINDOWS_UTF8
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include "fpng.h"
 
#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfFrameBuffer.h>
#include <OpenEXR/ImfThreading.h>
#include <OpenEXR/ImfNamespace.h>
#include <OpenEXR/ImfExport.h>
#include <OpenEXR/ImfForward.h>
#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfOutputFile.h>
#include <OpenEXR/ImfTiledOutputFile.h>
#include <OpenEXR/ImfGenericInputFile.h>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfRgba.h>
#include <OpenEXR/ImfChannelList.h>

#include <fstream>
#include <iostream>

using namespace std;
namespace Render {

    //bool HasExtension(std::string filename, std::string e) {
    //    std::string ext = e;
    //    if (!ext.empty() && ext[0] == '.')
    //        ext.erase(0, 1);
    //
    //    size_t pos = filename.find('.');
    //    std::string filenameExtension;
    //    if (pos != string::npos)
    //    {
    //        filenameExtension = filename.substr(pos + 1);
    //    }
    //
    //    if (ext.size() > filenameExtension.size())
    //        return false;
    //    return std::equal(ext.rbegin(), ext.rend(), filenameExtension.rbegin(),
    //        [](char a, char b) { return std::tolower(a) == std::tolower(b); });
    //}

    int TexelBytes(PixelFormat format) {
        switch (format) {
        case PixelFormat::U256:
            return 1;
        case PixelFormat::Half:
            return 2;
        case PixelFormat::Float:
            return 4;
        default:
            cout << "Unhandled PixelFormat in TexelBytes()" << endl;
            return 0;
        }
    }

    Image::Image() :
        alloc(nullptr),
        scalePixel(1.f),
        clampPixel(255.f),
        p8(nullptr),
        p32(nullptr),
        p8_buffer(nullptr),
        p32_buffer(nullptr),
        channelNames(nullptr),
        channelNameSize(0),
        format(PixelFormat::U256),
        resolution(Point2i(0.f, 0.f)) {

    }

    Image::Image(uint8_t* p8c, Point2i resolution, char** channelNames, int n) :
        alloc(nullptr),
        scalePixel(1.f),
        clampPixel(255.f),
        p8(p8c),
        p32(nullptr),
        p8_buffer(nullptr),
        p32_buffer(nullptr),
        channelNames(channelNames),
        channelNameSize(n),
        format(PixelFormat::U256),
        resolution(resolution) {
        // printf("Image::Image(uint8_t* p8c) %p %p, channelNameSize %d, resolution %d %d\n", p8, p8c, channelNameSize, resolution.x, resolution.y);
    }

    Image::Image(Float* p32c, Point2i resolution, char** channelNames, int n) :
        alloc(nullptr),
        scalePixel(1.f),
        clampPixel(255.f),
        p8(nullptr),
        p32(p32c),
        p8_buffer(nullptr),
        p32_buffer(nullptr),
        channelNames(channelNames),
        channelNameSize(n),
        format(PixelFormat::Float),
        resolution(resolution) {

    }

    Image::Image(PixelFormat format, Point2i resolution, char** channelNames1, int n, Allocator* alloc) :
        alloc(nullptr),
        scalePixel(1.f),
        clampPixel(255.f),
        p8(nullptr),
        p32(nullptr),
        p8_buffer(nullptr),
        p32_buffer(nullptr),
        channelNames(channelNames1),
        channelNameSize(n),
        format(format),
        resolution(resolution) {

        //channelNames = alloc->allocate_object<char *>(n);
        //cout << "Image" << endl;
        //for (int i = 0; i < n; i++) {
        //    auto ptr = alloc->allocate_object<char *>(1);
        //    ptr = &channelNames1[i];
        //    channelNames[i] = ptr[0];
        //    cout << channelNames1[i] << endl;
        //}

        if (Is8Bit()) {
            p8_buffer = alloc->new_object<MemoryBuffer<uint8_t>>(n * resolution[0] * resolution[1], alloc);
            p8 = p8_buffer->getPointer();
        }
        else {
            p32_buffer = alloc->new_object<MemoryBuffer<Float>>(n * resolution[0] * resolution[1], alloc);
            p32 = p32_buffer->getPointer();
        }
    }

    Image Image::FloatResizeUp(Point2i newRes, WrapMode2D wrapMode) const {
#if 0
        Image resampledImage(PixelFormat::Float, newRes, channelNames, channelNameSize);
        // Compute $x$ and $y$ resampling weights for image resizing
        std::vector<ResampleWeight> xWeights, yWeights;
        xWeights = ResampleWeights(resolution[0], newRes[0]);
        yWeights = ResampleWeights(resolution[1], newRes[1]);

        // Resize image in parallel, working by tiles
        ParallelFor2D(Bounds2i({ 0, 0 }, newRes), [&](Bounds2i outExtent) {
            // Determine extent in source image and copy pixel values to _inBuf_
            Bounds2i inExtent(Point2i(xWeights[outExtent.pMin.x].firstPixel,
            yWeights[outExtent.pMin.y].firstPixel),
            Point2i(xWeights[outExtent.pMax.x - 1].firstPixel + 4,
            yWeights[outExtent.pMax.y - 1].firstPixel + 4));
        std::vector<float> inBuf(NChannels() * inExtent.Area());
        CopyRectOut(inExtent, Span<float>(inBuf), wrapMode);

        // Resize image in the $x$ dimension
        // Compute image extents and allocate _xBuf_
        int nxOut = outExtent.pMax.x - outExtent.pMin.x;
        int nyOut = outExtent.pMax.y - outExtent.pMin.y;
        int nxIn = inExtent.pMax.x - inExtent.pMin.x;
        int nyIn = inExtent.pMax.y - inExtent.pMin.y;
        std::vector<float> xBuf(NChannels() * nyIn * nxOut);

        int xBufOffset = 0;
        for (int yOut = inExtent.pMin.y; yOut < inExtent.pMax.y; ++yOut) {
            for (int xOut = outExtent.pMin.x; xOut < outExtent.pMax.x; ++xOut) {
                // Resample image pixel _(xOut, yOut)_
                const ResampleWeight& rsw = xWeights[xOut];
                // Compute _inOffset_ into _inBuf_ for _(xOut, yOut)_
                // w.r.t. inBuf
                int xIn = rsw.firstPixel - inExtent.pMin.x;
                int yIn = yOut - inExtent.pMin.y;
                int inOffset = NChannels() * (xIn + yIn * nxIn);

                for (int c = 0; c < NChannels(); ++c, ++xBufOffset, ++inOffset)
                    xBuf[xBufOffset] = rsw.weight[0] * inBuf[inOffset] +
                    rsw.weight[1] * inBuf[inOffset + NChannels()] +
                    rsw.weight[2] * inBuf[inOffset + 2 * NChannels()] +
                    rsw.weight[3] * inBuf[inOffset + 3 * NChannels()];
            }
        }

        // Resize image in the $y$ dimension
        std::vector<float> outBuf(NChannels() * nxOut * nyOut);
        for (int x = 0; x < nxOut; ++x) {
            for (int y = 0; y < nyOut; ++y) {
                int yOut = y + outExtent[0][1];
                const ResampleWeight& rsw = yWeights[yOut];
                int xBufOffset =
                    NChannels() * (x + nxOut * (rsw.firstPixel - inExtent[0][1]));
                int step = NChannels() * nxOut;

                int outOffset = NChannels() * (x + y * nxOut);
                for (int c = 0; c < NChannels(); ++c, ++outOffset, ++xBufOffset)
                    outBuf[outOffset] =
                    std::max<Float>(0, (rsw.weight[0] * xBuf[xBufOffset] +
                        rsw.weight[1] * xBuf[xBufOffset + step] +
                        rsw.weight[2] * xBuf[xBufOffset + 2 * step] +
                        rsw.weight[3] * xBuf[xBufOffset + 3 * step]));
            }
        }

        // Copy resampled image pixels out into _resampledImage_
        resampledImage.CopyRectIn(outExtent, outBuf);
            });

        return resampledImage;
#endif
        return {};
    }


    template <typename F>
    void ForExtent(const Bounds2i& extent, WrapMode2D wrapMode, const Image& image, F op) {
        int nx = extent.pMax[0] - extent.pMin[0];
        int nc = image.NChannels();
        if (Intersect(extent, Bounds2i({ 0, 0 }, image.Resolution())) == extent) {
            // All in bounds
            for (int y = extent.pMin[1]; y < extent.pMax[1]; ++y) {
                int offset = image.PixelOffset({ extent.pMin[0], y });
                for (int x = 0; x < nx; ++x)
                    for (int c = 0; c < nc; ++c)
                        op(offset++);
            }
        }
        else {
            for (int y = extent.pMin[1]; y < extent.pMax[1]; ++y) {
                for (int x = 0; x < nx; ++x) {
                    printf("%d %d\n", x, y);
                    Point2i p(extent.pMin[0] + x, y);
                    // FIXME: this will return false on Black wrap mode
                    RemapPixelCoords(&p, image.Resolution(), wrapMode);
                    int offset = image.PixelOffset(p);
                    for (int c = 0; c < nc; ++c)
                        op(offset++);
                }
            }
        }
    }

    void Image::CopyRectOut(const Bounds2i& extent, Span<Float> buf,
        WrapMode2D wrapMode) const {
        auto bufIter = buf.begin();
        switch (format) {
        case PixelFormat::Float:
            ForExtent(extent, wrapMode, *this,
                [&bufIter, this](int offset) { *bufIter++ = Float(p32[offset]); });
            break;

        default:
            printf("Unhandled PixelFormat\n");
        }
    }

    void Image::CopyRectIn(const Bounds2i& extent, Span<const Float> buf) {
        auto bufIter = buf.begin();
        switch (format) {
        case PixelFormat::Float:
            ForExtent(extent, WrapMode::Clamp, *this,
                [&bufIter, this](int offset) { p32[offset] = *bufIter++; });
            break;
        default:
            printf("Unhandled PixelFormat\n");
        }
    }

    ImageChannelDesc Image::GetChannelDesc(Span<const std::string> requestedChannels) const
    {
            ImageChannelDesc desc;
            desc.offset.resize(requestedChannels.size());
            for (size_t i = 0; i < requestedChannels.size(); ++i) {
                size_t j;
                for (j = 0; j < channelNameSize; ++j)
                    if (requestedChannels[i] == channelNames[j]) {
                        desc.offset[i] = j;
                        break;
                    }
                if (j == channelNameSize)
                    return {};
            }

            return desc;
    }


    Image* Image::Read(std::string filename, Allocator* alloc) {
        //cout << "READ " << filename << endl;
        if (HasExtension(filename, "png"))
            return ReadPNG_lodepng(filename, alloc);
        else if (HasExtension(filename, "hdr"))
            return ReadHDR(filename, alloc);
        if (HasExtension(filename, "jpg") || HasExtension(filename, "jpeg"))
            return ReadJPG(filename, alloc);
        else if (HasExtension(filename, "tga"))
            return ReadTGA(filename, alloc);
        else if (HasExtension(filename, "bmp"))
            return ReadBMP(filename, alloc);
        else if (HasExtension(filename, "exr"))
            return ReadEXR(filename, alloc);
        else
        {
            Image* image = nullptr;
            int width, height, channels;
            unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
            if (data) {
                vector<unsigned char> pixels;
                std::copy(data, data + width * height * channels / sizeof(unsigned char), std::back_inserter(pixels));

                switch (channels) {
                case 3: {
                    char* RGB[] = { "R", "G", "B" };
                    image = alloc->new_object<Image>(PixelFormat::U256, Point2i(width, height), RGB, channels, alloc);
                    std::copy(pixels.begin(), pixels.end(), (uint8_t*)image->RawPointer({ 0, 0 }));
                }
                      break;
                case 4: {
                    char* RGBA[] = { "R", "G", "B", "A" };
                    image = alloc->new_object<Image>(PixelFormat::U256, Point2i(width, height), RGBA, channels, alloc);
                    std::copy(pixels.begin(), pixels.end(), (uint8_t*)image->RawPointer({ 0, 0 }));
                }
                      break;
                default:
                    printf("%s: %d channel image unsupported.", filename.c_str(), channels);
                }
            }
            return image;
        }
        return {};
    }

    Image* Image::ReadPNG_lodepng(const std::string& name, Allocator* alloc) {
        std::string contents = ReadFileContents(name);

        unsigned width, height;
        LodePNGState state;
        lodepng_state_init(&state);

        unsigned int error = lodepng_inspect(
            &width, &height, &state, (const unsigned char*)contents.data(), contents.size());
        if (error != 0)
            cout << name << ": " << lodepng_error_text(error) << endl;
        Image* image = nullptr;
        //cout << state.info_png.color.colortype << endl;
        switch (state.info_png.color.colortype) {
        case LCT_GREY:
        case LCT_GREY_ALPHA:
            break;
        default:
            std::vector<unsigned char> buf;
            int bpp = state.info_png.color.bitdepth == 16 ? 16 : 8;
            bool hasAlpha = (state.info_png.color.colortype == LCT_RGBA);
            // Force RGB if it's paletted or whatever.
            error =
                lodepng::decode(buf, width, height, (const unsigned char*)contents.data(),
                    contents.size(), hasAlpha ? LCT_RGBA : LCT_RGB, bpp);
            if (error != 0)
                cout << name << ": " << lodepng_error_text(error) << endl;

            //cout << "hasAlpha " << hasAlpha << " state.info_png.color.bitdepth  " << state.info_png.color.bitdepth << endl;
            if (state.info_png.color.bitdepth == 16) {
                char* names[] = { "R", "G", "B", "A" };
                image = alloc->new_object<Image>(PixelFormat::U256, Point2i(width, height), names, 4, alloc);

                cout << image->resolution.x << " " << image->resolution.y << endl;
                cout << width  << " " << height << endl;
                auto bufIter = buf.begin();
                //cout << "buf.size " << buf.size() << " " << width * height * 4 << endl;
               //for (unsigned int y = 0; y < height; ++y) {
               //    //cout << "y " << y << endl;
               //    for (unsigned int x = 0; x < width; ++x, bufIter += 8) {
               //        //if (y > 3071)
               //        //    cout << "X " << x << (y *  width + x) * 8 << endl;
               //        // Convert from little endian.
               //        //Float rgba[4] = {
               //        //    (((int)bufIter[0] << 8) + (int)bufIter[1]) / 65535.f,
               //        //    (((int)bufIter[2] << 8) + (int)bufIter[3]) / 65535.f,
               //        //    (((int)bufIter[4] << 8) + (int)bufIter[5]) / 65535.f,
               //        //    (((int)bufIter[6] << 8) + (int)bufIter[7]) / 65535.f };
               //        for (int c = 0; c < 4; ++c) {
               //            //image->SetChannel(Point2i(x, y), c, rgba[c]);
               //            image->SetChannel(Point2i(x, y), c, 0);
               //        }
               //    }
               //}
            } else if (hasAlpha) {
                char* names[] = { "R", "G", "B", "A" };
                //image = alloc->new_object<Image>(PixelFormat::U256, Point2i(width, height), names, 4, alloc);
                image = alloc->new_object<Image>(PixelFormat::Float, Point2i(width, height), names, 4, alloc);
                std::copy(buf.begin(), buf.end(), (Float*)image->RawPointer({ 0, 0 }));
            }
            else {
               // cout << "PNG U256 width " << width << " height " << height << endl;
                char* names[] = { "R", "G", "B" };
                //image = alloc->new_object<Image>(PixelFormat::U256, Point2i(width, height), names, 3, alloc);
                image = alloc->new_object<Image>(PixelFormat::Float, Point2i(width, height), names, 3, alloc);
                std::copy(buf.begin(), buf.end(), (Float*)image->RawPointer({ 0, 0 }));
            }
            break;
        }

        if (nullptr == image) {
            cout << "image is null\n";
        }

        return image;
    }

    Image* Image::ReadPNG(const std::string& name, Allocator* alloc) {
        return nullptr;
    }

    Image* Image::ReadJPG(const std::string& name, Allocator* alloc) {
        Image* image = nullptr;
        int width, height, channels;
        unsigned char* data = stbi_load(name.c_str(), &width, &height, &channels, 0);
        if (data) {
            vector<unsigned char> pixels;
            std::copy(data, data + width * height * channels / sizeof(unsigned char), std::back_inserter(pixels));

            switch (channels) {
            case 3: {
                char* RGB[] = { "R", "G", "B" };
                image = alloc->new_object<Image>(PixelFormat::Float, Point2i(width, height), RGB, channels, alloc);
                std::copy(pixels.begin(), pixels.end(), (Float*)image->RawPointer({ 0, 0 }));
            }
                  break;
            case 4: {
                char* RGBA[] = { "R", "G", "B", "A" };
                image = alloc->new_object<Image>(PixelFormat::Float, Point2i(width, height), RGBA, channels, alloc);
                std::copy(pixels.begin(), pixels.end(), (Float*)image->RawPointer({ 0, 0 }));
            }
                  break;
            default:
                printf("%s: %d channel image unsupported.", name.c_str(), channels);
            }
        }
        return image;
    }

    Image* Image::ReadTGA(const std::string& name, Allocator* alloc) {
        Image* image = nullptr;
        int width, height, channels;
        unsigned char* data = stbi_load(name.c_str(), &width, &height, &channels, 0);
        if (data) {
            vector<unsigned char> pixels;
            std::copy(data, data + width * height * channels / sizeof(unsigned char), std::back_inserter(pixels));

            switch (channels) {
            case 3: {
                char* RGB[] = { "R", "G", "B" };
                image = alloc->new_object<Image>(PixelFormat::U256, Point2i(width, height), RGB, channels, alloc);
                std::copy(pixels.begin(), pixels.end(), (uint8_t*)image->RawPointer({ 0, 0 }));
            }
                  break;
            case 4: {
                char* RGBA[] = { "R", "G", "B", "A" };
                image = alloc->new_object<Image>(PixelFormat::U256, Point2i(width, height), RGBA, channels, alloc);
                std::copy(pixels.begin(), pixels.end(), (uint8_t*)image->RawPointer({ 0, 0 }));
            }
                  break;
            default:
                printf("%s: %d channel image unsupported.", name.c_str(), channels);
            }
        }
        return image;
    }

    Image* Image::ReadBMP(const std::string& name, Allocator* alloc) {
        Image* image = nullptr;
        int width, height, channels;
        unsigned char* data = stbi_load(name.c_str(), &width, &height, &channels, 0);
        if (data) {
            vector<unsigned char> pixels;
            std::copy(data, data + width * height * channels / sizeof(unsigned char), std::back_inserter(pixels));

            switch (channels) {
            case 3: {
                char* RGB[] = { "R", "G", "B" };
                image = alloc->new_object<Image>(PixelFormat::U256, Point2i(width, height), RGB, channels, alloc);
                std::copy(pixels.begin(), pixels.end(), (uint8_t*)image->RawPointer({ 0, 0 }));
            }
                  break;
            case 4: {
                char* RGBA[] = { "R", "G", "B", "A" };
                image = alloc->new_object<Image>(PixelFormat::U256, Point2i(width, height), RGBA, channels, alloc);
                std::copy(pixels.begin(), pixels.end(), (uint8_t*)image->RawPointer({ 0, 0 }));
            }
                  break;
            default:
                printf("%s: %d channel image unsupported.", name.c_str(), channels);
            }
        }
        return image;
    }

    Image* Image::ReadEXR(const std::string& name, Allocator* alloc) {
        // 输入接口
        Imf::InputFile file(name.c_str());
        // openexr data window
        Imath::Box2i         dw = file.header().dataWindow();
        // 图像分辨率
        int width = dw.max.x - dw.min.x + 1;
        int height = dw.max.y - dw.min.y + 1;
        // 像素类型
        Imf::PixelType pixelType;
        // 通道数量
        int nChannels = 0;
        // 通道名称
        std::vector<std::string> channelNames;
        // openexr 通道列表
        const Imf::ChannelList& channels = file.header().channels();

        int nameLength = 0;
        // 目前只支持单通道类型, 只读RBGA通道
        char* chs[] = { "R", "G", "B", "A" };
        for (int i = 0; i < 4; i++) {
            for (auto iter = channels.begin(); iter != channels.end(); ++iter) {
                if (strcmp(iter.name(), chs[i]) == 0) {

                    if (nChannels++ == 0)
                        pixelType = iter.channel().type;
                    else if (pixelType != iter.channel().type)
                        cout << "ReadEXR() doesn't currently support images with multiple channel types." << endl;

                    //cout << "chs " << chs[i] << " iter.name() " << iter.name() << endl;
                    channelNames.push_back(iter.name());
                    nameLength += channelNames.back().length() + 1;
                    break;
                }
            }
        }
        char** ppChannelNames = alloc->allocate_object<char*>(4);
        char* pChannelNames = alloc->allocate_object<char>(nameLength);
        char* pName = pChannelNames;
        int cc = 0;
        for (auto iter = channelNames.begin(); iter != channelNames.end(); ++iter) {
            strncpy(pName, iter->c_str(), channelNames[cc].size());
            pName[channelNames[cc].size()] = '\0';
            ppChannelNames[cc] = pName;
            pName += channelNames[cc].size() + 1;
            cc++;
        }

        // 图像对象
        Image* image = alloc->new_object<Image>(PixelFormat::Float, Point2i(width, height), ppChannelNames, nChannels, alloc);

        if (image) {
            // 像素
            Float* ptr = (Float*)image->RawPointer({ 0, 0 });

            size_t xStride = image->NChannels() * TexelBytes(image->Format());
            size_t yStride = image->Resolution().x * xStride;

            char* originPtr = (((char*)image->RawPointer({ 0, 0 })) - dw.min.x * xStride - dw.min.y * yStride);

            Imf::FrameBuffer fb;
            for (int channelIndex = 0; channelIndex < image->NChannels(); channelIndex++)
                fb.insert(channelNames[channelIndex],
                    Imf::Slice(Imf::FLOAT, originPtr + channelIndex * sizeof(float), xStride, yStride));

            file.setFrameBuffer(fb);
            file.readPixels(dw.min.y, dw.max.y); 

            image->ScalePixel(255);
            //image->ClampPixel(255);
        }
    
        return image;
    }

#if 1
    Image* Image::ReadHDR(const std::string& filename, Allocator* alloc) {
        Image* image = nullptr;
        
        int width, height, n;
        float* data = stbi_loadf(filename.c_str(), &width, &height, &n, 0);
        
        cout << " read hdr " << width << " " << height << " n " << n << endl;
        if (nullptr == data) {
            cout << filename << ":" << stbi_failure_reason() << endl;
            return nullptr;
        }
        switch (n) {
        case 1: {
            char* names[] = { "Y" };
            image = alloc->new_object<Image>(PixelFormat::Float, Point2i(width, height), names, n, alloc);
            std::copy(data, data + width * height * n, (Float*)image->RawPointer({ 0, 0 }));
        }
              break;
        case 2: {
            char* names[] = { "Y", "A" };
            image = alloc->new_object<Image>(PixelFormat::Float, Point2i(width, height), names, n, alloc);
            std::copy(data, data + width * height * n, (Float*)image->RawPointer({ 0, 0 }));
        }
              break;
        case 3: {
            char* names[] = { "R", "G", "B" };
            image = alloc->new_object<Image>(PixelFormat::Float, Point2i(width, height), names, n, alloc);
            std::copy(data, data + width * height * n, (Float*)image->RawPointer({ 0, 0 }));
        }
              break;
        case 4: {
            char* names[] = { "R", "G", "B", "A" };
            image = alloc->new_object<Image>(PixelFormat::Float, Point2i(width, height), names, n, alloc);
            std::copy(data, data + width * height * n, (Float*)image->RawPointer({ 0, 0 }));
        }
              break;
        default:
            cout << filename << ": " << n << "channel image unsupported." << endl;
        }
        return image;
    }
#else
    Image* Image::ReadHDR(const std::string& name, Allocator* alloc) {
        Image* image = nullptr;
        int width = 0, height = 0;
        rgbe_header_info header;
        FILE* file = fopen(name.c_str(), "rb");
        if (file) {
            RGBE_ReadHeader(file, &width, &height, &header);
            Float* data = (Float*)malloc(3 * height * width * sizeof(Float));
            if (data) {
                RGBE_ReadPixels_RLE(file, data, width, height);
                cout << "hdr "
                    << data[2048 * 3048 * 3 + 0] << " "
                    << data[2048 * 0 * 3 + 0] << " "
                    << endl;
                //cout << "width " << width << " " << height << endl;
                char* names[] = { "R", "G", "B" };
                image = alloc->new_object<Image>(PixelFormat::Float, Point2i(width, height), names, 3, alloc);
                std::copy(data, data + 3 * width * height, (uint8_t*)image->RawPointer({ 0, 0 }));


                free(data);
            }
            fclose(file);
        }
        return image;
    }
#endif

    bool Image::Write(std::string filename) const {
        cout << "write " << filename << endl;
        if (HasExtension(filename, "lod.png"))
            return WritePNG_lodepng(filename);
        if (HasExtension(filename, "png"))
            //return WritePNG_lodepng(filename);
            return WritePNG(filename);
        if (HasExtension(filename, "jpg") || HasExtension(filename, "jpeg"))
            return WriteJPG(filename);
        if (HasExtension(filename, "hdr"))
            return WriteHDR(filename);
        if (HasExtension(filename, "tga"))
            return WriteTGA(filename);
        if (HasExtension(filename, "bmp"))
            return WriteBMP(filename);
        if (HasExtension(filename, "exr"))
            return WriteEXR(filename);
        else {

        }
        return false;
    }

    bool Image::WritePNG_lodepng(std::string filename) const {
        LodePNGColorType pngColor;
        switch (NChannels()) {
        case 1:
            pngColor = LCT_GREY;
            break;
        case 3:
            // TODO: it would be nice to store the color encoding used in the PNG metadata...
            pngColor = LCT_RGB;
            break;
        case 4:
            pngColor = LCT_RGBA;
            break;
        default:
            cout << "Unexpected number of channels in WritePNG()" << endl;
        }

        unsigned int error = 0;
        unsigned char* png;
        size_t pngSize;
        if (format == PixelFormat::U256) {
            error = lodepng_encode_memory(&png, &pngSize, p8, resolution.x, resolution.y, pngColor, 8 /* bitdepth */);
        }
        else {
            std::unique_ptr<uint8_t[]> pix8 = std::make_unique<uint8_t[]>(NChannels() * resolution.x * resolution.y);
            auto resolution = Resolution();
            for (int row = 0; row < resolution.y; row++) {
                for (int col = 0; col < resolution.x; col++) {
                    for (int c = 0; c < NChannels(); ++c) {
                        pix8[NChannels() * (row * resolution.x + col) + c] = GetChannel({ col, row }, c);
                    }
                }
            }
            // cout << "aaaaaaasdwqe qwd qwd \n";
            error = lodepng_encode_memory(&png, &pngSize, pix8.get(), resolution.x, resolution.y, pngColor, 8 /* bitdepth */);
        }

        if (error == 0) {
            std::string encodedPNG(png, png + pngSize);
            if (!WriteFileContents(filename, encodedPNG)) {
                cout << filename << ": error writing PNG." << endl;
                return false;
            }
            else {
                cout << "write ok " << filename << endl;
            }
        }
        else {
            cout << filename << ": " << lodepng_error_text(error) << endl;
            return false;
        }

        free(png);

        return true;
    }

    bool Image::WritePNG(std::string filename) const {
        switch (NChannels()) {
        case 1:
        case 3:
        case 4:
            break;
        default:
            cout << "Unexpected number of channels in WritePNG()" << endl;
        }
#if 0
        std::unique_ptr<uint8_t[]> pix8 = std::make_unique<uint8_t[]>(NChannels() * resolution.x * resolution.y);
        for (int row = 0; row < resolution.y; row++) {
            for (int col = 0; col < resolution.x; col++) {
                for (int c = 0; c < NChannels(); ++c) {
                    pix8[NChannels() * (row * resolution.x + col) + c] = GetChannel({ col, row }, c);
                }
            }
        }
        stbi_write_png(filename.c_str(), resolution.x, resolution.y, NChannels(), pix8.get(), 0);
        return true;
#else
        return fpng::fpng_encode_image_to_file(filename.c_str(), p8, resolution.x, resolution.y, channelNameSize);
#endif
    }

    bool Image::WriteJPG(std::string filename) const {
        std::unique_ptr<uint8_t[]> pix8 = std::make_unique<uint8_t[]>(NChannels() * resolution.x * resolution.y);
        auto resolution = Resolution();
        for (int row = 0; row < resolution.y; row++) {
            for (int col = 0; col < resolution.x; col++) {
                for (int c = 0; c < NChannels(); ++c) {
                    pix8[NChannels() * (row * resolution.x + col) + c] = GetChannel({ col, row }, c);
                }
            }
        }
        stbi_write_jpg(filename.c_str(), resolution.x, resolution.y, NChannels(), pix8.get(), 100);
        return true;
    }

    bool Image::WriteTGA(std::string filename) const {
        std::unique_ptr<uint8_t[]> pix8 = std::make_unique<uint8_t[]>(NChannels() * resolution.x * resolution.y);
        auto resolution = Resolution();
        for (int row = 0; row < resolution.y; row++) {
            for (int col = 0; col < resolution.x; col++) {
                for (int c = 0; c < NChannels(); ++c) {
                    pix8[NChannels() * (row * resolution.x + col) + c] = GetChannel({ col, row }, c);
                }
            }
        }
        stbi_write_tga(filename.c_str(), resolution.x, resolution.y, NChannels(), pix8.get());
        return true;
    }

    bool Image::WriteBMP(std::string filename) const {
        std::unique_ptr<uint8_t[]> pix8 = std::make_unique<uint8_t[]>(NChannels() * resolution.x * resolution.y);
        auto resolution = Resolution();
        for (int row = 0; row < resolution.y; row++) {
            for (int col = 0; col < resolution.x; col++) {
                for (int c = 0; c < NChannels(); ++c) {
                    pix8[NChannels() * (row * resolution.x + col) + c] = GetChannel({ col, row }, c);
                }
            }
        }
        stbi_write_bmp(filename.c_str(), resolution.x, resolution.y, NChannels(), pix8.get());
        return true;
    }

    bool Image::WriteEXR(std::string name) const {
        Imf::Rgba* pixels = new Imf::Rgba[Resolution().x * Resolution().y];
        auto resolution = Resolution();
        cout << "NChannels() " << NChannels() << " "
             << "resolution.x " << resolution.x << " "
             << "resolution.y " << resolution.y << " "
             << endl;
        for (int row = 0; row < resolution.y; row++) {
            for (int col = 0; col < resolution.x; col++) {
                if (NChannels() > 0)
                    pixels[(row * resolution.x + col)].r = (GetChannel({ col, row }, 0));
                if (NChannels() > 1)
                    pixels[(row * resolution.x + col)].g = (GetChannel({ col, row }, 1));
                if (NChannels() > 2)
                    pixels[(row * resolution.x + col)].b = (GetChannel({ col, row }, 2));
                if (NChannels() > 3)
                    pixels[(row * resolution.x + col)].a = (GetChannel({ col, row }, 3));
                else pixels[(row * resolution.x + col)].a = 1.0f;
            }
        }

        Imf::RgbaChannels chs;
        if (NChannels() == 4)
            chs = Imf::WRITE_RGBA;
        else
            chs = Imf::WRITE_RGB;
        chs = Imf::WRITE_RGBA;
        Imf::RgbaOutputFile file(name.c_str(), Resolution().x, Resolution().y, chs);
        file.setFrameBuffer(pixels, 1, Resolution().x);
        file.writePixels(Resolution().y);

        delete[] pixels;

        return true;
    }

    bool Image::WriteHDR(std::string filename) const {
        if (format == PixelFormat::Float) {
            stbi_write_hdr(filename.c_str(), resolution.x, resolution.y, NChannels(), p32);
        }
        else {
            return false;
        }
        return true;
    }

    std::vector<ResampleWeight> Image::ResampleWeights(int oldRes, int newRes) {
        std::vector<ResampleWeight> wt(newRes);
        Float filterRadius = 2, tau = 2;
        for (int i = 0; i < newRes; ++i) {
            // Compute image resampling weights for _i_th pixel
            Float center = (i + .5f) * oldRes / newRes;
            wt[i].firstPixel = std::floor((center - filterRadius) + 0.5f);
            for (int j = 0; j < 4; ++j) {
                Float pos = wt[i].firstPixel + j + .5f;
                wt[i].weight[j] = WindowedSinc(pos - center, filterRadius, tau);
            }

            // Normalize filter weights for pixel resampling
            Float invSumWts =
                1 / (wt[i].weight[0] + wt[i].weight[1] + wt[i].weight[2] + wt[i].weight[3]);
            for (int j = 0; j < 4; ++j)
                wt[i].weight[j] *= invSumWts;
        }
        return wt;
    }

    Float simpleReinhardToneMapping(Float color)
    {
        float exposure = 1.5;
        color *= exposure / (1. + color / exposure);
        // color = std::powf(color, 1.f / 2.2f);
        return color;
    }

    void Image::ToneMapping() {
        return;
        RGB max(0, 0, 0);
        for (int row = 0; row < Resolution().y; row++) {
            for (int col = 0; col < Resolution().x; col++) {
                Float r = GetChannel({ col, row }, 0);
                Float g = GetChannel({ col, row }, 1);
                Float b = GetChannel({ col, row }, 2);
                if (r > max.r) {
                    max.r = r;
                }
                if (g > max.g) {
                    max.g = g;
                }
                if (b > max.b) {
                    max.b = b;
                }
            }
        }
        cout << "tonemapping max " << max.r << " " << max.g << " " << max.b << " " << endl;
        // return;
         /*
         const float MIDDLE_GREY = 1;
         color *= MIDDLE_GREY / adapted_lum;
         return color / (1.0f + color);
         */
        RGB adapted_lum = max;
        for (int row = 0; row < Resolution().y; row++) {
            for (int col = 0; col < Resolution().x; col++) {
                Point2i p = { col, row };
                Float r = GetChannel(p, 0);
                //r /= adapted_lum.r;
                //r = r / (1.0f + r);
                r = simpleReinhardToneMapping(r);
                SetChannel(p, 0, r * 255);
                Float g = GetChannel(p, 1);
                //g /= adapted_lum.g;
                //g = g / (1.0f + g);
                g = simpleReinhardToneMapping(g);
                SetChannel(p, 1, g * 255);
                Float b = GetChannel(p, 2);
                //b /= adapted_lum.b;
                //b = b / (1.0f + b);  
                b = simpleReinhardToneMapping(b);
                SetChannel(p, 2, b * 255);
            }
        }
    }
}