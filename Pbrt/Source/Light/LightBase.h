#pragma once
#include <Math/Math.h>
#include <Math/Transform.h>
namespace Render {
	class LightBase {
	public:
		LightBase(const Transform& worldFromLight);
	protected:
		Transform worldFromLight;
	};
}