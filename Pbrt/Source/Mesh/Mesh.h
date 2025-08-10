#pragma once
#include <cpu_gpu.h>
#include <Math/Transform.h>
#include <Material/Material.h>
namespace Render {
	class Mesh {
	public:
		Mesh(Transform worldFromObject, Transform objectFromWorld, Material material);
		CPU_GPU const Transform& WorldFromObject() { return worldFromObject; }
		CPU_GPU const Transform& ObjectFromWorld() { return objectFromWorld; }
	public:
		Material material;
		Transform worldFromObject;
		Transform objectFromWorld;
	};
}