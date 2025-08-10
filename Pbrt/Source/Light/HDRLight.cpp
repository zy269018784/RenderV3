#include <Light/HDRLight.h>
#include <Light/LightSampleContext.h>
#include <Light/LightLiSample.h>
#include <Camera/SampledWavelengths.h>
#include <Math/Math.h>
#include <Type/Types.h>
#include <Math/EqualAreaMapping.h>
#include <Texture/TextureEvalContext.h>
#include <numeric>

namespace Render {

    // 计算光度学量的函数
    float RGBToPhotometric(float r, float g, float b) {
        return 0.2126f * r + 0.7152f * g + 0.0722f * b;
    }

    // 计算整张图片的 scale
    float CalculateImageScale(const Image& image) {
        float totalLuminance = 0.0f;
        auto resolution = image.Resolution();
        int pixelCount = resolution.x * resolution.y;
        for (int y = 0; y < resolution[1]; ++y) {
            for (int x = 0; x < resolution[0]; ++x) {
                // 获取当前像素的 RGB 值
                Point2i uv = { x, y };
                Float r = image.GetChannel(uv, 0);
                Float g = image.GetChannel(uv, 1);
                Float b = image.GetChannel(uv, 2);

                // 计算该像素的光度学量
                float luminance = RGBToPhotometric(r, g, b);

                // 累加到总光度学量
                totalLuminance += luminance;
            }
        }

        // 计算平均光度学量
        float averageLuminance = totalLuminance / pixelCount;

        // 假设初始 scale 值
        float scale = 1.0f;

        // 调整 scale
        scale /= averageLuminance;

        return scale;
    }
   
    HDRLight::HDRLight(Transform worldFromLight, Float phi, Float theta, SpectrumTexture L, Image* image, Float power, Allocator alloc)
        :   LightBase(worldFromLight), phi(phi), theta(theta), L(L), power(power), distribution(alloc), compensatedDistribution(alloc) {
        ImageChannelDesc channelDesc = image->GetChannelDesc({ "R", "G", "B" });
        if (!channelDesc)
            printf(" image used for ImageInfiniteLight doesn't have R, G, B channels.");
        /*CHECK_EQ(3, channelDesc.size());
        CHECK(channelDesc.IsIdentity());*/
        if (image->Resolution().x != image->Resolution().y)
            printf("image resolution (%d, %d) is non-square. It's unlikely "
                "this is an equal area environment map.",
                image->Resolution().x, image->Resolution().y);
        Array2D<Float> d = image->GetSamplingDistribution();
        Bounds2f domain = Bounds2f(Point2f(0, 0), Point2f(1, 1));
        distribution = PiecewiseConstant2D(d, domain, alloc);

        // Initialize compensated PDF for image infinite area light
        Float average = std::accumulate(d.begin(), d.end(), 0.) / d.size();
        for (Float& v : d)
           v = std::max<Float>(v - average, 0);
        if (std::all_of(d.begin(), d.end(), [](Float v) { return v == 0; }))
           std::fill(d.begin(), d.end(), Float(1));
        compensatedDistribution = PiecewiseConstant2D(d, domain, alloc);

        scale = CalculateImageScale(*image);
        printf("hdr scale %f\n", scale);
        rotate = RotateY(360 - theta);
    }
     
    HDRLight* HDRLight::Create(const Transform& worldFromLight, 
        Float phi, Float theta,  SpectrumTexture L, Image* image, Float power, Allocator alloc) {


        return alloc.new_object<HDRLight>(worldFromLight, phi, theta, L, image, power, alloc);
    }


    CPU_GPU LightLiSample HDRLight::SampleLi(
        LightSampleContext ctx, Point2f u, SampledWavelengths lambda,
        bool allowIncompletePDF) const {

        // Find $(u,v)$ sample coordinates in infinite light texture
        Float mapPDF = 0;
        Point2f uv;
        if (allowIncompletePDF)
           uv = compensatedDistribution.Sample(u, &mapPDF);
        else
            uv = distribution.Sample(u, &mapPDF);
        if (mapPDF == 0)
            return {};
        uv.y = 1 - uv.y; //不知道是不是v 反了
        // Convert infinite light sample point to direction
        Vector3f wLight = EqualAreaSquareToSphere(uv);
        Swap(wLight.y, wLight.z);
        Vector3f wi = wLight;

        // Compute PDF for sampled infinite light direction
        Float pdf = mapPDF / (4 * Pi);
        
        // Return radiance value for infinite light direction
        return LightLiSample(
            ImageLe(uv, lambda), RGB(1, 1, 1), wi, pdf,
            SurfaceInteraction(ctx.p() + wi * (2 * sceneRadius)));
    }

    CPU_GPU
        RGB HDRLight::ImageLe(Point2f uv, const SampledWavelengths& lambda) const {
        RGB  color(0, 0, 0);
        TextureEvalContext tec;
        tec.uv = uv;
        if (L.Tag() > 0)
            color = L.Evaluate(tec, {});
        color.r *= power;
        color.g *= power;
        color.b *= power;
        return scale * color;
    }

    CPU_GPU LightLiSample HDRLight::SampleRay(Ray r) const {

        RGB color(0, 0, 0);
        Float pdf = 1.f;
        Vector3f dir = rotate(r.d);
        Swap(dir.y, dir.z);


        TextureEvalContext tec;
        tec.uv = EqualAreaSphereToSquare(dir);
        if (L.Tag() > 0)
            color = L.Evaluate(tec, {});
        color.r *= 255.f * power;
        color.g *= 255.f * power;
        color.b *= 255.f * power;
        return LightLiSample(color, color, {}, pdf, {});
    }
}