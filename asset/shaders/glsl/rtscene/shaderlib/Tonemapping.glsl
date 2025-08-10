#ifndef TONEMAPPING_H
#define TONEMAPPING_H

vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

vec3 toneMapReinhard(in vec3 rgb, float exposure)
{
    vec3 mapped = rgb * exposure;
    float luminance = 0.299 * mapped.x + 0.587 * mapped.y + 0.114 * mapped.z;
	return mapped / (1.0 + luminance / 1.5);

    //vec3 mapped = rgb * exposure;
    //return mapped / (1.0 + mapped);  
}
#endif