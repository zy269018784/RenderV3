#pragma once
#include <cpu_gpu.h>
#include <Type/Types.h>
#include <Math/Math.h>
namespace Render {
    CPU_GPU Point2f SampleSphericalMap(Vector3f v);
    CPU_GPU Vector3f InvertSampleSphericalMap(Point2f uv);
}