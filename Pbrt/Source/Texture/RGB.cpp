#include <Texture/RGB.h>
namespace Render {
	CPU_GPU RGBA::RGBA(RGB rgb, Float a) : r(rgb.r), g(rgb.g), b(rgb.b), a(a) {}
}