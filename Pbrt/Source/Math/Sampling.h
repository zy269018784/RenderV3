#pragma once
#include <cpu_gpu.h>
#include <Math/Math.h>
#include <Util/Optional.h>
#include <Container/Span.h>
#include <Container/Vector.h>
#include <Container/Array2D.h>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace Render;
    
namespace Render
{

    CPU_GPU inline Vector3f SampleUniformHemisphere(Point2f u) {
        Float z = u[0];
        Float r = SafeSqrt(1 - Sqr(z));
        Float phi = 2 * Pi * u[1];
        return { r * std::cos(phi), r * std::sin(phi), z };
    }

    CPU_GPU inline Float UniformHemispherePDF() {
        return Inv2Pi;
    }

    CPU_GPU inline Vector3f SampleUniformSphere(Point2f u) {
        Float z = 1 - 2 * u[0];
        Float r = SafeSqrt(1 - Sqr(z));
        Float phi = 2 * Pi * u[1];
        return { r * std::cos(phi), r * std::sin(phi), z };
    }

    CPU_GPU inline Float UniformSpherePDF() {
        return Inv4Pi;
    }

    CPU_GPU inline Point2f SampleUniformDiskConcentric(Point2f u) {
        // Map _u_ to $[-1,1]^2$ and handle degeneracy at the origin
        Point2f uOffset = 2 * u - Vector2f(1, 1);
        if (uOffset.x == 0 && uOffset.y == 0)
            return { 0, 0 };

        // Apply concentric mapping to point
        Float theta, r;
        if (std::abs(uOffset.x) > std::abs(uOffset.y)) {
            r = uOffset.x;
            theta = PiOver4 * (uOffset.y / uOffset.x);
        }
        else {
            r = uOffset.y;
            theta = PiOver2 - PiOver4 * (uOffset.x / uOffset.y);
        }
        return r * Point2f(std::cos(theta), std::sin(theta));
    }

    CPU_GPU inline Vector3f SampleCosineHemisphere(Point2f u) {
        Point2f d = SampleUniformDiskConcentric(u);
        Float z = SafeSqrt(1 - Sqr(d.x) - Sqr(d.y));
        return Vector3f(d.x, d.y, z);
    }

    CPU_GPU inline Float CosineHemispherePDF(Float cosTheta) {
        return cosTheta * InvPi;
    }

    // 采样三角形的重心坐标
    CPU_GPU inline Array<Float, 3> SampleUniformTriangle(Point2f u) {
        Float b0, b1;
        if (u[0] < u[1]) {
            b0 = u[0] / 2;
            b1 = u[1] - b0;
        }
        else {
            b1 = u[1] / 2;
            b0 = u[0] - b1;
        }
        return { b0, b1, 1 - b0 - b1 };
    }

    CPU_GPU inline Point2f SampleUniformDiskPolar(Point2f u) {
        Float r = sqrtf(u[0]);
        Float theta = 2 * Pi * u[1];
        return { r * cos(theta), r * sin(theta) };
    }

   CPU_GPU
        inline Point2f InvertUniformDiskPolarSample(Point2f p) {
        Float phi = std::atan2(p.y, p.x);
        if (phi < 0)
            phi += 2 * Pi;
        return Point2f(Sqr(p.x) + Sqr(p.y), phi / (2 * Pi));
    }

   class PiecewiseConstant1D {
   public:
       PiecewiseConstant1D() = default;
       PiecewiseConstant1D(Allocator alloc) : func(alloc), cdf(alloc) {}
       PiecewiseConstant1D(Span<const Float> f, Allocator alloc = {})
           : PiecewiseConstant1D(f, 0., 1., alloc) {}

       PiecewiseConstant1D(Span<const Float> f, Float min, Float max,
           Allocator alloc = {})
           : func(f.begin(), f.end(), alloc), cdf(f.size() + 1, alloc), min(min), max(max) {
           //CHECK_GT(max, min);
           // Take absolute value of _func_
           for (Float& f : func)
               f = std::abs(f);

           // Compute integral of step function at $x_i$
           cdf[0] = 0;
           //cout << "min " << min << " max " << max << endl;
           //cout << "cdf[0] "   << cdf[0] << endl;
           //cout << "f.size() " << f.size() << endl;
           size_t n = f.size();
           for (size_t i = 1; i < n + 1; ++i) {
               //CHECK_GE(func[i - 1], 0);
               cdf[i] = cdf[i - 1] + func[i - 1] * (max - min) / n;
               //cout << "cdf[" << i << "]  " << cdf[i] << endl;
           }


           // Transform step function integral into CDF
           funcInt = cdf[n];
           if (funcInt == 0)
               for (size_t i = 1; i < n + 1; ++i)
                   cdf[i] = Float(i) / Float(n);
           else
               for (size_t i = 1; i < n + 1; ++i)
                   cdf[i] /= funcInt;
           //for (size_t i = 0; i < n + 1; ++i) {
           //    cout << "cdf[" << i << "]  " << cdf[i] << endl;
           //}
           //cout << "funcInt " << funcInt << endl;
       }

       CPU_GPU
           Float Integral() const { return funcInt; }
       CPU_GPU
           size_t size() const { return func.size(); }

       CPU_GPU
           Float Sample(Float u, Float* pdf = nullptr, int* offset = nullptr) const {
           // Find surrounding CDF segments and _offset_
           int o = FindInterval((int)cdf.size(), [&](int index) { return cdf[index] <= u; });
           if (offset)
               *offset = o;

           //printf("o = %d\n", o);
           //printf("u = %f\n", u);
           //printf("cdf[o] = %f, cdf[o + 1] = %f\n", cdf[o], cdf[o + 1]);

           // Compute offset along CDF segment
           Float du = u - cdf[o];
           //printf("du = %f\n", du);

           if (cdf[o + 1] - cdf[o] > 0)
               du /= cdf[o + 1] - cdf[o];
           //DCHECK(!IsNaN(du));
           //printf("du = %f\n", du);
           // Compute PDF for sampled offset
           if (pdf)
               *pdf = (funcInt > 0) ? func[o] / funcInt : 0;

           //printf("o + du  = %f\n", o + du);
           //printf("size() = %llu\n", size());
           //printf("(o + du) / size()  = %f\n", (o + du) / size());
           // Return $x$ corresponding to sample
           return Lerp((o + du) / size(), min, max);
       }


       CPU_GPU
           Optional<Float> Invert(Float x) const {
           // Compute offset to CDF values that bracket $x$
           if (x < min || x > max)
               return {};
           Float c = (x - min) / (max - min) * func.size();
           int offset = Clamp(int(c), 0, func.size() - 1);
           //DCHECK(offset >= 0 && offset + 1 < cdf.size());

           // Linearly interpolate between adjacent CDF values to find sample value
           Float delta = c - offset;
           return Lerp(delta, cdf[offset], cdf[offset + 1]);
       }

       CPU_GPU
           size_t BytesUsed() const {
           return (func.capacity() + cdf.capacity()) * sizeof(Float);
       }


       Vector<Float> func, cdf;
       Float min, max;
       Float funcInt = 0;
   };

   // PiecewiseConstant2D Definition
   class PiecewiseConstant2D {
   public:
       // PiecewiseConstant2D Public Methods
       PiecewiseConstant2D() = default;
       PiecewiseConstant2D(Allocator alloc) : pConditionalV(alloc), pMarginal(alloc) {}
       PiecewiseConstant2D(Span<const Float> data, int nx, int ny,
           Allocator alloc = {})
           : PiecewiseConstant2D(data, nx, ny, Bounds2f(Point2f(0, 0), Point2f(1, 1)),
               alloc) {}
       explicit PiecewiseConstant2D(const Array2D<Float>& data, Allocator alloc = {})
           : PiecewiseConstant2D(Span<const Float>(data), data.XSize(), data.YSize(),
               alloc) {}
       PiecewiseConstant2D(const Array2D<Float>& data, Bounds2f domain, Allocator alloc = {})
           : PiecewiseConstant2D(Span<const Float>(data), data.XSize(), data.YSize(),
               domain, alloc) {}

       CPU_GPU
           size_t BytesUsed() const {
           return pConditionalV.size() *
               (pConditionalV[0].BytesUsed() + sizeof(pConditionalV[0])) +
               pMarginal.BytesUsed();
       }

       CPU_GPU
           Bounds2f Domain() const { return domain; }

       CPU_GPU
           Point2i Resolution() const {
           return { int(pConditionalV[0].size()), int(pMarginal.size()) };
       }

       //std::string ToString() const {
       //    return StringPrintf("[ PiecewiseConstant2D domain: %s pConditionalV: %s "
       //        "pMarginal: %s ]",
       //        domain, pConditionalV, pMarginal);
       //}
       static void TestCompareDistributions(const PiecewiseConstant2D& da,
           const PiecewiseConstant2D& db, Float eps = 1e-5);

       PiecewiseConstant2D(Span<const Float> func, int nu, int nv, Bounds2f domain,
           Allocator alloc = {})
           : domain(domain), pConditionalV(alloc), pMarginal(alloc) {
           //CHECK_EQ(func.size(), (size_t)nu * (size_t)nv);
           pConditionalV.reserve(nv);
           for (int v = 0; v < nv; ++v)
               // Compute conditional sampling distribution for $\tilde{v}$
               pConditionalV.emplace_back(func.subspan(v * nu, nu), domain.pMin[0],
                   domain.pMax[0], alloc);

           // Compute marginal sampling distribution $p[\tilde{v}]$
           Vector<Float> marginalFunc;
           marginalFunc.reserve(nv);
           for (int v = 0; v < nv; ++v)
               marginalFunc.push_back(pConditionalV[v].Integral());
           pMarginal =
               PiecewiseConstant1D(marginalFunc, domain.pMin[1], domain.pMax[1], alloc);

           //cout << "pMarginal " << endl;
           //for (int i = 0; i < pMarginal.size(); i++) {
           //    cout << "i " << i << " " << pMarginal.func[i] << endl;
           //}
       }

       CPU_GPU
           Float Integral() const { return pMarginal.Integral(); }

       CPU_GPU
           inline Point2f Sample(Point2f u, Float* pdf = nullptr, Point2i* offset = nullptr) const {
         
           Float pdfs[2];
           Point2i uv;
           Float d1 = pMarginal.Sample(u[1], &pdfs[1], &uv[1]);
           Float d0 = pConditionalV[uv[1]].Sample(u[0], &pdfs[0], &uv[0]); 
           //printf("u[1] %f, d1 %f\n", u[1], d1);
           //printf("u[0] %f, d1 %f\n", u[0], d0);
           if (pdf)
               *pdf = pdfs[0] * pdfs[1];
           if (offset)
               *offset = uv;
           return Point2f(d0, d1);
       }

       CPU_GPU
           Float PDF(Point2f pr) const {
           Point2f p = Point2f(domain.Offset(pr));
           int iu =
               Clamp(int(p[0] * pConditionalV[0].size()), 0, pConditionalV[0].size() - 1);
           int iv = Clamp(int(p[1] * pMarginal.size()), 0, pMarginal.size() - 1);
           return pConditionalV[iv].func[iu] / pMarginal.Integral();
       }

       CPU_GPU
           Optional<Point2f> Invert(Point2f p) const {
           Optional<Float> mInv = pMarginal.Invert(p[1]);
           if (!mInv)
               return {};
           Float p1o = (p[1] - domain.pMin[1]) / (domain.pMax[1] - domain.pMin[1]);
           if (p1o < 0 || p1o > 1)
               return {};
           int offset = Clamp(p1o * pConditionalV.size(), 0, pConditionalV.size() - 1);
           Optional<Float> cInv = pConditionalV[offset].Invert(p[0]);
           if (!cInv)
               return {};
           return Point2f(*cInv, *mInv);
       }

   public:
       // PiecewiseConstant2D Private Members
       Bounds2f domain;
       Vector<PiecewiseConstant1D> pConditionalV;
       PiecewiseConstant1D pMarginal;
   };

}