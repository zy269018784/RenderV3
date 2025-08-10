#include <Integrator/WaterIntegrator.h>
#include <Texture/TextureEvalContext.h>
#include <Texture/MaterialEvalContext.h>
#include <Camera/SampledWavelengths.h>
#include <Camera/CameraSample.h>
#include <Camera/CameraRay.h>
#include <Ray/Ray.h>
#include <Intersection/Intersection.h>
#include <Intersection/Interaction.h>
#include <Light/LightSampleContext.h>
#include <Light/LightLiSample.h>
#include <Scene/Scene.h>
#include <Math/Scattering.h>
#include <Math/Noise.h> 
#include <Math/SDF.h>

namespace Render {

    //-------------------------------------------------------------------------------------------
    WaterIntegrator::WaterIntegrator(Primitive aggregate, Scene* scene) {
        this->aggregate = aggregate;
        this->scene = scene;
    }

    Optional<ShapeIntersection> WaterIntegrator::Intersect(const Ray& ray,
        Float tMax) const {
        if (aggregate)
            return aggregate.Intersect(ray, tMax);
        return {};
    }

    RGB WaterIntegrator::Li(const Ray& ray, Float u, Float tMax) const {

        RGB sky(0, 0, 0);
        if (ray.d.y < 0)
            return sky;
        Vector3f sunDir = Normalize(Vector3f(1, 0.5f, 0));
        Float sundot = Clamp(Dot(ray.d, sunDir), 0.f, 1.f);

        RGB blueSky = RGB(0.3, .55, 0.8);
        RGB redSky = RGB(0.8, 0.8, 0.6);

        sky = Mix(blueSky, redSky, 1.5 * pow(sundot, 8.));
        //sky += RGB(0.9, 0.3, 0.9) * pow(sundot, 0.5)  * 0.1f;
        //sky += RGB(1., 0.7, 0.7)  * pow(sundot, 1.)   * 0.2f;
        sky += RGB(1., 1, 1) * pow(sundot, 256.) * 0.95f;
        // 地平线
        sky = Mix(sky, RGB(0.9, 0.75, 0.8) * 0.9f, pow(1. - max(ray.d.y, 0.f), 8.0));

        sky = Clamp(sky, 0, 1);
        return sky;
    }
#if 0
    CPU_GPU Float GetWaves(Point2f position, int iterations) {
        return Sin(position.x * 1e-4f);
    }

    CPU_GPU Vector3f Normal(Point3f pos, Float e, Float depth) {
        return Normalize(Vector3f(-Cos(pos.x * 1e-4f), 1, 0));
    }

    CPU_GPU Float RaymarchWater(Ray ray) {
        int steps = 10240;
        for (int i = 0; i < steps; i++) {
            Float t = i * 1e2;
            Point3f pos = ray.o + t * ray.d;
            Float height = GetWaves({ pos.x, pos.z }, 0) * 30000;
            if (pos.y < height + 0.01f)
                return t;
        }
        return -1;
    }


    CPU_GPU Float intersectPlane(Vector3f origin, Vector3f direction, Vector3f point, Normal3f normal) {
        return Clamp(Dot(point - origin, normal) / Dot(direction, normal), -1.0f, 9991999.0f);
    }

    RGB WaterIntegrator::LiPixel(int row, int col) const {
        unsigned Width = scene->renderBuffer->Width();
        unsigned Height = scene->renderBuffer->Height();

        uint8_t* RenderBuffer = (uint8_t*)scene->renderBuffer->getColorBufferPointer();
        Float* accBuffer = (Float*)scene->renderBuffer->getAccBufferPointer();
        uint8_t* pixel_color = &RenderBuffer[3 * (row * Width + col) + 0];
        Float* acc_color = &accBuffer[3 * (row * Width + col) + 0];

        CameraSample cs;
        cs.pFilm.x = col;
        cs.pFilm.y = row;
        SampledWavelengths w;
        CameraRay r = scene->camera.GenerateRay(cs, w);

        pixel_color[0] = 0;
        pixel_color[1] = 0;
        pixel_color[2] = 0;   
        if (r.ray.d.y > 0) {
            pixel_color[0] = 0;
            pixel_color[1] = 0;
            pixel_color[2] = 255;
        }
        Float t = RaymarchWater(r.ray);
        if (t >= 0.f) {
            Point3f HitPos = r.ray.o + r.ray.d * t;
            Vector3f N1 = Normal(HitPos, 0.01, 0);
            Float f2 = Dot(Normalize(Vector3f(0,1, 0)), N1);
            //f2 = Remap(f2, 0, 1, 0, 1);
            f2 = Clamp(f2, 0, 1);

            pixel_color[0] = f2 * 255;
            pixel_color[1] = f2 * 255;
            pixel_color[2] = f2 * 255;
        }

        return {};
    }
#endif

#if 0

    CPU_GPU Float GetWaves(Point2f position, int iterations) {
        return Sin(position.x * 1e-4f);
    }

    CPU_GPU Vector3f Normal(Point3f pos, Float e, Float depth) {
        return Normalize(Vector3f(-Cos(pos.x * 1e-4f), 1, 0));
    }

    CPU_GPU Float RaymarchWater(Ray ray) {
        Float stepSize = 0;
        Float steps = 0;
        Float h = ray.o.y;
        if (ray.d.y >= 0) {
            stepSize = 1e3;
            steps = 1280;
            for (int i = 0; i < steps; i++) {
                Float t = i * stepSize;
                Point3f pos = ray.o + ray.d * t;
                Float  wh = GetWaves({ pos.x, pos.z }, 1) * 30000;
                // printf("wh %f ", wh);
                if (pos.y < wh + 0.01f) {
                    return t;
                }
            }
        }
        else {
            Float h = ray.o.y;
            steps = 1280000;
            stepSize = -h / steps;
            for (int i = 0; i < steps; i++) {
                Float t = i * stepSize / ray.d.y;
                Point3f pos = ray.o + ray.d * t;
                Float  wh = GetWaves({ pos.x, pos.z }, 1) * 30000;
                // printf("wh %f ", wh);
                if (pos.y < wh + 0.01f) {
                    return t;
                }
            }
        }
        return -1;
    }



    RGB WaterIntegrator::LiPixel(int row, int col) const {
        unsigned Width = scene->renderBuffer->Width();
        unsigned Height = scene->renderBuffer->Height();

        uint8_t* RenderBuffer = (uint8_t*)scene->renderBuffer->getColorBufferPointer();
        Float* accBuffer = (Float*)scene->renderBuffer->getAccBufferPointer();
        uint8_t* pixel_color = &RenderBuffer[3 * (row * Width + col) + 0];
        Float* acc_color = &accBuffer[3 * (row * Width + col) + 0];

        CameraSample cs;
        cs.pFilm.x = col;
        cs.pFilm.y = row;
        SampledWavelengths w;
        CameraRay r = scene->camera.GenerateRay(cs, w);

        pixel_color[0] = 0;
        pixel_color[1] = 0;
        pixel_color[2] = 0;
        if (r.ray.d.y > 0) {
            pixel_color[0] = 0;
            pixel_color[1] = 0;
            pixel_color[2] = 255;
        }

       //if (0 == row && 0 == col) {
       //    Vector3f N1 = Normal22({ 0, 0, 0 }, 0.01, 0);
       //    printf("N1 %f %f %f\n", N1.x, N1.y, N1.z);
       //}

        Float t = RaymarchWater(r.ray);
        if (t >= 0.f) {
            Point3f HitPos = r.ray.o + r.ray.d * t;
            Vector3f N1 = Normal(HitPos, 0.1f, 0);
            Float f2 = Dot(Normalize(Vector3f(0, 1, 0)), N1);
            //f2 = Remap(f2, 0, 1, 0, 1);
            f2 = Clamp(f2, 0, 1);

            pixel_color[0] = f2 * 255;
            pixel_color[1] = f2 * 255;
            pixel_color[2] = f2 * 255;
        }

        return {};
    }
#endif

#if 0
        /*
        frequency: 频率
        timeshift: 偏移
    */
    CPU_GPU Vector2f Waves(Point2f position, Vector2f direction, Float frequency, Float timeshift) {
        Float x = Dot(direction, position) * frequency + timeshift;
        Float wave = exp(sin(x) - 1.0);
        Float dx = wave * cos(x);
        return Vector2f(wave, -dx);
    }

    CPU_GPU Float GetWaves(Point2f position, int iterations) {
        Float iter = 0.0f;
        Float frequency = 1.0f;
        Float timeMultiplier = 2.0f;
        Float weight = 1.0;             // weight in final sum for the wave, this will change every iteration
        Float sumOfValues = 0.0;        // will store final sum of values
        Float sumOfWeights = 0.0;       // will store final sum of weights
        for (int i = 0; i < iterations; i++) {
            Vector2f p(sin(iter), cos(iter));
            Vector2f res = Waves(position, p, frequency, timeMultiplier * 0.f);

            sumOfValues += res.x * weight;
            sumOfWeights += weight;

            weight *= 0.82;
            frequency *= 1.18;
            timeMultiplier *= 1.07;
            iter += 1232.399963;
        }
        return 1000.f * sumOfValues / sumOfWeights;
    }

    #define ITERATIONS_NORMAL 40
    CPU_GPU Vector3f Normal(Point2f pos, Float e, Float depth) {
        Point2f ex = Point2f(e, 0);
        Float H = GetWaves({ pos.x, pos.y }, ITERATIONS_NORMAL) * depth;
        Vector3f a = Vector3f(pos.x, H, pos.y);
        return Normalize(
            Cross(
                a - Vector3f(pos.x - e, GetWaves({ pos.x - ex.x, pos.y - ex.y }, ITERATIONS_NORMAL) * depth, pos.y),
                a - Vector3f(pos.x, GetWaves({ pos.x - ex.y, pos.y + ex.x }, ITERATIONS_NORMAL) * depth, pos.y + e)
            )
        );
    }


    CPU_GPU Float RaymarchWater (Ray ray) {
        Float stepSize = 0;
        Float steps = 0;
        Float h = ray.o.y;
        if (ray.d.y >= 0) {
            stepSize = 1e3;
            steps = 128;
            for (int i = 0; i < steps; i++) {
                Float t = i * stepSize;
                Point3f pos = ray.o + ray.d * t;
                Float  wh = GetWaves({ pos.x, pos.z }, 12);
                // printf("wh %f ", wh);
                if (pos.y < wh + 0.01f) {
                    return t;
                }
            }
        }
        else {
            Float h = ray.o.y;
            steps = 128;
            stepSize = -h / steps;
            for (int i = 0; i < steps; i++) {
                Float t = i * stepSize / ray.d.y;
                Point3f pos = ray.o + ray.d * t;
                Float  wh = GetWaves({ pos.x, pos.z }, 12);
                // printf("wh %f ", wh);
                if (pos.y < wh + 0.01f) {
                    return t;
                }
            }
        }
        return -1;
    }


    RGB WaterIntegrator::LiPixel(int row, int col) const {
        unsigned Width = scene->renderBuffer->Width();
        unsigned Height = scene->renderBuffer->Height();

        uint8_t* RenderBuffer = (uint8_t*)scene->renderBuffer->getColorBufferPointer();
        Float* accBuffer = (Float*)scene->renderBuffer->getAccBufferPointer();
        uint8_t* pixel_color = &RenderBuffer[3 * (row * Width + col) + 0];
        Float* acc_color = &accBuffer[3 * (row * Width + col) + 0];

        CameraSample cs;
        cs.pFilm.x = col;
        cs.pFilm.y = row;
        SampledWavelengths w;
        CameraRay r = scene->camera.GenerateRay(cs, w);

        pixel_color[0] = 0;
        pixel_color[1] = 0;
        pixel_color[2] = 0;
        if (r.ray.d.y > 0) {
            pixel_color[0] = 0;
            pixel_color[1] = 0;
            pixel_color[2] = 255;
        }

        if (0 == row && 0 == col) {
            Vector3f N1 = Normal({ 0, 0 }, 0.1f, 0);
            printf("N1 %f %f %f\n", N1.x, N1.y, N1.z);
            Float e = 0.01f;
            Float h1 = GetWaves({ 0, 0 }, 40);
            Float h2 = GetWaves({ 0, e }, 40);
            Float h3 = GetWaves({ e, 0 }, 40);
            Point3f p1(0, h1, 0);
            Point3f p2(0, h2, e);
            Point3f p3(e, h3, 0);
            printf("h1 %f, h2 %f, h3 %f\n", h1, h2, h3);
        }
 

        Float t = RaymarchWater(r.ray);
        if (t >= 0.f) {
            Point3f HitPos = r.ray.o + r.ray.d * t;
            Vector3f N1 = Normal({ HitPos.x, HitPos.y },  0.1f, 0);
            Float f2 = Dot(Normalize(Vector3f(0, 1, 0)), N1);
            //f2 = Remap(f2, 0, 1, 0, 1);
            f2 = Clamp(f2, 0, 1);

            pixel_color[0] = f2 * 255;
            pixel_color[1] = f2 * 255;
            pixel_color[2] = f2 * 255;
        }

        return {};
    }
#endif

#if 0
    /*
    frequency: 频率
    timeshift: 偏移
*/
    CPU_GPU Vector2f Waves(Point2f position, Vector2f direction, Float frequency, Float timeshift) {
        Float x = Dot(direction, position) * frequency + timeshift;
        Float wave = exp(sin(x) - 1.0);
        Float dx = wave * cos(x);
        return Vector2f(wave, -dx);
    }

    CPU_GPU Float GetWaves(Point2f position, int iterations) {
        Float iter = 0.0f;
        Float frequency = 1.0f;
        Float timeMultiplier = 2.0f;
        Float weight = 1.0;             // weight in final sum for the wave, this will change every iteration
        Float sumOfValues = 0.0;        // will store final sum of values
        Float sumOfWeights = 0.0;       // will store final sum of weights
        for (int i = 0; i < iterations; i++) {
            Vector2f p(sin(iter), cos(iter));
            Vector2f res = Waves(position, p, frequency, timeMultiplier * 0.f);

            sumOfValues += res.x * weight;
            sumOfWeights += weight;

            weight *= 0.82;
            frequency *= 1.18;
            timeMultiplier *= 1.07;
            iter += 1232.399963;
        }
        return 1000.f * sumOfValues / sumOfWeights;
    }

   // #define ITERATIONS_NORMAL 40
   // CPU_GPU Vector3f Normal(Point2f pos, Float e, Float depth) {
   //     Point2f ex = Point2f(e, 0);
   //     Float H = GetWaves({ pos.x, pos.y }, ITERATIONS_NORMAL) * depth;
   //     Vector3f a = Vector3f(pos.x, H, pos.y);
   //     return Normalize(
   //         Cross(
   //             a - Vector3f(pos.x - e, GetWaves({ pos.x - ex.x, pos.y - ex.y }, ITERATIONS_NORMAL) * depth, pos.y),
   //             a - Vector3f(pos.x, GetWaves({ pos.x - ex.y, pos.y + ex.x }, ITERATIONS_NORMAL) * depth, pos.y + e)
   //         )
   //     );
   // }


    CPU_GPU Vector3f Normal(Point2f pos, Float e, Float depth) {
        return Normalize(Vector3f(-Cos(pos.x * 1e-2f), 1, 0));
    }


    CPU_GPU Float RaymarchWater(Ray ray) {
        Float stepSize = 0;
        Float steps = 0;
        Float h = ray.o.y;
        if (ray.d.y >= 0) {
            stepSize = 1e3;
            steps = 128;
            for (int i = 0; i < steps; i++) {
                Float t = i * stepSize;
                Point3f pos = ray.o + ray.d * t;
                Float  wh = GetWaves({ pos.x, pos.z }, 12);
                // printf("wh %f ", wh);
                if (pos.y < wh + 0.01f) {
                    return t;
                }
            }
        }
        else {
            Float h = ray.o.y;
            steps = 128;
            stepSize = -h / steps;
            for (int i = 0; i < steps; i++) {
                Float t = i * stepSize / ray.d.y;
                Point3f pos = ray.o + ray.d * t;
                Float  wh = GetWaves({ pos.x, pos.z }, 12);
                // printf("wh %f ", wh);
                if (pos.y < wh + 0.01f) {
                    return t;
                }
            }
        }
        return -1;
    }


    RGB WaterIntegrator::LiPixel(int row, int col) const {
        unsigned Width = scene->renderBuffer->Width();
        unsigned Height = scene->renderBuffer->Height();

        uint8_t* RenderBuffer = (uint8_t*)scene->renderBuffer->getColorBufferPointer();
        Float* accBuffer = (Float*)scene->renderBuffer->getAccBufferPointer();
        uint8_t* pixel_color = &RenderBuffer[3 * (row * Width + col) + 0];
        Float* acc_color = &accBuffer[3 * (row * Width + col) + 0];

        CameraSample cs;
        cs.pFilm.x = col;
        cs.pFilm.y = row;
        SampledWavelengths w;
        CameraRay r = scene->camera.GenerateRay(cs, w);

        pixel_color[0] = 0;
        pixel_color[1] = 0;
        pixel_color[2] = 0;
        if (r.ray.d.y > 0) {
            pixel_color[0] = 0;
            pixel_color[1] = 0;
            pixel_color[2] = 255;
        }

        if (0 == row && 0 == col) {
            Vector3f N1 = Normal({ 0, 0 }, 0.1f, 0);
            printf("N1 %f %f %f\n", N1.x, N1.y, N1.z);
            Float e = 0.01f;
            Float h1 = GetWaves({ 0, 0 }, 40);
            Float h2 = GetWaves({ 0, e }, 40);
            Float h3 = GetWaves({ e, 0 }, 40);
            Point3f p1(0, h1, 0);
            Point3f p2(0, h2, e);
            Point3f p3(e, h3, 0);
            printf("h1 %f, h2 %f, h3 %f\n", h1, h2, h3);
        }

        Float t = -r.ray.o.y / r.ray.d.y;
        if (t > 0.f) {
            Point3f HitPos = r.ray.o + r.ray.d * t;
            if (Distance(r.ray.o, HitPos) < 1e4) {
                Vector3f N1 = Normal({ HitPos.x, HitPos.y }, 0.1f, 0);
                Float f = Dot(Normalize(Vector3f(1, 1, 0)), N1);
                pixel_color[0] = f * 255;
                pixel_color[1] = f * 255;
                pixel_color[2] = f * 255;
            }
        }

        //Float t = RaymarchWater(r.ray);
        //if (t >= 0.f) {
        //    Point3f HitPos = r.ray.o + r.ray.d * t;
        //    Vector3f N1 = Normal({ HitPos.x, HitPos.y }, 0.1f, 0);
        //    Float f2 = Dot(Normalize(Vector3f(0, 1, 0)), N1);
        //    //f2 = Remap(f2, 0, 1, 0, 1);
        //    f2 = Clamp(f2, 0, 1);
        //
        //    pixel_color[0] = f2 * 255;
        //    pixel_color[1] = f2 * 255;
        //    pixel_color[2] = f2 * 255;
        //}
        return {};
    }
#endif
#if 0

#define SDF_START_STEP_SIZE 1
#define SDF_END_STEP_SIZE 10
#define MAX_SDF_STEPS 20
    RGB WaterIntegrator::LiPixel(int row, int col) const {
        unsigned Width = scene->renderBuffer->Width();
        unsigned Height = scene->renderBuffer->Height();

        uint8_t* RenderBuffer = (uint8_t*)scene->renderBuffer->getColorBufferPointer();
        Float* accBuffer = (Float*)scene->renderBuffer->getAccBufferPointer();
        uint8_t* pixel_color = &RenderBuffer[3 * (row * Width + col) + 0];
        Float* acc_color = &accBuffer[3 * (row * Width + col) + 0];

        CameraSample cs;
        cs.pFilm.x = col;
        cs.pFilm.y = row;
        SampledWavelengths w;
        CameraRay r = scene->camera.GenerateRay(cs, w);

        pixel_color[0] = 0;
        pixel_color[1] = 0;
        pixel_color[2] = 0;
        //if (r.ray.d.y > 0)
        {
  

            Float stepSize = SDF_START_STEP_SIZE;
            Float stepIncrement = Float(SDF_END_STEP_SIZE - SDF_START_STEP_SIZE) / Float(MAX_SDF_STEPS);
            Float t = 0.0f;
            Float sdfValue = 0.0;
            Float maxT = 1e4;
            for (int i = 0; i < MAX_SDF_STEPS; i++) {
                
                Point3f p = r.ray.o + t * r.ray.d;
                sdfValue = sdSphere(p, 20);

                if (sdfValue < 0.0 || t>maxT) 
                    break;

                t += max(sdfValue, stepSize);

                stepSize += stepIncrement;
            }

            if (sdfValue < 0.0) {
                Point3f p = r.ray.o + t * r.ray.d;
                //auto n = sdSphereNormal(r.ray.o, 20);
                auto n = sdSphereNormal(p, 20);
                Float f = Dot(n, Normal3f(0, 1, 0));
                pixel_color[0] = f * 0;
                pixel_color[1] = f * 0;
                pixel_color[2] = f * 255;
            }
        }

        return {};
    }
#endif

#if 1
#define TMAX 1000
    CPU_GPU Float inline  map(Vector3f pos)
    {
        Float d = 0.0;

        //float t = fract(iTime);
        //float y = 4.0 * t * (1.0 - t);
        //Vector3f cen = Vector3f(0.0, y, 0.0);

        Float d1 = Length(pos - Vector3f(0, 60, 0)) - 20;
        d = d1;
        Float d2 = pos.y - 30 * (Sin(pos.x * 0.01f) + Cos(pos.z * 0.01f));
       // Float d2 = pos.y;
        d = min(d, d2);

        return d;
    }

    CPU_GPU Normal3f calcNormal(Vector3f pos)
    {
        Float e = 0.0001f;
        Vector3f exyy = Vector3f(e,     0.f,    0.f);
        Vector3f eyxy = Vector3f(0.f,   e,      0.f);
        Vector3f eyyx = Vector3f(0.f,   0.f,      e);
        return Normalize(Normal3f(map(pos + exyy) - map(pos - exyy),
                                  map(pos + eyxy) - map(pos - eyxy),
                                  map(pos + eyyx) - map(pos - eyyx)));
    }

    CPU_GPU Float inline cast_ray(Point3f ro, Vector3f rd) {
        float t = 0.0;
        for (int i = 0; i < 1000; i++) {
            Point3f pos = ro + t * rd;
            Float h = map(pos);
            if (h < 0.0001)
                break;
            t += h;
            if (t > TMAX)
                break;
        }
        return t;
    }

    RGB WaterIntegrator::LiPixel(int row, int col) const {
        unsigned Width = scene->renderBuffer->Width();
        unsigned Height = scene->renderBuffer->Height();

        uint8_t* RenderBuffer = (uint8_t*)scene->renderBuffer->getColorBufferPointer();
        Float* accBuffer = (Float*)scene->renderBuffer->getAccBufferPointer();
        uint8_t* pixel_color = &RenderBuffer[3 * (row * Width + col) + 0];
        Float* acc_color = &accBuffer[3 * (row * Width + col) + 0];

        CameraSample cs;
        cs.pFilm.x = col + 0.5f;
        cs.pFilm.y = row + 0.5f;
        SampledWavelengths w;
        CameraRay r = scene->camera.GenerateRay(cs, w);

        pixel_color[0] = 0;
        pixel_color[1] = 0;
        pixel_color[2] = 0;

        Float t = cast_ray(r.ray.o, r.ray.d);
        if (t < TMAX) {
            Point3f pos = r.ray.o + t * r.ray.d;
            Normal3f n = calcNormal(pos);
            float dif = Clamp(Dot(n, Normalize(Vector3f(1, 1, 0))), 0.0, 1.0);
            pixel_color[0] = 51 * dif + 100.f;
            pixel_color[1] = 86 * dif + 100.f;
            pixel_color[2] = 52 * dif + 100.f;
        }

        return {};
    }
#endif
}

