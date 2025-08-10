#pragma once
#include <cpu_gpu.h>
#include <Type/Types.h>
#include <Math/Math.h>
CPU_GPU Vector3f EqualAreaSquareToSphere(Point2f p);
CPU_GPU Point2f EqualAreaSphereToSquare(Vector3f d);